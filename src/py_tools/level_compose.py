from common.find_path import find_path
from PIL import Image, ImageFont, ImageDraw
from glob import glob
import struct
import os

minitile_index_mask_size = 10
minitile_attribute_bit_flip_x = 5
minitile_attribute_bit_flip_y = 4
minitile_attribute_bit_front = 3
minitile_index_mask = (1 << minitile_index_mask_size) - 1
tile_index_mask = 0b11_1111_1111
tile_separation = 1

object_classes = {
    0: "Erik",
    1: "Baelog",
    2: "Olaf",
    3: "Fang",
    4: "Scorch",
    5: "Fang locked", # tw0!: 0829/0000
    6: "Scorch locked", # b0mb: 082B/0000

    11: "dunno11 W1/W2/W3/W4/W5",

    16: "Openable terrain horz", # tw0!: 0062/0020
    17: "W2 terrain bridge", # wzrd: 0022/0020 0022/0040
    18: "Lever", # tw0!: 0024/0008

    20: "W4 Button momentary", # d4rk: 0024/0100 (enables geyser 0100)
    21: "Button", # sw1m: 0024/0008
    22: "Button stays pushed", # b4dd: 0024/0004
    23: "W2 block puzzle button", # 0024/010 0024/0030 0024/0040
    24: "Hit block",
    25: "W4 pushblock pressure plate", # d4rk: 0024/0040
    26: "Key hole red", # k3ys: 0024/0002
    27: "Key hole silver", # k3ys: 0024/0001
    28: "Key hole gold", # k3ys: 0024/0004
    29: "Item", # unk1 0020, unk2:
        # 0001: shield, 0002: bomb, 0003: nuke,
        # 0004: W1 red key, 0005: W1 skeleton key, 0006: W1 gold key,
        # 0007: W5 red card, 0009: W5 yellow card
        # 000A: W1 eyeball, 000B: W1 mushroom, 000C: W1 wing,
        # 000D: W2 egg, 000E: W2 scroll, 000F: W2 wand,
        # 0010: W3 ????, 0011: W3 tarot, 0012: W3 crystal ball,
        # 0013: skull, 0014: voodoo, 0015: horn,
        # 0016: pcb, 0018: cd
        # 0019: time gear, 001A time battery, 001B: time capacitor
        # 001C: torch, 001E: garlic, 001F: meatloaf, 0020: beer, 0021: banana, 0022: burger
    30: "Help box", # st3w: 0024/0008, 0024/0009, 0024/000A
    31: "Help box autotrigger", # strt: 0024/0106
    32: "W2 Bomb-detonatable blocks", # b0mb: 0024/0000

    36: "W2/W3 Teleporter", # b0mb: 0024/384E 0024/5A0A 0024/7A4E 0024/4A2A
    37: "W2 Teleporter one-way active", # b0mb: 0024/7606
    38: "W3 chest", # b3sv: 0024/0006, unk2 is item code
    39: "W3 underwater chest", # b3sv: 0022/0001, unk2 is item code
    40: "Pushblock", # wzrd: 0025/0000
    41: "W4 darkness", # d4rk: 0000/0087
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
    62: "Geyser", # k4rn: 0024/0000 d4rk: 0024/0100 (inactive)
    63: "W2/W4 bone swing", # b0mb: 0065/0000

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

    90: "W4 lift on rail", # d4rk: 0822/0001 0822/0000
    91: "W4 disappearing block", # h4rd: 0022/0000

    93: "Pressure button 1", # y0vr: 0024/0080, d4rk: 0022/0080 (opens terrain with unk2 0080)
    94: "W3 pressure button 2", # drnk: 0024/0200
    95: "W4 pushblock bottom blocker", # h4rd: 0022/0100 (button unk2)
    96: "W4 pillar swing", # d4rk: 0063/0000

    98: "W4 underwater spikes from bottom", # d4rk: 00A4/0000
    99: "W4 underwater spear from left", # h4rd: 00A4/0000
    100: "W4 lift up/down", # h4rd: 0022/8000
    101: "W4 button gate multiple", # d4rk: 0062/0030 (hit block unk2 OR'd)
    102: "W4 button gate single", # d4rk: 0062/000C (hit block unk2)
    103: "W4 pressure button gate", # hrdr: 0062/0008 (pressure button unk2)
    104: "W4 falling gateblock", # h4rd: 0022/0000

    106: "W4 falling debris x3", # d4rk: 0022/0000
    107: "W4 enemy barnacle", # d4rk: 0023/0000
    108: "W4 enemy monkey", # d4rk: 0021/2100
    109: "W4 enemy spearman", # d4rk: 002B/2100

    113: "W3 bubble stream to left", # t1n3: 0061/0000
    114: "W3 enemy jellyfish", # r3t0: 0027/0000
    115: "W2 enemy piranha", # b0mb: 0027
    116: "W5 gravity lift", # shck: 0000/003A 0000/0026
    117: "W5 tripwire permanent", # shck: 0024/0400
    118: "W5 electric deathfield bottom hit block-disablable", # shck: 0024/0100 (hit block unk2)
    119: "W5 electric deathfield momentary tripwire-disablable", # h3ll: 0062/0020 (tripwire unk2)
    120: "W5 electric deathfield permanent tripwire-disablable", # 4rgh: 0062/1000 (hit block unk2)

    123: "Dunno123 title", # 0830/0000
    124: "Continue dunno", # 0820/0000, in bottom-left corner of screen
    125: "Continue viking", # unk1: 082F (facing right), 086F (facing left); unk2 0000-0003
    126: "Continue valkyrie", # 0823/0000
    127: "Cutscene outro spaceship display", # unk1: 0817; unk2: 0001: leftmost, 0002: rightmost, 0003: upper right, 0004: upper left
    128: "Cutscene outro spaceship tomator head", # 082F/0000
    129: "Continue rainbow", # 0821/0000
    130: "Dunno130 logo Interplay initial/other blizzard", # 0000/00C8
    131: "Dunno131 title falling", # 082F/0000
    132: "Cutscene spaceship Tomator hand", # 0800/0000, also in intro ship and prison
    133: "Cutscene intro ship/prison old viking & roboknight", # unk1: 0821: Erik, 0823: Baelog/Olaf, 0863: Roboknight

    135: "W4 lift self-driving up/down", # d4rk: 0022/0300
    136: "W4 scales", # d4rk: 0022/0040

    138: "W4 Shaman pt2", # d4rk: 0022/0003

    140: "W4 bridge", # l0st: 0022/0400 (to right, button unk2)

    142: "dunno142 W3 r3t0 trigger", # r3t0: 0024/000C
    143: "W4 lift on rail rectangular", # hrdr: 0022/0002

    145: "W4 lift self-driving roundabout", # d4rk: 0022/0300
    146: "W1 toggle gate to right", # tw0!: 0064/0008
    147: "W1 key gate", # k3ys: 0064/0001 0064/0002
    148: "W1 bridge?", # sw1m: 0022/0008 (to right), t1m3: 0062/004 (to left)

    151: "W1 lift", # trsh: 0022/0000
    152: "Cauldron fire",
    153: "W5 key gate", # shck: 0022/0001 0022/0002 0022/004 (key hole unk2)
    154: "W5 key gate",
    155: "W5 bridge", # shck: 0062/0020 (to left, hit block unk2)

    158: "W5 lift up/down", # shck: 0024/0000
    159: "W5 scales", # h3ll: 0024/FFA0

    161: "W5 lift on rail rectangular", # shck: 0024/0001
    162: "W5 electric rope hit block-expandable", # shck: 0022/0800 (hit block unk2)
    163: "W5 enemy xeno", # shck: 0029/0000
    164: "W5 enemy robot", # shck: 002B/0000

    167: "W5 fireball spawner", # shck: 0028/0000 (to right), 0068/0000 (to left)

    171: "W3/W5 expandable chain", # t1n3: 0022/0200 0022/0400
    172: "W5 electric deathfield tripwire-disablable", # shck: 0024/0400 (tripwire unk2)
    173: "W5 tripwire momentary", # h3ll: 0024/0008 (chain unk2)
    175: "W1 Witch",
    176: "W2 Sorceress pt1", # k4rn: 006B/0002
    177: "Time machine pt2", # t1m3: 002F/0003
    178: "W3 Gypsy pt1", # b3sv: 0029/0000
    179: "W4 Shaman pt1", # d4rk: 002B/0002
    180: "W5 Connor", # shck: 006B/0002
    181: "Locked Fang (dialogue? lock?)", # tw0!: 0029/0000
    182: "Kid", # t1m3: 0069/0000
    183: "Locked Scorch (dialogue? lock?)", # b0mb: 002A/0000
    184: "W4 spike ball on chain", # d4rk: 0022/0000
    185: "W5 spike ball on chain", # shck: 0022/0000
    186: "W3 flame", # b3sv: 0025/0000
    187: "W5 forcefield button-momentary disablable", # 4rgh: 0022/0200 (button unk2)
    188: "W3/W5 retractable chain/forcefield", # y0vr: 0062/0080
    189: "W5 electric deathfield timed", # h3ll: 0062/0020
    190: "Baelog NPC", # t1m3: 002D/0400
    191: "W1 Underwater spikes from bottom", # t1m3: 00A2/0000
    192: "W1 enemy bat", # st3w: 0029/0C00

    194: "Time machine pt1", # t1m3: 002D/0800
    195: "W3 Underwater spikes from bottom/top", # r3t0: 00A2/FFE0
    197: "W2 Underwater spikes from bottom/top", # wzrd: 00A4/0000 (bottom), blks: 0024/0000 (top)
    198: "W5 electric deathfield timed-toggled", # b4dd: 0022/0200
    199: "Tomator", # b4dd: 0827/0000

    203: "W5 xeno spawner", # b4dd: 002D/1D00 002D/2200 002D/1D02 002D/2202

    206: "dunno206 W1",
    207: "W5 cannon button-operated", # d4dy: 0064/0040 (button unk2)
    208: "dunno208 logo Interplay other", # 0869/0000
    209: "dunno209 logo Interplay other", # 086B/0000

    211: "Logo falling viking", # unk1: 0827 (Erik), 082D (Baelog), 082F (Olaf), unk2: 0000
    212: "W4 Connor holo", # shck: 006B/0000

    216: "W5 electric deathfield enabled on tripwires", # b4dd: 0062/0018 (tripwire unk2 OR'd)
    217: "dunno217 cutscene intro/outro ship", # 0800/0001
    218: "dunno218 cutscene intro/outro ship", # 0800/0000
    219: "W5 Tomator flying rectangular?", # b4dd: 0022/0600
    220: "dunno220 credits W1/W2/W3/W4/W5", # 0800/0000
    221: "dunno221 cutscene outro ship on Erik", # 0800/0000

    223: "Cutscene outro spaceship lolipop hand", # 0823/0000
    224: "W5 pushblock bottom blocker", # h3ll: 0022/0080 (button unk2)
}

