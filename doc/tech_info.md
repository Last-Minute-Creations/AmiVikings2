# Misc technical information

Mesen's tileset view suggests that only 8 32x32 objects can be present on-screen at once - they occupy ram positions `0x2000` - `0x2800`.
Object's current frame is copied form unknown location into this region.

Each 32x32 object has its own 16-color palette.
Some pickups and interactables seem to have separate palettes from the others.

Not all files seem to be compressed - notable exceptions are player character frames, probably for fast source copy access directly from ROM space.
Some compressions don't make sense - e.g. 16-color palettes are compressed even though they'd be smaller uncompressed.
It seems that whether file is compressed or not depends on its kind.

## Decoding palettes

After reversing the asset decompress routine, I've opened Mesen emulator in STRT level to see how palettes are organized.
Looking at the tileset preview, I've quickly determined that objects and tiles themselves have 16-color palettes assigned to them.
I've noted down a few of them and tried to search for their parts in decoded files - trace of it can be found in `find_sequence.py`.

After inspecting the files, it seemed that they are usually starting with `20FF` bytes, but further inspection revealed extra high bit count bytes interleaved every 8 bytes - as if the files were compressed, but the extraction tool assumed that they're not.
Upon further inspection it became obvious that those files are bigger when compressed, so the decompressor assumed this can't be and treated the files as decompressed.
The only reasoning on why the files are stored this way is that all palettes are compressed, and palette load code includes decompression at all times.

## Decoding where tiles are bound to palettes

Since SNES has 16 16-color palettes, my first thought was that the 16x16 tiles are storing info on which palette to use.
However, upon further inspection of the game, it became apparent that STRT level stores tile colors on palettes 3 and 5, whereas K4RN uses 4 and 7.
Because of this inconsistency, it can't be flipped by a single bit game-wide and needs a bit more bits to encode.

I've inspected attribute bits of several tiles in STRT world only to discover that lower attribute bits correspond to palette index.
My first guess was that since level palettes are usually 128-color long, only 3 bits are needed, but I've tested STRT map display when taking account 4 bits as palette index, and it was completely garbled.
Indeed, only 3 bits are responsible for storing palette index.
