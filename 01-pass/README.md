# Instrumenting LLVM pass

## How to build

```bash
cmake --build build -j12 --target lloyd_relaxation_instrumented
```

## Collecting statistics

```bash
python3 trace_stats.py --help
# usage: trace_stats.py [-h] [--duration DURATION] [--bars BARS] [--bars-dir DIR] executable
#
# This script analyzes which llvm::Instruction's were executed the most often while running an instrumented application. It performs the same analyses in relation to pairs, triples, ...,
# packs of 5 instructions. The result is represented as bar charts.
#
# positional arguments:
#   executable           path to an executable instrumented by LLVM pass that prints opcodes of instructions
#
# options:
#   -h, --help           show this help message and exit
#   --duration DURATION  time in seconds that tracing will last (default: 5.0 s)
#   --bars BARS          the bars on each bar chart (default: 5)
#   --bars-dir DIR       path to the folder to places the bars in (default: ./images)
```

## Statistics

![one](/01-pass/images/pack-of-1.png)

![two](/01-pass/images/pack-of-2.png)

![three](/01-pass/images/pack-of-3.png)

![four](/01-pass/images/pack-of-4.png)

![five](/01-pass/images/pack-of-5.png)
