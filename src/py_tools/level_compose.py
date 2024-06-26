from PIL import Image, ImageFont, ImageDraw
from glob import glob
import struct
import os

minitile_index_mask_size = 10
minitile_attribute_bit_flip_x = 5
minitile_attribute_bit_flip_y = 4
minitile_attribute_bit_front = 3
minitile_index_mask = (1 << minitile_index_mask_size) - 1

front_tiles = []

def read_mini_tiles(mini_tiles_path: str):
    tile_count = os.path.getsize(mini_tiles_path) // (4 * 8)
    mini_tiles = [[[0 for x in range(8)] for y in range(8)] for i in range(tile_count)]
    with open(mini_tiles_path, "rb") as file_mini_tiles:
        for i in range(tile_count):
            rows = []
            for y in range(8):
                rows.append(struct.unpack("<BB", file_mini_tiles.read(2)))
            for y in range(8):
                rows[y] += struct.unpack("<BB", file_mini_tiles.read(2))
            for y in range(8):
                chunky_row = [0 for x in range(8)]
                for b in range(4):
                    for x in range(8):
                        chunky_row[x] |= ((rows[y][b] >> (7 - x)) & 1) << b
                for x in range(8):
                    mini_tiles[i][x][y] = chunky_row[x]
    return mini_tiles

def find_path(index: int) -> str:
    return glob("../../assets/dec/{:03d}*".format(index))[0]

def compose_minitile(tile_image: Image, mini_tiles, palette, tiledef, pos) -> bool:
    index = tiledef & minitile_index_mask
    attribute = tiledef >> minitile_index_mask_size
    palette_index = attribute & 0b111 # using here 0b1111 is wrong
    minitile_data = mini_tiles[index]
    minitile = Image.new("RGBA", [8, 8], (0, 0, 0, 0))
    for y in range(8):
        for x in range(8):
            color = (255, 255, 255, 0) if minitile_data[x][y] == 0 else palette[palette_index * 16 + minitile_data[x][y]]
            minitile.putpixel([x, y], color)
    if attribute & (1 << (minitile_attribute_bit_flip_y)) != 0:
        minitile = minitile.transpose(Image.Transpose.FLIP_LEFT_RIGHT)
    if attribute & (1 << (minitile_attribute_bit_flip_x)) != 0:
        minitile = minitile.transpose(Image.Transpose.FLIP_TOP_BOTTOM)
    is_front = (attribute & (1 << (minitile_attribute_bit_front)) != 0)
    tile_image.paste(minitile, pos)
    minitile.close()
    return is_front


level_def_path = find_path(28)
print(f"loading_level {level_def_path}...")

