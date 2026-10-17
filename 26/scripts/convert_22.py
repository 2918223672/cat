"""
将 Image2Lcd 生成的 256 色 C 数组 (如 22.c) 转换为 LCD_ShowMonoBitmap 可用的
1-bit 单色位图格式，并自动裁剪留白区域。
"""
import re
import os
from PIL import Image
import numpy as np


def parse_image2lcd_c(c_file: str) -> tuple[list, int, int, list]:
    """解析 Image2Lcd 生成的 C 数组，返回 (palette, width, height, pixels)"""
    with open(c_file, "r", encoding="utf-8") as f:
        text = f.read()

    # 提取所有十六进制字节
    hex_bytes = re.findall(r'0[Xx]([0-9A-Fa-f]{2})', text)
    data = [int(b, 16) for b in hex_bytes]

    print(f"总字节数: {len(data)}")

    # Image2Lcd 头部: [scan_mode, scan_mode2, width_lo, width_hi, height_lo, height_hi, colors_lo, colors_hi]
    scan1, scan2 = data[0], data[1]
    width = data[2] | (data[3] << 8)
    height = data[4] | (data[5] << 8)
    colors = data[6] | (data[7] << 8)

    print(f"扫描模式: 0x{scan1:02X} 0x{scan2:02X}")
    print(f"尺寸: {width} × {height}")
    print(f"颜色数: {colors}")

    # 调色板: 256 × 3 = 768 字节 (RGB)
    palette_offset = 8
    palette = []
    for i in range(256):
        r = data[palette_offset + i * 3]
        g = data[palette_offset + i * 3 + 1]
        b = data[palette_offset + i * 3 + 2]
        palette.append((r, g, b))

    # 像素数据
    pixel_offset = palette_offset + 768
    pixels = data[pixel_offset:pixel_offset + width * height]

    print(f"调色板条目: {len(palette)}")
    print(f"像素数据: {len(pixels)} 字节 (期望 {width * height})")

    return palette, width, height, pixels


def reconstruct_image(palette, width, height, pixels) -> Image.Image:
    """用调色板重建 RGB 图像"""
    img = Image.new("RGB", (width, height))
    for y in range(height):
        for x in range(width):
            idx = pixels[y * width + x]
            img.putpixel((x, y), palette[idx])
    return img


def crop_content(img: Image.Image, bg_threshold: int = 240) -> Image.Image:
    """
    裁剪掉留白/背景区域。
    背景判定：RGB 三通道均 > bg_threshold 的像素。
    """
    arr = np.array(img)
    # 找出非背景像素 (任一通道 < bg_threshold)
    mask = np.any(arr < bg_threshold, axis=2)
    rows = np.any(mask, axis=1)
    cols = np.any(mask, axis=0)

    if not np.any(rows):
        print("警告: 整张图都是背景，不裁剪")
        return img

    row_min, row_max = np.where(rows)[0][[0, -1]]
    col_min, col_max = np.where(cols)[0][[0, -1]]

    # 留一点边距
    margin = 4
    row_min = max(0, row_min - margin)
    row_max = min(img.height - 1, row_max + margin)
    col_min = max(0, col_min - margin)
    col_max = min(img.width - 1, col_max + margin)

    print(f"裁剪区域: ({col_min}, {row_min}) → ({col_max}, {row_max})")
    print(f"裁剪后尺寸: {col_max - col_min + 1} × {row_max - row_min + 1}")

    return img.crop((col_min, row_min, col_max + 1, row_max + 1))


def to_mono_bitmap(img: Image.Image, target_w: int, target_h: int) -> list[int]:
    """
    缩放 + 居中填充 + Floyd-Steinberg 抖动 → 1-bit 位图字节流 (MSB first)
    返回与 gif_data_new.h 兼容的 byte_array
    """
    from PIL import ImageOps

    # 保持比例缩放到目标区域
    img = ImageOps.pad(img, (target_w, target_h), color=(255, 255, 255))
    # Floyd-Steinberg 抖动转黑白
    img = img.convert("1", dither=Image.FLOYDSTEINBERG)

    # 生成字节流 (MSB first, 与 slice_to_lcd.py 一致)
    byte_array = []
    for y in range(target_h):
        row_byte = 0
        for x in range(target_w):
            bit = 1 if img.getpixel((x, y)) > 0 else 0
            row_byte = (row_byte << 1) | bit
            if (x + 1) % 8 == 0:
                byte_array.append(row_byte)
                row_byte = 0

    return byte_array


def main():
    script_dir = os.path.dirname(os.path.abspath(__file__))
    c_file = os.path.join(os.path.dirname(script_dir), "..", "22.c")
    c_file = os.path.abspath(c_file)  # C:\Users\29182\Desktop\22.c

    if not os.path.exists(c_file):
        # 尝试同目录
        c_file = os.path.join(script_dir, "22.c")
    if not os.path.exists(c_file):
        c_file = r"C:\Users\29182\Desktop\22.c"

    print(f"解析文件: {c_file}")
    palette, w, h, pixels = parse_image2lcd_c(c_file)

    # 重建图像
    img = reconstruct_image(palette, w, h, pixels)
    img.save(os.path.join(script_dir, "22_reconstructed.png"))
    print("已保存重建图: 22_reconstructed.png")

    # 裁剪留白
    cropped = crop_content(img)
    cropped.save(os.path.join(script_dir, "22_cropped.png"))
    print("已保存裁剪图: 22_cropped.png")

    # 转 1-bit 位图 (LCD 全屏 160×128，居中显示)
    lcd_w, lcd_h = 160, 128
    byte_array = to_mono_bitmap(cropped, lcd_w, lcd_h)

    # 输出 C 数组格式
    print(f"\n位图字节数: {len(byte_array)} (1-bit, {lcd_w}×{lcd_h})")
    print("C 数组格式 (可直接替换 gif_frameXX):")
    print(f"// 尺寸: {lcd_w}×{lcd_h}, {len(byte_array)} bytes")
    print(f"static const unsigned char gImage_single[] = {{")
    hex_lines = [
        "    " + ", ".join([f"0x{b:02X}" for b in byte_array[i:i + 16]])
        for i in range(0, len(byte_array), 16)
    ]
    print(",\n".join(hex_lines))
    print("};")

    # 同时输出到文件
    out_path = os.path.join(script_dir, "22_mono.txt")
    with open(out_path, "w") as f:
        f.write(f"// 尺寸: {lcd_w}×{lcd_h}, {len(byte_array)} bytes\n")
        f.write(f"static const unsigned char gImage_single[] = {{\n")
        f.write(",\n".join(hex_lines))
        f.write("\n};\n")
    print(f"\n已保存到: {out_path}")


if __name__ == "__main__":
    main()
