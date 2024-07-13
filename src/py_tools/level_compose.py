from PIL import Image, ImageFont, ImageDraw
from glob import glob
import struct
import os

minitile_index_mask_size = 10
minitile_attribute_bit_flip_x = 5
minitile_attribute_bit_flip_y = 4
minitile_attribute_bit_front = 3
minitile_index_mask = (1 << minitile_index_mask_size) - 1

object_classes = {
    0: "Erik",
    1: "Baelog",
    2: "Olaf",
    3: "Fang",
    4: "Scorch",
    5: "Fang locked", # tw0!: 0829/0000
    6: "Scorch locked", # b0mb: 082B/0000

    11: "dunno11 W1/W2/W3",

    16: "Openable terrain horz", # tw0!: 0062/0020
    17: "W2 terrain bridge", # wzrd: 0022/0020 0022/0040
    18: "Lever", # tw0!: 0024/0008

    21: "Button", # sw1m: 0024/0008

    23: "W2 block puzzle button", # 0024/010 0024/0030 0024/0040
    24: "Hit block",

    26: "Key hole red", # k3ys: 0024/0002
    27: "Key hole silver", # k3ys: 0024/0001
    28: "Key hole gold", # k3ys: 0024/0004
    29: "Key item", # unk1 0020, unk2 0001: shield, 0002: bomb, 0003: nuke, 0004: W1 red key, 0005: W1 skeleton key, 0006: W1 gold key, 000A: W1 eyeball, 000B: W1 mushroom, 000C: W1 wing, 000D: W2 egg, 000E: W2 scroll, 000F: W2 wand, 0011: W3 tarot, 0012: W3 crystal ball, 0019: time gear, 001A time battery, 001B: time capacitor 001E: garlic, 001F: meatloaf, 0020: beer,
    30: "Help box", # st3w: 0024/0008, 0024/0009, 0024/000A
    31: "Help box autotrigger", # strt: 0024/0106
    32: "W2 Bomb-detonatable blocks", # b0mb: 0024/0000

    36: "W2 Teleporter", # b0mb: 0024/384E 0024/5A0A 0024/7A4E 0024/4A2A
    37: "W2 Teleporter one-way active", # b0mb: 0024/7606
    38: "W3 chest", # b3sv: 0024/0006, unk2 is item code
    39: "W3 underwater chest", # b3sv: 0022/0001, unk2 is item code
    40: "W2/W3 pushblock", # wzrd: 0025/0000

    42: "Fireball spawner from top double", # k4rn: 0024/0000
    43: "Fireball spawner", #st3w: 0064/0000 (left), k3ys/sw1m: 0024/0000 (right)

    47: "Fire courtain spawner to left", # t1m3: 0064/0000
    48: "Fire courtain spawner to bottom", #st3w: 0024/0000
    49: "Spike ball on chain", # b3sv: 0022/0000
    50: "Underwater spikes from bottom", # b3sv: 00A2/0000

    52: "W1 enemy skeleton", # st3w: 0029/0000
    53: "W1 enemy vamp/bat", # k3ys: 002B/0C00
    54: "W2 key gate", # k4rn: 0062/0004
    55: "W2 button gate", # wzrd: 0022/0010
    56: "W2 bridge?", # wzrd: 0062/0010

    59: "W2 scales", # k4rn: 0022/0030

    61: "W2 lift", # wzrd: 0022/0000
    62: "W2 geyser", # k4rn: 0024/0000
    63: "W2 bone swing", # b0mb: 0065/0000

    65: "W2 Sorceress pt2", # k4rn: 0022/0003
    66: "W2 block puzzle block", # blks: 0025/0010 0025/0020 0024/0040
    67: "W2 enemy knight", # k4rn: 002B/0500
    68: "W2 enemy dragon", # k4rn: 0021/1F00
    69: "W2 enemy wizard", # k4rn: 0029/1F00 0029/0000

    71: "W3 pushblock bottom blocker", # b3sv: 0022/0060

    73: "W3 lift bottom blocker", # b3sv: 0022/0020
    74: "W3 swing", # r3t0: 0063/0001

    76: "W3 lift", # b3sv: 0022/0000
    77: "W3 lift self-driving left/right", # drnk: 0022/0300
    78: "W3 lift self-driving up/down", # r3t0: 0024/000C
    79: "W3 button gate", # b3sv: 0062/0002
    80: "W3 key gate", # b3sv: 0062/0001
    81: "W3 plank springy", #b3sv: 0022/0000
    82: "W3 retractable ladder", # b3sv: 0020/0001
    83: "W3 Gypsy pt2", # b3sv: 0022/0003
    84: "W3 cannon", # b3sv: 0062/0000

    86: "W3 enemy pirate", # b3sv: 0029/0000
    87: "W3 enemy parrot", #b3sv: 0028/0000
    88: "W3 enemy captain", # b3sv: 0021/0000 (red) 0029/0000 (green)

    93: "W3 pressure button 1", # y0vr: 0024/0080
    94: "W3 pressure button 2", # drnk: 0024/0200

    113: "W3 bubble stream to left", # t1n3: 0061/0000
    114: "W3 enemy jellyfish", # r3t0: 0027/0000
    115: "W2 enemy piranha", # b0mb: 0027

    142: "dunno142 W3 r3t0 trigger", # r3t0: 0024/000C

    146: "W1 toggle gate to right", # tw0!: 0064/0008
    147: "W1 key gate", # k3ys: 0064/0001 0064/0002
    148: "W1 bridge?", # sw1m: 0022/0008 (to right), t1m3: 0062/004 (to left)

    151: "W1 lift", # trsh: 0022/0000
    152: "Cauldron fire",

    171: "W3 expandable chain", # t1n3: 0022/0200 0022/0400

    175: "W1 Witch",
    176: "W2 Sorceress pt1", # k4rn: 006B/0002
    177: "Time machine pt2", # t1m3: 002F/0003
    178: "W3 Gypsy pt1", #b3sv: 0029/0000

    181: "Locked Fang (dialogue? lock?)", # tw0!: 0029/0000
    182: "Kid", # t1m3: 0069/0000
    183: "Locked Scorch (dialogue? lock?)", # b0mb: 002A/0000

    186: "W3 flame", # b3sv: 0025/0000

    188: "W3 retractable chain", # y0vr: 0062/0080

    190: "Baelog NPC", # t1m3: 002D/0400
    191: "W1 Underwater spikes from bottom", # t1m3: 00A2/0000
    192: "W1 enemy bat", # st3w: 0029/0C00

    194: "Time machine pt1", # t1m3: 002D/0800

    195: "W3 Underwater spikes from bottom/top", # r3t0: 00A2/FFE0
    197: "W2 Underwater spikes from bottom/top", # wzrd: 00A4/0000 (bottom), blks: 0024/0000 (top)

    206: "dunno206 W1",
}

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
    [end2] = struct.unpack("<H", file_level_def.read(2))
    if end2 != 0xFFFF and is_section2_empty:
        is_section2_empty = False
        print(f"2nd section is not empty! Value: {end2:04X}")

    [objlist_unk1] = struct.unpack("<H", file_level_def.read(2))
    [objlist_unk2] = struct.unpack("<H", file_level_def.read(2))
    print(f"Obj list unk1: {objlist_unk1:04X} ({objlist_unk1}), unk2: {objlist_unk2:04X} ({objlist_unk2})")
    # Read objects
    objects = []
    while True:
        [obj_x] = struct.unpack("<h", file_level_def.read(2))
        if obj_x == -1:
            break
        [obj_y] = struct.unpack("<h", file_level_def.read(2))
        [obj_cx] = struct.unpack("<H", file_level_def.read(2))
        [obj_cy] = struct.unpack("<H", file_level_def.read(2))
        [obj_class_index] = struct.unpack("<H", file_level_def.read(2))
        [obj_unk1] = struct.unpack("<H", file_level_def.read(2))
        [obj_unk2] = struct.unpack("<H", file_level_def.read(2))
        kind_str = object_classes[obj_class_index] if obj_class_index in object_classes else "???"
        print(f"object {len(objects):02d} class {obj_class_index:3d} @{obj_x:4d},{obj_y:4d}, center: {obj_cx:2d},{obj_cy:2d}, unk1: {obj_unk1:04X}, unk2: {obj_unk2:04X} - {kind_str}")
        objects.append({"class": obj_class_index, "x": obj_x, "y": obj_y, "cx": obj_cx, "cy": obj_cy})

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
tile_separation = 1
with Image.new("RGBA", [level_width * (16 + tile_separation), level_height * (16 + tile_separation)], (255, 255, 255, 0)) as level_preview:
    with Image.new("RGBA", level_preview.size, (0, 0, 64, 255)) as level_background:
        with Image.new("RGBA", level_preview.size, (255, 255, 255, 0)) as annotate_bitmap:
            fnt = ImageFont.truetype("uni05.ttf", 8)
            annotate_context = ImageDraw.Draw(annotate_bitmap)
            with open(level_tilemap_path, "rb") as file_tilemap:
                for y in range(level_height):
                    for x in range(level_width):
                        chunk = file_tilemap.read(2)
                        [value] = struct.unpack("<H", chunk)
                        # value = (y * level_width + x) % len(tiles)
                        tile_index = value & tile_index_mask
                        tile_attribute = value >> 10
                        level_background.paste(tiles[background_tilemap[x % background_width][y % background_height]], [x * (16 + tile_separation), y * (16 + tile_separation)])
                        level_preview.paste(tiles[tile_index], [x * (16 + tile_separation), y * (16 + tile_separation)])
                        tile_index_color = (0, 0, 255, 192) if front_tiles[tile_index] > 0 else (128, 128, 0, 192)
                        # annotate_context.text([x * (16 + tile_separation), y * (16 + tile_separation) - 2], f"{tile_index:02X}", font = fnt, fill = tile_index_color)
                        # if tile_attribute != 0:
                        #     annotate_context.text([x * (16 + tile_separation), y * (16 + tile_separation) - 2 + 8], "{:02X}".format(tile_attribute), font=fnt, fill=(255, 128, 0, 192))
            for obj_index, obj in enumerate(objects):
                x1 = max(0, obj["x"] - obj["cx"])
                y1 = max(0, obj["y"] - obj["cy"])
                x2 = max(0, obj["x"] + obj["cx"] - 1)
                y2 = max(0, obj["y"] + obj["cy"] - 1)
                dx1 = (x1 // 16) * (16 + tile_separation) + (x1 % 16)
                dy1 = (y1 // 16) * (16 + tile_separation) + (y1 % 16)
                dx2 = (x2 // 16) * (16 + tile_separation) + (x2 % 16)
                dy2 = (y2 // 16) * (16 + tile_separation) + (y2 % 16)
                obj_class = obj["class"]
                color_rgb = (0, 255, 255) if obj_class in object_classes else (255, 0, 0)
                annotate_context.rectangle([dx1, dy1, dx2, dy2], outline=color_rgb + (80,))
                annotate_context.text([dx1 + 2, dy1 + 1], f"{obj_index:d}", font=fnt, fill=color_rgb + (192,))
            out = Image.alpha_composite(level_background, Image.alpha_composite(level_preview, annotate_bitmap))
            out.show()
