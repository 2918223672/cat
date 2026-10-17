#!/usr/bin/env python3
"""
8路循迹 ADC 校准工具
从 white_data.txt 和 black_data.txt 读取静态测量数据，
分别计算各通道平均值，直接输出可复制到 track_algo.c 的数组。

用法：把车停在纯白/纯黑线上方录好数据，放入同目录的 txt 文件，然后:
    python adc_avg.py
"""

import os
import sys


def parse_lines(text: str) -> list[list[int]]:
    """解析 ADC 数据，每行8个数字，支持 'ADC=' 前缀"""
    rows = []
    for line in text.strip().splitlines():
        line = line.strip()
        if not line:
            continue
        if line.upper().startswith("ADC="):
            line = line[4:]
        elif line.upper().startswith("??"):
            line = line[2:]          # 去掉串口的 ??
        if line.upper().startswith("ADC:"):
            line = line[4:]
        parts = line.split()
        if len(parts) != 8:
            print(f"  警告: 跳过非8值行: {line[:50]}...", file=sys.stderr)
            continue
        try:
            rows.append([int(p) for p in parts])
        except ValueError:
            print(f"  警告: 跳过非数字行: {line[:50]}...", file=sys.stderr)
    return rows


def compute_stats(rows: list[list[int]]) -> dict:
    """按列（通道）计算平均值、最小值、最大值、标准差"""
    if not rows:
        return {}
    transposed = list(zip(*rows))
    stats = {}
    for ch in range(8):
        vals = transposed[ch]
        avg = sum(vals) / len(vals)
        vmin, vmax = min(vals), max(vals)
        std = (sum((v - avg) ** 2 for v in vals) / len(vals)) ** 0.5
        stats[f"ch{ch}"] = {
            "avg": avg, "min": vmin, "max": vmax,
            "std": std, "range": vmax - vmin, "count": len(vals),
        }
    return stats


def print_report(stats: dict, label: str):
    """打印统计报告"""
    if not stats:
        print(f"  无有效 {label} 数据。")
        return

    print(f"\n{'='*60}")
    print(f"  {label}  —  {stats['ch0']['count']} 行")
    print(f"{'='*60}")
    print(f"  {'通道':>4s}  {'均值':>8s}  {'最小':>6s}  {'最大':>6s}  {'标准差':>7s}  {'波动':>5s}")
    print(f"  {'-'*46}")

    for i in range(8):
        s = stats[f"ch{i}"]
        print(f"  CH{i}   {s['avg']:8.1f}  {s['min']:6d}  {s['max']:6d}  {s['std']:7.1f}  {s['range']:5d}")

    avgs = [int(round(stats[f"ch{i}"]["avg"])) for i in range(8)]
    print(f"\n  // env_{label.lower()}[8] = {{{', '.join(str(a) for a in avgs)}}};\n")


def main():
    script_dir = os.path.dirname(os.path.abspath(__file__))

    for fname, label in [("white_data.txt", "white"), ("black_data.txt", "black")]:
        fpath = os.path.join(script_dir, fname)
        if not os.path.exists(fpath):
            print(f"\n? {fname} 不存在，跳过。（路径: {fpath}）")
            continue

        with open(fpath, "r", encoding="utf-8") as f:
            text = f.read()

        rows = parse_lines(text)
        if not rows:
            print(f"\n? {fname} 无有效数据，跳过。")
            continue

        stats = compute_stats(rows)
        print_report(stats, label)


if __name__ == "__main__":
    main()
