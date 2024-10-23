#include <array>
#include <cassert>
#include <llvm-18/llvm/IR/InstrTypes.h>
#include <llvm-18/llvm/IR/Instruction.h>
#include <llvm-18/llvm/IR/Instructions.h>
#include <llvm-18/llvm/Support/Alignment.h>
#include <memory>
#include <string_view>
#include <unordered_map>

#include "llvm/ADT/ArrayRef.h"
#include "llvm/IR/Attributes.h"
#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/DerivedTypes.h"
#include "llvm/IR/Function.h"
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

ConstantInt *ConstantInt32(LLVMContext &Ctx, int32_t X) {
  return ConstantInt::getSigned(Type::getInt32Ty(Ctx), X);
}

ConstantInt *ConstantInt64(LLVMContext &Ctx, int64_t X) {
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
  Points->setAlignment(MaybeAlign{16});
  Points->setLinkage(GlobalValue::InternalLinkage);
  Points->setUnnamedAddr(GlobalValue::UnnamedAddr::Global);
  Points->setInitializer(Constant::getNullValue(Points->getValueType()));

  // @get_cells.cells = internal unnamed_addr global [100 x %struct.polygon_s] zeroinitializer, align 16
  auto *Cells = dyn_cast<GlobalVariable>(
      M->getOrInsertGlobal("get_cells.cells", ArrayType::get(PolygonSTy, 100)));
  assert(Cells);
  Cells->setAlignment(MaybeAlign{16});
  Cells->setLinkage(GlobalValue::InternalLinkage);
  Cells->setUnnamedAddr(GlobalValue::UnnamedAddr::Global);
  Cells->setInitializer(Constant::getNullValue(Cells->getValueType()));

  // @center = internal unnamed_addr global %struct.point_s zeroinitializer, align 8
  auto *Center =
      dyn_cast<GlobalVariable>(M->getOrInsertGlobal("center", PointSTy));
  assert(Center);
  Center->setAlignment(MaybeAlign{8});
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
  Intersections->setAlignment(MaybeAlign{16});
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
  Perps->setAlignment(MaybeAlign{16});
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

//     327, 337,
//     339, 341, 351, 355, 357, 367, 369, 374, 410, 413, 418, 422, 423,
//     431, 432, 439, 441, 442, 464, 471, 474, 476, 497, 503, 507, 519

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
  auto *Int64Ty = Type::getInt64Ty(Ctx);
  auto *Int32Vec4 = FixedVectorType::get(Int32Ty, 4);
  auto *Int32Vec8 = FixedVectorType::get(Int32Ty, 8);
  auto *PerpsArrTy = M->getNamedValue("compute_normals.perps")->getType();
  auto *CellsTy = M->getNamedValue("get_cells.cells")->getType();
  auto *IntersectionsTy =
      M->getNamedValue("get_cells.intersections")->getType();
  IRBuilder<> Builder{Ctx};

  BBMap BB;
  for (auto I : BBIds) {
    BB.emplace(I, BasicBlock::Create(Ctx, "", Lloyd));
  }

  // ================================== BB 0 ==================================
  Builder.SetInsertPoint(BB.at(0));

  // br label %1
  Builder.CreateBr(BB.at(1));
  // ==========================================================================

  // ================================== BB 1 ==================================
  Builder.SetInsertPoint(BB.at(1));

  // %2 = phi i64 [ 0, %0 ], [ %15, %1 ]
  auto *Value2 = Builder.CreatePHI(Int64Ty, 2);
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
      Builder.CreateInBoundsGEP(ArrayType::get(Types.at("struct.point_s"), 100),
                                M->getNamedValue("generate_points.points"),
                                {ConstantInt64(Ctx, 0), Value2});
  // store i32 %7, ptr %8, align 8, !tbaa !5
  Builder.CreateAlignedStore(Value7, Value8, MaybeAlign{8});
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
  auto *Value14 = Builder.CreateInBoundsGEP(
      ArrayType::get(Types.at("struct.point_s"), 100),
      M->getNamedValue("generate_points.points"),
      {ConstantInt64(Ctx, 0), Value2, ConstantInt32(Ctx, 1)});
  // store i32 %13, ptr %14, align 4, !tbaa !10
  Builder.CreateAlignedStore(Value13, Value14, MaybeAlign{4});
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
  auto *Value26 = Builder.CreatePHI(Int64Ty, 2);
  // %27 = phi i64 [ %24, %23 ], [ 1, %519 ]
  auto *Value27 = Builder.CreatePHI(Int64Ty, 2);
  // br label %28, !llvm.loop !15
  Builder.CreateBr(BB.at(28));
  // ==========================================================================

  // ================================== BB 28 =================================
  Builder.SetInsertPoint(BB.at(28));

  // %29 = phi i64 [ %26, %25 ], [ 0, %1 ]
  auto *Value29 = Builder.CreatePHI(Int64Ty, 2);
  // %30 = phi i64 [ %27, %25 ], [ 1, %1 ]
  auto *Value30 = Builder.CreatePHI(Int64Ty, 2);
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
  auto *Value35 = Builder.CreateInBoundsGEP(
      Types.at("struct.point_s"), M->getNamedValue("generate_points.points"),
      {Value29});
  // %36 = load i32, ptr %35, align 8, !tbaa !5
  auto *Value36 = Builder.CreateAlignedLoad(Int32Ty, Value35, MaybeAlign{8});
  // %37 = getelementptr i8, ptr %35, i64 4
  auto *Value37 =
      Builder.CreateGEP(Type::getInt8Ty(Ctx), Value35, {ConstantInt64(Ctx, 4)});
  // %38 = load i32, ptr %37, align 4, !tbaa !10
  auto *Value38 = Builder.CreateAlignedLoad(Int32Ty, Value37, MaybeAlign{4});
  // %39 = icmp ult i64 %31, 4
  auto *Value39 = Builder.CreateICmpULT(Value31, ConstantInt64(Ctx, 4));
  // br i1 %39, label %73, label %40
  Builder.CreateCondBr(Value39, BB.at(73), BB.at(40));
  // ==========================================================================

  // ================================== BB 40 =================================
  Builder.SetInsertPoint(BB.at(40));

  // %41 = and i64 %31, -4
  auto *Value41 = Builder.CreateAnd(Value31, ConstantInt64(Ctx, -4));
  // %42 = add i64 %30, %41
  auto *Value42 = Builder.CreateAdd(Value30, Value41);
  // %43 = insertelement <4 x i32> poison, i32 %36, i64 0
  auto *Value43 = Builder.CreateInsertElement(PoisonValue::get(Int32Vec4),
                                              Value36, uint64_t{0});
  // %44 = shufflevector <4 x i32> %43, <4 x i32> poison, <4 x i32> zeroinitializer
  auto *Value44 = Builder.CreateShuffleVector(
      Value43, PoisonValue::get(Int32Vec4), Constant::getNullValue(Int32Vec4));
  // %45 = insertelement <4 x i32> poison, i32 %38, i64 0
  auto *Value45 = Builder.CreateInsertElement(PoisonValue::get(Int32Vec4),
                                              Value38, uint64_t{0});
  // %46 = shufflevector <4 x i32> %45, <4 x i32> poison, <4 x i32> zeroinitializer
  auto *Value46 = Builder.CreateShuffleVector(
      Value45, PoisonValue::get(Int32Vec4), Constant::getNullValue(Int32Vec4));
  // br label %47
  Builder.CreateBr(BB.at(47));
  // ==========================================================================

  // ================================== BB 47 =================================
  Builder.SetInsertPoint(BB.at(47));

  // %48 = phi i64 [ 0, %40 ], [ %69, %47 ]
  auto *Value48 = Builder.CreatePHI(Int64Ty, 2);
  // %49 = add i64 %30, %48
  auto *Value49 = Builder.CreateAdd(Value30, Value48);
  // %50 = add nsw i64 %49, -1
  auto *Value50 = Builder.CreateNSWAdd(Value49, ConstantInt64(Ctx, -1));
  // %51 = getelementptr inbounds %struct.point_s, ptr @generate_points.points, i64 %49
  auto *Value51 =
      Builder.CreateGEP(Types.at("struct.point_s"),
                        M->getNamedValue("generate_points.points"), {Value49});
  // %52 = load <8 x i32>, ptr %51, align 8, !tbaa !14
  auto *Value52 = Builder.CreateAlignedLoad(Int32Vec8, Value51, MaybeAlign{8});
  // %53 = shufflevector <8 x i32> %52, <8 x i32> poison, <4 x i32> <i32 0, i32 2, i32 4, i32 6>
  auto *Value53 = Builder.CreateShuffleVector(
      Value52, PoisonValue::get(Int32Vec8), {0, 2, 4, 6});
  // %54 = shufflevector <8 x i32> %52, <8 x i32> poison, <4 x i32> <i32 1, i32 3, i32 5, i32 7>
  auto *Value54 = Builder.CreateShuffleVector(
      Value52, PoisonValue::get(Int32Vec8), {1, 3, 5, 7});
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
  auto *Value61 =
      Builder.CreateNSWSub(Constant::getNullValue(Int32Vec4), Value59);
  // %62 = mul nsw <4 x i32> %58, %59
  auto *Value62 = Builder.CreateNSWMul(Value58, Value59);
  // %63 = mul <4 x i32> %56, %60
  auto *Value63 = Builder.CreateNSWMul(Value56, Value60);
  // %64 = add <4 x i32> %62, %63
  auto *Value64 = Builder.CreateAdd(Value62, Value63);
  // %65 = getelementptr inbounds [100 x [99 x %struct.line_s]], ptr @compute_normals.perps, i64 0, i64 %29, i64 %50
  auto *Value65 = Builder.CreateInBoundsGEP(
      PerpsArrTy, M->getNamedValue("compute_normals.perps"),
      {ConstantInt64(Ctx, 0), Value29, Value50});
  // %66 = shufflevector <4 x i32> %60, <4 x i32> %61, <8 x i32> <i32 0, i32 1, i32 2, i32 3, i32 4, i32 5, i32 6, i32 7>
  auto *Value66 =
      Builder.CreateShuffleVector(Value60, Value61, {0, 1, 2, 3, 4, 5, 6, 7});
  // %67 = shufflevector <4 x i32> %64, <4 x i32> poison, <8 x i32> <i32 0, i32 1, i32 2, i32 3, i32 poison, i32 poison, i32 poison, i32 poison>
  auto *Value67 = Builder.CreateShuffleVector(
      Value64, PoisonValue::get(Int32Vec4),
      ConstantVector::get({ConstantInt32(Ctx, 0), ConstantInt32(Ctx, 1),
                           ConstantInt32(Ctx, 2), ConstantInt32(Ctx, 3),
                           PoisonValue::get(Int32Ty), PoisonValue::get(Int32Ty),
                           PoisonValue::get(Int32Ty),
                           PoisonValue::get(Int32Ty)}));
  // %68 = shufflevector <8 x i32> %66, <8 x i32> %67, <12 x i32> <i32 0, i32 4, i32 8, i32 1, i32 5, i32 9, i32 2, i32 6, i32 10, i32 3, i32 7, i32 11>
  auto *Value68 = Builder.CreateShuffleVector(
      Value66, Value67, {0, 4, 8, 1, 5, 9, 2, 6, 10, 3, 7, 11});
  // store <12 x i32> %68, ptr %65, align 4, !tbaa !14
  Builder.CreateAlignedStore(Value68, Value65, MaybeAlign{4});
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
  auto *Value74 = Builder.CreatePHI(Int64Ty, 2);
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
  Builder.CreateCondBr(Value80, BB.at(17), BB.at(81));
  // ==========================================================================

  // ================================== BB 17 =================================
  Builder.SetInsertPoint(BB.at(17));

  // %18 = phi i64 [ 0, %77 ], [ %113, %105 ]
  auto *Value18 = Builder.CreatePHI(Int64Ty, 2);
  // %19 = icmp eq i64 %79, 0
  auto *Value19 = Builder.CreateICmpEQ(Value79, ConstantInt64(Ctx, 100));
  // br i1 %19, label %23, label %20
  Builder.CreateCondBr(Value19, BB.at(23), BB.at(20));
  // ==========================================================================

  // ================================== BB 20 =================================
  Builder.SetInsertPoint(BB.at(20));

  // %21 = getelementptr inbounds [100 x [99 x %struct.line_s]], ptr @compute_normals.perps, i64 0, i64 %29, i64 %18
  auto *Value21 = Builder.CreateInBoundsGEP(
      PerpsArrTy, M->getNamedValue("compute_normals.perps"),
      {ConstantInt64(Ctx, 0), Value29, Value18});
  // %22 = getelementptr inbounds [100 x [99 x %struct.line_s]], ptr @compute_normals.perps, i64 0, i64 %18, i64 %78
  auto *Value22 = Builder.CreateInBoundsGEP(
      PerpsArrTy, M->getNamedValue("compute_normals.perps"),
      {ConstantInt64(Ctx, 0), Value18, Value78});
  // tail call void @llvm.memcpy.p0.p0.i64(ptr noundef nonnull align 4 dereferenceable(12) %21, ptr noundef nonnull align 4 dereferenceable(12) %22, i64 12, i1 false), !tbaa.struct !13
  auto *BB20TailCall = Builder.CreateCall(
      Memcpy->getFunctionType(), Memcpy,
      {Value21, Value22, ConstantInt64(Ctx, 12), ConstantInt::getFalse(Ctx)});
  BB20TailCall->setTailCall(true);
  // br label %23
  Builder.CreateBr(BB.at(23));
  // ==========================================================================

  // ================================== BB 81 =================================
  Builder.SetInsertPoint(BB.at(81));

  // %82 = and i64 %29, -2
  auto *Value82 = Builder.CreateAnd(Value29, ConstantInt64(Ctx, -2));
  // br label %105
  Builder.CreateBr(BB.at(105));
  // ==========================================================================

  // ================================== BB 83 =================================
  Builder.SetInsertPoint(BB.at(83));

  // %84 = phi i64 [ %103, %83 ], [ %74, %73 ]
  auto *Value84 = Builder.CreatePHI(Int64Ty, 2);
  // %85 = add nsw i64 %84, -1
  auto *Value85 = Builder.CreateNSWAdd(Value84, ConstantInt64(Ctx, -1));
  // %86 = getelementptr inbounds [100 x [99 x %struct.line_s]], ptr @compute_normals.perps, i64 0, i64 %29, i64 %85
  auto *Value86 = Builder.CreateInBoundsGEP(
      PerpsArrTy, M->getNamedValue("compute_normals.perps"),
      {ConstantInt64(Ctx, 0), Value29, Value85});
  // %87 = getelementptr inbounds %struct.point_s, ptr @generate_points.points, i64 %84
  auto *Value87 = Builder.CreateInBoundsGEP(
      Types.at("struct.point_s"), M->getNamedValue("generate_points.points"),
      {Value84});
  // %88 = load i32, ptr %87, align 8, !tbaa !5
  auto *Value88 = Builder.CreateAlignedLoad(Int32Ty, Value87, MaybeAlign{8});
  // %89 = getelementptr i8, ptr %87, i64 4
  auto *Value89 =
      Builder.CreateGEP(Type::getInt8Ty(Ctx), Value87, {ConstantInt64(Ctx, 4)});
  // %90 = load i32, ptr %89, align 4, !tbaa !10
  auto *Value90 = Builder.CreateAlignedLoad(Int32Ty, Value89, MaybeAlign{4});
  // %91 = add nsw i32 %88, %36
  auto *Value91 = Builder.CreateNSWAdd(Value88, Value36);
  // %92 = sdiv i32 %91, -2
  auto *Value92 = Builder.CreateSDiv(Value91, ConstantInt32(Ctx, -2));
  // %93 = add nsw i32 %90, %38
  auto *Value93 = Builder.CreateNSWAdd(Value90, Value38);
  // %94 = sdiv i32 %93, 2
  auto *Value94 = Builder.CreateSDiv(Value93, ConstantInt32(Ctx, 2));
  // %95 = sub nsw i32 %38, %90
  auto *Value95 = Builder.CreateNSWSub(Value38, Value90);
  // %96 = sub nsw i32 %88, %36
  auto *Value96 = Builder.CreateNSWSub(Value88, Value36);
  // store i32 %96, ptr %86, align 4, !tbaa !19
  Builder.CreateAlignedStore(Value96, Value86, MaybeAlign{4});
  // %97 = sub nsw i32 0, %95
  auto *Value97 = Builder.CreateNSWSub(ConstantInt32(Ctx, 0), Value95);
  // %98 = getelementptr inbounds [100 x [99 x %struct.line_s]], ptr @compute_normals.perps, i64 0, i64 %29, i64 %85, i32 1
  auto *Value98 = Builder.CreateInBoundsGEP(
      PerpsArrTy, M->getNamedValue("compute_normals.perps"),
      {ConstantInt64(Ctx, 0), Value29, Value85, ConstantInt32(Ctx, 1)});
  // store i32 %97, ptr %98, align 4, !tbaa !21
  Builder.CreateAlignedStore(Value97, Value98, MaybeAlign{4});
  // %99 = mul nsw i32 %94, %95
  auto *Value99 = Builder.CreateNSWMul(Value94, Value95);
  // %100 = mul i32 %92, %96
  auto *Value100 = Builder.CreateMul(Value92, Value96);
  // %101 = add i32 %99, %100
  auto *Value101 = Builder.CreateAdd(Value99, Value100);
  // %102 = getelementptr inbounds [100 x [99 x %struct.line_s]], ptr @compute_normals.perps, i64 0, i64 %29, i64 %85, i32 2
  auto *Value102 = Builder.CreateInBoundsGEP(
      PerpsArrTy, M->getNamedValue("compute_normals.perps"),
      {ConstantInt64(Ctx, 0), Value29, Value85, ConstantInt32(Ctx, 2)});
  // store i32 %101, ptr %102, align 4, !tbaa !22
  Builder.CreateAlignedStore(Value101, Value102, MaybeAlign{4});
  // %103 = add nuw nsw i64 %84, 1
  auto *Value103 = Builder.CreateNUWAdd(Value84, ConstantInt64(Ctx, 1));
  // %104 = icmp eq i64 %103, 100
  auto *Value104 = Builder.CreateICmpEQ(Value103, ConstantInt64(Ctx, 100));
  // br i1 %104, label %75, label %83, !llvm.loop !23
  Builder.CreateCondBr(Value104, BB.at(75), BB.at(83));
  // ==========================================================================

  // ================================= BB 105 =================================
  Builder.SetInsertPoint(BB.at(105));

  // %106 = phi i64 [ 0, %81 ], [ %113, %105 ]
  auto *Value106 = Builder.CreatePHI(Int64Ty, 2);
  // %107 = phi i64 [ 0, %81 ], [ %114, %105 ]
  auto *Value107 = Builder.CreatePHI(Int64Ty, 2);
  // %108 = getelementptr inbounds [100 x [99 x %struct.line_s]], ptr @compute_normals.perps, i64 0, i64 %29, i64 %106
  auto *Value108 = Builder.CreateInBoundsGEP(
      PerpsArrTy, M->getNamedValue("compute_normals.perps"),
      {ConstantInt64(Ctx, 0), Value29, Value106});
  // %109 = getelementptr inbounds [100 x [99 x %struct.line_s]], ptr @compute_normals.perps, i64 0, i64 %106, i64 %78
  auto *Value109 = Builder.CreateInBoundsGEP(
      PerpsArrTy, M->getNamedValue("compute_normals.perps"),
      {ConstantInt64(Ctx, 0), Value106, Value78});
  // tail call void @llvm.memcpy.p0.p0.i64(ptr noundef nonnull align 4 dereferenceable(12) %108, ptr noundef nonnull align 4 dereferenceable(12) %109, i64 12, i1 false), !tbaa.struct !13
  auto *BB105TailCall = Builder.CreateCall(
      Memcpy->getFunctionType(), Memcpy,
      {Value108, Value109, ConstantInt64(Ctx, 12), ConstantInt::getFalse(Ctx)});
  BB105TailCall->setTailCall(true);
  // %110 = or disjoint i64 %106, 1
  auto *Value110 =
      BinaryOperator::CreateDisjointOr(Value106, ConstantInt64(Ctx, 1));
  Builder.Insert(Value110);
  // %111 = getelementptr inbounds [100 x [99 x %struct.line_s]], ptr @compute_normals.perps, i64 0, i64 %29, i64 %110
  auto *Value111 = Builder.CreateInBoundsGEP(
      PerpsArrTy, M->getNamedValue("compute_normals.perps"),
      {ConstantInt64(Ctx, 0), Value29, Value110});
  // %112 = getelementptr inbounds [100 x [99 x %struct.line_s]], ptr @compute_normals.perps, i64 0, i64 %110, i64 %78
  auto *Value112 = Builder.CreateInBoundsGEP(
      PerpsArrTy, M->getNamedValue("compute_normals.perps"),
      {ConstantInt64(Ctx, 0), Value110, Value78});
  // tail call void @llvm.memcpy.p0.p0.i64(ptr noundef nonnull align 4 dereferenceable(12) %111, ptr noundef nonnull align 4 dereferenceable(12) %112, i64 12, i1 false), !tbaa.struct !13
  auto *BB105TailCall2 = Builder.CreateCall(
      Memcpy->getFunctionType(), Memcpy,
      {Value111, Value112, ConstantInt64(Ctx, 12), ConstantInt::getFalse(Ctx)});
  BB105TailCall2->setTailCall(true);
  // %113 = add nuw nsw i64 %106, 2
  auto *Value113 = Builder.CreateNUWAdd(Value106, ConstantInt64(Ctx, 2));
  // %114 = add i64 %107, 2
  auto *Value114 = Builder.CreateAdd(Value107, ConstantInt64(Ctx, 2));
  // %115 = icmp eq i64 %114, %82
  auto *Value115 = Builder.CreateICmpEQ(Value114, Value82);
  // br i1 %115, label %17, label %105, !llvm.loop !24
  Builder.CreateCondBr(Value115, BB.at(17), BB.at(105));
  // ==========================================================================

  // ================================= BB 116 =================================
  Builder.SetInsertPoint(BB.at(116));

  // %117 = phi i64 [ %309, %308 ], [ 0, %23 ]
  auto *Value117 = Builder.CreatePHI(Int64Ty, 2);
  // %118 = mul nuw nsw i64 %117, 99
  auto *Value118 =
      Builder.CreateMul(Value117, ConstantInt64(Ctx, 99), "", true, true);
  // br label %127
  Builder.CreateBr(BB.at(127));
  // ==========================================================================

  // ================================= BB 123 =================================
  Builder.SetInsertPoint(BB.at(123));

  // %124 = getelementptr inbounds %struct.point_s, ptr @generate_points.points, i64 %117
  auto *Value124 = Builder.CreateInBoundsGEP(
      Types.at("struct.point_s"), M->getNamedValue("generate_points.points"),
      {Value117});
  // %125 = getelementptr i8, ptr %124, i64 4
  auto *Value125 = Builder.CreateGEP(Type::getInt8Ty(Ctx), Value124,
                                     {ConstantInt64(Ctx, 4)});
  // %126 = getelementptr inbounds [100 x %struct.polygon_s], ptr @get_cells.cells, i64 0, i64 %117
  auto *Value126 =
      Builder.CreateInBoundsGEP(CellsTy, M->getNamedValue("get_cells.cells"),
                                {ConstantInt64(Ctx, 0), Value117});
  // br label %254
  Builder.CreateBr(BB.at(254));
  // ==========================================================================

  // ================================= BB 127 =================================
  Builder.SetInsertPoint(BB.at(127));

  // %128 = phi i64 [ 0, %116 ], [ %139, %250 ]
  auto *Value128 = Builder.CreatePHI(Int64Ty, 2);
  // %129 = phi i64 [ 1, %116 ], [ %253, %250 ]
  auto *Value129 = Builder.CreatePHI(Int64Ty, 2);
  // %130 = phi i32 [ 4, %116 ], [ %251, %250 ]
  auto *Value130 = Builder.CreatePHI(Int32Ty, 2);
  // %131 = add nuw nsw i64 %128, %118
  auto *Value131 = Builder.CreateAdd(Value128, Value118, "", true, true);
  // %132 = getelementptr inbounds %struct.line_s, ptr @compute_normals.perps, i64 %131
  auto *Value132 = Builder.CreateInBoundsGEP(
      Types.at("struct.line_s"), M->getNamedValue("compute_normals.perps"),
      {Value131});
  // %133 = load i32, ptr %132, align 4, !tbaa !19
  auto *Value133 = Builder.CreateAlignedLoad(Int32Ty, Value132, MaybeAlign{4});
  // %134 = getelementptr inbounds %struct.line_s, ptr @compute_normals.perps, i64 %131, i32 1
  auto *Value134 = Builder.CreateInBoundsGEP(
      Types.at("struct.line_s"), M->getNamedValue("compute_normals.perps"),
      {Value131, ConstantInt32(Ctx, 1)});
  // %135 = load i32, ptr %134, align 4, !tbaa !21
  auto *Value135 = Builder.CreateAlignedLoad(Int32Ty, Value134, MaybeAlign{4});
  // %136 = getelementptr inbounds %struct.line_s, ptr @compute_normals.perps, i64 %131, i32 2
  auto *Value136 = Builder.CreateInBoundsGEP(
      Types.at("struct.line_s"), M->getNamedValue("compute_normals.perps"),
      {Value131, ConstantInt32(Ctx, 2)});
  // %137 = trunc i64 %128 to i32
  auto *Value137 = Builder.CreateTrunc(Value128, Int32Ty);
  // br label %141
  Builder.CreateBr(BB.at(141));
  // ==========================================================================

  // ================================= BB 138 =================================
  Builder.SetInsertPoint(BB.at(138));

  // %139 = add nuw nsw i64 %128, 1
  auto *Value139 =
      Builder.CreateAdd(Value128, ConstantInt64(Ctx, 1), "", true, true);
  // %140 = icmp eq i32 %133, 0
  auto *Value140 = Builder.CreateICmpEQ(Value133, ConstantInt32(Ctx, 0));
  // br i1 %140, label %197, label %185
  Builder.CreateCondBr(Value140, BB.at(197), BB.at(185));
  // ==========================================================================

  // ================================= BB 141 =================================
  Builder.SetInsertPoint(BB.at(141));

  // %142 = phi i64 [ %129, %127 ], [ %183, %181 ]
  auto *Value142 = Builder.CreatePHI(Int64Ty, 2);
  // %143 = phi i32 [ %130, %127 ], [ %182, %181 ]
  auto *Value143 = Builder.CreatePHI(Int32Ty, 2);
  // %144 = add nuw nsw i64 %142, %118
  auto *Value144 = Builder.CreateAdd(Value142, Value118, "", true, true);
  // %145 = getelementptr inbounds %struct.line_s, ptr @compute_normals.perps, i64 %144
  auto *Value145 = Builder.CreateInBoundsGEP(
      Types.at("struct.line_s"), M->getNamedValue("compute_normals.perps"),
      {Value144});
  // %146 = getelementptr inbounds %struct.line_s, ptr @compute_normals.perps, i64 %144, i32 1
  auto *Value146 = Builder.CreateInBoundsGEP(
      Types.at("struct.line_s"), M->getNamedValue("compute_normals.perps"),
      {Value144, ConstantInt32(Ctx, 1)});
  // %147 = load i32, ptr %146, align 4, !tbaa !21
  auto *Value147 = Builder.CreateAlignedLoad(Int32Ty, Value146, MaybeAlign{4});
  // %148 = mul nsw i32 %147, %133
  auto *Value148 = Builder.CreateNSWMul(Value147, Value133);
  // %149 = load i32, ptr %145, align 4, !tbaa !19
  auto *Value149 = Builder.CreateAlignedLoad(Int32Ty, Value145, MaybeAlign{4});
  // %150 = mul nsw i32 %149, %135
  auto *Value150 = Builder.CreateNSWMul(Value149, Value135);
  // %151 = icmp eq i32 %148, %150
  auto *Value151 = Builder.CreateICmpEQ(Value148, Value150);
  // br i1 %151, label %181, label %152
  Builder.CreateCondBr(Value151, BB.at(181), BB.at(152));
  // ==========================================================================

  // ================================= BB 152 =================================
  Builder.SetInsertPoint(BB.at(152));

  // %153 = sub nsw i32 %148, %150
  auto *Value153 = Builder.CreateNSWSub(Value148, Value150);
  // %154 = getelementptr inbounds %struct.line_s, ptr @compute_normals.perps, i64 %144, i32 2
  auto *Value154 = Builder.CreateInBoundsGEP(
      Types.at("struct.line_s"), M->getNamedValue("compute_normals.perps"),
      {Value144, ConstantInt32(Ctx, 2)});
  // %155 = load i32, ptr %154, align 4, !tbaa !22
  auto *Value155 = Builder.CreateAlignedLoad(Int32Ty, Value154, MaybeAlign{4});
  // %156 = mul nsw i32 %155, %135
  auto *Value156 = Builder.CreateNSWMul(Value155, Value135);
  // %157 = load i32, ptr %136, align 4, !tbaa !22
  auto *Value157 = Builder.CreateAlignedLoad(Int32Ty, Value136, MaybeAlign{4});
  // %158 = mul nsw i32 %157, %147
  auto *Value158 = Builder.CreateNSWMul(Value157, Value147);
  // %159 = sub nsw i32 %156, %158
  auto *Value159 = Builder.CreateNSWSub(Value156, Value158);
  // %160 = sdiv i32 %159, %153
  auto *Value160 = Builder.CreateSDiv(Value159, Value153);
  // %161 = mul nsw i32 %157, %149
  auto *Value161 = Builder.CreateNSWMul(Value157, Value149);
  // %162 = mul nsw i32 %155, %133
  auto *Value162 = Builder.CreateNSWMul(Value155, Value133);
  // %163 = sub nsw i32 %161, %162
  auto *Value163 = Builder.CreateNSWSub(Value161, Value162);
  // %164 = sdiv i32 %163, %153
  auto *Value164 = Builder.CreateSDiv(Value163, Value153);
  // %165 = add i32 %160, -720
  auto *Value165 = Builder.CreateAdd(Value160, ConstantInt32(Ctx, -720));
  // %166 = icmp ult i32 %165, -719
  auto *Value166 = Builder.CreateICmpULT(Value165, ConstantInt32(Ctx, -719));
  // %167 = add i32 %164, -720
  auto *Value167 = Builder.CreateAdd(Value164, ConstantInt32(Ctx, -720));
  // %168 = icmp ult i32 %167, -719
  auto *Value168 = Builder.CreateICmpULT(Value167, ConstantInt32(Ctx, -719));
  // %169 = select i1 %166, i1 true, i1 %168
  auto *Value169 =
      Builder.CreateSelect(Value166, ConstantInt::getTrue(Ctx), Value168);
  // br i1 %169, label %181, label %170
  Builder.CreateCondBr(Value169, BB.at(181), BB.at(170));
  // ==========================================================================

  // ================================= BB 170 =================================
  Builder.SetInsertPoint(BB.at(170));

  // %171 = sext i32 %143 to i64
  auto *Value171 = Builder.CreateSExt(Value143, Int64Ty);
  // %172 = getelementptr inbounds [5102 x %struct.tagged_point_s], ptr @get_cells.intersections, i64 0, i64 %171
  auto *Value172 = Builder.CreateInBoundsGEP(
      IntersectionsTy, M->getNamedValue("get_cells.intersections"),
      {ConstantInt64(Ctx, 0), Value171});
  // %173 = zext nneg i32 %164 to i64
  auto *Value173 = Builder.CreateZExt(Value164, Int64Ty);
  dyn_cast<Instruction>(Value173)->setNonNeg(true);
  // %174 = shl nuw nsw i64 %173, 32
  auto *Value174 = Builder.CreateShl(Value173, uint64_t{32}, "", true, true);
  // %175 = zext nneg i32 %160 to i64
  auto *Value175 = Builder.CreateZExt(Value160, Int64Ty);
  dyn_cast<Instruction>(Value175)->setNonNeg(true);
  // %176 = or disjoint i64 %174, %175
  auto *Value176 = BinaryOperator::CreateDisjointOr(Value174, Value175);
  Builder.Insert(Value176);
  // Builder.CreateOr(Value174, Value175);
  // store i64 %176, ptr %172, align 16
  Builder.CreateAlignedStore(Value176, Value172, MaybeAlign{16});
  // %177 = getelementptr inbounds [5102 x %struct.tagged_point_s], ptr @get_cells.intersections, i64 0, i64 %171, i32 1
  auto *Value177 = Builder.CreateInBoundsGEP(
      IntersectionsTy, M->getNamedValue("get_cells.intersections"),
      {ConstantInt64(Ctx, 0), Value171, ConstantInt32(Ctx, 1)});
  // store i32 %137, ptr %177, align 8, !tbaa !25
  Builder.CreateAlignedStore(Value137, Value177, MaybeAlign{8});
  // %178 = add nsw i32 %143, 1
  auto *Value178 = Builder.CreateNSWAdd(Value143, ConstantInt32(Ctx, 1));
  // %179 = getelementptr inbounds [5102 x %struct.tagged_point_s], ptr @get_cells.intersections, i64 0, i64 %171, i32 2
  auto *Value179 = Builder.CreateInBoundsGEP(
      IntersectionsTy, M->getNamedValue("get_cells.intersections"),
      {ConstantInt64(Ctx, 0), Value171, ConstantInt32(Ctx, 2)});
  // %180 = trunc i64 %142 to i32
  auto *Value180 = Builder.CreateTrunc(Value142, Int32Ty);
  // store i32 %180, ptr %179, align 4, !tbaa !27
  Builder.CreateAlignedStore(Value180, Value179, MaybeAlign{4});
  // br label %181
  Builder.CreateBr(BB.at(181));
  // ==========================================================================

  // ================================= BB 181 =================================
  Builder.SetInsertPoint(BB.at(181));

  // %182 = phi i32 [ %178, %170 ], [ %143, %152 ], [ %143, %141 ]
  auto *Value182 = Builder.CreatePHI(Int32Ty, 3);
  // %183 = add nuw nsw i64 %142, 1
  auto *Value183 =
      Builder.CreateAdd(Value142, ConstantInt64(Ctx, 1), "", true, true);
  // %184 = icmp eq i64 %183, 99
  auto *Value184 = Builder.CreateICmpEQ(Value183, ConstantInt64(Ctx, 99));
  // br i1 %184, label %138, label %141, !llvm.loop !28
  Builder.CreateCondBr(Value184, BB.at(138), BB.at(141));
  // ==========================================================================

  // ================================= BB 185 =================================
  Builder.SetInsertPoint(BB.at(185));

  // %186 = load i32, ptr %136, align 4, !tbaa !22
  auto *Value186 = Builder.CreateAlignedLoad(Int32Ty, Value136, MaybeAlign{4});
  // %187 = sdiv i32 %186, %133
  auto *Value187 = Builder.CreateSDiv(Value186, Value133);
  // %188 = icmp ult i32 %187, -719
  auto *Value188 = Builder.CreateICmpULT(Value187, ConstantInt32(Ctx, -719));
  // br i1 %188, label %197, label %189
  Builder.CreateCondBr(Value188, BB.at(197), BB.at(189));
  // ==========================================================================

  // ================================= BB 189 =================================
  Builder.SetInsertPoint(BB.at(189));

  // %190 = sub nsw i32 0, %187
  auto *Value190 = Builder.CreateNSWSub(ConstantInt32(Ctx, 0), Value187);
  // %191 = sext i32 %182 to i64
  auto *Value191 = Builder.CreateSExt(Value182, Int64Ty);
  // %192 = getelementptr inbounds [5102 x %struct.tagged_point_s], ptr @get_cells.intersections, i64 0, i64 %191
  auto *Value192 = Builder.CreateInBoundsGEP(
      IntersectionsTy, M->getNamedValue("get_cells.intersections"),
      {ConstantInt64(Ctx, 0), Value191});
  // %193 = zext nneg i32 %190 to i64
  auto *Value193 = Builder.CreateZExt(Value190, Int64Ty);
  dyn_cast<Instruction>(Value193)->setNonNeg(true);
  // store i64 %193, ptr %192, align 16
  Builder.CreateAlignedStore(Value193, Value192, MaybeAlign{16});
  // %194 = getelementptr inbounds [5102 x %struct.tagged_point_s], ptr @get_cells.intersections, i64 0, i64 %191, i32 1
  auto *Value194 = Builder.CreateInBoundsGEP(
      IntersectionsTy, M->getNamedValue("get_cells.intersections"),
      {ConstantInt64(Ctx, 0), Value191, ConstantInt32(Ctx, 1)});
  // store i32 %137, ptr %194, align 8, !tbaa !25
  Builder.CreateAlignedStore(Value137, Value194, MaybeAlign{8});
  // %195 = add nsw i32 %182, 1
  auto *Value195 = Builder.CreateNSWAdd(Value182, ConstantInt32(Ctx, 1));
  // %196 = getelementptr inbounds [5102 x %struct.tagged_point_s], ptr @get_cells.intersections, i64 0, i64 %191, i32 2
  auto *Value196 = Builder.CreateInBoundsGEP(
      IntersectionsTy, M->getNamedValue("get_cells.intersections"),
      {ConstantInt64(Ctx, 0), Value191, ConstantInt32(Ctx, 2)});
  // store i32 -4, ptr %196, align 4, !tbaa !27
  Builder.CreateAlignedStore(ConstantInt32(Ctx, -4), Value196, MaybeAlign{4});
  // br label %197
  Builder.CreateBr(BB.at(197));
  // ==========================================================================

  // ================================= BB 197 =================================
  Builder.SetInsertPoint(BB.at(197));

  // %198 = phi i32 [ %195, %189 ], [ %182, %185 ], [ %182, %138 ]
  auto *Value198 = Builder.CreatePHI(Int32Ty, 3);
  // %199 = icmp eq i32 %135, 0
  auto *Value199 = Builder.CreateICmpEQ(Value135, ConstantInt32(Ctx, 0));
  // br i1 %199, label %233, label %200
  Builder.CreateCondBr(Value199, BB.at(233), BB.at(200));
  // ==========================================================================

  // ================================= BB 200 =================================
  Builder.SetInsertPoint(BB.at(200));

  // %201 = sub nsw i32 0, %135
  auto *Value201 = Builder.CreateNSWSub(ConstantInt32(Ctx, 0), Value135);
  // %202 = load i32, ptr %136, align 4, !tbaa !22
  auto *Value202 = Builder.CreateAlignedLoad(Int32Ty, Value136, MaybeAlign{4});
  // %203 = sdiv i32 %202, %201
  auto *Value203 = Builder.CreateSDiv(Value202, Value201);
  // %204 = add i32 %203, -720
  auto *Value204 = Builder.CreateAdd(Value203, ConstantInt32(Ctx, -720));
  // %205 = icmp ult i32 %204, -719
  auto *Value205 = Builder.CreateICmpULT(Value204, ConstantInt32(Ctx, -719));
  // br i1 %205, label %214, label %206
  Builder.CreateCondBr(Value205, BB.at(214), BB.at(206));
  // ==========================================================================

  // ================================= BB 206 =================================
  Builder.SetInsertPoint(BB.at(206));

  // %207 = sext i32 %198 to i64
  auto *Value207 = Builder.CreateSExt(Value198, Int64Ty);
  // %208 = getelementptr inbounds [5102 x %struct.tagged_point_s], ptr @get_cells.intersections, i64 0, i64 %207
  auto *Value208 = Builder.CreateInBoundsGEP(
      IntersectionsTy, M->getNamedValue("get_cells.intersections"),
      {ConstantInt64(Ctx, 0), Value207});
  // %209 = zext nneg i32 %203 to i64
  auto *Value209 = Builder.CreateZExt(Value203, Int64Ty);
  dyn_cast<Instruction>(Value209)->setNonNeg(true);
  // %210 = shl nuw nsw i64 %209, 32
  auto *Value210 = Builder.CreateShl(Value209, uint64_t{32}, "", true, true);
  // store i64 %210, ptr %208, align 16
  Builder.CreateAlignedStore(Value210, Value208, MaybeAlign{16});
  // %211 = getelementptr inbounds [5102 x %struct.tagged_point_s], ptr @get_cells.intersections, i64 0, i64 %207, i32 1
  auto *Value211 = Builder.CreateInBoundsGEP(
      IntersectionsTy, M->getNamedValue("get_cells.intersections"),
      {ConstantInt64(Ctx, 0), Value207, ConstantInt32(Ctx, 1)});
  // store i32 %137, ptr %211, align 8, !tbaa !25
  Builder.CreateAlignedStore(Value137, Value211, MaybeAlign{8});
  // %212 = add nsw i32 %198, 1
  auto *Value212 = Builder.CreateNSWAdd(Value198, ConstantInt32(Ctx, 1));
  // %213 = getelementptr inbounds [5102 x %struct.tagged_point_s], ptr @get_cells.intersections, i64 0, i64 %207, i32 2
  auto *Value213 = Builder.CreateInBoundsGEP(
      IntersectionsTy, M->getNamedValue("get_cells.intersections"),
      {ConstantInt64(Ctx, 0), Value207, ConstantInt32(Ctx, 2)});
  // store i32 -3, ptr %213, align 4, !tbaa !27
  Builder.CreateAlignedStore(ConstantInt32(Ctx, -3), Value213, MaybeAlign{4});
  // br label %214
  Builder.CreateBr(BB.at(214));
  // ==========================================================================

  // ================================= BB 214 =================================
  Builder.SetInsertPoint(BB.at(214));

  // %215 = phi i32 [ %198, %200 ], [ %212, %206 ]
  auto *Value215 = Builder.CreatePHI(Int32Ty, 2);
  // %216 = mul i32 %133, 720
  auto *Value216 = Builder.CreateMul(Value133, ConstantInt32(Ctx, 720));
  // %217 = add i32 %202, %216
  auto *Value217 = Builder.CreateAdd(Value202, Value216);
  // %218 = sdiv i32 %217, %201
  auto *Value218 = Builder.CreateSDiv(Value217, Value201);
  // %219 = add i32 %218, -720
  auto *Value219 = Builder.CreateAdd(Value218, ConstantInt32(Ctx, -720));
  // %220 = icmp ult i32 %219, -719
  auto *Value220 = Builder.CreateICmpULT(Value219, ConstantInt32(Ctx, -719));
  // br i1 %220, label %233, label %221
  Builder.CreateCondBr(Value220, BB.at(233), BB.at(221));
  // ==========================================================================

  // ================================= BB 221 =================================
  Builder.SetInsertPoint(BB.at(221));

  // %222 = mul nsw i32 %135, -720
  auto *Value222 = Builder.CreateNSWMul(Value135, ConstantInt32(Ctx, -720));
  // %223 = sdiv i32 %222, %201
  auto *Value223 = Builder.CreateSDiv(Value222, Value201);
  // %224 = sext i32 %215 to i64
  auto *Value224 = Builder.CreateSExt(Value215, Int64Ty);
  // %225 = getelementptr inbounds [5102 x %struct.tagged_point_s], ptr @get_cells.intersections, i64 0, i64 %224
  auto *Value225 = Builder.CreateInBoundsGEP(
      IntersectionsTy, M->getNamedValue("get_cells.intersections"),
      {ConstantInt64(Ctx, 0), Value224});
  // %226 = zext nneg i32 %218 to i64
  auto *Value226 = Builder.CreateZExt(Value218, Int64Ty);
  dyn_cast<Instruction>(Value226)->setNonNeg(true);
  // %227 = shl nuw nsw i64 %226, 32
  auto *Value227 = Builder.CreateShl(Value226, uint64_t{32}, "", true, true);
  // %228 = zext i32 %223 to i64
  auto *Value228 = Builder.CreateZExt(Value223, Int64Ty);
  // %229 = or disjoint i64 %227, %228
  auto *Value229 = BinaryOperator::CreateDisjointOr(Value227, Value228);
  Builder.Insert(Value229);
  // store i64 %229, ptr %225, align 16
  Builder.CreateAlignedStore(Value229, Value225, MaybeAlign{16});
  // %230 = getelementptr inbounds [5102 x %struct.tagged_point_s], ptr @get_cells.intersections, i64 0, i64 %224, i32 1
  auto *Value230 = Builder.CreateInBoundsGEP(
      IntersectionsTy, M->getNamedValue("get_cells.intersections"),
      {ConstantInt64(Ctx, 0), Value224, ConstantInt32(Ctx, 1)});
  // store i32 %137, ptr %230, align 8, !tbaa !25
  Builder.CreateAlignedStore(Value137, Value230, MaybeAlign{8});
  // %231 = add nsw i32 %215, 1
  auto *Value231 = Builder.CreateNSWAdd(Value215, ConstantInt32(Ctx, 1));
  // %232 = getelementptr inbounds [5102 x %struct.tagged_point_s], ptr @get_cells.intersections, i64 0, i64 %224, i32 2
  auto *Value232 = Builder.CreateInBoundsGEP(
      IntersectionsTy, M->getNamedValue("get_cells.intersections"),
      {ConstantInt64(Ctx, 0), Value224, ConstantInt32(Ctx, 2)});
  // store i32 -2, ptr %232, align 4, !tbaa !27
  Builder.CreateAlignedStore(ConstantInt32(Ctx, -2), Value232, MaybeAlign{4});
  // br label %233
  Builder.CreateBr(BB.at(233));
  // ==========================================================================

  // ================================= BB 233 =================================
  Builder.SetInsertPoint(BB.at(233));

  // %234 = phi i32 [ %231, %221 ], [ %215, %214 ], [ %198, %197 ]
  auto *Value234 = Builder.CreatePHI(Int32Ty, 3);
  // br i1 %140, label %250, label %235
  Builder.CreateCondBr(Value140, BB.at(250), BB.at(235));
  // ==========================================================================

  // ================================= BB 235 =================================
  Builder.SetInsertPoint(BB.at(235));

  // %236 = mul nsw i32 %135, -720
  auto *Value236 = Builder.CreateNSWMul(Value135, ConstantInt32(Ctx, -720));
  // %237 = load i32, ptr %136, align 4, !tbaa !22
  auto *Value237 = Builder.CreateAlignedLoad(Int32Ty, Value136, MaybeAlign{4});
  // %238 = sub nsw i32 %236, %237
  auto *Value238 = Builder.CreateNSWSub(Value236, Value237);
  // %239 = sdiv i32 %238, %133
  auto *Value239 = Builder.CreateSDiv(Value238, Value133);
  // %240 = add i32 %239, -720
  auto *Value240 = Builder.CreateAdd(Value239, ConstantInt32(Ctx, -720));
  // %241 = icmp ult i32 %240, -719
  auto *Value241 = Builder.CreateICmpULT(Value240, ConstantInt32(Ctx, -719));
  // br i1 %241, label %250, label %242
  Builder.CreateCondBr(Value241, BB.at(250), BB.at(242));
  // ==========================================================================

  // ================================= BB 242 =================================
  Builder.SetInsertPoint(BB.at(242));

  // %243 = sext i32 %234 to i64
  auto *Value243 = Builder.CreateSExt(Value234, Int64Ty);
  // %244 = getelementptr inbounds [5102 x %struct.tagged_point_s], ptr @get_cells.intersections, i64 0, i64 %243
  auto *Value244 = Builder.CreateInBoundsGEP(
      IntersectionsTy, M->getNamedValue("get_cells.intersections"),
      {ConstantInt64(Ctx, 0), Value243});
  // %245 = zext nneg i32 %239 to i64
  auto *Value245 = Builder.CreateZExt(Value239, Int64Ty);
  dyn_cast<Instruction>(Value245)->setNonNeg(true);
  // %246 = or disjoint i64 %245, 3092376453120
  auto *Value246 = BinaryOperator::CreateDisjointOr(
      Value245, ConstantInt64(Ctx, 3092376453120));
  Builder.Insert(Value246);
  // store i64 %246, ptr %244, align 16
  Builder.CreateAlignedStore(Value246, Value244, MaybeAlign{16});
  // %247 = getelementptr inbounds [5102 x %struct.tagged_point_s], ptr @get_cells.intersections, i64 0, i64 %243, i32 1
  auto *Value247 = Builder.CreateInBoundsGEP(
      IntersectionsTy, M->getNamedValue("get_cells.intersections"),
      {ConstantInt64(Ctx, 0), Value243, ConstantInt32(Ctx, 1)});
  // store i32 %137, ptr %247, align 8, !tbaa !25
  Builder.CreateAlignedStore(Value137, Value247, MaybeAlign{8});
  // %248 = add nsw i32 %234, 1
  auto *Value248 = Builder.CreateNSWAdd(Value234, ConstantInt32(Ctx, 1));
  // %249 = getelementptr inbounds [5102 x %struct.tagged_point_s], ptr @get_cells.intersections, i64 0, i64 %243, i32 2
  auto *Value249 = Builder.CreateInBoundsGEP(
      IntersectionsTy, M->getNamedValue("get_cells.intersections"),
      {ConstantInt64(Ctx, 0), Value243, ConstantInt32(Ctx, 2)});
  // store i32 -1, ptr %249, align 4, !tbaa !27
  Builder.CreateAlignedStore(ConstantInt32(Ctx, -1), Value249, MaybeAlign{4});
  // br label %250
  Builder.CreateBr(BB.at(250));
  // ==========================================================================

  // ================================= BB 250 =================================
  Builder.SetInsertPoint(BB.at(250));

  // %251 = phi i32 [ %248, %242 ], [ %234, %235 ], [ %234, %233 ]
  auto *Value251 = Builder.CreatePHI(Int32Ty, 3);
  // %252 = icmp eq i64 %139, 98
  auto *Value252 = Builder.CreateICmpEQ(Value139, ConstantInt64(Ctx, 98));
  // %253 = add nuw nsw i64 %129, 1
  auto *Value253 =
      Builder.CreateAdd(Value129, ConstantInt64(Ctx, 1), "", true, true);
  // br i1 %252, label %119, label %127, !llvm.loop !29
  Builder.CreateCondBr(Value252, BB.at(119), BB.at(127));
  // ==========================================================================

  // ================================= BB 119 =================================
  Builder.SetInsertPoint(BB.at(119));

  // %120 = sext i32 %251 to i64
  auto *Value120 = Builder.CreateSExt(Value251, Int64Ty);
  // %121 = getelementptr inbounds %struct.tagged_point_s, ptr @get_cells.intersections, i64 %120
  auto *Value121 = Builder.CreateInBoundsGEP(
      Types.at("struct.tagged_point_s"),
      M->getNamedValue("get_cells.intersections"), {Value120});
  // %122 = icmp eq i32 %251, 0
  auto *Value122 = Builder.CreateICmpEQ(Value251, ConstantInt32(Ctx, 0));
  // br i1 %122, label %264, label %123
  Builder.CreateCondBr(Value122, BB.at(264), BB.at(123));
  // ==========================================================================

  // ================================= BB 254 =================================
  Builder.SetInsertPoint(BB.at(254));

  // %255 = phi ptr [ @get_cells.intersections, %123 ], [ %420, %418 ]
  auto *Value255 = Builder.CreatePHI(PointerType::getUnqual(Ctx), 2);
  // %256 = phi i32 [ 0, %123 ], [ %419, %418 ]
  auto *Value256 = Builder.CreatePHI(Int32Ty, 2);
  // %257 = getelementptr inbounds %struct.tagged_point_s, ptr %255, i64 0, i32 1
  auto *Value257 =
      Builder.CreateInBoundsGEP(Types.at("struct.tagged_point_s"), Value255,
                                {ConstantInt64(Ctx, 0), ConstantInt32(Ctx, 1)});
  // %258 = load i32, ptr %257, align 4, !tbaa !25
  auto *Value258 = Builder.CreateAlignedLoad(Int32Ty, Value257, MaybeAlign{4});
  // %259 = getelementptr inbounds %struct.tagged_point_s, ptr %255, i64 0, i32 2
  auto *Value259 =
      Builder.CreateInBoundsGEP(Types.at("struct.tagged_point_s"), Value255,
                                {ConstantInt64(Ctx, 0), ConstantInt32(Ctx, 2)});
  // %260 = getelementptr i8, ptr %255, i64 4
  auto *Value260 = Builder.CreateGEP(Type::getInt8Ty(Ctx), Value255,
                                     {ConstantInt64(Ctx, 4)});
  // %261 = zext i32 %258 to i64
  auto *Value261 = Builder.CreateZExt(Value258, Int64Ty);
  // %262 = icmp eq i32 %258, -4
  auto *Value262 = Builder.CreateICmpEQ(Value258, ConstantInt32(Ctx, -4));
  // %263 = load i32, ptr %259, align 4, !tbaa !27
  auto *Value263 = Builder.CreateAlignedLoad(Int32Ty, Value259, MaybeAlign{4});
  // br i1 %262, label %325, label %311
  Builder.CreateCondBr(Value262, BB.at(325), BB.at(311));
  // ==========================================================================

  // ================================= BB 264 =================================
  Builder.SetInsertPoint(BB.at(264));

  // %265 = phi i32 [ 0, %119 ], [ %419, %418 ]
  auto *Value265 = Builder.CreatePHI(Int32Ty, 2);
  // %266 = getelementptr inbounds [100 x %struct.polygon_s], ptr @get_cells.cells, i64 0, i64 %117
  auto *Value266 =
      Builder.CreateInBoundsGEP(CellsTy, M->getNamedValue("get_cells.cells"),
                                {ConstantInt64(Ctx, 0), Value117});
  // %267 = getelementptr inbounds [100 x %struct.polygon_s], ptr @get_cells.cells, i64 0, i64 %117, i32 1
  auto *Value267 = Builder.CreateInBoundsGEP(
      CellsTy, M->getNamedValue("get_cells.cells"),
      {ConstantInt64(Ctx, 0), Value117, ConstantInt32(Ctx, 1)});
  // store i32 %265, ptr %267, align 4, !tbaa !30
  Builder.CreateAlignedStore(Value265, Value267, MaybeAlign{4});
  // %268 = getelementptr inbounds %struct.point_s, ptr @generate_points.points, i64 %117
  auto *Value268 = Builder.CreateInBoundsGEP(
      Types.at("struct.point_s"), M->getNamedValue("generate_points.points"),
      {Value117});
  // %269 = load i64, ptr %268, align 8
  auto *Value269 = Builder.CreateAlignedLoad(Int64Ty, Value268, MaybeAlign{8});
  // store i64 %269, ptr @center, align 8
  Builder.CreateAlignedStore(Value269, M->getNamedValue("center"),
                             MaybeAlign{8});
  // %270 = icmp eq i32 %265, 1
  auto *Value270 = Builder.CreateICmpEQ(Value265, ConstantInt32(Ctx, 1));
  // br i1 %270, label %308, label %271
  Builder.CreateCondBr(Value270, BB.at(308), BB.at(271));
  // ==========================================================================

  // ================================= BB 271 =================================
  Builder.SetInsertPoint(BB.at(271));

  // %272 = zext i32 %265 to i64
  auto *Value272 = Builder.CreateZExt(Value265, Int64Ty);
  // br label %273
  Builder.CreateBr(BB.at(273));
  // ==========================================================================

  // ================================= BB 273 =================================
  Builder.SetInsertPoint(BB.at(273));

  // %274 = phi i64 [ 1, %271 ], [ %279, %278 ]
  auto *Value274 = Builder.CreatePHI(Int64Ty, 2);
  // %275 = trunc i64 %274 to i32
  auto *Value275 = Builder.CreateTrunc(Value274, Int32Ty);
  // %276 = sub i32 %265, %275
  auto *Value276 = Builder.CreateSub(Value265, Value275);
  // %277 = zext i32 %276 to i64
  auto *Value277 = Builder.CreateZExt(Value276, Int64Ty);
  // br label %281
  Builder.CreateBr(BB.at(281));
  // ==========================================================================

  // ================================= BB 278 =================================
  Builder.SetInsertPoint(BB.at(278));

  // %279 = add nuw nsw i64 %274, 1
  auto *Value279 =
      Builder.CreateAdd(Value274, ConstantInt64(Ctx, 1), "", true, true);
  // %280 = icmp eq i64 %279, %272
  auto *Value280 = Builder.CreateICmpEQ(Value279, Value272);
  // br i1 %280, label %308, label %273, !llvm.loop !32
  Builder.CreateCondBr(Value280, BB.at(308), BB.at(273));
  // ==========================================================================

  // ================================= BB 281 =================================
  Builder.SetInsertPoint(BB.at(281));
  auto *ConstGEP = ConstantExpr::getInBoundsGetElementPtr(
      Types.at("struct.point_s"),
      M->getOrInsertGlobal("center", Types.at("struct.point_s")),
      ArrayRef<Constant *>{ConstantInt64(Ctx, 0), ConstantInt32(Ctx, 1)});

  // %282 = phi i64 [ 0, %273 ], [ %284, %306 ]
  auto *Value282 = Builder.CreatePHI(Int64Ty, 2);
  // %283 = getelementptr inbounds %struct.point_s, ptr %266, i64 %282
  auto *Value283 = Builder.CreateInBoundsGEP(Types.at("struct.point_s"),
                                             Value266, {Value282});
  // %284 = add nuw nsw i64 %282, 1
  auto *Value284 =
      Builder.CreateAdd(Value282, ConstantInt64(Ctx, 1), "", true, true);
  // %285 = getelementptr inbounds %struct.point_s, ptr %266, i64 %284
  auto *Value285 = Builder.CreateInBoundsGEP(Types.at("struct.point_s"),
                                             Value266, {Value284});
  // %286 = load i32, ptr %283, align 4, !tbaa !5
  auto *Value286 = Builder.CreateAlignedLoad(Int32Ty, Value283, MaybeAlign{4});
  // %287 = getelementptr i8, ptr %283, i64 4
  auto *Value287 = Builder.CreateGEP(Type::getInt8Ty(Ctx), Value283,
                                     {ConstantInt64(Ctx, 4)});
  // %288 = load i32, ptr %287, align 4, !tbaa !10
  auto *Value288 = Builder.CreateAlignedLoad(Int32Ty, Value287, MaybeAlign{4});
  // %289 = load i32, ptr getelementptr inbounds (%struct.point_s, ptr @center, i64 0, i32 1), align 4, !tbaa !10
  auto *Value289 = Builder.CreateAlignedLoad(Int32Ty, ConstGEP, MaybeAlign{4});
  // %290 = sub nsw i32 %288, %289
  auto *Value290 = Builder.CreateNSWSub(Value288, Value289);
  // %291 = load i32, ptr @center, align 8, !tbaa !5
  auto *Value291 = Builder.CreateAlignedLoad(
      Int32Ty, M->getNamedValue("center"), MaybeAlign{8});
  // %292 = sub nsw i32 %286, %291
  auto *Value292 = Builder.CreateNSWSub(Value286, Value291);
  // %293 = tail call i32 @atan2_int(i32 noundef %290, i32 noundef %292) #4
  auto *Value293 = Builder.CreateCall(Atan2Int->getFunctionType(), Atan2Int,
                                      {Value290, Value292});
  Value293->setTailCall(true);
  // %294 = getelementptr inbounds %struct.point_s, ptr %266, i64 %284, i32 1
  auto *Value294 = Builder.CreateInBoundsGEP(
      Types.at("struct.point_s"), Value266, {Value284, ConstantInt32(Ctx, 1)});
  // %295 = load i32, ptr %294, align 4, !tbaa !10
  auto *Value295 = Builder.CreateAlignedLoad(Int32Ty, Value294, MaybeAlign{4});
  // %296 = load i32, ptr getelementptr inbounds (%struct.point_s, ptr @center, i64 0, i32 1), align 4, !tbaa !10
  auto *Value296 = Builder.CreateAlignedLoad(Int32Ty, ConstGEP, MaybeAlign{4});
  // %297 = sub nsw i32 %295, %296
  auto *Value297 = Builder.CreateNSWSub(Value295, Value296);
  // %298 = load i32, ptr %285, align 4, !tbaa !5
  auto *Value298 = Builder.CreateAlignedLoad(Int32Ty, Value285, MaybeAlign{4});
  // %299 = load i32, ptr @center, align 8, !tbaa !5
  auto *Value299 = Builder.CreateAlignedLoad(
      Int32Ty, M->getNamedValue("center"), MaybeAlign{8});
  // %300 = sub nsw i32 %298, %299
  auto *Value300 = Builder.CreateNSWSub(Value298, Value299);
  // %301 = tail call i32 @atan2_int(i32 noundef %297, i32 noundef %300) #4
  auto *Value301 = Builder.CreateCall(Atan2Int->getFunctionType(), Atan2Int,
                                      {Value297, Value300});
  Value301->setTailCall(true);
  // %302 = icmp sgt i32 %293, %301
  auto *Value302 = Builder.CreateICmpSGT(Value293, Value301);
  // br i1 %302, label %303, label %306
  Builder.CreateCondBr(Value302, BB.at(303), BB.at(306));
  // ==========================================================================

  // ================================= BB 303 =================================
  Builder.SetInsertPoint(BB.at(303));

  // %304 = load <4 x i32>, ptr %283, align 4, !tbaa !14
  auto *Value304 =
      Builder.CreateAlignedLoad(Int32Vec4, Value283, MaybeAlign{4});
  // %305 = shufflevector <4 x i32> %304, <4 x i32> poison, <4 x i32> <i32 2, i32 3, i32 0, i32 1>
  auto *Value305 = Builder.CreateShuffleVector(
      Value304, PoisonValue::get(Int32Vec4), {2, 3, 0, 1});
  // store <4 x i32> %305, ptr %283, align 4, !tbaa !14
  Builder.CreateAlignedStore(Value305, Value283, MaybeAlign{4});
  // br label %306
  Builder.CreateBr(BB.at(306));
  // ==========================================================================

  // ================================= BB 306 =================================
  Builder.SetInsertPoint(BB.at(306));

  // %307 = icmp eq i64 %284, %277
  auto *Value307 = Builder.CreateICmpEQ(Value284, Value277);
  // br i1 %307, label %278, label %281, !llvm.loop !33
  Builder.CreateCondBr(Value307, BB.at(278), BB.at(281));
  // ==========================================================================

  // ================================= BB 308 =================================
  Builder.SetInsertPoint(BB.at(308));

  // %309 = add nuw nsw i64 %117, 1
  auto *Value309 =
      Builder.CreateAdd(Value117, ConstantInt64(Ctx, 1), "", true, true);
  // %310 = icmp eq i64 %309, 100
  auto *Value310 = Builder.CreateICmpEQ(Value309, ConstantInt64(Ctx, 100));
  // br i1 %310, label %422, label %116, !llvm.loop !34
  Builder.CreateCondBr(Value310, BB.at(422), BB.at(116));
  // ==========================================================================

  // ================================= BB 311 =================================
  Builder.SetInsertPoint(BB.at(311));

  // %312 = icmp eq i32 %263, -4
  auto *Value312 = Builder.CreateICmpEQ(Value263, ConstantInt32(Ctx, -4));
  // br i1 %312, label %323, label %313
  Builder.CreateCondBr(Value312, BB.at(323), BB.at(313));
  // ==========================================================================

  // ================================= BB 313 =================================
  Builder.SetInsertPoint(BB.at(313));

  // %314 = load i32, ptr %125, align 4, !tbaa !10
  auto *Value314 = Builder.CreateAlignedLoad(Int32Ty, Value125, MaybeAlign{4});
  // %315 = load i32, ptr %260, align 4, !tbaa !10
  auto *Value315 = Builder.CreateAlignedLoad(Int32Ty, Value260, MaybeAlign{4});
  // %316 = icmp sgt i32 %314, 0
  auto *Value316 = Builder.CreateICmpSGT(Value314, ConstantInt32(Ctx, 0));
  // %317 = icmp sgt i32 %315, 0
  auto *Value317 = Builder.CreateICmpSGT(Value315, ConstantInt32(Ctx, 0));
  // %318 = select i1 %316, i1 %317, i1 false
  auto *Value318 = Builder.CreateSelect(Value316, Value317, ConstantInt::getFalse(Ctx));
  // %319 = icmp slt i32 %314, 0
  auto *Value319 = Builder.CreateICmpSLT(Value314, ConstantInt32(Ctx, 0));
  // %320 = icmp slt i32 %315, 0
  auto *Value320 = Builder.CreateICmpSLT(Value315, ConstantInt32(Ctx, 0));
  // %321 = select i1 %319, i1 %320, i1 false
  auto *Value321 = Builder.CreateSelect(Value319, Value320, ConstantInt::getFalse(Ctx));
  // %322 = select i1 %318, i1 true, i1 %321
  auto *Value322 = Builder.CreateSelect(Value318, ConstantInt::getTrue(Ctx), Value321);
  // br i1 %322, label %323, label %418
  Builder.CreateCondBr(Value322, BB.at(323), BB.at(418));
  // ==========================================================================

  // ================================= BB 323 =================================
  Builder.SetInsertPoint(BB.at(323));

  // %324 = icmp eq i32 %258, -3
  auto *Value324 = Builder.CreateICmpEQ(Value258, ConstantInt32(Ctx, -3));
  // br i1 %324, label %339, label %325
  Builder.CreateCondBr(Value324, BB.at(339), BB.at(325));
  // ==========================================================================

  // ================================= BB 325 =================================
  Builder.SetInsertPoint(BB.at(325));

  // %326 = icmp eq i32 %263, -3
  auto *Value326 = Builder.CreateICmpEQ(Value263, ConstantInt32(Ctx, -3));
  // br i1 %326, label %337, label %327
  Builder.CreateCondBr(Value326, BB.at(337), BB.at(327));
  // ==========================================================================

  // ================================= BB 327 =================================
  Builder.SetInsertPoint(BB.at(327));

  // %328 = load i32, ptr %124, align 8, !tbaa !5
  // %329 = load i32, ptr %255, align 4, !tbaa !5
  // %330 = icmp sgt i32 %328, 0
  // %331 = icmp sgt i32 %329, 0
  // %332 = select i1 %330, i1 %331, i1 false
  // %333 = icmp slt i32 %328, 0
  // %334 = icmp slt i32 %329, 0
  // %335 = select i1 %333, i1 %334, i1 false
  // %336 = select i1 %332, i1 true, i1 %335
  // br i1 %336, label %337, label %418
  // ==========================================================================

  // ================================= BB 337 =================================
  Builder.SetInsertPoint(BB.at(337));

  // %338 = icmp eq i32 %258, -2
  // br i1 %338, label %355, label %339
  // ==========================================================================

  M->print(outs(), nullptr);

  return 0;
}
