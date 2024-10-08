import argparse
import subprocess
import numpy as np
from pathlib import Path
from matplotlib import pyplot as plt


def parse_options():

    default_bars = 5
    default_duration = 5.0
    default_images = "./images"

    parser = argparse.ArgumentParser(description =
    "This script analyzes which llvm::Instruction's were executed the most often while running an "\
    "instrumented application. It performs the same analyses in relation to pairs, triples, ..., "
    "packs of 5 instructions. The result is represented as bar charts.")

    parser.add_argument("--executable", help = "path to an executable instrumented by LLVM pass "\
                        "that prints opcodes of instructions", action = "store",
                        required = True)
    parser.add_argument("--duration", help = "time in seconds that tracing will last "\
                        f"(default: {default_duration} s)", type = float, default = default_duration,
                        action = "store")
    parser.add_argument("--bars", help = f"the bars on each bar chart (default: {default_bars})",
                        type = int, default = default_bars, action = "store")
    parser.add_argument("--bars-dir", help = "path to the folder to places the bars in "\
                        f"(default: {default_images})", dest = "dir", default = default_images,
                        action = "store")

    args = parser.parse_args()

    if not Path.is_file(Path(args.executable)):
        raise Exception("executable doesn't exist")

    Path.mkdir(Path(args.dir), exist_ok = True)

    return args


def get_instructions(args):
    cmd = ["timeout", "-k", f"{args.duration + 0.5}s", f"{args.duration}s", args.executable]
    proc = subprocess.run(cmd, capture_output=True, text=True)
    instructions = proc.stdout.splitlines()
    return instructions


def collect_statistics(instructions, max_window_size):
    window_sizes = np.arange(1, max_window_size + 1)
    n_instructions = len(instructions)

    stats = {}
    for w_size in window_sizes:
        window_stats = {}
        for i in range(n_instructions - w_size + 1):
            pattern = '\n'.join(instr for instr in instructions[i : (i + w_size)])
            if pattern in window_stats:
                window_stats[pattern] += 1
            else:
                window_stats[pattern] = 0
        stats[w_size] = window_stats

    return stats


def main():
    args = parse_options()
    instructions = get_instructions(args)

    stats = collect_statistics(instructions, max_window_size = 5)

    for window_size, window_stats in stats.items():

        window_stats = sorted(window_stats.items(), key = lambda item : item[1])
        n_stats = len(window_stats)

        if args.bars == None or args.bars < n_stats:
            window_stats = dict(window_stats)
        else:
            window_stats = dict(window_stats[n_stats - args.bars : n_stats])

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
