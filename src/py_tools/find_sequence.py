from glob import glob

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
