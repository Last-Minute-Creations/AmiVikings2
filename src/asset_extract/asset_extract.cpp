/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include <fstream>
#include <filesystem>
#include <map>
#include <optional>
#include <lv2/level/level_def.hpp>

#include "logging.h"
#include "fs.h"
#include "rgb.h"
#include "bitmap.h"
#include "rle_table.hpp"
#include "rom_metadata.hpp"
#include "vector_reader.hpp"

namespace AmiLostVikings2::AssetExtract {

struct tAssetTocEntry {
	std::uint32_t ulOffs;
	std::uint32_t ulSizeInRom;
};

struct tTileMap {
	std::uint16_t m_uwWidth;
	std::uint16_t m_uwHeight;
	std::vector<std::uint16_t> m_Tiles;

	tTileMap(std::uint16_t uwWidth, std::uint16_t uwHeight):
		m_uwWidth(uwWidth),
		m_uwHeight(uwHeight)
	{
		m_Tiles.resize(m_uwWidth * m_uwHeight);
	}

	std::uint16_t &tileAt(std::uint16_t uwX, std::uint16_t uwY)
	{
		return m_Tiles[uwX + uwY * m_uwWidth];
	}

	std::uint16_t tileAt(std::uint16_t uwX, std::uint16_t uwY) const
	{
		return m_Tiles[uwX + uwY * m_uwWidth];
	}
};

using tAssetBytes = std::vector<std::uint8_t>;

std::shared_ptr<tTileMap> extractTileMap(
	const tAssetBytes &AssetBytes,
	std::uint16_t uwWidth,
	std::uint16_t uwHeight
)
{
	auto TileMap = std::make_shared<tTileMap>(uwWidth, uwHeight);
	auto Reader = tVectorReader(AssetBytes);
	for(std::uint16_t uwY = 0; uwY < uwHeight; ++uwY) {
		for(std::uint16_t uwX = 0; uwX < uwWidth; ++uwX) {
			auto Lo = Reader.readU8();
			auto Hi = Reader.readU8();
			std::uint16_t uwTile = (Hi << 8) | Lo;
			TileMap->tileAt(uwX, uwY) = uwTile;
		}
	}

	return TileMap;
}

void transcodeLevelTiles(
	const std::shared_ptr<const tTileMap> ForegroundTiles,
	const std::shared_ptr<const tTileMap> BackgroundTiles,
	const std::vector<bool> &FrontFlags,
	const std::string &OutPath
)
{
	static const auto writeU16BigEndian = [](
		std::ofstream &OutFile, std::uint16_t uwData
	) {
		std::uint8_t Hi = uwData >> 8;
		std::uint8_t Lo = uwData & 0xFF;
		OutFile.write(reinterpret_cast<char*>(&Hi), sizeof(Hi));
		OutFile.write(reinterpret_cast<char*>(&Lo), sizeof(Lo));
	};

	std::ofstream OutFile;
	OutFile.open(OutPath, std::ios::binary);
	writeU16BigEndian(OutFile, ForegroundTiles->m_uwWidth);
	writeU16BigEndian(OutFile, ForegroundTiles->m_uwHeight);
	for(std::uint16_t uwY = 0; uwY < ForegroundTiles->m_uwHeight; ++uwY) {
		for(std::uint16_t uwX = 0; uwX < BackgroundTiles->m_uwWidth; ++uwX) {
			writeU16BigEndian(OutFile, ForegroundTiles->tileAt(uwX, uwY));
			writeU16BigEndian(OutFile,  BackgroundTiles->tileAt(
				uwX % BackgroundTiles->m_uwWidth,
				uwY % BackgroundTiles->m_uwHeight
			) | (FrontFlags[uwX + uwY * ForegroundTiles->m_uwWidth] ? 0x8000 : 0));
		}
	}
}

std::shared_ptr<lv2::level::tLevelDef> loadLevelDef(const tAssetBytes &vAssetBytes) {
	auto Reader = tVectorReader(vAssetBytes);

	auto LevelDef = std::make_shared<lv2::level::tLevelDef>();
	Reader.readTo(LevelDef->Header.ubUnk0);
	Reader.readTo(LevelDef->Header.ubUnk1);
	Reader.readTo(LevelDef->Header.ubUnk2);
	Reader.readTo(LevelDef->Header.ubUnk3);
	Reader.readTo(LevelDef->Header.ubUnk4);
	Reader.readTo(LevelDef->Header.ubMusic);
	Reader.readTo(LevelDef->Header.ubUnk6);
	Reader.readTo(LevelDef->Header.ubUnk7);
	Reader.readTo(LevelDef->Header.ubUnk8);
	Reader.readTo(LevelDef->Header.ubUnk9);
	Reader.readTo(LevelDef->Header.ubUnk10);
	Reader.readTo(LevelDef->Header.uwNextLevelHeaderIdx);
	Reader.readTo(LevelDef->Header.ubUnk13);
	Reader.readTo(LevelDef->Header.ubUnk14);
	Reader.readTo(LevelDef->Header.ubUnk15);
	Reader.readTo(LevelDef->Header.ubUnk16);
	Reader.readTo(LevelDef->Header.ubUnk17);
	Reader.readTo(LevelDef->Header.ubUnk18);
	Reader.readTo(LevelDef->Header.ubUnk19);
	Reader.readTo(LevelDef->Header.ubUnk20);
	Reader.readTo(LevelDef->Header.ubUnk21);
	Reader.readTo(LevelDef->Header.ubUnk22);
	Reader.readTo(LevelDef->Header.ubUnk23);
	Reader.readTo(LevelDef->Header.ubUnk24);
	Reader.readTo(LevelDef->Header.ubUnk25);
	Reader.readTo(LevelDef->Header.ubUnk26);
	Reader.readTo(LevelDef->Header.ubUnk27);
	Reader.readTo(LevelDef->Header.ubUnk28);
	Reader.readTo(LevelDef->Header.ubUnk29);
	Reader.readTo(LevelDef->Header.ubUnk30);
	Reader.readTo(LevelDef->Header.uwTileWidth);
	Reader.readTo(LevelDef->Header.uwTileHeight);
	Reader.readTo(LevelDef->Header.ubUnk35);
	Reader.readTo(LevelDef->Header.uwTilemapFileIndex);
	Reader.readTo(LevelDef->Header.uwTilesetFileIndex);
	Reader.readTo(LevelDef->Header.uwTiledefFileIndex);
	Reader.readTo(LevelDef->Header.uwBackgroundWidth);
	Reader.readTo(LevelDef->Header.uwBackgroundHeight);
	Reader.readTo(LevelDef->Header.ubUnk46);
	Reader.readTo(LevelDef->Header.uwBackgroundFileIndex);

	Reader.readTo(LevelDef->uwUnk1);
	Reader.readTo(LevelDef->uwUnk2);
	Reader.readTo(LevelDef->uwUnk3);
	Reader.readTo(LevelDef->uwUnk4);

	while(true) {
		lv2::level::tLevelObjectDef ObjectDef;
		Reader.readTo(ObjectDef.wX);
		if(ObjectDef.wX == -1) {
			break;
		}

		Reader.readTo(ObjectDef.wY);
		Reader.readTo(ObjectDef.wCenterX);
		Reader.readTo(ObjectDef.wCenterY);
		Reader.readTo(ObjectDef.uwEntityKind);
		Reader.readTo(ObjectDef.uwParam1);
		Reader.readTo(ObjectDef.uwParam2);
		LevelDef->vObjects.push_back(ObjectDef);
	}

	while(true) {
		lv2::level::tPaletteFragmentLoadDef PaletteFragment;
		Reader.readTo(PaletteFragment.uwFileIndex);
		if(PaletteFragment.uwFileIndex == 0xFFFF) {
			break;
		}
		Reader.readTo(PaletteFragment.ubPaletePosStart);
		LevelDef->vPaletteFragments.push_back(PaletteFragment);
	}

	Reader.readTo(LevelDef->uwColorCycleActivationMask);
	while(true) {
		lv2::level::tColorCycleDef ColorCycleDef;
		Reader.readTo(ColorCycleDef.ubCooldown);
		if(ColorCycleDef.ubCooldown == 0) {
			break;
		}
		Reader.readTo(ColorCycleDef.ubFirstIndex);
		Reader.readTo(ColorCycleDef.ubLastIndex);

		while(true) {
			std::uint16_t uwColorCode;
			Reader.readTo(uwColorCode);
			if(uwColorCode == 0xFFFF) {
				break;
			}
			ColorCycleDef.vCycleColors.push_back(uwColorCode);
		}

		LevelDef->vColorCycles.push_back(ColorCycleDef);
	}

	Reader.readTo(LevelDef->uwTileCycleActivationMask);
	while(true) {
		lv2::level::tTileCycleDef TileCycleDef;
		Reader.readTo(TileCycleDef.ubCooldown);
		if(TileCycleDef.ubCooldown == 0) {
			break;
		}

		Reader.readTo(TileCycleDef.ubFramesPerTile);
		Reader.readTo(TileCycleDef.ubUnk1);
		Reader.readTo(TileCycleDef.ubUnk2);
		Reader.readTo(TileCycleDef.uwUnk3);
		Reader.readTo(TileCycleDef.uwFileIndex);

		LevelDef->vTileCycles.push_back(TileCycleDef);
	}

	while(true) {
		lv2::level::tGfxPreloadDef GfxPreloadDef;
		Reader.readTo(GfxPreloadDef.uwFileIndex);
		if(GfxPreloadDef.uwFileIndex == 0xFFFF) {
			break;
		}

		Reader.readTo(GfxPreloadDef.uwUnk1);
		Reader.readTo(GfxPreloadDef.ubBlockWidth);
		Reader.readTo(GfxPreloadDef.ubBlockHeight);
		LevelDef->vGfxPreloads.push_back(GfxPreloadDef);
	}

	while(true) {
		lv2::level::tGfxAnimPreloadDef GfxAnimPreloadDef;
		Reader.readTo(GfxAnimPreloadDef.uwFileIndex);
		if(GfxAnimPreloadDef.uwFileIndex == 0xFFFF) {
			break;
		}

		Reader.readTo(GfxAnimPreloadDef.ubBlockWidth);
		Reader.readTo(GfxAnimPreloadDef.ubBlockHeight);
		Reader.readTo(GfxAnimPreloadDef.isCompressed);
		LevelDef->vAnimPreloads.push_back(GfxAnimPreloadDef);
	}

	return LevelDef;
}

//-------------------------------------------------------------- TILE EXTRACTING

std::vector<std::shared_ptr<tChunkyBitmap>> extractTiles(
	tAssetBytes vDataRaw, uint8_t ubBpp, const tPalette &Palette
)
{
	// Read every tile
	uint32_t ulTileCnt = uint32_t(vDataRaw.size()) / (8 * ubBpp); // number of SNES 8x8 tiles
	std::vector<std::shared_ptr<tChunkyBitmap>> vTiles;
	uint32_t ulRawPos = 0;
	for(uint32_t i = 0; i < ulTileCnt; ++i) {
		// tPlanarBitmap supports only width being multiple of 16 - fill only 8x8
		tPlanarBitmap TilePlanar(16, 8, ubBpp);
		for(uint8_t ubRow = 0; ubRow < 8; ++ubRow) {
			for(uint8_t ubPlane = 0; ubPlane < std::min(ubBpp, uint8_t(2)); ++ubPlane) {
				uint8_t ubRaw = vDataRaw[ulRawPos++];
				TilePlanar.m_pPlanes[ubPlane][ubRow] = ubRaw << 8;
			}
		}
		for(uint8_t ubRow = 0; ubRow < 8; ++ubRow) {
			for(uint8_t ubPlane = 2; ubPlane < std::min(ubBpp, uint8_t(4)); ++ubPlane) {
				uint8_t ubRaw = vDataRaw[ulRawPos++];
				TilePlanar.m_pPlanes[ubPlane][ubRow] = ubRaw << 8;
			}
		}
		auto pTileChunky = std::make_shared<tChunkyBitmap>(TilePlanar, Palette);
		vTiles.push_back(pTileChunky);
	}
	return vTiles;
}

std::vector<std::shared_ptr<tChunkyBitmap>> extractIndexedTiles(
	tAssetBytes vDataRaw, uint8_t ubBpp
)
{
	static const tPalette PaletteIndexed({
		tRgb(0), tRgb(1), tRgb(2), tRgb(3), tRgb(4), tRgb(5), tRgb(6), tRgb(7),
		tRgb(8), tRgb(9), tRgb(10), tRgb(11), tRgb(12), tRgb(13), tRgb(14), tRgb(15)
	});
	return extractTiles(vDataRaw, ubBpp, PaletteIndexed);
}

//-------------------------------------------------------------- EXTRACT: EFFECT
static const uint32_t s_ulOffsEffectStart = 0xC2418;
static const uint32_t s_ulOffsEffectEnd = 0xC4818;
static const tPalette s_PaletteEffect({
	tRgb(0x000001), tRgb(0x0060f8),
	tRgb(0x0030c8), tRgb(0x001090),
	tRgb(0x000001), tRgb(0xf80000),
	tRgb(0xc00800), tRgb(0x901000),
	tRgb(0x581000), tRgb(0xf8e030),
	tRgb(0xd8a018), tRgb(0xc06808),
	tRgb(0xd8d8d8), tRgb(0x989898),
	tRgb(0x606060), tRgb(0x303030)
});

//-------------------------------------------------------------- EXTRACT: SHADOW
// C5128

//------------------------------------------------------------------------- CODE

void printUsage(const std::string &szAppName)
{
	fmt::print("Usage:\n\t{} romPath outPath\n", szAppName);
}

[[nodiscard]]
static tAssetBytes extractAsset(std::ifstream &FileRom, uint32_t ulOffsStart, uint32_t ulSize) {
	tAssetBytes vContents(ulSize, 0x00);
	FileRom.seekg(ulOffsStart, std::ios::beg);
	FileRom.read(reinterpret_cast<char*>(vContents.data()), ulSize);
	return vContents;
}

[[nodiscard]]
static tAssetBytes decompressAsset(tAssetBytes vAssetData) {
	auto Reader = tVectorReader(vAssetData);

	uint16_t uwDecompressedSize = Reader.readU16();
	uint8_t ubRepeatBits;
	if(uwDecompressedSize == 0) {
		throw std::runtime_error(fmt::format(FMT_STRING("Decompressed size = %hu"), uwDecompressedSize));
	}

	// Decompression algorithm depends on the first 4096 bytes being set to zero.
	tRleTable RleTable;
	tAssetBytes vDecoded;
	vDecoded.reserve(uwDecompressedSize);

	bool wasCopy = false;
	do {
		if(wasCopy) {
			fmt::print("\n");
		}
		wasCopy = false;
		ubRepeatBits = Reader.readU8();

		for(uint8_t ubBit = 0; ubBit < 8 && vDecoded.size() < uwDecompressedSize; ++ubBit) {
			bool isCopy = ((ubRepeatBits & 1) == 1);
			ubRepeatBits >>= 1;
			if(isCopy) {
				// Fill with next byte as-is
				if(vDecoded.size() >= uwDecompressedSize) {
					throw std::runtime_error(fmt::format(FMT_STRING("Write out of buffer bounds")));
				}

				std::uint8_t ubReadValue;
				ubReadValue = Reader.readU8();
				vDecoded.push_back(ubReadValue);
				RleTable.writeValue(ubReadValue);

				if(!wasCopy) {
					fmt::print("raw byte: ");
					wasCopy = true;
				}

				fmt::print(FMT_STRING("{:02X} "), ubReadValue);
			}
			else {
				// Decompress stuff
				uint16_t uwDecompressControl;
				uwDecompressControl = Reader.readU16();
				uint16_t uwRlePos = uwDecompressControl & 0xFFF;
				uint16_t uwRlePosEnd = ((uwDecompressControl >> 12) + 3 + uwRlePos) & 0x0FFF;

				if(wasCopy) {
					fmt::print("\n");
					wasCopy = false;
				}
				fmt::print(
					FMT_STRING("Decompress cmd: {:04X}, copying in bytes at range {}..{}\n"),
					uwDecompressControl, uwRlePos, uwRlePosEnd
				);

				while(uwRlePos != uwRlePosEnd) {
					if(vDecoded.size() >= uwDecompressedSize) {
						throw std::runtime_error(fmt::format(FMT_STRING("Write out of buffer bounds")));
					}

					std::uint8_t ubReadValue = RleTable.readValue(&uwRlePos);
					vDecoded.push_back(ubReadValue);
					RleTable.writeValue(ubReadValue);
				}
			}
		}
	} while(vDecoded.size() < uwDecompressedSize);

	vDecoded.resize(uwDecompressedSize);
	return vDecoded;
}

[[nodiscard]]
static uint32_t snesAddressToRomOffset(uint32_t ulBaseAddress) {
	ulBaseAddress &= 0x3FFFFF;
	uint32_t ulBank = ulBaseAddress >> 16;
	uint32_t ulAddress = ulBaseAddress & 0xFFFF;

	uint32_t ulRomOffset;
	if((ulBank & 1) == 0) {
		// Even
		ulBank /= 2;
		ulAddress -= 0x8000;
		ulRomOffset = (ulBank << 16) | ulAddress;
	}
	else {
		// Odd
		ulBank /= 2;
		ulRomOffset = (ulBank << 16) | ulAddress;
	}

	return ulRomOffset;
}

void readPakFileToc(std::ifstream &FileRom, std::vector<tAssetTocEntry> &vToc)
{
	// Read asset TOC
	auto PackFileAddr = g_PalRomMetadata.m_ulPakAddress;
	auto PackFileAddrRom = snesAddressToRomOffset(PackFileAddr);
	FileRom.seekg(PackFileAddrRom, std::ios::beg);
	uint32_t ulFirstOffs;
	FileRom.read(reinterpret_cast<char *>(&ulFirstOffs), sizeof(ulFirstOffs));
	auto AssetCount = ulFirstOffs / sizeof(ulFirstOffs);
	FileRom.seekg(PackFileAddrRom, std::ios::beg);
	fmt::print("Found {} assets at 0x{:06X}, rom offs 0x{:06X}:\n", AssetCount, PackFileAddr, PackFileAddrRom);

	for (uint32_t i = 0; i < AssetCount; ++i)
	{
		// Read file offset
		uint32_t ulFileOffset;
		FileRom.read(reinterpret_cast<char *>(&ulFileOffset), sizeof(ulFileOffset));
		uint32_t ulOffsEntryNext = uint32_t(FileRom.tellg());
		auto OffsCpu = ulFileOffset + PackFileAddr;
		auto OffsRom = snesAddressToRomOffset(OffsCpu);

		if (i > 0)
		{
			// Update deduced size on previous entry
			uint32_t ulSizeInRom = OffsRom - vToc[i - 1].ulOffs;
			vToc[i - 1].ulSizeInRom = ulSizeInRom;
		}
		vToc.push_back({.ulOffs = OffsRom, .ulSizeInRom = 0});
		FileRom.seekg(ulOffsEntryNext, std::ios::beg);
	}
	// Last entry size
	vToc.back().ulSizeInRom = g_PalRomMetadata.m_uwLastPakFileSize;
}

[[nodiscard]]
std::string getAssetPath(const std::string &BasePath, std::uint16_t uwAssetIndex) {
	auto szAssetName = g_PalRomMetadata.m_PakFileEntries[uwAssetIndex];
	auto OutPath = fmt::format(
		FMT_STRING("{}/{:03d}_{}.dat"),
		BasePath, uwAssetIndex,
		(szAssetName == nullptr) ? "unk" : szAssetName
	);
	return OutPath;
}

[[nodiscard]]
std::vector<tAssetBytes> extractRawAssets(
	std::ifstream &FileRom,
	const std::string &OutDirPath,
	bool isDump = false
) {

	std::vector<tAssetTocEntry> vRomPakToc;
	readPakFileToc(FileRom, vRomPakToc);

	std::vector<tAssetBytes> vAssets;
	vAssets.reserve(vRomPakToc.size());
	for(std::uint16_t i = 0; i < vRomPakToc.size(); ++i) {
		auto &TocEntry = vRomPakToc[i];

		tAssetBytes vAssetContents = extractAsset(
			FileRom,
			TocEntry.ulOffs,
			TocEntry.ulSizeInRom
		);
		vAssets.push_back(vAssetContents);

		if(isDump && vAssetContents.size() != 0) {
			std::string OutPath = getAssetPath(OutDirPath, i);
			fmt::print(FMT_STRING("Writing asset {}...\n"), OutPath);

			std::ofstream FileOut;
			FileOut.open(OutPath,	std::ios::binary);
			FileOut.write(reinterpret_cast<char*>(vAssetContents.data()), vAssetContents.size());
			FileOut.close();
		}
	}

	return vAssets;
}

void writeTiles(
	const std::vector<std::shared_ptr<tChunkyBitmap>> &vTiles, std::string OutPath,
	std::uint8_t uwFirstOutTileIndex
) {
	nFs::dirCreate(OutPath);

	for(auto i = 0; i < vTiles.size(); ++i) {
		vTiles[i]->toPng(fmt::format("{}/{}.png", OutPath, uwFirstOutTileIndex + i));
	}
}

void writeTilesToPng(
	const std::vector<std::shared_ptr<tChunkyBitmap>> &vTiles, std::string OutPath
) {
	std::uint16_t uwTotalHeight = 0;
	for(auto i = 0; i < vTiles.size(); ++i) {
		uwTotalHeight += vTiles[i]->m_uwHeight;
	}

	tChunkyBitmap Merged(vTiles[0]->m_uwWidth, uwTotalHeight);
	std::uint16_t uwDstY = 0;
	for(auto i = 0; i < vTiles.size(); ++i) {
		vTiles[i]->copyRect(0, 0, Merged, 0, uwDstY, vTiles[i]->m_uwWidth, vTiles[i]->m_uwHeight);
		uwDstY += vTiles[i]->m_uwHeight;
	}

	Merged.toPng(OutPath);
}

[[nodiscard]]
std::vector<std::shared_ptr<tChunkyBitmap>> composeTiles(
	const std::vector<std::shared_ptr<tChunkyBitmap>> &vTiles,
	std::uint8_t ubTileWidth, std::uint8_t ubTileHeight
) {
	auto MiniTilesPerTile = ubTileWidth * ubTileHeight;
	auto TileCount = vTiles.size() / (MiniTilesPerTile);

	std::vector<std::shared_ptr<tChunkyBitmap>> vMerged;
	vMerged.reserve(TileCount);

	for(auto i = 0; i < TileCount; ++i) {
		auto Merged = std::make_shared<tChunkyBitmap>(ubTileWidth * 8, ubTileHeight * 8);
		vMerged.push_back(Merged);

		uint8_t ubMergeListPos = 0;
		auto MiniTileIndex = i * MiniTilesPerTile;
		for(uint8_t ubY = 0; ubY < ubTileHeight; ++ubY) {
			for(uint8_t ubX = 0; ubX < ubTileWidth; ++ubX) {
				vTiles.at(MiniTileIndex)->copyRect(0, 0, *Merged, ubX * 8, ubY * 8, 8, 8);
				++MiniTileIndex;
				++ubMergeListPos;
			}
		}
	}

	return vMerged;
}

[[nodiscard]]
std::vector<std::array<std::uint16_t, 4>> extractTileDefs(tAssetBytes Bytes)
{
	std::vector<std::array<std::uint16_t, 4>> TileDefs;
	auto EntrySize = 4 * sizeof(std::uint16_t);
	auto DefCount = Bytes.size() / EntrySize;
	for(auto i = 0; i < DefCount; ++i) {
		auto TileDef = std::to_array({
			static_cast<std::uint16_t>(Bytes[i * EntrySize + 0] | (Bytes[i * EntrySize + 1] << 8)),
			static_cast<std::uint16_t>(Bytes[i * EntrySize + 2] | (Bytes[i * EntrySize + 3] << 8)),
			static_cast<std::uint16_t>(Bytes[i * EntrySize + 4] | (Bytes[i * EntrySize + 5] << 8)),
			static_cast<std::uint16_t>(Bytes[i * EntrySize + 6] | (Bytes[i * EntrySize + 7] << 8)),
		});
		TileDefs.push_back(TileDef);
	}

	return TileDefs;
}

[[nodiscard]]
tChunkyBitmap mirrorTile(const tChunkyBitmap &Source, bool isUpDown)
{
	constexpr auto TileWidth = 8;
	tChunkyBitmap Dst(Source.m_uwWidth, Source.m_uwHeight);
	if(isUpDown) {
		for(auto Y = 0; Y < Source.m_uwHeight; ++Y) {
			for(auto X = 0; X < TileWidth; ++X) {
				Dst.pixelAt(X, Y) = Source.pixelAt(X, (Source.m_uwHeight - 1) - Y);
			}
		}
	}
	else {
		for(auto Y = 0; Y < Source.m_uwHeight; ++Y) {
			for(auto X = 0; X < TileWidth; ++X) {
				Dst.pixelAt(X, Y) = Source.pixelAt((TileWidth - 1) - X, Y);
			}
		}
	}
	return Dst;
}

bool composeMinitile(
	tChunkyBitmap &Tile,
	const std::vector<std::shared_ptr<tChunkyBitmap>> &vMiniTiles,
	std::uint16_t uwTileDef, std::uint8_t ubOffsX, std::uint8_t ubOffsY,
	const tPalette &Palette
)
{
	constexpr auto MinitileIndexMaskSize = 10;
	constexpr std::uint16_t uwMinitileIndexMask = (1 << MinitileIndexMaskSize) - 1;
	constexpr auto MinitileAttributeBitFlipX = 5;
	constexpr auto MinitileAttributeBitFlipY = 4;
	constexpr auto MinitileAttributeBitFront = 3;
	constexpr std::uint8_t ubAttributePaletteMask = 0b111; // using here 0b1111 yields bad data

	auto Index = uwTileDef & uwMinitileIndexMask;
	auto Attribute = uwTileDef >> MinitileIndexMaskSize;
	auto Palette_index = Attribute & ubAttributePaletteMask;
	auto &Minitile_data = vMiniTiles[Index];

	auto Minitile = *vMiniTiles[Index];
	if((Attribute & (1 << (MinitileAttributeBitFlipY))) != 0) {
			Minitile = mirrorTile(Minitile, false);
	}
	if((Attribute & (1 << (MinitileAttributeBitFlipX))) != 0) {
			Minitile = mirrorTile(Minitile, true);
	}
	bool IsFront = ((Attribute & (1 << (MinitileAttributeBitFront))) != 0);
	Minitile.copyRect(0, 0, Tile, ubOffsX, ubOffsY, 8, 8);

	// TODO: use colors from level palette, convert colors later on
	for(std::uint8_t ubY = 0; ubY < 8; ++ubY) {
		for(std::uint8_t ubX = 0; ubX < 8; ++ubX) {
			Tile.pixelAt(ubOffsX + ubX, ubOffsY + ubY) = Palette.m_vColors[Tile.pixelAt(ubOffsX + ubX, ubOffsY + ubY).ubB];
		}
	}

	return IsFront;
}

[[nodiscard]]
std::vector<std::shared_ptr<tChunkyBitmap>> composeWorldTiles(
	const std::vector<std::shared_ptr<tChunkyBitmap>> &vMiniTiles,
	const std::vector<std::array<std::uint16_t, 4>> &TileDefs,
	const tPalette &Palette,
	std::vector<bool> &r_TileFrontFlags
)
{
	std::vector<std::shared_ptr<tChunkyBitmap>> worldTiles;
	for(const auto &TileDef: TileDefs) {
		auto Tile = std::make_shared<tChunkyBitmap>(16, 16);
		auto isFront = composeMinitile(
			*Tile, vMiniTiles, TileDef[0],
			0, 0, Palette
		);
		isFront |= composeMinitile(
			*Tile, vMiniTiles, TileDef[1],
			8, 0, Palette
		);
		isFront |= composeMinitile(
			*Tile, vMiniTiles, TileDef[2],
			0, 8, Palette
		);
		isFront |= composeMinitile(
			*Tile, vMiniTiles, TileDef[3],
			8, 8, Palette
		);
		worldTiles.push_back(Tile);
		r_TileFrontFlags.push_back(isFront);
	}

	return worldTiles;
}

[[nodiscard]]
std::vector<std::shared_ptr<tChunkyBitmap>> remapTiles(
	const std::vector<std::shared_ptr<tChunkyBitmap>> &vTiles,
	std::span<const std::uint16_t> RemapIndices
) {
	std::vector<std::shared_ptr<tChunkyBitmap>> vRemapped;
	vRemapped.reserve(RemapIndices.size());
	for(auto index: RemapIndices) {
		vRemapped.push_back(vTiles[index]);
	}

	return vRemapped;
}

[[nodiscard]]
tPalette loadPalette(const tAssetBytes &AssetBytes) {
	std::uint16_t uwReadPos = 0;
	auto readU16 = [&uwReadPos, &AssetBytes]() -> std::uint16_t {
		std::uint16_t uwValue = 0;
		uwValue |= AssetBytes[uwReadPos++];
		uwValue |= AssetBytes[uwReadPos++] << 8;
		return uwValue;
	};

	tPalette Palette;
	auto ColorCount = AssetBytes.size() / 2;
	for(auto i = 0; i < ColorCount; ++i) {
		std::uint16_t uwSnesColor = readU16();
    std::uint8_t ubR = (((uwSnesColor >> 0) & 0b11111) * 255 / 0b11111);
    std::uint8_t ubG = (((uwSnesColor >> 5) & 0b11111) * 255 / 0b11111);
    std::uint8_t ubB = (((uwSnesColor >> 10) & 0b11111) * 255 / 0b11111);
		Palette.m_vColors.push_back(tRgb(ubR, ubG, ubB));
	}

	return Palette;
}

[[nodiscard]]
tPalette amigafyPalette(const tPalette &PaletteIn) {
	tPalette PaletteOut;
	for(const auto &Color: PaletteIn.m_vColors) {
		std::uint8_t ubR = ((Color.ubR + 8) / 17) * 17;
		std::uint8_t ubG = ((Color.ubG + 8) / 17) * 17;
		std::uint8_t ubB = ((Color.ubB + 8) / 17) * 17;
		PaletteOut.m_vColors.push_back(tRgb(ubR, ubG, ubB));
	}
	return PaletteOut;
}

void convertAssets(
	const std::vector<tAssetBytes> &vRawAssets,
	const std::string &ConvertedAssetPath
) {
	auto PaletteHud = amigafyPalette(loadPalette(decompressAsset(
		vRawAssets[g_PalRomMetadata.getAssetIndexByName("palette_hud")]
	)));
	auto PaletteTiles = amigafyPalette(loadPalette(decompressAsset(
		vRawAssets[g_PalRomMetadata.getAssetIndexByName("palette_hud")]
	)));
	PaletteTiles.m_vColors[0] = tRgb(255, 0, 255);

	writeTilesToPng(composeTiles(
		extractTiles(vRawAssets[g_PalRomMetadata.getAssetIndexByName("hud_items")], 4, PaletteHud),
		2, 2
	), ConvertedAssetPath +"/hud_items.png");

	static const auto Portraits = std::to_array({
		"hud_portrait_erik_active",
		"hud_portrait_baelog_active",
		"hud_portrait_olaf_active",
		"hud_portrait_fang_active",
		"hud_portrait_scorch_active",
		"hud_portrait_erik_inactive",
		"hud_portrait_baelog_inactive",
		"hud_portrait_olaf_inactive",
		"hud_portrait_fang_inactive",
		"hud_portrait_scorch_inactive",
		"hud_portrait_erik_dead",
		"hud_portrait_baelog_dead",
		"hud_portrait_olaf_dead",
		"hud_portrait_scorch_dead",
		"hud_portrait_fang_dead",
		"hud_portrait_unk1",
	});

	std::vector<std::shared_ptr<tChunkyBitmap>> vPortraits;
	for(std::uint8_t i = 0; i < Portraits.size(); ++i) {
		auto Portrait = composeTiles(
			extractTiles(vRawAssets[g_PalRomMetadata.getAssetIndexByName(Portraits[i])], 4, PaletteHud),
			4, 3
		).front();
		vPortraits.push_back(Portrait);
	}
	writeTilesToPng(vPortraits, ConvertedAssetPath + "/hud_portraits.png");

	static const auto HudBorderRemap = std::to_array<std::uint16_t>({
		129, 130, 133, 134, 137, 139, 141, 142, 145, 148,
		131, 132, 135, 136, 138, 140, 143, 144, 146, 147,
		149, 150, 0, 1, 2, 9, 10, 28, 29, 30,
		60, 61, 62, 94, 96, 97, 98, 101, 106, 125,
		126, 127,
	});

	composeTiles(remapTiles(extractTiles(
		decompressAsset(vRawAssets[g_PalRomMetadata.getAssetIndexByName("hud_border")]),
		4, PaletteHud
	), HudBorderRemap), 2, HudBorderRemap.size() / 2).front()->toPng(ConvertedAssetPath +"/hud_border.png");

	composeTiles(extractTiles(
		decompressAsset(vRawAssets[g_PalRomMetadata.getAssetIndexByName("hud_cursor_up")]),
		4, PaletteHud
	), 2, 1).front()->toPng(ConvertedAssetPath + "/hud_cursor.png");

	PaletteHud.toPlt(ConvertedAssetPath + "/hud.plt");

	tPalette PaletteFont(std::vector<tRgb> {tRgb(0xFF00FF), tRgb(0x000000), tRgb(0xEEEEEE), tRgb(0xFF00FF)});
	auto FontTiles = composeTiles(extractTiles(
		decompressAsset(vRawAssets[g_PalRomMetadata.getAssetIndexByName("font")]),
		2, PaletteFont
	), 2, 1);
	writeTilesToPng(FontTiles, ConvertedAssetPath + "/font.png");

	auto HelpBox = composeTiles(extractTiles(
		decompressAsset(vRawAssets[g_PalRomMetadata.getAssetIndexByName("interact_help_box")]),
		4, PaletteHud
	), 2, 2).front()->toPng(ConvertedAssetPath + "/help_box.png");

	auto TileDefsWorld1 = extractTileDefs(decompressAsset(vRawAssets[g_PalRomMetadata.getAssetIndexByName("tiledef_w1")]));
	auto MiniTilesWorld1 = extractIndexedTiles(decompressAsset(vRawAssets[g_PalRomMetadata.getAssetIndexByName("tileset_w1")]), 4);
	std::vector<bool> FrontFlagsW1;
	auto TilesWorld1 = composeWorldTiles(MiniTilesWorld1, TileDefsWorld1, PaletteTiles, FrontFlagsW1);
	writeTilesToPng(TilesWorld1, ConvertedAssetPath + "/tiles_w1.png");

	auto LevelDefW1A0 = loadLevelDef(decompressAsset(vRawAssets[g_PalRomMetadata.getAssetIndexByName("level_w1_a0_strt_defs")]));
	auto BackgroundW1 = extractTileMap(
		decompressAsset(vRawAssets[LevelDefW1A0->Header.uwBackgroundFileIndex]),
		LevelDefW1A0->Header.uwBackgroundWidth,
		LevelDefW1A0->Header.uwBackgroundHeight
	);
	auto TilesW1A0 = extractTileMap(
	decompressAsset(vRawAssets[LevelDefW1A0->Header.uwTilemapFileIndex]),
		LevelDefW1A0->Header.uwTileWidth,
		LevelDefW1A0->Header.uwTileHeight
	);
	transcodeLevelTiles(
		TilesW1A0, BackgroundW1, FrontFlagsW1,
		ConvertedAssetPath + "/tilemap_w1_a0.dat"
	);
}

} // namespace AmiLostVikings2::AssetExtract

