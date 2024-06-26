from glob import glob
from binascii import hexlify

def find_text(text: str):
    encoded = b''
    for c in text:
        v = ord(c) - ord('A') + 49
        e = v.to_bytes(1)
        encoded += e
    print(f"searching for '{text}': {hexlify(encoded, ' ', 1).decode()}")
    file_paths = glob("../../assets/dec/*.*")
    # file_paths = glob("../../assets/Lost Vikings II, The - Norse by Norsewest (Europe) (En,Fr,De).sfc")
    for file_path in file_paths:
        with open(file_path, 'rb') as file:
            file_contents = file.read()
        if file_contents.find(encoded) != -1:
            print(f"MATCH: {file_path}")
        if file_contents.find(text.encode()) != -1:
            print(f"MATCH: {file_path}")

def find_passwords():
    file_paths = glob("../../assets/dec/*.*")
    file_paths = glob("../../assets/Lost Vikings II, The - Norse by Norsewest (Europe) (En,Fr,De).sfc")
    for file_path in file_paths:
        with open(file_path, 'rb') as file:
            file_contents = file.read()
        # char table uppercase
        if file_contents.find(b'STRT') != -1:
            print(f"MATCH A: {file_path}")
        # char table lowercase
        if file_contents.find(b'strt') != -1:
            print(f"MATCH B: {file_path}")
        if file_contents.find(b'\x43\x44\x42\x44') != -1:
            print(f"MATCH C: {file_path}")
        # 32-bit little endian
        if file_contents.find(b'\x44\x42\x44\x43') != -1:
            print(f"MATCH D: {file_path}")
        # 2x16-bit little endian
        if file_contents.find(b'\x44\x43\x44\x42') != -1:
            print(f"MATCH E: {file_path}")

def find_palette():
    file_paths = glob("../../assets/dec/*.*")
    # file_paths = glob("../../assets/Lost Vikings II, The - Norse by Norsewest (Europe) (En,Fr,De).sfc")
    for file_path in file_paths:
        with open(file_path, 'rb') as file:
            file_contents = file.read()
        # if file_contents.find(b'\x3B\x7B\x7D\x7D') != -1: # strt
        # if file_contents.find(b'\x84\x2C\xBF\x3A') != -1: # erik
        # if file_contents.find(b'\x00\x01\xBF\x3A\xB6\x1D\x0F\x0D') != -1: # baelog
        # if file_contents.find(b'\xAD\x38\xBF\x3A\xB7\x19\xEE\x04') != -1: # olaf
        # if file_contents.find(b'\x13\x50\x9C\x73\x73\x4E\x4A\x29') != -1: # fang
        # if file_contents.find(b'\x06\x18\xF6\x03\x51\x03\xCC\x02') != -1: # scorch
        # if file_contents.find(b'\x29\x0C\x3A\x03\xCE\x0A\x09\x02') != -1: # tomator
        if file_contents.find(b'\xA0\x10\x06\x10\x0B\x24\x11\x40') != -1: # w5 alien
            print(f"MATCH A: {file_path}")

# CAN YOU TAKE US
# TO THE BIG SHINY
# METAL THING THAT
# BROUGHT US HERE?
find_text('BROUGHT')
