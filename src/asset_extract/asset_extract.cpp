/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include <fstream>
#include <filesystem>
#include <map>
#include <optional>

#include "logging.h"
#include "fs.h"
#include "rgb.h"
#include "bitmap.h"
#include "rle_table.hpp"
#include "rom_metadata.hpp"

namespace AmiLostVikings2::AssetExtract {

struct tAssetDef {
	std::string AssetName;
	std::optional<bool> isCompressed;
};

using tAssetBytes = std::vector<std::uint8_t>;

static const tPalette s_PaletteErik({
	tRgb(0xff00ff), tRgb(0xf8a870),
	tRgb(0xb86830), tRgb(0x703808),
	tRgb(0x301000), tRgb(0xd8d8e8),
	tRgb(0x8890a8), tRgb(0x505070),
	tRgb(0x202038), tRgb(0xc83008),
	tRgb(0x780800), tRgb(0x300000),
	tRgb(0x6858f0), tRgb(0x3028a8),
	tRgb(0x100868), tRgb(0xf0d000)
});

static const tPalette s_PaletteBaelog({
	tRgb(0xff00ff), tRgb(0xf8a870),
	tRgb(0xb06838), tRgb(0x784018),
	tRgb(0x482000), tRgb(0xf8f8f8),
	tRgb(0xc8c8c8), tRgb(0x787878),
	tRgb(0x383838), tRgb(0xf8f800),
	tRgb(0xb8b800), tRgb(0x888800),
	tRgb(0x0080f8), tRgb(0x0050b8),
	tRgb(0x001890), tRgb(0x000001)
});

static const tPalette s_PaletteFang({
	tRgb(0xff00ff), tRgb(0xe0e0e0),
	tRgb(0x989898), tRgb(0x505050),
	tRgb(0xc8c800), tRgb(0xb08000),
	tRgb(0xe0a880), tRgb(0xb88050),
	tRgb(0x906030), tRgb(0x704010),
	tRgb(0x482000), tRgb(0x201000),
	tRgb(0x000001), tRgb(0xf80000),
	tRgb(0xa00000), tRgb(0x500000)
});

static const tPalette s_PaletteScorch({ // TODO: proper palette
	tRgb(0xff00ff), tRgb(0xB5FF00),
	tRgb(0x8CD600), tRgb(0x63B500),
	tRgb(0x428C00), tRgb(0x296B00),
	tRgb(0x004A00), tRgb(0x002900),
	tRgb(0x000000), tRgb(0xD6D6D6),
	tRgb(0x8C8C8C), tRgb(0x424242),
	tRgb(0xDEDE00), tRgb(0xA59C00),
	tRgb(0x635A00), tRgb(0x292100)
});

static const tPalette s_PaletteOlaf({
	tRgb(0xff00ff), tRgb(0xf8a870),
	tRgb(0xb86830), tRgb(0x703808),
	tRgb(0xe8e8e8), tRgb(0xb8b8b8),
	tRgb(0x888888), tRgb(0x585860),
	tRgb(0x383838), tRgb(0xf8e800),
	tRgb(0xc09000), tRgb(0x0030a8),
	tRgb(0x0080f8), tRgb(0x0038c0),
	tRgb(0x001088), tRgb(0x101010)
});

struct tMergeRule {
	uint8_t m_ubTileWidth;
	uint8_t m_ubTileHeight;
	std::string m_Name;
	std::vector<uint32_t> m_vTileIndices;

	tMergeRule(
		uint8_t ubTileWidth, uint8_t ubTileHeight,
		const std::string &Name, uint32_t ulFirstTile
	):
		m_ubTileWidth(ubTileWidth), m_ubTileHeight(ubTileHeight),
		m_Name(Name)
	{
		for(uint32_t i = ulFirstTile; i < ulFirstTile + ubTileHeight * ubTileWidth; ++i) {
			m_vTileIndices.push_back(i);
		}
	}

