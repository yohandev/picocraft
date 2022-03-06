# Converts image files to RGB565
from PIL import Image

img = Image.open("test.png")

with open("test.txt", "w") as f:
    for x in range(0, 160):
        out = ""
        for y in range(0, 128):
            r, g, b = img.getpixel((x, y))
            rgb = (int(r / 255 * 31) << 11) | (int(g / 255 * 63) << 5) | (int(b / 255 * 31))
            out += "0x{:04x}, ".format(rgb)
        f.write(out + "\n")