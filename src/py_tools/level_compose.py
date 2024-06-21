from PIL import Image, ImageFont, ImageDraw
from glob import glob
import struct

minitile_index_mask_size = 10
minitile_attribute_bit_flip_y = 5
minitile_index_mask = (1 << minitile_index_mask_size) - 1

def find_path(index: int) -> str:
    return glob("../../assets/dec/{:03d}*".format(index))[0]

def compose_minitile(tile_image: Image, tileset_path: str, tiledef, pos):
    index = tiledef & minitile_index_mask
    attribute = tiledef >> minitile_index_mask_size
    minitile = Image.open(f"{tileset_path}/{index}.png")
    if attribute & (1 << (minitile_attribute_bit_flip_y - 1)) != 0:
        minitile = minitile.transpose(Image.Transpose.FLIP_LEFT_RIGHT)
    tile_image.paste(minitile, pos)
    minitile.close()


level_def_path = find_path(323)
print(f"loading_level {level_def_path}...")

with open(level_def_path, "rb") as file_level_def:
    file_level_def.seek(31, 0)
    [level_width] = struct.unpack("<H", file_level_def.read(2))
    [level_height] = struct.unpack("<H", file_level_def.read(2))
    file_level_def.seek(36, 0)
    [level_tilemap_index] = struct.unpack("<H", file_level_def.read(2))
    [mini_tiles_index] = struct.unpack("<H", file_level_def.read(2))
    [tile_defs_index] = struct.unpack("<H", file_level_def.read(2))
    file_level_def.seek(49, 0)

    [end1] = struct.unpack("<H", file_level_def.read(2))
    if end1 != 0xFFFF:
        print(f"1st section is not empty! Pos: {file_level_def.tell()}")
        exit(1)

    # Section 2 - DRNK 0V4L T1N3 Y0VR B3SV R3T0 intro_ship
    is_section2_empty = True
    while True:
        [end2] = struct.unpack("<H", file_level_def.read(2))
        if end2 != 0xFFFF and is_section2_empty:
            is_section2_empty = False
            print(f"2nd section is not empty! Pos: {file_level_def.tell()}")
        else:
            break

    section3_length = 0
    while struct.unpack("<H", file_level_def.read(2))[0] != 0xFFFF:
        section3_length += 2
    # Load palette
    while True:
        [subpalette_file_index] = struct.unpack("<H", file_level_def.read(2))
        if subpalette_file_index == 0xFFFF:
            break
        [palette_start_pos] = struct.unpack("<B", file_level_def.read(1))
        subpalette_file_path = find_path(subpalette_file_index)
        print(f"Loading palette from {subpalette_file_path} at pos {palette_start_pos}...")

print(f"dimensions: {level_width}x{level_height}, level tilemap: {level_tilemap_index}, minitiles: {mini_tiles_index}, tiledefs: {tile_defs_index}")
mini_tiles_path = find_path(mini_tiles_index)
tile_defs_path = find_path(tile_defs_index)
level_tilemap_path = find_path(level_tilemap_index)
defs = []
with open(tile_defs_path, "rb") as file_mini_tiles:
    while True:
        chunk = file_mini_tiles.read(8)
        if len(chunk) != 8:
            if len(chunk) != 0:
                print(f"unexpected last chunk length: {len(chunk)}")
            break
        defs.append(struct.unpack("<HHHH", chunk))

tiles = []
tile_count = len(defs)
print(f"tile count: {tile_count}")
for i in range(tile_count):
    tile_image = Image.new("RGB", [16, 16])

    # attribute_a = defs[i][0] >> minitile_index_mask_size
    # attribute_b = defs[i][1] >> minitile_index_mask_size
    # attribute_c = defs[i][2] >> minitile_index_mask_size
    # attribute_d = defs[i][3] >> minitile_index_mask_size
    # print("tile {:02X} attribute {:06b} {:06b} {:06b} {:06b}".format(i, attribute_a, attribute_b, attribute_c, attribute_d))

    compose_minitile(tile_image, mini_tiles_path, defs[i][0], [0, 0])
    compose_minitile(tile_image, mini_tiles_path, defs[i][1], [8, 0])
    compose_minitile(tile_image, mini_tiles_path, defs[i][2], [0, 8])
    compose_minitile(tile_image, mini_tiles_path, defs[i][3], [8, 8])
    tiles.append(tile_image)

tile_index_mask = 0b11_1111_1111
with Image.new("RGBA", [level_width * 17, level_height * 17], (0, 0, 64, 255)) as level_preview:
    with Image.new("RGBA", level_preview.size, (255, 255, 255, 0)) as txt:
        fnt = ImageFont.truetype("uni05.ttf", 8)
        d = ImageDraw.Draw(txt)
        with open(level_tilemap_path, "rb") as file_tilemap:
            for y in range(level_height):
                for x in range(level_width):
                    chunk = file_tilemap.read(2)
                    [value] = struct.unpack("<H", chunk)
                    tile_index = value & tile_index_mask
                    tile_attribute = value >> 10
                    level_preview.paste(tiles[tile_index], [x * 17, y * 17])
                    d.text([x * 17, y * 17 - 2], "{:02X}".format(tile_index), font=fnt, fill=(0, 0, 255, 192))
                    if tile_attribute != 0:
                        d.text([x * 17, y * 17 - 2 + 8], "{:02X}".format(tile_attribute), font=fnt, fill=(255, 128, 0, 192))
        out = Image.alpha_composite(level_preview, txt)
        out.show()
