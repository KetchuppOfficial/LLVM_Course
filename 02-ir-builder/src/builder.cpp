#include <array>
#include <cassert>
#include <llvm-18/llvm/IR/Function.h>
#include <llvm-18/llvm/IR/Instructions.h>
#include <llvm-18/llvm/Support/Casting.h>
#include <memory>
#include <string_view>
#include <unordered_map>

#include "llvm/ADT/ArrayRef.h"
#include "llvm/IR/Attributes.h"
#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/DerivedTypes.h"
#include "llvm/IR/GlobalValue.h"
#include "llvm/IR/GlobalVariable.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Type.h"
#include "llvm/Support/raw_ostream.h"

using namespace llvm;

namespace {

auto createModuleHeader(LLVMContext &Ctx) {
  static auto *kSource = "/home/ketchupp/Programming/llvm-course/"
                         "00-graphics_application/src/lloyd_relaxation.c";
  static auto *kDataLayout =
      "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-i128:128-"
      "f80:128-n8:16:32:64-S128";

  auto M = std::make_unique<Module>(kSource, Ctx);
  M->setSourceFileName(kSource);
  M->setDataLayout(kDataLayout);
  M->setTargetTriple("x86_64-pc-linux-gnu");

  return M;
}

using TypesMap = std::unordered_map<std::string_view, StructType *>;

TypesMap defineStructs(LLVMContext &Ctx) {
  std::array<StructType *, 4> TypesArr{
      StructType::create(Ctx, "struct.point_s"),
      StructType::create(Ctx, "struct.polygon_s"),
      StructType::create(Ctx, "struct.tagged_point_s"),
      StructType::create(Ctx, "struct.line_s")};

  auto *I32Type = Type::getInt32Ty(Ctx);

  // %struct.point_s = type { i32, i32 }
  TypesArr[0]->setBody({I32Type, I32Type});
  // %struct.polygon_s = type { [104 x %struct.point_s], i32 }
  TypesArr[1]->setBody({ArrayType::get(TypesArr[0], 104), I32Type});
  // %struct.tagged_point_s = type { %struct.point_s, i32, i32 }
  TypesArr[2]->setBody({I32Type, I32Type, I32Type});
  // %struct.line_s = type { i32, i32, i32 }
  TypesArr[3]->setBody({TypesArr[0], I32Type, I32Type});

  TypesMap Types;
  for (auto *StType : TypesArr) {
    auto Name = StType->getName();
    Types.emplace(std::string_view{Name.data(), Name.size()}, StType);
  }

  return Types;
}

ConstantInt *ConstantInt32(LLVMContext &Ctx, int X) {
  return ConstantInt::getSigned(Type::getInt32Ty(Ctx), X);
}

ConstantInt *ConstantInt64(LLVMContext &Ctx, int X) {
  return ConstantInt::getSigned(Type::getInt64Ty(Ctx), X);
}

void createGlobalVariables(LLVMContext &Ctx, Module *M, const TypesMap &Types) {
  auto *PointSTy = Types.at("struct.point_s");
  auto *PolygonSTy = Types.at("struct.polygon_s");
  auto *TaggedPointSTy = Types.at("struct.tagged_point_s");
  auto *LineSTy = Types.at("struct.line_s");

  // @generate_points.points = internal unnamed_addr global [100 x %struct.point_s] zeroinitializer, align 16
  auto *Points = dyn_cast<GlobalVariable>(M->getOrInsertGlobal(
      "generate_points.points", ArrayType::get(PointSTy, 100)));
  assert(Points);
  Points->setAlignment(Align{16});
  Points->setLinkage(GlobalValue::InternalLinkage);
  Points->setUnnamedAddr(GlobalValue::UnnamedAddr::Global);
  Points->setInitializer(Constant::getNullValue(Points->getValueType()));

  // @get_cells.cells = internal unnamed_addr global [100 x %struct.polygon_s] zeroinitializer, align 16
  auto *Cells = dyn_cast<GlobalVariable>(
      M->getOrInsertGlobal("get_cells.cells", ArrayType::get(PolygonSTy, 100)));
  assert(Cells);
  Cells->setAlignment(Align{16});
  Cells->setLinkage(GlobalValue::InternalLinkage);
  Cells->setUnnamedAddr(GlobalValue::UnnamedAddr::Global);
  Cells->setInitializer(Constant::getNullValue(Cells->getValueType()));

  // @center = internal unnamed_addr global %struct.point_s zeroinitializer, align 8
  auto *Center =
      dyn_cast<GlobalVariable>(M->getOrInsertGlobal("center", PointSTy));
  assert(Center);
  Center->setAlignment(Align{8});
  Center->setLinkage(GlobalValue::InternalLinkage);
  Center->setUnnamedAddr(GlobalValue::UnnamedAddr::Global);
  Center->setInitializer(Constant::getNullValue(Center->getValueType()));

  // @get_cells.intersections = internal global <{ %struct.tagged_point_s, %struct.tagged_point_s, %struct.tagged_point_s, %struct.tagged_point_s, [5098 x %struct.tagged_point_s] }> <{ %struct.tagged_point_s { %struct.point_s zeroinitializer, i32 -4, i32 -3 }, %struct.tagged_point_s { %struct.point_s { i32 720, i32 0 }, i32 -4, i32 -2 }, %struct.tagged_point_s { %struct.point_s { i32 720, i32 720 }, i32 -2, i32 -1 }, %struct.tagged_point_s { %struct.point_s { i32 0, i32 720 }, i32 -1, i32 -3 }, [5098 x %struct.tagged_point_s] zeroinitializer }>, align 16
  auto *Intersections = dyn_cast<GlobalVariable>(M->getOrInsertGlobal(
      "get_cells.intersections",
      StructType::get(Ctx,
                      {TaggedPointSTy, TaggedPointSTy, TaggedPointSTy,
                       TaggedPointSTy, ArrayType::get(TaggedPointSTy, 5098)},
                      true /* isPacked*/)));
  assert(Intersections);
  Intersections->setAlignment(Align{16});
  Intersections->setLinkage(GlobalValue::InternalLinkage);
  Intersections->setInitializer(ConstantStruct::get(
      dyn_cast<StructType>(Intersections->getValueType()),
      {ConstantStruct::get(TaggedPointSTy,
                           {Constant::getNullValue(PointSTy),
                            ConstantInt32(Ctx, -4), ConstantInt32(Ctx, -3)}),
       ConstantStruct::get(
           TaggedPointSTy,
           {ConstantStruct::get(
                PointSTy, {ConstantInt32(Ctx, 720), ConstantInt32(Ctx, 0)}),
            ConstantInt32(Ctx, -4), ConstantInt32(Ctx, -2)}),
       ConstantStruct::get(
           TaggedPointSTy,
           {ConstantStruct::get(
                PointSTy, {ConstantInt32(Ctx, 720), ConstantInt32(Ctx, 720)}),
            ConstantInt32(Ctx, -2), ConstantInt32(Ctx, -1)}),
       ConstantStruct::get(
           TaggedPointSTy,
           {ConstantStruct::get(
                PointSTy, {ConstantInt32(Ctx, 0), ConstantInt32(Ctx, 720)}),
            ConstantInt32(Ctx, -1), ConstantInt32(Ctx, -3)})}));

  // @compute_normals.perps = internal unnamed_addr global [100 x [99 x %struct.line_s]] zeroinitializer, align 16
  auto *Perps = dyn_cast<GlobalVariable>(
      M->getOrInsertGlobal("compute_normals.perps",
                           ArrayType::get(ArrayType::get(LineSTy, 99), 100)));
  assert(Perps);
  Perps->setAlignment(Align{16});
  Perps->setLinkage(GlobalValue::InternalLinkage);
  Perps->setUnnamedAddr(GlobalValue::UnnamedAddr::Global);
  Perps->setInitializer(Constant::getNullValue(Perps->getValueType()));
}

Function *declareLloyd(LLVMContext &Ctx, Module *M) {
  // define dso_local void @lloyd_relaxation() local_unnamed_addr #0 {
  auto *Lloyd = Function::Create(
      FunctionType::get(Type::getVoidTy(Ctx), false /* isVarArg */),
      GlobalValue::ExternalLinkage, "lloyd_relaxation", M);
  dyn_cast<GlobalValue>(Lloyd)->setDSOLocal(true);
  dyn_cast<GlobalValue>(Lloyd)->setUnnamedAddr(GlobalValue::UnnamedAddr::Local);
  dyn_cast<Function>(Lloyd)->addFnAttr(Attribute::AttrKind::NoReturn);
  dyn_cast<Function>(Lloyd)->addFnAttr(Attribute::AttrKind::NoUnwind);
#if 0 // produces strange results
  dyn_cast<Function>(Lloyd)->addFnAttr(Attribute::AttrKind::UWTable);
#endif

  return Lloyd;
}

Function *declareSetColor(LLVMContext &Ctx, Module *M) {
  // declare void @set_color(i32 noundef, i32 noundef, i32 noundef) local_unnamed_addr #1
  auto *Int32Ty = Type::getInt32Ty(Ctx);
  auto *SetColorTy = FunctionType::get(
      Type::getVoidTy(Ctx), {Int32Ty, Int32Ty, Int32Ty}, false /* isVarArg */);
  auto *SetColor = Function::Create(SetColorTy, GlobalValue::ExternalLinkage,
                                    "set_color", M);
  dyn_cast<GlobalValue>(SetColor)->setUnnamedAddr(
      GlobalValue::UnnamedAddr::Local);

  for (auto I : {0, 1, 2}) {
    SetColor->addParamAttr(I, Attribute::NoUndef);
  }

  return SetColor;
}

Function *declareDrawPoint(LLVMContext &Ctx, Module *M) {
  // declare void @draw_point(i32 noundef, i32 noundef) local_unnamed_addr #1
  auto *Int32Ty = Type::getInt32Ty(Ctx);
  auto *DrawPointTy = FunctionType::get(
      Type::getVoidTy(Ctx), {Int32Ty, Int32Ty}, false /* isVarArg */);
  auto *DrawPoint = Function::Create(DrawPointTy, GlobalValue::ExternalLinkage,
                                     "draw_point", M);
  dyn_cast<GlobalValue>(DrawPoint)->setUnnamedAddr(
      GlobalValue::UnnamedAddr::Local);

  for (auto I : {0, 1}) {
    DrawPoint->addParamAttr(I, Attribute::NoUndef);
  }

  return DrawPoint;
}

Function *declareDrawLine(LLVMContext &Ctx, Module *M) {
  // declare void @draw_line(i32 noundef, i32 noundef, i32 noundef, i32 noundef) local_unnamed_addr #1
  auto *Int32Ty = Type::getInt32Ty(Ctx);
  auto *DrawLineTy = FunctionType::get(Type::getVoidTy(Ctx),
                                       {Int32Ty, Int32Ty, Int32Ty, Int32Ty},
                                       false /* isVarArg */);
  auto *DrawLine = Function::Create(DrawLineTy, GlobalValue::ExternalLinkage,
                                    "draw_line", M);
  dyn_cast<GlobalValue>(DrawLine)->setUnnamedAddr(
      GlobalValue::UnnamedAddr::Local);

  for (auto I : {0, 1, 2, 3}) {
    DrawLine->addParamAttr(I, Attribute::NoUndef);
  }

  return DrawLine;
}

Function *declareUpdateScreen(LLVMContext &Ctx, Module *M) {
  // declare void @update_screen() local_unnamed_addr #1
  auto *UpdateScrTy =
      FunctionType::get(Type::getVoidTy(Ctx), false /* isVarArg */);
  auto *UpdateScr = Function::Create(UpdateScrTy, GlobalValue::ExternalLinkage,
                                     "update_screen", M);
  dyn_cast<GlobalValue>(UpdateScr)->setUnnamedAddr(
      GlobalValue::UnnamedAddr::Local);
  return UpdateScr;
}

Function *declareLLVMMemcpy(LLVMContext &Ctx, Module *M) {
  // declare void @llvm.memcpy.p0.p0.i64(ptr noalias nocapture writeonly, ptr noalias nocapture readonly, i64, i1 immarg) #2
  auto *PtrTy = PointerType::getUnqual(Ctx);
  auto *MemcpyTy = FunctionType::get(
      Type::getVoidTy(Ctx),
      {PtrTy, PtrTy, Type::getInt64Ty(Ctx), Type::getInt1Ty(Ctx)},
      false /* isVarArg */);
  auto *Memcpy = Function::Create(MemcpyTy, GlobalValue::ExternalLinkage,
                                  "llvm.memcpy.p0.p0.i64", M);

  Memcpy->addParamAttr(0, Attribute::NoAlias);
  Memcpy->addParamAttr(0, Attribute::NoCapture);
  Memcpy->addParamAttr(0, Attribute::WriteOnly);
  Memcpy->addParamAttr(1, Attribute::NoAlias);
  Memcpy->addParamAttr(1, Attribute::NoCapture);
  Memcpy->addParamAttr(1, Attribute::ReadOnly);
  Memcpy->addParamAttr(3, Attribute::ImmArg);

  return Memcpy;
}

Function *declareRand(LLVMContext &Ctx, Module *M) {
  // declare i32 @Rand() local_unnamed_addr #1
  auto *RandTy = FunctionType::get(Type::getInt32Ty(Ctx), false /* isVarArg */);
  auto *Rand =
      Function::Create(RandTy, GlobalValue::ExternalLinkage, "Rand", M);
  dyn_cast<GlobalValue>(Rand)->setUnnamedAddr(GlobalValue::UnnamedAddr::Local);
  return Rand;
}

Function *declareAtan2Int(LLVMContext &Ctx, Module *M) {
  // declare void @draw_point(i32 noundef, i32 noundef) local_unnamed_addr #1
  auto *Int32Ty = Type::getInt32Ty(Ctx);
  auto *Atan2Ty =
      FunctionType::get(Int32Ty, {Int32Ty, Int32Ty}, false /* isVarArg */);
  auto *Atan2 =
      Function::Create(Atan2Ty, GlobalValue::ExternalLinkage, "atan2_int", M);
  dyn_cast<GlobalValue>(Atan2)->setUnnamedAddr(GlobalValue::UnnamedAddr::Local);

  for (auto I : {0, 1}) {
    Atan2->addParamAttr(I, Attribute::NoUndef);
  }

  return Atan2;
}

Function *declareLLVMUMin(LLVMContext &Ctx, Module *M) {
  // declare i32 @llvm.umin.i32(i32, i32) #3
  auto *Int32Ty = Type::getInt32Ty(Ctx);
  auto *UMinTy =
      FunctionType::get(Int32Ty, {Int32Ty, Int32Ty}, false /* isVarArg */);
  auto *UMin = Function::Create(UMinTy, GlobalValue::ExternalLinkage,
                                "llvm.umin.i32", M);
  return UMin;
}

Function *declareLLVMVectorReduceAdd(LLVMContext &Ctx, Module *M) {
  // declare i32 @llvm.vector.reduce.add.v4i32(<4 x i32>) #3
  auto *Int32Ty = Type::getInt32Ty(Ctx);
  auto *ReduceTy = FunctionType::get(
      Int32Ty, {FixedVectorType::get(Int32Ty, 4)}, false /* isVarArg */);
  auto *Reduce = Function::Create(ReduceTy, GlobalValue::ExternalLinkage,
                                  "llvm.vector.reduce.add.v4i32", M);
  return Reduce;
}

using BBMap = std::unordered_map<std::size_t, BasicBlock *>;

static std::size_t BBIds[] = {
    0,   1,   17,  20,  23,  25,  28,  34,  40,  47,  71,  73,  75,
    77,  81,  83,  105, 116, 119, 123, 127, 138, 141, 152, 170, 181,
    185, 189, 197, 200, 206, 214, 221, 233, 235, 242, 250, 254, 264,
    271, 273, 278, 281, 303, 306, 308, 311, 313, 323, 325, 327, 337,
    339, 341, 351, 355, 357, 367, 369, 374, 410, 413, 418, 422, 423,
    431, 432, 439, 441, 442, 464, 471, 474, 476, 497, 503, 507, 519};

//     81,  83,  105, 116, 119, 123, 127, 138, 141, 152, 170, 181,
//     185, 189, 197, 200, 206, 214, 221, 233, 235, 242, 250, 254, 264,
//     271, 273, 278, 281, 303, 306, 308, 311, 313, 323, 325, 327, 337,
//     339, 341, 351, 355, 357, 367, 369, 374, 410, 413, 418, 422, 423,
//     431, 432, 439, 441, 442, 464, 471, 474, 476, 497, 503, 507, 519}

} // unnamed namespace