	tMergeRule(
		uint8_t ubTileWidth, uint8_t ubTileHeight,
		const std::string &Name, const std::vector<uint32_t> &vTileIndices
	):
		m_ubTileWidth(ubTileWidth), m_ubTileHeight(ubTileHeight),
		m_Name(Name), m_vTileIndices(vTileIndices)
	{

	}
};

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

//------------------------------------------------------ ASSET PROCESS CALLBACKS

// void handleExtractTileset(
// 	const tAssetBytes &vDataUnprocessed, const std::string &PathOut
// )
// {
// 	const std::uint8_t ubBpp = 4;
// 	std::vector<tRgb> vColors;
// 	for(std::uint16_t i = 0; i < (1 << ubBpp); ++i) {
// 		std::uint8_t ubComponent = 0xFF * i / ((1 << ubBpp) - 1);
// 		vColors.push_back(tRgb(ubComponent, ubComponent, ubComponent));
// 	}
// 	tPalette Palette(vColors);

// 	std::filesystem::create_directories(PathOut);
// 	uint32_t ulFrameByteSize = (8 * 8 * ubBpp) / 8; // w * h * bpp / bitsInByte
// 	uint32_t ulFrameCount = uint32_t(vDataUnprocessed.size()) / ulFrameByteSize;

// 	std::vector<tMergeRule> vMergeRules;
// 	for(uint32_t i = 0; i < ulFrameCount; ++i) {
// 		vMergeRules.push_back(tMergeRule(
// 			1, 1, fmt::format(FMT_STRING("{}"), i), i
// 		));
// 	}
// 	extractTiles(vDataUnprocessed, ubBpp, vMergeRules, Palette, PathOut);
// }

// void handleExtractFont(
// 	const tAssetBytes &vDataUnprocessed, const std::string &PathOut
// ) {
// 	std::filesystem::create_directories(PathOut);

// 	tPalette Palette(std::vector<tRgb> {tRgb(0x000000), tRgb(0x555555), tRgb(0xAAAAAA), tRgb(0xFFFFFF)});
// 	std::vector<tMergeRule> vMergeRules;
// 	for(uint8_t i = 0; i < 80; ++i) {
// 		vMergeRules.push_back(tMergeRule(1, 1, fmt::format(FMT_STRING("{}"), i), i));
// 	}
// 	extractTiles(vDataUnprocessed, 2, vMergeRules, Palette, PathOut);
// }

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
	uint16_t uwReadPos = 0;

	auto readU8 = [&uwReadPos, &vAssetData]() -> std::uint8_t {
		std::uint8_t ubValue = vAssetData[uwReadPos++];
		return ubValue;
	};

	auto readU16 = [&uwReadPos, &vAssetData]() -> std::uint16_t {
		std::uint16_t uwValue = 0;
		uwValue |= vAssetData[uwReadPos++];
		uwValue |= vAssetData[uwReadPos++] << 8;
		return uwValue;
	};

	uint16_t uwDecompressedSize = readU16();
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
		ubRepeatBits = readU8();

