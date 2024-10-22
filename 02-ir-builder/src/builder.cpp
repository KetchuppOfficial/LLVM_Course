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

Function *declareRand(LLVMContext &Ctx, Module *M) {
  // declare i32 @Rand() local_unnamed_addr #1
  auto *RandTy = FunctionType::get(Type::getInt32Ty(Ctx), false /* isVarArg */);
  auto *Rand =
      Function::Create(RandTy, GlobalValue::ExternalLinkage, "Rand", M);
  dyn_cast<GlobalValue>(Rand)->setUnnamedAddr(GlobalValue::UnnamedAddr::Local);
  return Rand;
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

using BBMap = std::unordered_map<std::size_t, BasicBlock *>;

void fillBB0(IRBuilder<> &Builder, const BBMap &BB) {
  Builder.SetInsertPoint(BB.at(0));

  // br label %1
  Builder.CreateBr(BB.at(1));
}

void fillBB1(LLVMContext &Ctx, Module *M, IRBuilder<> &Builder, const BBMap &BB,
             const TypesMap &Types) {
  auto *Rand = dyn_cast<Function>(M->getNamedValue("Rand"));
  auto *UMin = dyn_cast<Function>(M->getNamedValue("llvm.umin.i32"));

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
}

} // unnamed namespace

int main() {
  LLVMContext Ctx;

  auto M = createModuleHeader(Ctx);
  auto Types = defineStructs(Ctx);
  createGlobalVariables(Ctx, M.get(), Types);

  auto *Lloyd = declareLloyd(Ctx, M.get());
  auto *Rand = declareRand(Ctx, M.get());
  auto *UMin = declareLLVMUMin(Ctx, M.get());

  auto *Int32Ty = Type::getInt32Ty(Ctx);
  IRBuilder<> Builder{Ctx};

  BBMap BB;
  for (auto I : {0, 1, 28}) {
    BB.emplace(I, BasicBlock::Create(Ctx, "", Lloyd));
  }

  fillBB0(Builder, BB);
  fillBB1(Ctx, M.get(), Builder, BB, Types);

  M->print(outs(), nullptr);

  return 0;
}
