import os
from PIL import Image, ImageOps
import numpy as np


def generate_tft_optimized_frames(folder_path, output_h_file, frame_w=128, frame_h=64, threshold=128):
    h_content = []
    h_content.append(f"// TFT 专用点阵数据 (行扫描, MSB在前, 无页寻址干扰)")
    files = [f for f in os.listdir(folder_path) if f.endswith('.png')]
    files.sort(key=lambda x: int(os.path.splitext(x)[0]))

    total_frames = len(files)
    h_content.append(f"#define GIF_FRAME_COUNT {total_frames}")

    for frame_idx, filename in enumerate(files):
        img = Image.open(os.path.join(folder_path, filename))

        # ---- 关键修复：处理 RGBA 透明背景 ----
        if img.mode == 'RGBA':
            # 将透明像素合成到黑色背景上，避免透明区域干扰
            background = Image.new('RGBA', img.size, (0, 0, 0, 255))
            img = Image.alpha_composite(background, img)
        img = img.convert("L")

        # ---- 关键修复：裁剪到实际内容区域 ----
        arr = np.array(img)
        # 找到所有非纯黑像素的边界 (值>0 即有内容)
        content_y, content_x = np.where(arr > 10)  # >10 过滤噪点
        if len(content_y) > 0:
            margin = 4  # 留一点边距
            left = max(0, content_x.min() - margin)
            top = max(0, content_y.min() - margin)
            right = min(img.width, content_x.max() + margin)
            bottom = min(img.height, content_y.max() + margin)
            img = img.crop((left, top, right, bottom))

        # ---- 缩放到目标尺寸 ----
        # 等比缩放使内容占满 128x64，保持宽高比，黑色填充
        img = ImageOps.pad(img, (frame_w, frame_h), color=0)

        # ---- 二值化 ----
        byte_array = []
        for y in range(frame_h):
            row_byte = 0
            for x in range(frame_w):
                bit = 1 if img.getpixel((x, y)) > threshold else 0
                row_byte = (row_byte << 1) | bit
                if (x + 1) % 8 == 0:
                    byte_array.append(row_byte)
                    row_byte = 0

        h_content.append(f"static const unsigned char gif_frame{frame_idx}[] = {{")
        hex_lines = [", ".join([f"0x{b:02X}" for b in byte_array[i:i + 8]]) for i in range(0, len(byte_array), 8)]
        h_content.append("    " + ",\n    ".join(hex_lines) + "\n};\n")

    h_content.append("static const unsigned char* const gif_frames_array[] = {" + ", ".join(
        [f"gif_frame{i}" for i in range(total_frames)]) + "};")
    with open(output_h_file, "w") as f:
        f.write("\n".join(h_content) + "\n")
    print("已生成 TFT 专用数据！")


generate_tft_optimized_frames("./", "gif_data_new.h")