		for(uint8_t ubBit = 0; ubBit < 8 && vDecoded.size() < uwDecompressedSize; ++ubBit) {
			bool isCopy = ((ubRepeatBits & 1) == 1);
			ubRepeatBits >>= 1;
			if(isCopy) {
				// Fill with next byte as-is
				if(vDecoded.size() >= uwDecompressedSize) {
					throw std::runtime_error(fmt::format(FMT_STRING("Write out of buffer bounds")));
				}

				std::uint8_t ubReadValue;
				ubReadValue = readU8();
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
				uwDecompressControl = readU16();
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

struct tAssetTocEntry {
	uint32_t ulOffs;
	uint32_t ulSizeInRom;
};

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

std::string getAssetPath(const std::string &BasePath, std::uint16_t uwAssetIndex) {
	auto szAssetName = g_PalRomMetadata.m_PakFileEntries[uwAssetIndex];
	auto OutPath = fmt::format(
		FMT_STRING("{}/{:03d}_{}.dat"),
		BasePath, uwAssetIndex,
		(szAssetName == nullptr) ? "unk" : szAssetName
	);
	return OutPath;
}

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

tChunkyBitmap mirrorBitmap(const tChunkyBitmap &Source, bool isUpDown)
{
	tChunkyBitmap Dst(Source.m_uwWidth, Source.m_uwHeight);
	if(isUpDown) {
		for(auto X = 0; X < Source.m_uwWidth; ++X) {
			for(auto Y = 0; Y < Source.m_uwHeight; ++Y) {
				Dst.pixelAt(X, Y) = Source.pixelAt(X, (Source.m_uwHeight - 1) - Y);
			}
		}
	}
	else {
		for(auto X = 0; X < Source.m_uwWidth; ++X) {
			for(auto Y = 0; Y < Source.m_uwHeight; ++Y) {
				Dst.pixelAt(X, Y) = Source.pixelAt((Source.m_uwWidth - 1) - X, Y);
			}
		}
	}
	return Dst;
}

void composeMinitile(
	tChunkyBitmap &Tile,
	const std::vector<std::shared_ptr<tChunkyBitmap>> &vMinitiles,
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
	auto &Minitile_data = vMinitiles[Index];

	auto Minitile = *vMinitiles[Index];
	if((Attribute & (1 << (MinitileAttributeBitFlipY))) != 0) {
			Minitile = mirrorBitmap(Minitile, false);
	}
	if((Attribute & (1 << (MinitileAttributeBitFlipX))) != 0) {
			Minitile = mirrorBitmap(Minitile, true);
	}
	bool IsFront = ((Attribute & (1 << (MinitileAttributeBitFront))) != 0);
	Minitile.copyRect(0, 0, Tile, ubOffsX, ubOffsY, 8, 8);

	// TODO: use colors from level palette, convert colors later on
	for(std::uint8_t ubY = 0; ubY < 8; ++ubY) {
		for(std::uint8_t ubX = 0; ubX < 8; ++ubX) {
			Tile.pixelAt(ubOffsX + ubX, ubOffsY + ubY) = Palette.m_vColors[Tile.pixelAt(ubOffsX + ubX, ubOffsY + ubY).ubB];
		}
	}
}

std::vector<std::shared_ptr<tChunkyBitmap>> composeWorldTiles(
	const std::vector<std::shared_ptr<tChunkyBitmap>> &vMiniTiles,
	const std::vector<std::array<std::uint16_t, 4>> &TileDefs,
	const tPalette &Palette
)
{
	std::vector<std::shared_ptr<tChunkyBitmap>> worldTiles;
	for(const auto &TileDef: TileDefs) {
		auto Tile = std::make_shared<tChunkyBitmap>(16, 16);
		composeMinitile(*Tile, vMiniTiles, TileDef[0], 0, 0, Palette);
		composeMinitile(*Tile, vMiniTiles, TileDef[1], 8, 0, Palette);
		composeMinitile(*Tile, vMiniTiles, TileDef[2], 0, 8, Palette);
		composeMinitile(*Tile, vMiniTiles, TileDef[3], 8, 8, Palette);
		worldTiles.push_back(Tile);
	}

	return worldTiles;
}

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

	auto tileDefsWorld1 = extractTileDefs(decompressAsset(vRawAssets[g_PalRomMetadata.getAssetIndexByName("tiledef_w1")]));
	auto miniTilesWorld1 = extractIndexedTiles(decompressAsset(vRawAssets[g_PalRomMetadata.getAssetIndexByName("tileset_w1")]), 4);
	auto tilesWorld1 = composeWorldTiles(miniTilesWorld1, tileDefsWorld1, PaletteHud);
	writeTilesToPng(tilesWorld1, ConvertedAssetPath + "/tiles_w1.png");
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