int main() {
  LLVMContext Ctx;

  auto M = createModuleHeader(Ctx);
  auto Types = defineStructs(Ctx);
  createGlobalVariables(Ctx, M.get(), Types);

  auto *Lloyd = declareLloyd(Ctx, M.get());
  auto *SetColor = declareSetColor(Ctx, M.get());
  auto *DrawPoint = declareDrawPoint(Ctx, M.get());
  auto *DrawLine = declareDrawLine(Ctx, M.get());
  auto *UpdateScreen = declareUpdateScreen(Ctx, M.get());
  auto *Memcpy = declareLLVMMemcpy(Ctx, M.get());
  auto *Rand = declareRand(Ctx, M.get());
  auto *Atan2Int = declareAtan2Int(Ctx, M.get());
  auto *UMin = declareLLVMUMin(Ctx, M.get());
  auto *VectorReduceAdd = declareLLVMVectorReduceAdd(Ctx, M.get());

  auto *Int32Ty = Type::getInt32Ty(Ctx);
  IRBuilder<> Builder{Ctx};

  BBMap BB;
  for (auto I : BBIds) {
    BB.emplace(I, BasicBlock::Create(Ctx, "", Lloyd));
  }

  // ================================== BB 0 ==================================
  Builder.SetInsertPoint(BB[0]);

  // br label %1
  Builder.CreateBr(BB[1]);
  // ==========================================================================

  // ================================== BB 1 ==================================
  Builder.SetInsertPoint(BB.at(1));

  // %2 = phi i64 [ 0, %0 ], [ %15, %1 ]
  auto *Value2 = Builder.CreatePHI(Type::getInt64Ty(Ctx), 2);
  // %3 = tail call i32 @Rand() #4
  auto *Value3 = Builder.CreateCall(Rand->getFunctionType(), Rand);
  Value3->setTailCall(true);
  // %4 = srem i32 %3, 720
  auto *Value4 = Builder.CreateSRem(Value3, ConstantInt32(Ctx, 720));
  // %5 = icmp slt i32 %4, 10
  auto *Value5 = Builder.CreateICmpSLT(Value4, ConstantInt32(Ctx, 10));
  // %6 = tail call i32 @llvm.umin.i32(i32 %4, i32 710)
  auto *Value6 = Builder.CreateCall(UMin->getFunctionType(), UMin,
                                    {Value4, ConstantInt32(Ctx, 710)});
  Value6->setTailCall(true);
  // %7 = select i1 %5, i32 10, i32 %6
  auto *Value7 = Builder.CreateSelect(Value5, ConstantInt32(Ctx, 10), Value6);
  // %8 = getelementptr inbounds [100 x %struct.point_s], ptr @generate_points.points, i64 0, i64 %2
  auto *Value8 =
      Builder.CreateGEP(ArrayType::get(Types.at("struct.point_s"), 100),
                        M->getNamedValue("generate_points.points"),
                        {ConstantInt64(Ctx, 0), Value2});
  dyn_cast<GetElementPtrInst>(Value8)->setIsInBounds(true);
  // store i32 %7, ptr %8, align 8, !tbaa !5
  Builder.CreateAlignedStore(Value7, Value8, Align{8});
  // %9 = tail call i32 @Rand() #4
  auto *Value9 = Builder.CreateCall(Rand->getFunctionType(), Rand);
  Value3->setTailCall(true);
  // %10 = srem i32 %9, 720
  auto *Value10 = Builder.CreateSRem(Value9, ConstantInt32(Ctx, 720));
  // %11 = icmp slt i32 %10, 10
  auto *Value11 = Builder.CreateICmpSLT(Value10, ConstantInt32(Ctx, 10));
  // %12 = tail call i32 @llvm.umin.i32(i32 %10, i32 710)
  auto *Value12 = Builder.CreateCall(UMin->getFunctionType(), UMin,
                                     {Value10, ConstantInt32(Ctx, 710)});
  Value12->setTailCall(true);
  // %13 = select i1 %11, i32 10, i32 %12
  auto *Value13 =
      Builder.CreateSelect(Value11, ConstantInt32(Ctx, 10), Value12);
  // %14 = getelementptr inbounds [100 x %struct.point_s], ptr @generate_points.points, i64 0, i64 %2, i32 1
  auto *Value14 =
      Builder.CreateGEP(ArrayType::get(Types.at("struct.point_s"), 100),
                        M->getNamedValue("generate_points.points"),
                        {ConstantInt64(Ctx, 0), Value2, ConstantInt32(Ctx, 1)});
  dyn_cast<GetElementPtrInst>(Value14)->setIsInBounds(true);
  // store i32 %13, ptr %14, align 4, !tbaa !10
  Builder.CreateAlignedStore(Value13, Value14, Align{4});
  // %15 = add nuw nsw i64 %2, 1
  auto *Value15 = Builder.CreateNUWAdd(Value2, ConstantInt64(Ctx, 1));
  // %16 = icmp eq i64 %15, 100
  auto *Value16 = Builder.CreateICmpEQ(Value15, ConstantInt64(Ctx, 100));
  // br i1 %16, label %28, label %1, !llvm.loop !11
  Builder.CreateCondBr(Value16, BB.at(28), BB.at(1));
  // ==========================================================================

  // ================================== BB 25 =================================
  Builder.SetInsertPoint(BB.at(25));

  // %26 = phi i64 [ %32, %23 ], [ 0, %519 ]
  auto *Value26 = Builder.CreatePHI(Type::getInt64Ty(Ctx), 2);
  // %27 = phi i64 [ %24, %23 ], [ 1, %519 ]
  auto *Value27 = Builder.CreatePHI(Type::getInt64Ty(Ctx), 2);
  // br label %28, !llvm.loop !15
  Builder.CreateBr(BB.at(28));
  // ==========================================================================

  // ================================== BB 28 =================================
  Builder.SetInsertPoint(BB.at(28));

  // %29 = phi i64 [ %26, %25 ], [ 0, %1 ]
  auto *Value29 = Builder.CreatePHI(Type::getInt64Ty(Ctx), 2);
  // %30 = phi i64 [ %27, %25 ], [ 1, %1 ]
  auto *Value30 = Builder.CreatePHI(Type::getInt64Ty(Ctx), 2);
  // %31 = sub nsw i64 99, %29
  auto *Value31 = Builder.CreateNSWSub(ConstantInt64(Ctx, 99), Value29);
  // %32 = add nuw nsw i64 %29, 1
  auto *Value32 = Builder.CreateNUWAdd(Value29, ConstantInt64(Ctx, 1));
  // %33 = icmp eq i64 %32, 100
  auto *Value33 = Builder.CreateICmpEQ(Value32, ConstantInt64(Ctx, 100));
  // br i1 %33, label %77, label %34
  Builder.CreateCondBr(Value33, BB.at(77), BB.at(34));
  // ==========================================================================

  // ================================== BB 23 =================================
  Builder.SetInsertPoint(BB.at(23));

  // %24 = add nuw nsw i64 %30, 1
  Builder.CreateNUWAdd(Value30, ConstantInt64(Ctx, 1));
  // br i1 %33, label %116, label %25
  Builder.CreateCondBr(Value33, BB.at(116), BB.at(25));
  // ==========================================================================

  // ================================== BB 34 =================================
  Builder.SetInsertPoint(BB.at(34));

  // %35 = getelementptr inbounds %struct.point_s, ptr @generate_points.points, i64 %29
  auto *Value35 =
      Builder.CreateGEP(Types.at("struct.point_s"),
                        M->getNamedValue("generate_points.points"), {Value29});
  dyn_cast<GetElementPtrInst>(Value35)->setIsInBounds(true);
  // %36 = load i32, ptr %35, align 8, !tbaa !5
  auto *Value36 = Builder.CreateAlignedLoad(Int32Ty, Value35, Align{8});
  // %37 = getelementptr i8, ptr %35, i64 4
  auto *Value37 =
      Builder.CreateGEP(Type::getInt8Ty(Ctx), Value35, {ConstantInt64(Ctx, 4)});
  // %38 = load i32, ptr %37, align 4, !tbaa !10
  auto *Value38 = Builder.CreateAlignedLoad(Int32Ty, Value37, Align{4});
  // %39 = icmp ult i64 %31, 4
  auto *Value39 = Builder.CreateICmpULT(Value31, ConstantInt64(Ctx, 4));
  // br i1 %39, label %73, label %40
  Builder.CreateCondBr(Value39, BB.at(73), BB.at(40));
  // ==========================================================================

  // ================================== BB 40 =================================
  Builder.SetInsertPoint(BB.at(40));
  auto *BB40VecTy = FixedVectorType::get(Int32Ty, 4);

  // %41 = and i64 %31, -4
  auto *Value41 = Builder.CreateAnd(Value31, ConstantInt64(Ctx, -4));
  // %42 = add i64 %30, %41
  auto *Value42 = Builder.CreateAdd(Value30, Value41);
  // %43 = insertelement <4 x i32> poison, i32 %36, i64 0
  auto *Value43 = Builder.CreateInsertElement(PoisonValue::get(BB40VecTy),
                                              Value36, uint64_t{0});
  // %44 = shufflevector <4 x i32> %43, <4 x i32> poison, <4 x i32> zeroinitializer
  auto *Value44 = Builder.CreateShuffleVector(
      Value43, PoisonValue::get(BB40VecTy), Constant::getNullValue(BB40VecTy));
  // %45 = insertelement <4 x i32> poison, i32 %38, i64 0
  auto *Value45 = Builder.CreateInsertElement(PoisonValue::get(BB40VecTy),
                                              Value38, uint64_t{0});
  // %46 = shufflevector <4 x i32> %45, <4 x i32> poison, <4 x i32> zeroinitializer
  auto *Value46 = Builder.CreateShuffleVector(
      Value45, PoisonValue::get(BB40VecTy), Constant::getNullValue(BB40VecTy));
  // br label %47
  Builder.CreateBr(BB.at(47));
  // ==========================================================================

  // ================================== BB 47 =================================
  Builder.SetInsertPoint(BB.at(47));
  auto *BB47VecTy = FixedVectorType::get(Int32Ty, 8);

  // %48 = phi i64 [ 0, %40 ], [ %69, %47 ]
  auto *Value48 = Builder.CreatePHI(Type::getInt64Ty(Ctx), 2);
  // %49 = add i64 %30, %48
  auto *Value49 = Builder.CreateAdd(Value30, Value48);
  // %50 = add nsw i64 %49, -1
  auto *Value50 = Builder.CreateNSWAdd(Value49, ConstantInt64(Ctx, -1));
  // %51 = getelementptr inbounds %struct.point_s, ptr @generate_points.points, i64 %49
  auto *Value51 =
      Builder.CreateGEP(Types.at("struct.point_s"),
                        M->getNamedValue("generate_points.points"), {Value49});
  // %52 = load <8 x i32>, ptr %51, align 8, !tbaa !14
  auto *Value52 = Builder.CreateAlignedLoad(BB47VecTy, Value51, Align{8});
  // %53 = shufflevector <8 x i32> %52, <8 x i32> poison, <4 x i32> <i32 0, i32 2, i32 4, i32 6>
  auto *Value53 = Builder.CreateShuffleVector(
      Value52, PoisonValue::get(BB47VecTy), {0, 2, 4, 6});
  // %54 = shufflevector <8 x i32> %52, <8 x i32> poison, <4 x i32> <i32 1, i32 3, i32 5, i32 7>
  auto *Value54 = Builder.CreateShuffleVector(
      Value52, PoisonValue::get(BB47VecTy), {1, 3, 5, 7});
  // %55 = add nsw <4 x i32> %53, %44
  auto *Value55 = Builder.CreateNSWAdd(Value53, Value44);
  // %56 = sdiv <4 x i32> %55, <i32 -2, i32 -2, i32 -2, i32 -2>
  auto *MinusTwo = ConstantInt32(Ctx, -2);
  auto *Value56 = Builder.CreateSDiv(
      Value55, ConstantVector::get({MinusTwo, MinusTwo, MinusTwo, MinusTwo}));
  // %57 = add nsw <4 x i32> %54, %46
  auto *Value57 = Builder.CreateNSWAdd(Value54, Value46);
  // %58 = sdiv <4 x i32> %57, <i32 2, i32 2, i32 2, i32 2>
  auto *PlusTwo = ConstantInt32(Ctx, -2);
  auto *Value58 = Builder.CreateSDiv(
      Value57, ConstantVector::get({PlusTwo, PlusTwo, PlusTwo, PlusTwo}));
  // %59 = sub nsw <4 x i32> %46, %54
  auto *Value59 = Builder.CreateNSWSub(Value46, Value54);
  // %60 = sub nsw <4 x i32> %53, %44
  auto *Value60 = Builder.CreateNSWSub(Value53, Value44);
  // %61 = sub nsw <4 x i32> zeroinitializer, %59
  auto *Value61 = Builder.CreateNSWSub(
      Constant::getNullValue(FixedVectorType::get(Int32Ty, 4)), Value59);
  // %62 = mul nsw <4 x i32> %58, %59
  auto *Value62 = Builder.CreateNSWMul(Value58, Value59);
  // %63 = mul <4 x i32> %56, %60
  auto *Value63 = Builder.CreateNSWMul(Value56, Value60);
  // %64 = add <4 x i32> %62, %63
  auto *Value64 = Builder.CreateAdd(Value62, Value63);
  // %65 = getelementptr inbounds [100 x [99 x %struct.line_s]], ptr @compute_normals.perps, i64 0, i64 %29, i64 %50
  auto *Value65 = Builder.CreateGEP(
      ArrayType::get(ArrayType::get(Types.at("struct.line_s"), 99), 100),
      M->getNamedValue("compute_normals.perps"),
      {ConstantInt64(Ctx, 0), Value29, Value50});
  dyn_cast<GetElementPtrInst>(Value65)->setIsInBounds(true);
  // %66 = shufflevector <4 x i32> %60, <4 x i32> %61, <8 x i32> <i32 0, i32 1, i32 2, i32 3, i32 4, i32 5, i32 6, i32 7>
  auto *Value66 =
      Builder.CreateShuffleVector(Value60, Value61, {0, 1, 2, 3, 4, 5, 6, 7});
  // %67 = shufflevector <4 x i32> %64, <4 x i32> poison, <8 x i32> <i32 0, i32 1, i32 2, i32 3, i32 poison, i32 poison, i32 poison, i32 poison>
  auto *Value67 = Builder.CreateShuffleVector(
      Value64, PoisonValue::get(FixedVectorType::get(Int32Ty, 4)),
      ConstantVector::get({ConstantInt32(Ctx, 0), ConstantInt32(Ctx, 1),
                           ConstantInt32(Ctx, 2), ConstantInt32(Ctx, 3),
                           PoisonValue::get(Int32Ty), PoisonValue::get(Int32Ty),
                           PoisonValue::get(Int32Ty),
                           PoisonValue::get(Int32Ty)}));
  // %68 = shufflevector <8 x i32> %66, <8 x i32> %67, <12 x i32> <i32 0, i32 4, i32 8, i32 1, i32 5, i32 9, i32 2, i32 6, i32 10, i32 3, i32 7, i32 11>
  auto *Value68 = Builder.CreateShuffleVector(
      Value66, Value67, {0, 4, 8, 1, 5, 9, 2, 6, 10, 3, 7, 11});
  // store <12 x i32> %68, ptr %65, align 4, !tbaa !14
  Builder.CreateAlignedStore(Value68, Value65, Align{4});
  // %69 = add nuw i64 %48, 4
  auto *Value69 = Builder.CreateNSWAdd(Value48, ConstantInt64(Ctx, 4));
  // %70 = icmp eq i64 %69, %41
  auto *Value70 = Builder.CreateICmpEQ(Value69, Value41);
  // br i1 %70, label %71, label %47, !llvm.loop !16
  Builder.CreateCondBr(Value70, BB.at(71), BB.at(47));
  // ==========================================================================

  // ================================== BB 71 =================================
  Builder.SetInsertPoint(BB.at(71));

  // %72 = icmp eq i64 %31, %41
  auto *Value72 = Builder.CreateICmpEQ(Value31, Value41);
  // br i1 %72, label %75, label %73
  Builder.CreateCondBr(Value72, BB.at(75), BB.at(73));
  // ==========================================================================

  // ================================== BB 73 =================================
  Builder.SetInsertPoint(BB.at(73));

  // %74 = phi i64 [ %30, %34 ], [ %42, %71 ]
  auto *Value74 = Builder.CreatePHI(Type::getInt64Ty(Ctx), 2);
  // br label %83
  Builder.CreateBr(BB.at(83));
  // ==========================================================================

  // ================================== BB 75 =================================
  Builder.SetInsertPoint(BB.at(75));

  // %76 = icmp eq i64 %29, 0
  auto *Value76 = Builder.CreateICmpEQ(Value29, ConstantInt64(Ctx, 0));
  // br i1 %76, label %23, label %77
  Builder.CreateCondBr(Value76, BB.at(23), BB.at(77));
  // ==========================================================================

  // ================================== BB 77 =================================
  Builder.SetInsertPoint(BB.at(77));

  // %78 = add nsw i64 %29, -1
  auto *Value78 = Builder.CreateNSWAdd(Value29, ConstantInt64(Ctx, -1));
  // %79 = and i64 %29, 1
  auto *Value79 = Builder.CreateAnd(Value29, ConstantInt64(Ctx, 1));
  // %80 = icmp eq i64 %29, 1
  auto *Value80 = Builder.CreateICmpEQ(Value29, ConstantInt64(Ctx, 1));
  // br i1 %80, label %17, label %81
  Builder.CreateCondBr(Value80, BB[17], BB[81]);
  // ==========================================================================

  // ================================== BB 17 =================================
  Builder.SetInsertPoint(BB.at(17));

  // %18 = phi i64 [ 0, %77 ], [ %113, %105 ]
  auto *Value18 = Builder.CreatePHI(Type::getInt64Ty(Ctx), 2);
  // %19 = icmp eq i64 %79, 0
  auto *Value19 = Builder.CreateICmpEQ(Value79, ConstantInt64(Ctx, 100));
  // br i1 %19, label %23, label %20
  Builder.CreateCondBr(Value19, BB.at(23), BB.at(20));
  // ==========================================================================

  // ================================== BB 20 =================================
  Builder.SetInsertPoint(BB.at(20));
  auto *BB20GEPTy =
      ArrayType::get(ArrayType::get(Types.at("struct.line_s"), 99), 100);

  // %21 = getelementptr inbounds [100 x [99 x %struct.line_s]], ptr @compute_normals.perps, i64 0, i64 %29, i64 %18
  auto *Value21 =
      Builder.CreateGEP(BB20GEPTy, M->getNamedValue("compute_normals.perps"),
                        {ConstantInt64(Ctx, 0), Value29, Value18});
  dyn_cast<GetElementPtrInst>(Value21)->setIsInBounds(true);
  // %22 = getelementptr inbounds [100 x [99 x %struct.line_s]], ptr @compute_normals.perps, i64 0, i64 %18, i64 %78
  auto *Value22 =
      Builder.CreateGEP(BB20GEPTy, M->getNamedValue("compute_normals.perps"),
                        {ConstantInt64(Ctx, 0), Value18, Value78});
  dyn_cast<GetElementPtrInst>(Value22)->setIsInBounds(true);
  // tail call void @llvm.memcpy.p0.p0.i64(ptr noundef nonnull align 4 dereferenceable(12) %21, ptr noundef nonnull align 4 dereferenceable(12) %22, i64 12, i1 false), !tbaa.struct !13
  auto *BB20TailCall = Builder.CreateCall(
      Memcpy->getFunctionType(), Memcpy,
      {Value21, Value22, ConstantInt64(Ctx, 12), ConstantInt::getFalse(Ctx)});
  BB20TailCall->setTailCall(true);
  // br label %23
  Builder.CreateBr(BB.at(23));
  // ==========================================================================

  M->print(outs(), nullptr);

  return 0;
}
