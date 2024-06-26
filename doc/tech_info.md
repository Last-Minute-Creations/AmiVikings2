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

## Decoding where audio is stored

The Snessor95 app have successfully played back some of the sfx/music samples.
Going through that lead, I've read that SNES uses audio in [BRR format](https://wiki.superfamicom.org/bit-rate-reduction-(brr)) and that's  what Snessor probably have found.
I've then found [noiSNESs_Brr_Finder](https://github.com/noisecross/noiSNESs_Brr_Finder) which extracted the files into wav files with offsets/lengths in their names.
Because of Windows' file sorting not handling hex offsets nicely, I've modified the app a bit to emit sample offsets/length in order of occurence and got following result:

```plain
Exporting the 52 detected BRR files...
Exporting 020496_03E7
Exporting 02087F_20FA
Exporting 02297B_0B52
Exporting 0234CF_0252
Exporting 023723_0DDA
Exporting 0244FF_049B
Exporting 02499C_09BD
Exporting 025BFC_05D6
Exporting 0261D4_084C
Exporting 026A22_0489
Exporting 026EAD_15BA
Exporting 028469_09E1
Exporting 028E4C_06AE
Exporting 0294FC_0E73
Exporting 02A371_1143
Exporting 02B4B6_064B
Exporting 02BB03_037B
Exporting 02BE80_0666
Exporting 02C4E8_08DC
Exporting 02CDC6_03D5
Exporting 02D19D_04DA
Exporting 02D6CC_072C
Exporting 02DDFA_0291
Exporting 02E1B8_153C
Exporting 02F6F6_041D
Exporting 02FB15_0D38
Exporting 03084F_0750
Exporting 030FA1_0786
Exporting 031729_0573
Exporting 032249_0510
Exporting 03275B_049B
Exporting 032BF8_10FB
Exporting 033CF5_0477
Exporting 03416E_026D
Exporting 0343DD_0276
Exporting 034655_0573
Exporting 034D36_02E2
Exporting 035133_0438
Exporting 03556D_0264
Exporting 035AA7_0492
Exporting 0360E4_0507
Exporting 03670F_037B
Exporting 036A8C_0372
Exporting 036E00_02C7
Exporting 03770F_05DF
Exporting 037CF0_06FF
Exporting 0383F1_06AE
Exporting 038AA1_0585
Exporting 039028_033C
Exporting 039366_04F5
Exporting 039E86_06FF
Exporting 03A587_04FE
```

What I've instantly noticed is that each size added to an offset doesn't add up to start position of next file and there's a 2-byte gap to next sample.
Upon further inspection it appeared that each sample is preceded by 2-byte length of it, in little endian.
At least that's what I've thought at start, but it isn't the case for `02297B_0B52` file since that field is set to `06F6`, which isn't the correct value.
Looks like it haven't found some audio files, notably at `2535A` and `2E08D`.
