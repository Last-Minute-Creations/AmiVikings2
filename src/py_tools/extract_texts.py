# Works only on USA version!

import struct

offset: int = 0x89CA
file_path = "D:\gry\snes9x\Roms\Lost Vikings 2 (USA).sfc"

index : int = 0
is_read_size: bool = True
is_read_text: bool = True
with open(file_path, "rb") as rom_file:
    rom_file.seek(offset, 1)
    while True:
        # Handle quirks
        is_read_size = index not in [3, 4, 5, 341, 342, 343, 344, 345, 346, 347, 348, 349, 350, 351, 352, 353, 354, 356, 476]
        is_read_text = index not in [287, 299]
        if index == 477:
            break

        if is_read_size:
            [box_width] = struct.unpack("<B", rom_file.read(1))
            [box_height] = struct.unpack("<B", rom_file.read(1))
        else:
            box_width = -1
            box_height = -1

        text: str = ""
        if is_read_text:
            while True:
                c = rom_file.read(1)
                if c == b'\0':
                    break
                decoded: str = c.decode()
                if decoded not in "ABCDEFGHIJKLMNOPQRSTUVWXYZ.\r!?', ()-+1234567890:/":
                    raise Exception(f"Unsupported char: '{decoded}'")
                text += decoded

        print_text: str = text.replace('\r', '\\r')
        print(f"/* {index} */ {{tUbCoordYX().withX({box_width}).withY({box_height}), \"{print_text}\"}},")
        if (box_width != -1 and box_width > 32) or (box_height != -1 and box_height > 32):
            raise Exception("Sus size")
        index += 1
