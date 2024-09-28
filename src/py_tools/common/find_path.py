from glob import glob


def find_path(index: int) -> str:
    return glob("../../assets/dec/{:03d}*".format(index))[0]