with open(level_def_path, "rb") as file_level_def:
    file_level_def.seek(31, 0)
    [level_width] = struct.unpack("<H", file_level_def.read(2))
    [level_height] = struct.unpack("<H", file_level_def.read(2))
    file_level_def.seek(36, 0)
    [level_tilemap_index] = struct.unpack("<H", file_level_def.read(2))
    [mini_tiles_index] = struct.unpack("<H", file_level_def.read(2))
    [tile_defs_index] = struct.unpack("<H", file_level_def.read(2))
    [background_width] = struct.unpack("<H", file_level_def.read(2))
    [background_height] = struct.unpack("<H", file_level_def.read(2))
    file_level_def.seek(47, 0)
    [background_tilemap_index] = struct.unpack("<H", file_level_def.read(2))
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

    [objlist_unk1] = struct.unpack("<H", file_level_def.read(2))
    [objlist_unk2] = struct.unpack("<H", file_level_def.read(2))
    print(f"Obj list unk1: {objlist_unk1:04X} ({objlist_unk1}), unk2: {objlist_unk2:04X} ({objlist_unk2})")
    # Read objects
    while True:
        [obj_x] = struct.unpack("<h", file_level_def.read(2))
        if obj_x == -1:
            break
        [obj_y] = struct.unpack("<h", file_level_def.read(2))
        [obj_cx] = struct.unpack("<H", file_level_def.read(2))
        [obj_cy] = struct.unpack("<H", file_level_def.read(2))
        [obj_kind] = struct.unpack("<H", file_level_def.read(2))
        [obj_unk1] = struct.unpack("<H", file_level_def.read(2))
        [obj_unk2] = struct.unpack("<H", file_level_def.read(2))
        print(f"object kind {obj_kind:5d} @{obj_x:4d},{obj_y:4d}, center: {obj_cx:2d},{obj_cy:2d}, unk1: {obj_unk1:04X}, unk2: {obj_unk2:04X}")

    # Load palette
    level_palette = [(255, 0, 255, 255) for i in range(256)]
    while True:
        [subpalette_file_index] = struct.unpack("<H", file_level_def.read(2))
        if subpalette_file_index == 0xFFFF:
            break
        [palette_start_pos] = struct.unpack("<B", file_level_def.read(1))
        subpalette_file_path = find_path(subpalette_file_index)
        print(f"Loading palette from {subpalette_file_path} at pos {palette_start_pos}...")
        subpalette_color_count = os.path.getsize(subpalette_file_path) // (2)
        with open(subpalette_file_path, "rb") as file_subpalette:
            for i in range(subpalette_color_count):
                [color_value] = struct.unpack("<H", file_subpalette.read(2))
                r = (((color_value >> 0) & 0b11111) * 255 // 0b11111)
                g = (((color_value >> 5) & 0b11111) * 255 // 0b11111)
                b = (((color_value >> 10) & 0b11111) * 255 // 0b11111)
                # print(f"color {palette_start_pos + i:02X}: {color_value:04X} -> {r:02X}{g:02X}{b:02X}")
                level_palette[palette_start_pos + i] = (r, g, b, 255)

print(f"dimensions: {level_width}x{level_height}, level tilemap: {level_tilemap_index}, mini_tiles: {mini_tiles_index}, tiledefs: {tile_defs_index}")
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

mini_tiles = read_mini_tiles(mini_tiles_path)

tiles = []
tile_count = len(defs)
print(f"tile count: {tile_count}")
for i in range(tile_count):
    tile_image = Image.new("RGBA", [16, 16], (0, 0, 0, 0))

    # attribute_a = defs[i][0] >> minitile_index_mask_size
    # attribute_b = defs[i][1] >> minitile_index_mask_size
    # attribute_c = defs[i][2] >> minitile_index_mask_size
    # attribute_d = defs[i][3] >> minitile_index_mask_size
    # print("tile {:03X} attribute {:06b} {:06b} {:06b} {:06b}".format(i, attribute_a, attribute_b, attribute_c, attribute_d))

    front_tiles.append(0)
    if compose_minitile(tile_image, mini_tiles, level_palette, defs[i][0], [0, 0]):
        front_tiles[i] += 1
    if compose_minitile(tile_image, mini_tiles, level_palette, defs[i][1], [8, 0]):
        front_tiles[i] += 1
    if compose_minitile(tile_image, mini_tiles, level_palette, defs[i][2], [0, 8]):
        front_tiles[i] += 1
    if compose_minitile(tile_image, mini_tiles, level_palette, defs[i][3], [8, 8]):
        front_tiles[i] += 1
    if front_tiles[i] != 0 and front_tiles[i] < 4:
        print(f"WARN: Tile {i:03X} has mixed front attributes: {front_tiles[i]}/4")
    tiles.append(tile_image)

background_tilemap = [[0 for y in range(background_height)] for x in range(background_width)]
if background_tilemap_index != 0xFFFF:
    background_tilemap_path = find_path(background_tilemap_index)
    print(f"Loading bg from {background_tilemap_path}...")
    with open(background_tilemap_path, "rb") as background_tilemap_file:
        for y in range(background_height):
            for x in range(background_width):
                background_tilemap[x][y] = struct.unpack("<H", background_tilemap_file.read(2))[0]

tile_index_mask = 0b11_1111_1111
with Image.new("RGBA", [level_width * 17, level_height * 17], (255, 255, 255, 0)) as level_preview:
    with Image.new("RGBA", level_preview.size, (0, 0, 64, 255)) as level_background:
        with Image.new("RGBA", level_preview.size, (255, 255, 255, 0)) as txt:
            fnt = ImageFont.truetype("uni05.ttf", 8)
            d = ImageDraw.Draw(txt)
            with open(level_tilemap_path, "rb") as file_tilemap:
                for y in range(level_height):
                    for x in range(level_width):
                        chunk = file_tilemap.read(2)
                        [value] = struct.unpack("<H", chunk)
                        # value = (y * level_width + x) % len(tiles)
                        tile_index = value & tile_index_mask
                        tile_attribute = value >> 10
                        level_background.paste(tiles[background_tilemap[x % background_width][y % background_height]], [x * 17, y * 17])
                        level_preview.paste(tiles[tile_index], [x * 17, y * 17])
                        tile_index_color = (0, 0, 255, 192) if front_tiles[tile_index] > 0 else (128, 128, 0, 192)
                        d.text([x * 17, y * 17 - 2], f"{tile_index:02X}", font = fnt, fill = tile_index_color)
                        if tile_attribute != 0:
                            d.text([x * 17, y * 17 - 2 + 8], "{:02X}".format(tile_attribute), font=fnt, fill=(255, 128, 0, 192))
            out = Image.alpha_composite(level_background, Image.alpha_composite(level_preview, txt))
            out.show()
