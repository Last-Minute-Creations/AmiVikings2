from PIL import Image
import struct

mini_tiles_path = "../../assets/dec/tileset_world1"
tile_defs_path = "../../assets/dec/026_tiledef_world1.dat"
level_tilemap_path = "../../assets/dec/029_level_w1_a0_strt_tiles.dat"

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
minitile_index_mask = 0b11_1111_1111
tile_count = len(defs)
print(f"tile count: {tile_count}")
for i in range(tile_count):
    tile_image = Image.new("RGB", [16, 16])
    index_a = defs[i][0] & minitile_index_mask
    attribute_a = defs[i][0] & ~minitile_index_mask
    index_b = defs[i][1] & minitile_index_mask
    attribute_b = defs[i][1] & ~minitile_index_mask
    index_c = defs[i][2] & minitile_index_mask
    attribute_c = defs[i][2] & ~minitile_index_mask
    index_d = defs[i][3] & minitile_index_mask
    attribute_d = defs[i][3] & ~minitile_index_mask

    minitile_a = Image.open(f"{mini_tiles_path}/{index_a}.png")
    minitile_b = Image.open(f"{mini_tiles_path}/{index_b}.png")
    minitile_c = Image.open(f"{mini_tiles_path}/{index_c}.png")
    minitile_d = Image.open(f"{mini_tiles_path}/{index_d}.png")

    tile_image.paste(minitile_a, [0, 0])
    tile_image.paste(minitile_b, [8, 0])
    tile_image.paste(minitile_c, [0, 8])
    tile_image.paste(minitile_d, [8, 8])

    minitile_a.close()
    minitile_b.close()
    minitile_c.close()
    minitile_d.close()
    tiles.append(tile_image)

tile_index_mask = 0b11_1111_1111
with Image.new("RGB", [48 * 16, 14 * 16]) as level_preview:
    with open(level_tilemap_path, "rb") as file_tilemap:
        for y in range(14):
            for x in range(32):
                chunk = file_tilemap.read(2)
                [value] = struct.unpack("<H", chunk)
                tile_index = value & tile_index_mask
                level_preview.paste(tiles[tile_index], [x * 16, y * 16])
    level_preview.save("level.png")
