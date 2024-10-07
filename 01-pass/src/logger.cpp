#include "llvm/Support/raw_ostream.h"

extern "C" void printInstr(const char *InstrName) {
  llvm::outs() << InstrName << '\n';
}
