# LLVM Course

Each task's directory contains its own README. Check them to get more information.

## Requirements

The following applications and libraries have to be installed:

- CMake of version 3.21 (or higher)
- libsdl2-dev

## How to install

```bash
git clone git@github.com:KetchuppOfficial/LLVM_Course.git
cd LLVM_Course
```

## How to build

### 0) Make sure you are in the root directory of the project (i.e. LLVM_Course/)

### 1) Build the project

```bash
cmake -B build -DCMAKE_BUILD_TYPE=Release -DCMAKE_C_COMPILER=clang -DCMAKE_CXX_COMPILER=clang++
cmake --build build -j12
```

> [!NOTE]
> Compilation with any compiler other than clang is forbidden. Moreover, versions of clang and
clang++ shall be equal.

### Installing

```bash
cmake --install build --prefix .
```
