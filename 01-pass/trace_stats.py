import argparse
import subprocess
import numpy as np
from pathlib import Path
from matplotlib import pyplot as plt

type CLIArgs = argparse.Namespace

def parse_options() -> CLIArgs:

    DEFAULT_BARS : int = 5
    DEFAULT_DURATION : float = 5.0
    DEFAULT_IMAGES : str = "./images"

    parser = argparse.ArgumentParser(description =
    "This script analyzes which llvm::Instruction's were executed the most often while running an "\
    "instrumented application. It performs the same analyses in relation to pairs, triples, ..., "
    "packs of 5 instructions. The result is represented as bar charts."
    )

    parser.add_argument(
        "executable",
        help = "path to an executable instrumented by LLVM pass that prints opcodes of instructions"
    )
    parser.add_argument(
        "--duration",
        help = f"time in seconds that tracing will last (default: {DEFAULT_DURATION} s)",
        type = float,
        default = DEFAULT_DURATION
    )
    parser.add_argument(
        "--bars",
        help = f"the bars on each bar chart (default: {DEFAULT_BARS})",
        type = int,
        default = DEFAULT_BARS
    )
    parser.add_argument(
        "--bars-dir",
        help = f"path to the folder to places the bars in (default: {DEFAULT_IMAGES})",
        dest = "dir",
        default = DEFAULT_IMAGES
    )

    args : CLIArgs = parser.parse_args()

    if not Path.is_file(Path(args.executable)):
        raise Exception("executable doesn't exist")

    Path.mkdir(Path(args.dir), exist_ok=True)

    return args


def get_instructions(args : CLIArgs) -> list[str]:
    cmd : list[str] = \
        ["timeout", "-k", f"{args.duration + 0.5}s", f"{args.duration}s", args.executable]
    proc : subprocess.CompletedProcess[str] = subprocess.run(cmd, capture_output=True, text=True)
    return proc.stdout.splitlines()


type PatternStats = dict[str, int]
type TotalStats = dict[int, PatternStats]

def collect_statistics(instructions : list[str], max_window_size : int) -> TotalStats:
    window_sizes = np.arange(1, max_window_size + 1)
    n_instructions : int = len(instructions)

    stats : TotalStats = {}
    for w_size in window_sizes:
        window_stats : PatternStats = {}
        for i in range(n_instructions - w_size + 1):
            pattern : str = '\n'.join(instr for instr in instructions[i : (i + w_size)])
            if pattern in window_stats:
                window_stats[pattern] += 1
            else:
                window_stats[pattern] = 0
        stats[w_size] = window_stats

    return stats


def main() -> None:
    args : CLIArgs = parse_options()
    instructions : list[str] = get_instructions(args)

    stats : TotalStats = collect_statistics(instructions, max_window_size = 5)

    for window_size, window_stats in stats.items():

        window_stats = sorted(window_stats.items(), key = lambda item : item[1])
        n_stats = len(window_stats)

        if args.bars < n_stats:
            window_stats = dict(window_stats[n_stats - args.bars : n_stats])
        else:
            window_stats = dict(window_stats)

        plt.figure(figsize = (16, 9), dpi = 100, layout = "compressed")
        plt.title("LLVM instructions statistics")
        plt.xlabel("llvm::Instruction opcode", loc = "right")
        plt.ylabel("The number of appearances in the trace")

        plt.bar(window_stats.keys(), window_stats.values())
        plt.savefig(f"{args.dir}/pack-of-{window_size}.png")


if __name__ == '__main__':
    try:
        main()
    except Exception as e:
        print(f"Caught an instance of type {type(e)}.\nwhat(): {e}")