using namespace AmiLostVikings2::AssetExtract;

int main(int lArgCount, const char *pArgs[])
{
	const uint8_t ubMandatoryArgCnt = 2;
	if(lArgCount - 1 < ubMandatoryArgCnt) {
		nLog::error("Too few arguments, expected {}", ubMandatoryArgCnt);
		printUsage(pArgs[0]);
		return EXIT_FAILURE;
	}

	std::string RomPath = pArgs[1], OutDirPath = pArgs[2];
	std::string RawAssetPath = OutDirPath + "/raw";
	std::string ConvertedAssetPath = OutDirPath + "/converted";
	nFs::dirCreate(OutDirPath);
	nFs::dirCreate(RawAssetPath);
	nFs::dirCreate(ConvertedAssetPath);

	std::ifstream FileRom;
	FileRom.open(RomPath.c_str(), std::ifstream::binary);
	if(!FileRom.good()) {
		nLog::error("Couldn't open ROM file at '{}'", RomPath);
		return EXIT_FAILURE;
	}

	// TODO: verify ROM size/checksum/header

	try {
		auto Assets = extractRawAssets(FileRom, RawAssetPath);
		convertAssets(Assets, ConvertedAssetPath);
	}
	catch(const std::exception &Exc) {
		fmt::print("Unhandled exception: '{}'!\n", Exc.what());
		return EXIT_FAILURE;
	}

	fmt::print("All done!\n");
	return EXIT_SUCCESS;
}