def read_mini_tiles(mini_tiles_path: str, is_8bpp: bool):
    tile_count = os.path.getsize(mini_tiles_path) // ((8 if is_8bpp else 4) * 8)
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

            if is_8bpp:
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
                        mini_tiles[i][x][y] |= chunky_row[x] << 4
    return mini_tiles

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

def snes_color_to_rgb(snes_color: int):
    r = (((snes_color >> 0) & 0b11111) * 255 // 0b11111)
    g = (((snes_color >> 5) & 0b11111) * 255 // 0b11111)
    b = (((snes_color >> 10) & 0b11111) * 255 // 0b11111)
    return (r, g, b)

def rgb_to_hex(rgb):
    return f"{rgb[0]:02X}{rgb[1]:02X}{rgb[2]:02X}"

def compose(level_def_index: int, is_display: bool):
    front_tiles = []
    level_def_path = find_path(level_def_index)
    print(f"loading_level {level_def_path}...")

    with open(level_def_path, "rb") as file_level_def:
        file_level_def.seek(5, 0) # 0: menu, 1-5: worlds, 6: game over, 7: spaceship, 8: outro, 9: no music, A+: no audio
        [music_index] = struct.unpack("<B", file_level_def.read(1))
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

        # Read objects
        [objlist_unk1] = struct.unpack("<H", file_level_def.read(2))
        [objlist_unk2] = struct.unpack("<H", file_level_def.read(2))
        print(f"\nObj list unk1: {objlist_unk1:04X} ({objlist_unk1}), unk2: {objlist_unk2:04X} ({objlist_unk2})")
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
            print(f"\tObject {len(objects):02d} class {obj_class_index:3d} @{obj_x:4d},{obj_y:4d}, center: {obj_cx:2d},{obj_cy:2d}, unk1: {obj_unk1:04X}, unk2: {obj_unk2:04X} - {kind_str}")
            objects.append({"class": obj_class_index, "x": obj_x, "y": obj_y, "cx": obj_cx, "cy": obj_cy})

        # Palette
        print("\nPalette elements:")
        level_palette = [(255, 0, 255, 255) for i in range(256)]
        while True:
            [subpalette_file_index] = struct.unpack("<H", file_level_def.read(2))
            if subpalette_file_index == 0xFFFF:
                break
            [palette_start_pos] = struct.unpack("<B", file_level_def.read(1))
            subpalette_file_path = find_path(subpalette_file_index)
            subpalette_color_count = os.path.getsize(subpalette_file_path) // (2)
            print(f"\tLoading {subpalette_color_count} colors from palette {subpalette_file_path} at pos 0x{palette_start_pos:02X}...")
            with open(subpalette_file_path, "rb") as file_subpalette:
                for i in range(subpalette_color_count):
                    [color_value] = struct.unpack("<H", file_subpalette.read(2))
                    rgb = snes_color_to_rgb(color_value)
                    # print(f"color {palette_start_pos + i:02X}: {color_value:04X} -> {r:02X}{g:02X}{b:02X}")
                    level_palette[palette_start_pos + i] = rgb + (255, )
        print("Final palette:")
        for i in range(256):
            if i % 16 == 0:
                print(f"\t{(i // 16 * 16):02X}: ", end="")
            print(rgb_to_hex(level_palette[i]), end=(f"\n" if (i % 16 == 15) else " "))

        # Color cycle section
        [color_cycle_active_mask] = struct.unpack("<H", file_level_def.read(2))
        i = 0
        print(f"\nColor cycle enable mask: {color_cycle_active_mask:04X}")
        while True:
            [cooldown] = struct.unpack("<B", file_level_def.read(1))
            if cooldown == 0:
                break
            [indexFirst] = struct.unpack("<B", file_level_def.read(1))
            [indexLast] = struct.unpack("<B", file_level_def.read(1))
            colors = []
            while True:
                [color] = struct.unpack("<H", file_level_def.read(2))
                if color == 0xFFFF:
                    break
                colors.append(color)
            enable_state_str = " ON" if (color_cycle_active_mask & (1 << i)) != 0 else "OFF"
            print(f"\t{enable_state_str} cooldown: {cooldown} ticks ({cooldown * 3} frames), indices: {indexFirst}..{indexLast}, colors: {', '.join(rgb_to_hex(snes_color_to_rgb(color)) for color in colors)}")
            i += 1

        # Tile anim section - water, rope fire, zap
        [tile_cycle_active_mask] = struct.unpack("<H", file_level_def.read(2))
        i = 0
        print(f"\nTile anim enable mask: {tile_cycle_active_mask:04X}")
        while True:
            [cooldown] = struct.unpack("<B", file_level_def.read(1))
            if cooldown == 0:
                break
            [frames_per_tile] = struct.unpack("<B", file_level_def.read(1))
            [unk1] = struct.unpack("<B", file_level_def.read(1))
            [unk2] = struct.unpack("<B", file_level_def.read(1))
            [unk3] = struct.unpack("<H", file_level_def.read(2))
            [file_index] = struct.unpack("<H", file_level_def.read(2))
            enable_state_str = " ON" if (tile_cycle_active_mask & (1 << i)) != 0 else "OFF"
            print(f"\t{enable_state_str} cooldown: {cooldown} ticks ({cooldown * 3} frames), frames per tile: {frames_per_tile}, unk {unk1:02X} {unk2:02X} {unk3:04X}, file: {find_path(file_index)}")
            i += 1
        # Water anim is animated 0-2-4-6 and 1-3-5-7 every 9 frames
        #       mask  freq framesPerTile               idx
        # sw1m: 03 00 03   04            00 01  80 00  27 01   # water - frame size: 2x1 blocks, 8 frames total, 4 frames per tile, updates every 9 frames
        #             03   04            02 05  80 00  27 01
        #             00
        # k4rn: 03 00 03   04            00 01  80 00  26 01   # water - frame size: 2x1 blocks, 8 frames total, 4 frames per tile
        #             03   04            02 05  80 00  26 01
        #             00
        # b3sv: 0f 00 03   04            00 01  80 00  25 01   # water - frame size: 2x1 blocks, 8 frames total, 4 frames per tile, updates every 3 frames
        #             03   04            02 05  80 00  25 01
        #             02   05            00 09  c0 00  2c 01   # burning rope - frame size: 2x3 blocks, 5 frames total, updates every 6 frames
        #             02   05            02 0f  c0 00  2c 01
        #             00
        # d4rk: 03 00 03   04            00 01  80 00  2a 01   # water - frame size: 2x1 blocks, 8 frames total, 4 frames per tile
        #             03   04            02 05  80 00  2a 01
        #             00
        # shck: 0f 00 03   04            00 01  80 00  2b 01   # water - frame size: 2x1 blocks, 8 frames total, 4 frames per tile
        #             03   04            02 05  80 00  2b 01
        #             02   02            00 09  80 00  30 01   # zap   - frame size: 2x2 blocks, 2 frames total, updates every 6 frames
        #             02   02            01 0d  80 00  30 01
        #             00

        print("\nGfx preload:")
        while True:
            [file_index] = struct.unpack("<H", file_level_def.read(2))
            if file_index == 0xFFFF:
                break
            [unk] = struct.unpack("<H", file_level_def.read(2))
            [block_width] = struct.unpack("<B", file_level_def.read(1))
            [block_height] = struct.unpack("<B", file_level_def.read(1))
            print(f"\tUnk: {unk:04X} ({unk}), size: {block_width}x{block_height} blocks ({block_width * 8}x{block_height * 8} px), file: {find_path(file_index)}")

        print("\nAnim gfx preload:")
        while True:
            [file_index] = struct.unpack("<H", file_level_def.read(2))
            if file_index == 0xFFFF:
                break
            [block_width] = struct.unpack("<B", file_level_def.read(1))
            [block_height] = struct.unpack("<B", file_level_def.read(1))
            [is_compressed] = struct.unpack("<B", file_level_def.read(1))
            print(f"\tsize: {block_width}x{block_height} blocks ({block_width * 8}x{block_height * 8} px), compressed: {is_compressed}, file: {find_path(file_index)}")

        remaining_size = os.path.getsize(level_def_path) - file_level_def.tell()
        if remaining_size != 0:
            print(f"\nWARN: Level def has {remaining_size} unread bytes starting at 0x{file_level_def.tell():X}")

    print(f"\nMusic index: {music_index}, dimensions: {level_width}x{level_height}, level tilemap: {level_tilemap_index}, mini_tiles: {mini_tiles_index}, tiledefs: {tile_defs_index}, background: {background_tilemap_index}")

    mini_tiles_path = find_path(mini_tiles_index)

    tile_defs = None
    tiles = None
    if tile_defs_index == 65535:
        print(f"WARN: No tile defs specified")
        print(f"Reading 8bpp mini tiles from {mini_tiles_path}...")
        mini_tiles = read_mini_tiles(mini_tiles_path, True)
        print(f"Found {len(mini_tiles)} 8bpp tiles")

    else:
        print(f"Reading 4bpp mini tiles from {mini_tiles_path}...")
        mini_tiles = read_mini_tiles(mini_tiles_path, False)
        print(f"Found {len(mini_tiles)} 4bpp tiles")
        tile_defs = []
        tile_defs_path = find_path(tile_defs_index)
        with open(tile_defs_path, "rb") as file_mini_tiles:
            while True:
                chunk = file_mini_tiles.read(8)
                if len(chunk) != 8:
                    if len(chunk) != 0:
                        print(f"unexpected last chunk length: {len(chunk)}")
                    break
                tile_defs.append(struct.unpack("<HHHH", chunk))


        tiles = []
        tile_count = len(tile_defs)
        print(f"tile count: {tile_count}")
        for i in range(tile_count):
            tile_image = Image.new("RGBA", [16, 16], (0, 0, 0, 0))

            # attribute_a = defs[i][0] >> minitile_index_mask_size
            # attribute_b = defs[i][1] >> minitile_index_mask_size
            # attribute_c = defs[i][2] >> minitile_index_mask_size
            # attribute_d = defs[i][3] >> minitile_index_mask_size
            # print("tile {:03X} attribute {:06b} {:06b} {:06b} {:06b}".format(i, attribute_a, attribute_b, attribute_c, attribute_d))

            front_tiles.append(0)
            if compose_minitile(tile_image, mini_tiles, level_palette, tile_defs[i][0], [0, 0]):
                front_tiles[i] += 1
            if compose_minitile(tile_image, mini_tiles, level_palette, tile_defs[i][1], [8, 0]):
                front_tiles[i] += 1
            if compose_minitile(tile_image, mini_tiles, level_palette, tile_defs[i][2], [0, 8]):
                front_tiles[i] += 1
            if compose_minitile(tile_image, mini_tiles, level_palette, tile_defs[i][3], [8, 8]):
                front_tiles[i] += 1
            if front_tiles[i] != 0 and front_tiles[i] < 4:
                print(f"WARN: Tile {i:03X} has mixed front attributes: {front_tiles[i]}/4")
            tiles.append(tile_image)


    background_tilemap = None
    if background_tilemap_index == 0xFFFF:
        print(f"WARN: No background tilemap")
    else:
        background_tilemap = [[0 for y in range(background_height)] for x in range(background_width)]
        background_tilemap_path = find_path(background_tilemap_index)
        print(f"Loading bg from {background_tilemap_path}...")
        with open(background_tilemap_path, "rb") as background_tilemap_file:
            for y in range(background_height):
                for x in range(background_width):
                    background_tilemap[x][y] = struct.unpack("<H", background_tilemap_file.read(2))[0]

    tiles_per_line = level_width
    tiles_lines = level_height
    tile_size = 16
    if tile_defs == None:
        tiles_per_line *= 2
        tiles_lines *= 2
        tile_size //= 2
    with Image.new("RGBA", [tiles_per_line * (tile_size + tile_separation), tiles_lines * (tile_size + tile_separation)], (255, 255, 255, 0)) as level_preview:
        with Image.new("RGBA", level_preview.size, (0, 0, 64, 255)) as level_background:
            with Image.new("RGBA", level_preview.size, (255, 255, 255, 0)) as annotate_bitmap:
                fnt = ImageFont.truetype("uni05.ttf", 8)
                annotate_context = ImageDraw.Draw(annotate_bitmap)
                level_tilemap_path = find_path(level_tilemap_index)
                print(f"Reading tilemap from {level_tilemap_path}...")
                with open(level_tilemap_path, "rb") as file_tilemap:
                    for y in range(tiles_lines):
                        for x in range(tiles_per_line):
                            chunk = file_tilemap.read(2)
                            [value] = struct.unpack("<H", chunk)
                            # value = (y * level_width + x) % len(tiles) # debug all tiles
                            tile_index = value & tile_index_mask
                            tile_attribute = value >> 10
                            if background_tilemap != None:
                                level_background.paste(
                                    tiles[background_tilemap[x % background_width][y % background_height]],
                                    [x * (tile_size + tile_separation), y * (tile_size + tile_separation)]
                                )
                            if tiles != None:
                                level_preview.paste(tiles[tile_index], [x * (tile_size + tile_separation), y * (tile_size + tile_separation)])
                                tile_index_color = (0, 0, 255, 192) if front_tiles[tile_index] > 0 else (128, 128, 0, 192)
                                annotate_context.text(
                                    [x * (tile_size + tile_separation), y * (tile_size + tile_separation) - 2],
                                    f"{tile_index:02X}", font = fnt, fill = tile_index_color
                                )
                                if tile_attribute != 0:
                                    annotate_context.text(
                                        [x * (tile_size + tile_separation), y * (tile_size + tile_separation) - 2 + 8],
                                        "{:02X}".format(tile_attribute), font=fnt, fill=(255, 128, 0, 192)
                                    )
                            else:
                                tile_image = Image.new("RGBA", [tile_size, tile_size], (0, 0, 0, 0))
                                compose_minitile(tile_image, mini_tiles, level_palette, tile_index, [0, 0])
                                level_preview.paste(tile_image, [x * (8 + tile_separation), y * (8 + tile_separation)])
                for obj_index, obj in enumerate(objects):
                    x1 = max(0, obj["x"] - obj["cx"])
                    y1 = max(0, obj["y"] - obj["cy"])
                    x2 = max(0, obj["x"] + obj["cx"] - 1)
                    y2 = max(0, obj["y"] + obj["cy"] - 1)
                    dx1 = (x1 // tile_size) * (tile_size + tile_separation) + (x1 % tile_size)
                    dy1 = (y1 // tile_size) * (tile_size + tile_separation) + (y1 % tile_size)
                    dx2 = (x2 // tile_size) * (tile_size + tile_separation) + (x2 % tile_size)
                    dy2 = (y2 // tile_size) * (tile_size + tile_separation) + (y2 % tile_size)
                    obj_class = obj["class"]
                    color_rgb = (0, 255, 255) if obj_class in object_classes else (255, 0, 0)
                    annotate_context.rectangle([dx1, dy1, dx2, dy2], outline=color_rgb + (80,))
                    annotate_context.text([dx1 + 2, dy1 + 1], f"{obj_index:d}", font=fnt, fill=color_rgb + (192,))
                if is_display:
                    out = Image.alpha_composite(level_background, Image.alpha_composite(level_preview, annotate_bitmap))
                    out.show()

def report_all():
    def_indices = [
        28, 30, 32, 34, 36, 38, 40, # W1
        47, 49, 51, 53, 55, 57, # W2
        63, 65, 67, 69, 71, 73, # W3
        79, 81, 83, 85, 87, 89, # W4
        95, 97, 99, 101, 103, 105, # W5
        239, 245, 246, 252, 257, 305, # Logo/menu
        314, 319, 321, 323, 327, 328, 329, 330, 331, 332 # Cutscene
    ]
    for index in def_indices:
        compose(index, is_display=False)
        print("")


# report_all()
compose(level_def_index = 28, is_display = True)
print("All done!")
