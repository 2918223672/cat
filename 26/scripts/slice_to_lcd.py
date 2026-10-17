import os
from PIL import Image, ImageOps
import numpy as np


def generate_tft_optimized_frames(folder_path, output_h_file, frame_w=128, frame_h=64):
    h_content = []
    h_content.append(f"// TFT 专用点阵数据 (抖动算法优化)")
    files = [f for f in os.listdir(folder_path) if f.lower().endswith(('.png', '.jpg', '.jpeg'))]
    files.sort(key=lambda x: int(os.path.splitext(x)[0]))

    total_frames = len(files)
    h_content.append(f"#define GIF_FRAME_COUNT {total_frames}")

    for frame_idx, filename in enumerate(files):
        img = Image.open(os.path.join(folder_path, filename))

        # 1. 处理透明背景
        if img.mode == 'RGBA':
            background = Image.new('RGB', img.size, (0, 0, 0))
            background.paste(img, mask=img.split()[3])
            img = background
        else:
            img = img.convert("RGB")

        # 2. 裁剪内容
        arr = np.array(img.convert("L"))
        content_y, content_x = np.where(arr > 10)
        if len(content_y) > 0:
            margin = 4
            left = max(0, content_x.min() - margin)
            top = max(0, content_y.min() - margin)
            right = min(img.width, content_x.max() + margin)
            bottom = min(img.height, content_y.max() + margin)
            img = img.crop((left, top, right, bottom))

        # 3. 缩放
        img = ImageOps.pad(img, (frame_w, frame_h), color=0)

        # 4. 【核心修改】应用抖动算法
        # 将 RGB 图片转换为黑白 (模式 '1')，开启 Floyd-Steinberg 抖动
        img = img.convert("1", dither=Image.FLOYDSTEINBERG)

        # 5. 生成字节流
        byte_array = []
        for y in range(frame_h):
            row_byte = 0
            for x in range(frame_w):
                # 模式 '1' 下，像素为 255 (白) 或 0 (黑)
                bit = 1 if img.getpixel((x, y)) > 0 else 0
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
    print("已生成带抖动效果的 TFT 数据！")


generate_tft_optimized_frames("./", "gif_data_new.h")