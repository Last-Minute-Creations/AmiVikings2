# Misc technical information

Mesen's tileset view suggests that only 8 32x32 objects can be present on-screen at once - they occupy ram positions `0x2000` - `0x2800`.
Object's current frame is copied form unknown location into this region.

Each 32x32 object has its own 16-color palette.
Some pickups and interactables seem to have separate palettes from the others.

Not all files seem to be compressed - notable exceptions are player character frames, probably for fast source copy access directly from ROM space.
Some compressions don't make sense - e.g. 16-color palettes are compressed even though they'd be smaller uncompressed.
It seems that whether file is compressed or not depends on its kind.
