#include <llvm-18/llvm/IR/Attributes.h>
#include <llvm-18/llvm/IR/GlobalValue.h>
#include <llvm-18/llvm/IR/GlobalVariable.h>
#include <memory>
#include <tuple>

#include "llvm/ADT/ArrayRef.h"
#include "llvm/IR/DerivedTypes.h"
#include "llvm/IR/GlobalObject.h"
#include "llvm/IR/GlobalValue.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include "llvm/Support/raw_ostream.h"

using namespace llvm;

namespace {

auto createModuleHeader(LLVMContext &Ctx) {
  static auto *kSource = "/home/ketchupp/Programming/llvm-course/"
                         "00-graphics_application/src/lloyd_relaxation.c";
  static auto *kDataLayout =
      "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-i128:128-"
      "f80:128-n8:16:32:64-S128";

  auto LLVMModule = std::make_unique<Module>(kSource, Ctx);
  LLVMModule->setSourceFileName(kSource);
  LLVMModule->setDataLayout(kDataLayout);
  LLVMModule->setTargetTriple("x86_64-pc-linux-gnu");

  return LLVMModule;
}

auto define_structs(LLVMContext &Ctx) {
  auto *I32Type = Type::getInt32Ty(Ctx);

  // %struct.point_s = type { i32, i32 }
  auto *PointSTy =
      StructType::create(Ctx, {I32Type, I32Type}, "struct.point_s");

  // %struct.polygon_s = type { [104 x %struct.point_s], i32 }
  auto *PolygonSTy = StructType::create(
      Ctx, {ArrayType::get(PointSTy, 104), I32Type}, "struct.polygon_s");

  // %struct.tagged_point_s = type { %struct.point_s, i32, i32 }
  auto *TaggedPointSTy = StructType::create(Ctx, {PointSTy, I32Type, I32Type},
                                            "struct.tagged_point_s");

  // %struct.line_s = type { i32, i32, i32 }
  auto *LineSTy =
      StructType::create(Ctx, {I32Type, I32Type, I32Type}, "struct.line_s");

  return std::tuple{PointSTy, PolygonSTy, TaggedPointSTy, LineSTy};
}

} // unnamed namespace

int main() {
  LLVMContext Ctx;

  auto LLVMModule = createModuleHeader(Ctx);
  auto [PointSTy, PolygonSTy, TaggedPointSTy, LineSTy] = define_structs(Ctx);

  // @generate_points.points = internal unnamed_addr global [100 x %struct.point_s] zeroinitializer, align 16
  auto *Points = dyn_cast<GlobalVariable>(LLVMModule->getOrInsertGlobal(
      "generate_points.points", ArrayType::get(PointSTy, 100)));
  assert(Points);
  Points->setAlignment(Align{16});
  Points->setLinkage(GlobalValue::LinkageTypes::InternalLinkage);
  Points->setUnnamedAddr(GlobalValue::UnnamedAddr::Global);
  Points->setInitializer(Constant::getNullValue(Points->getValueType()));

  // @get_cells.cells = internal unnamed_addr global [100 x %struct.polygon_s] zeroinitializer, align 16
  auto *Cells = dyn_cast<GlobalVariable>(LLVMModule->getOrInsertGlobal(
      "get_cells.cells", ArrayType::get(PointSTy, 100)));
  assert(Cells);
  Cells->setAlignment(Align{16});
  Cells->setLinkage(GlobalValue::LinkageTypes::InternalLinkage);
  Cells->setUnnamedAddr(GlobalValue::UnnamedAddr::Global);
  Cells->setInitializer(Constant::getNullValue(Cells->getValueType()));

  // @center = internal unnamed_addr global %struct.point_s zeroinitializer, align 8
  auto *Center = dyn_cast<GlobalVariable>(
      LLVMModule->getOrInsertGlobal("center", PointSTy));
  assert(Center);
  Center->setAlignment(Align{8});
  Center->setLinkage(GlobalValue::LinkageTypes::InternalLinkage);
  Center->setUnnamedAddr(GlobalValue::UnnamedAddr::Global);
  Center->setInitializer(Constant::getNullValue(Center->getValueType()));

  // @get_cells.intersections = internal global <{ %struct.tagged_point_s, %struct.tagged_point_s, %struct.tagged_point_s, %struct.tagged_point_s, [5098 x %struct.tagged_point_s] }> <{ %struct.tagged_point_s { %struct.point_s zeroinitializer, i32 -4, i32 -3 }, %struct.tagged_point_s { %struct.point_s { i32 720, i32 0 }, i32 -4, i32 -2 }, %struct.tagged_point_s { %struct.point_s { i32 720, i32 720 }, i32 -2, i32 -1 }, %struct.tagged_point_s { %struct.point_s { i32 0, i32 720 }, i32 -1, i32 -3 }, [5098 x %struct.tagged_point_s] zeroinitializer }>, align 16

  // @compute_normals.perps = internal unnamed_addr global [100 x [99 x %struct.line_s]] zeroinitializer, align 16
  auto *Perps = dyn_cast<GlobalVariable>(LLVMModule->getOrInsertGlobal(
      "compute_normals.perps",
      ArrayType::get(ArrayType::get(LineSTy, 99), 100)));
  assert(Perps);
  Perps->setAlignment(Align{16});
  Perps->setLinkage(GlobalValue::LinkageTypes::InternalLinkage);
  Perps->setUnnamedAddr(GlobalValue::UnnamedAddr::Global);
  Perps->setInitializer(Constant::getNullValue(Perps->getValueType()));

  IRBuilder<> Builder{Ctx};

  LLVMModule->print(outs(), nullptr);

  return 0;
}
