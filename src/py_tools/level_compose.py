from PIL import Image, ImageFont, ImageDraw
import struct

minitile_index_mask_size = 10
minitile_attribute_bit_flip_y = 5
minitile_index_mask = (1 << minitile_index_mask_size) - 1
mini_tiles_path = "../../assets/dec/tileset_world1"
tile_defs_path = "../../assets/dec/026_tiledef_world1.dat"
level_tilemap_path = "../../assets/dec/029_level_w1_a0_strt_tiles.dat"

def compose_minitile(tile_image: Image, tiledef, pos):
    index = tiledef & minitile_index_mask
    attribute = tiledef >> minitile_index_mask_size
    minitile = Image.open(f"{mini_tiles_path}/{index}.png")
    if attribute & (1 << (minitile_attribute_bit_flip_y - 1)) != 0:
        minitile = minitile.transpose(Image.Transpose.FLIP_LEFT_RIGHT)
    tile_image.paste(minitile, pos)
    minitile.close()

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

    attribute_a = defs[i][0] >> minitile_index_mask_size
    attribute_b = defs[i][1] >> minitile_index_mask_size
    attribute_c = defs[i][2] >> minitile_index_mask_size
    attribute_d = defs[i][3] >> minitile_index_mask_size
    print("tile {:02X} attribute {:06b} {:06b} {:06b} {:06b}".format(i, attribute_a, attribute_b, attribute_c, attribute_d))

    compose_minitile(tile_image, defs[i][0], [0, 0])
    compose_minitile(tile_image, defs[i][1], [8, 0])
    compose_minitile(tile_image, defs[i][2], [0, 8])
    compose_minitile(tile_image, defs[i][3], [8, 8])
    tiles.append(tile_image)

tile_index_mask = 0b11_1111_1111
with Image.new("RGBA", [32 * 17, 14 * 17], (0, 0, 64, 255)) as level_preview:
    with Image.new("RGBA", level_preview.size, (255, 255, 255, 0)) as txt:
        fnt = ImageFont.truetype("uni05.ttf", 8)
        d = ImageDraw.Draw(txt)
        with open(level_tilemap_path, "rb") as file_tilemap:
            for y in range(14):
                for x in range(32):
                    chunk = file_tilemap.read(2)
                    [value] = struct.unpack("<H", chunk)
                    tile_index = value & tile_index_mask
                    tile_attribute = value >> 10
                    level_preview.paste(tiles[tile_index], [x * 17, y * 17])
                    d.text([x * 17, y * 17 - 2], "{:02X}".format(tile_index), font=fnt, fill=(0, 0, 255, 192))
                    d.text([x * 17, y * 17 - 2 + 8], "{:02X}".format(tile_attribute), font=fnt, fill=(255, 128, 0, 192))
        out = Image.alpha_composite(level_preview, txt)
        out.show()
