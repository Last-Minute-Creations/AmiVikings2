/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "logging.h"
#include "fs.h"
#include "rgb.h"
#include "bitmap.h"
#include <fstream>
#include <map>

const std::map<uint32_t, std::string> mOffsToFileName = {
	{0xED14B, "logo_lv2_a"},
	{0xEDB38, "logo_lv2_b"},
	{0xE2583, "logo_interplay"},
	{0xE4D50, "logo_gameover_a"},
	{0xE3863, "logo_blizzard"},
	{0xC5BF1, "bomb"},
	{0xE9F06, "enemy_tomator"},
	{0xCAD87, "enemy_poof"},
	{0xCA8FC, "enemy_spikes"},
	{0xCB35F, "enemy_jelly"},
	{0xCBA0E, "enemy_fish"},
	{0xCCE03, "enemy_vamp"},
	{0xCC4D1, "enemy_skeleton"},
	{0xCDBE8, "enemy_knight"},
	{0xCEEA8, "enemy_wizard"},
	{0xCFB5F, "enemy_pirate"},
	{0xD0484, "enemy_corsair"},
	{0xD3582, "enemy_vine"},
	{0xF1420, "enemy_roboknight"},
	{0xD2328, "enemy_monkey"},
	{0xD1273, "enemy_montezuma"},
	{0xD3CBD, "enemy_robot"},
	{0xD4780, "enemy_xeno"},
	{0xC9A18, "obstacle_spike_ball_underwater"},
	{0xC9E5A, "obstacle_spike_ball_underwater_2"}, // other palette
	{0xC6730, "obstacle_spike_ball_underwater_3"}, // other palette
	{0xC95E7, "obstacle_spear_up"},
	{0xC9580, "obstacle_spear_left"},
	{0xC8143, "obstacle_drill_up"},
	{0xCA6CD, "obstacle_zap"},
	{0xCA5F5, "obstacle_zap2"},
	{0xEE368, "obstacle_zap3"},
	{0xC681D, "obstacle_spike_up"},
	{0xECD65, "obstacle_burning_rope"},
	{0x506BF, "font"},
	{0xC76D1, "door_bolted"},
	{0xC87E6, "door_skull"},
	{0xC7D97, "bounce_bone"},
	{0xC92DC, "dunno1"},
	{0xC8431, "dunno2"},
	{0xC8C13, "dunno3"},
	{0xC9C87, "dunno_ball"},
	{0xC63EB, "dunno_magic"},
	{0xC8D32, "dunno_ball2"},
	{0xCA894, "dunno4"},
	{0xE9D06, "dunno_water1"},
	{0xECB65, "dunno_water2"},
	{0xE9906, "dunno_water3"},
	{0xE9B06, "dunno_water4"},
	{0xEC965, "dunno_water5"},
	{0xC7B68, "gas"},
	{0xC5711, "items_keys"},
	{0xEFAEA, "cutscene_screens"},
	{0xF02D0, "cutscene_fist"},
	{0xF212C, "cutscene_lolipop"},
	{0xC8F07, "items_skull_voodoo"},
	{0xC55DF, "fart"},
	{0xC981D, "tile_block_crush"},
	{0xCA410, "items_pcb_battery_disk_burger"},
	{0xE6E1B, "npc_witch"},
	{0xE92F5, "npc_time_machine"},
	{0xE6FFD, "npc_mage"},
	{0xE7479, "npc_gypsy"},
	{0xE783B, "npc_shaman"},
	{0xE7C0E, "npc_connor"},
	{0xE822F, "npc_kid"},
	{0xC65E6, "interact_switch"},
	{0xC75DE, "platform_grass"},
	{0xC90FB, "platform_wood"},
	{0xCA0F7, "platform_future"},
	{0xC6538, "interact_button"},
	{0xEE50E, "bubbles"},
	{0xC7F04, "tile_elevator_updown1"},
	{0xC64C1, "cursor_hammer"},
	{0xE4D1C, "cursor_password"},
	{0xC85CC, "tile_corner1"},
	{0xC951F, "tile_corner2"},
	{0xF2368, "tile_corner3"},
	{0xCA3A2, "tile_elevator_updown2"},
	{0x509B9, "hud_border"},
	{0x50DAD, "hud_cursor_up"},
	{0xC56BB, "hud_cursor_down"},
	{0x5275B, "hud_hpbar"},
	{0x50DDB, "hud_portrait_erik_active"},
	{0x50F5B, "hud_portrait_baelog_active"},
	{0x510DB, "hud_portrait_olaf_active"},
	{0x5125B, "hud_portrait_fang_active"},
	{0x513DB, "hud_portrait_scorch_active"},
	{0x5155B, "hud_portrait_erik_inactive"},
	{0x516DB, "hud_portrait_baelog_inactive"},
	{0x5185B, "hud_portrait_olaf_inactive"},
	{0x51B5B, "hud_portrait_scorch_inactive"},
	{0x519DB, "hud_portrait_fang_inactive"},
	{0x51CDB, "hud_portrait_erik_dead"},
	{0x51E5B, "hud_portrait_baelog_dead"},
	{0x51FDB, "hud_portrait_olaf_dead"},
	{0x5215B, "hud_portrait_scorch_dead"},
	{0x522DB, "hud_portrait_fang_dead"},
	{0x5281B, "hud_items"},
	{0x5245B, "hud_portrait_unk1"},
	{0x525DB, "hud_portrait_unk2"},
	{0xC5418, "baelog_hand"},
	{0xC6038, "push_block"},
	{0xC9D76, "projectile_banana"},
	{0xC8E04, "projectile_rock"},
	{0xCF971, "projectile_magic"},
	{0xC5ABE, "projectile_fireball"},
	{0xCA7B3, "projectile_dunno1"},
	{0xC8D9B, "projectile_dunno2"},
	{0xC9F3C, "item_keycards"},
	{0xE6CC1, "item_machine_parts"},
	{0xE6D9E, "item_capacitor"},
	{0xCA001, "interact_keyslots"},
	{0xC5BA1, "tile_pipe"},
	{0x8CA18, "frames_erik1"},
	{0x94A18, "frames_erik2"},
	{0x98218, "frames_baelog1"},
	{0xA0018, "frames_baelog2"},
	{0xA3018, "frames_fang1"},
	{0xAA218, "frames_fang2"},
	{0xAD418, "frames_scorch1"},
	{0xB3818, "frames_scorch2"},
	{0xB6818, "frames_olaf1"},
	{0xBE618, "frames_olaf2"},
	{0xC1C18, "frames_special"},
	{0xD78E4, "continue_chars"},
	{0xE55FD, "continue_valkyrie"},
};

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

struct tRawTile {
	tChunkyBitmap m_Tile;
	bool m_isUsed;
	tRawTile(tChunkyBitmap Tile):
		m_Tile(Tile),
		m_isUsed(false)
	{

	}
};

//----------------------------------------------------------------- EXTRACT: HUD
static const uint32_t s_ulOffsHudStart = 0x50DDB;
static const uint32_t s_ulOffsHudEnd = 0x53A1B;
static const tPalette s_PaletteHud({
	tRgb(0x00, 0x00, 0x01), tRgb(0x00, 0x00, 0x01),
	tRgb(0xf0, 0xf0, 0xf0), tRgb(0x50, 0x50, 0x50),
	tRgb(0x90, 0x90, 0x90), tRgb(0x58, 0x58, 0x58),
	tRgb(0x30, 0x30, 0x30), tRgb(0x00, 0x00, 0xf8),
	tRgb(0x00, 0x68, 0x10), tRgb(0x28, 0xc0, 0x28),
	tRgb(0xf8, 0xf0, 0x00), tRgb(0xf8, 0xa8, 0x40),
	tRgb(0xd8, 0x80, 0x18), tRgb(0xb8, 0x58, 0x00),
	tRgb(0x80, 0x00, 0x00), tRgb(0xf8, 0x40, 0x10)
});
static const std::vector<tMergeRule> s_vMergeRulesHud = {
	tMergeRule(4, 3, "erik", 0),
	tMergeRule(4, 3, "baleog", 12),
	tMergeRule(4, 3, "olaf", 24),
	tMergeRule(4, 3, "fang", 36),
	tMergeRule(4, 3, "scorch", 48),
	tMergeRule(4, 3, "erik_inactive", 60 + 0),
	tMergeRule(4, 3, "baleog_inactive", 60 + 12),
	tMergeRule(4, 3, "olaf_inactive", 60 + 24),
	tMergeRule(4, 3, "fang_inactive", 60 + 36),
	tMergeRule(4, 3, "scorch_inactive", 60 + 48),
	tMergeRule(4, 3, "erik_dead", 120 + 0),
	tMergeRule(4, 3, "baleog_dead", 120 + 12),
	tMergeRule(4, 3, "olaf_dead", 120 + 24),
	tMergeRule(4, 3, "fang_dead", 120 + 36),
	tMergeRule(4, 3, "scorch_dead", 120 + 48),
	tMergeRule(4, 3, "unk1", 180),
	tMergeRule(4, 3, "unk2", 192),
	tMergeRule(6, 1, "hp_icons", 204),
	tMergeRule(2, 2, "item_trash", 210),
	tMergeRule(2, 2, "item_none", 214),
	tMergeRule(2, 2, "item_shield", 218),
	tMergeRule(2, 2, "item_bomb", 222),
	tMergeRule(2, 2, "item_nuke", 226),
	tMergeRule(2, 2, "item_key_red", 230),
	tMergeRule(2, 2, "item_key_skull", 234),
	tMergeRule(2, 2, "item_key_gold", 238),
	tMergeRule(2, 2, "item_card_red", 242),
	tMergeRule(2, 2, "item_card_blue", 246),
	tMergeRule(2, 2, "item_card_yellow", 250),
	tMergeRule(2, 2, "item_witch_eye", 254),
	tMergeRule(2, 2, "item_witch_shroom", 258),
	tMergeRule(2, 2, "item_witch_wing", 262),
	tMergeRule(2, 2, "item_mage_egg", 266),
	tMergeRule(2, 2, "item_mage_scroll", 270),
	tMergeRule(2, 2, "item_mage_staff", 274),
	tMergeRule(2, 2, "item_unk1_diamond", 278),
	tMergeRule(2, 2, "item_unk1_card", 282),
	tMergeRule(2, 2, "item_unk1_ball", 286),
	tMergeRule(2, 2, "item_unk2_skull", 290),
	tMergeRule(2, 2, "item_unk2_doll", 294),
	tMergeRule(2, 2, "item_unk2_horn", 298),
	tMergeRule(2, 2, "item_unk3_pcb", 302),
	tMergeRule(2, 2, "item_unk3_battery", 306),
	tMergeRule(2, 2, "item_unk3_cd", 310),
	tMergeRule(2, 2, "item_time_gear", 314),
	tMergeRule(2, 2, "item_time_lamp", 318),
	tMergeRule(2, 2, "item_time_cap", 322),
	tMergeRule(2, 2, "item_torch", 326),
	tMergeRule(2, 2, "item_ball", 330),
	tMergeRule(2, 2, "item_food_garlic", 334),
	tMergeRule(2, 2, "item_food_meat", 338),
	tMergeRule(2, 2, "item_food_beer", 342),
	tMergeRule(2, 2, "item_food_banana", 346),
	tMergeRule(2, 2, "item_food_burger", 350),
};

//---------------------------------------------------------------- EXTRACT: ERIK
static const uint32_t s_ulOffsErikStart = 0x8CA18;
static const uint32_t s_ulOffsErikEnd = 0x98218;
static const tPalette s_PaletteErik({
	tRgb(0x004000), tRgb(0xf8a870),
	tRgb(0xb86830), tRgb(0x703808),
	tRgb(0x301000), tRgb(0xd8d8e8),
	tRgb(0x8890a8), tRgb(0x505070),
	tRgb(0x202038), tRgb(0xc83008),
	tRgb(0x780800), tRgb(0x300000),
	tRgb(0x6858f0), tRgb(0x3028a8),
	tRgb(0x100868), tRgb(0xf0d000)
});

static const uint32_t s_ulOffsErikOldStart = 0xC4818;
static const uint32_t s_ulOffsErikOldEnd = 0xC4C18;

//-------------------------------------------------------------- EXTRACT: BAELOG
static const uint32_t s_ulOffsBaelogStart = 0x98218;
static const uint32_t s_ulOffsBaelogEnd = 0xA3018;
static const tPalette s_PaletteBaelog({
	tRgb(0x202058), tRgb(0xf8a870),
	tRgb(0xb06838), tRgb(0x784018),
	tRgb(0x482000), tRgb(0xf8f8f8),
	tRgb(0xc8c8c8), tRgb(0x787878),
	tRgb(0x383838), tRgb(0xf8f800),
	tRgb(0xb8b800), tRgb(0x888800),
	tRgb(0x0080f8), tRgb(0x0050b8),
	tRgb(0x001890), tRgb(0x000001)
});

static const uint32_t s_ulOffsBaelogOldStart = 0xC4C18;
static const uint32_t s_ulOffsBaelogOldEnd = 0xC5018;

//---------------------------------------------------------------- EXTRACT: FANG
static const uint32_t s_ulOffsFangStart = 0xA3018;
static const uint32_t s_ulOffsFangEnd = 0xAD418;
static const tPalette s_PaletteFang({
	tRgb(0x9800a0), tRgb(0xe0e0e0),
	tRgb(0x989898), tRgb(0x505050),
	tRgb(0xc8c800), tRgb(0xb08000),
	tRgb(0xe0a880), tRgb(0xb88050),
	tRgb(0x906030), tRgb(0x704010),
	tRgb(0x482000), tRgb(0x201000),
	tRgb(0x000001), tRgb(0xf80000),
	tRgb(0xa00000), tRgb(0x500000)
});

//-------------------------------------------------------------- EXTRACT: SCORCH
static const uint32_t s_ulOffsScorchStart = 0xAD418;
static const uint32_t s_ulOffsScorchEnd = 0xB6818;
static const tPalette s_PaletteScorch({ // TODO: proper palette
	tRgb(0x310031), tRgb(0xB5FF00),
	tRgb(0x8CD600), tRgb(0x63B500),
	tRgb(0x428C00), tRgb(0x296B00),
	tRgb(0x004A00), tRgb(0x002900),
	tRgb(0x000000), tRgb(0xD6D6D6),
	tRgb(0x8C8C8C), tRgb(0x424242),
	tRgb(0xDEDE00), tRgb(0xA59C00),
	tRgb(0x635A00), tRgb(0x292100)
});
//---------------------------------------------------------------- EXTRACT: OLAF
static const uint32_t s_ulOffsOlafStart = 0xB6818;
static const uint32_t s_ulOffsOlafEnd = 0xC1C18;
static const tPalette s_PaletteOlaf({
	tRgb(0x9800a0), tRgb(0xf8a870),
	tRgb(0xb86830), tRgb(0x703808),
	tRgb(0xe8e8e8), tRgb(0xb8b8b8),
	tRgb(0x888888), tRgb(0x585860),
	tRgb(0x383838), tRgb(0xf8e800),
	tRgb(0xc09000), tRgb(0x0030a8),
	tRgb(0x0080f8), tRgb(0x0038c0),
	tRgb(0x001088), tRgb(0x101010)
});

static const uint32_t s_ulOffsOlafOldStart = 0xC5018;
static const uint32_t s_ulOffsOlafOldEnd = 0xC5218;

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

void extractGfx(
	std::ifstream &FileRom, uint32_t ulOffsStart, uint32_t ulOffsEnd,
	const std::vector<tMergeRule> &vMergeRules, const tPalette &Palette,
	const std::string &DirName
)
{
	// Read every tile
	FileRom.seekg(ulOffsStart, std::ios::beg);
	uint32_t ulTileCnt = (ulOffsEnd - ulOffsStart) / (8 * 4);
	std::map<uint32_t, std::shared_ptr<tRawTile>> mTiles;
	for(uint32_t i = 0; i < ulTileCnt; ++i) {
		// tPlanarBitmap supports only width being multiple of 16 - fill only 8x8
		tPlanarBitmap TilePlanar(16, 8, 4);
		for(uint8_t ubRow = 0; ubRow < 8; ++ubRow) {
			for(uint8_t ubPlane = 0; ubPlane < 2; ++ubPlane) {
				uint8_t ubRaw;
				FileRom.read(reinterpret_cast<char*>(&ubRaw), 1);
				TilePlanar.m_pPlanes[ubPlane][ubRow] = ubRaw << 8;
			}
		}
		for(uint8_t ubRow = 0; ubRow < 8; ++ubRow) {
			for(uint8_t ubPlane = 2; ubPlane < 4; ++ubPlane) {
				uint8_t ubRaw;
				FileRom.read(reinterpret_cast<char*>(&ubRaw), 1);
				TilePlanar.m_pPlanes[ubPlane][ubRow] = ubRaw << 8;
			}
		}
		auto pTileChunky = std::make_shared<tRawTile>(tChunkyBitmap(TilePlanar, Palette));
		mTiles.emplace(i, pTileChunky);
		// TileChunky.toPng(fmt::format("{}/{}.png", szOutDir, i));
	}

	// Merge tiles accorging to rules
	for(const auto &Rule: vMergeRules) {
		tChunkyBitmap Merged(Rule.m_ubTileWidth * 8, Rule.m_ubTileHeight * 8);
		uint8_t ubMergeListPos = 0;
		for(uint8_t ubY = 0; ubY < Rule.m_ubTileHeight; ++ubY) {
			for(uint8_t ubX = 0; ubX < Rule.m_ubTileWidth; ++ubX) {
				auto TileIdx = Rule.m_vTileIndices[ubMergeListPos];
				mTiles.at(TileIdx)->m_Tile.copyRect(0, 0, Merged, ubX * 8, ubY * 8, 8, 8);
				mTiles.at(TileIdx)->m_isUsed = true;
				++ubMergeListPos;
			}
		}
		Merged.toPng(fmt::format("{}/{}.png", DirName, Rule.m_Name));
	}

	// Dump all unmerged
	for(const auto &[TileIdx, RawTile]: mTiles) {
		if(!RawTile->m_isUsed) {
			RawTile->m_Tile.toPng(fmt::format("{}/unused-{}.png", DirName, TileIdx));
		}
	}
}

void extractGfx(
	std::ifstream &FileRom, uint32_t ulOffsStart, uint32_t ulOffsEnd,
	uint8_t ubTileWidth, uint8_t ubTileHeight, const tPalette &Palette,
	const std::string &DirName
)
{
	uint16_t uwTilesPerFrame = ubTileHeight * ubTileWidth;
	uint32_t ulTileCnt = ((ulOffsEnd - ulOffsStart) / (8 * 4)) / uwTilesPerFrame;
	std::vector<tMergeRule> vRules;
	for(uint32_t i = 0; i < ulTileCnt; ++i) {
		vRules.push_back(tMergeRule(ubTileWidth, ubTileHeight, fmt::format("{}", i), i * uwTilesPerFrame));
	}
	extractGfx(FileRom, ulOffsStart, ulOffsEnd, vRules, Palette, DirName);
}

[[nodiscard]]
static std::vector<uint8_t> extractUncompressedAsset(std::ifstream &FileRom, uint32_t ulOffsStart, uint32_t ulSize) {
	std::vector<uint8_t> vContents(ulSize, 0x00);
	FileRom.seekg(ulOffsStart, std::ios::beg);
	FileRom.read(reinterpret_cast<char*>(vContents.data()), ulSize);
	return vContents;
}

[[nodiscard]]
static std::vector<uint8_t> extractCompressedAsset(std::ifstream &FileRom, uint32_t ulOffsStart) {
	uint16_t uwDecompressedSize, uwPos = 0;
	uint8_t ubRepeatBits;
	FileRom.seekg(ulOffsStart, std::ios::beg);
	FileRom.read(reinterpret_cast<char*>(&uwDecompressedSize), sizeof(uwDecompressedSize));

	// Decompression algorithm depends on the first 4096 bytes being set to zero.
	std::vector<uint8_t> vDecoded(std::max(uwDecompressedSize, uint16_t(0x1000)), 0x00);

	fmt::print(FMT_STRING("Decompressing asset at {:08X}, size: {}\n"), ulOffsStart, uwDecompressedSize);
	bool wasCopy = false;
	do {
		if(wasCopy) {
			fmt::print("\n");
		}
		wasCopy = false;
		FileRom.read(reinterpret_cast<char*>(&ubRepeatBits), sizeof(ubRepeatBits));
		fmt::print(
			FMT_STRING("ROM pos: {:06X}, repeat bits: {:08b}\n"),
			size_t(FileRom.tellg()) - 1, ubRepeatBits
		);
		for(uint8_t ubBit = 0; ubBit < 8 && uwPos < uwDecompressedSize; ++ubBit) {
			bool isCopy = ((ubRepeatBits & 1) == 1);
			ubRepeatBits >>= 1;
			if(isCopy) {
				// Fill with next byte as-is
				if(uwPos >= vDecoded.size()) {
					throw std::runtime_error(fmt::format(FMT_STRING("Write out of buffer bounds")));
				}
				FileRom.read(reinterpret_cast<char*>(&vDecoded[uwPos]), sizeof(vDecoded[uwPos]));
				if(!wasCopy) {
					fmt::print("raw byte: ");
					wasCopy = true;
				}
				fmt::print(FMT_STRING("{:02X} "), vDecoded[uwPos]);
				++uwPos;
			}
			else {
				// Decompress stuff
				uint16_t uwDecompressRaw;
				FileRom.read(reinterpret_cast<char*>(&uwDecompressRaw), sizeof(uwDecompressRaw));
				uint16_t uwCopyLoopIndex = uwDecompressRaw & 0xFFF;
				uint16_t uwCopyLoopSize = ((uwDecompressRaw >> 12) + 3 + uwCopyLoopIndex) & 0x0FFF;
				if(wasCopy) {
					fmt::print("\n");
					wasCopy = false;
				}
				fmt::print(
					FMT_STRING("Decompress cmd: {:04X}, copying in bytes from {}..{}\n"),
					uwDecompressRaw, uwCopyLoopIndex, uwCopyLoopSize
				);
				while(uwCopyLoopIndex != uwCopyLoopSize) {
					if(uwPos >= vDecoded.size()) {
						throw std::runtime_error(fmt::format(FMT_STRING("Write out of buffer bounds")));
					}
					if(uwCopyLoopIndex >= vDecoded.size()) {
						throw std::runtime_error(fmt::format(FMT_STRING("Read out of buffer bounds")));
					}
					vDecoded[uwPos++] = vDecoded[uwCopyLoopIndex++];

					if(uwCopyLoopIndex == 0x1000 && uwCopyLoopIndex != uwCopyLoopSize) {
						uwCopyLoopIndex = 0;
					}
				}
				if(uwCopyLoopIndex == 0x1000) {
					fmt::print(FMT_STRING("stopped at pos 0x1000\n"));
				}
			}
		}
	} while(uwPos < uwDecompressedSize);

	uint32_t ulEndPos = FileRom.tellg();
	vDecoded.resize(uwDecompressedSize);
	fmt::print(
		FMT_STRING("Compressed size: {}, decompressed: {}\n"),
		ulEndPos - ulOffsStart, vDecoded.size()
	);
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
	bool isUncompressed;
};

// asset_extract "c:/gry/snes9x/Roms/Lost Vikings II, The - Norse by Norsewest (Europe) (En,Fr,De).sfc" ../assets/dec
int main(int lArgCount, const char *pArgs[])
{
	const uint8_t ubMandatoryArgCnt = 2;
	if(lArgCount - 1 < ubMandatoryArgCnt) {
		nLog::error("Too few arguments, expected {}", ubMandatoryArgCnt);
		printUsage(pArgs[0]);
		return EXIT_FAILURE;
	}

	std::string szInput = pArgs[1], szOutDir = pArgs[2];

	// TODO: create dir

	std::ifstream FileRom;
	FileRom.open(szInput.c_str(), std::ifstream::binary);
	if(!FileRom.good()) {
		nLog::error("Couldn't open ROM file at '{}'", szInput);
		return EXIT_FAILURE;
	}

	// TODO: verify ROM size/checksum/header

	// extractGfx(FileRom, s_ulOffsHudStart, s_ulOffsHudEnd, s_vMergeRulesHud, s_PaletteHud, fmt::format("{}/{}", szOutDir, "hud"));
	// extractGfx(FileRom, s_ulOffsErikStart, s_ulOffsErikEnd, 4, 4, s_PaletteErik, fmt::format("{}/{}", szOutDir, "erik"));
	// extractGfx(FileRom, s_ulOffsBaelogStart, s_ulOffsBaelogEnd, 4, 4, s_PaletteBaelog, fmt::format("{}/{}", szOutDir, "baelog"));
	// extractGfx(FileRom, s_ulOffsFangStart, s_ulOffsFangEnd, 4, 4, s_PaletteFang, fmt::format("{}/{}", szOutDir, "fang"));
	// extractGfx(FileRom, s_ulOffsScorchStart, s_ulOffsScorchEnd, 4, 4, s_PaletteScorch, fmt::format("{}/{}", szOutDir, "scorch"));
	// extractGfx(FileRom, s_ulOffsOlafStart, s_ulOffsOlafEnd, 4, 4, s_PaletteOlaf, fmt::format("{}/{}", szOutDir, "olaf"));
	// extractGfx(FileRom, s_ulOffsEffectStart, s_ulOffsEffectEnd, 4, 4, s_PaletteEffect, fmt::format("{}/{}", szOutDir, "effect"));
	// extractGfx(FileRom, s_ulOffsErikOldStart, s_ulOffsErikOldEnd, 4, 4, s_PaletteErik, fmt::format("{}/{}", szOutDir, "erik_old"));
	// extractGfx(FileRom, s_ulOffsBaelogOldStart, s_ulOffsBaelogOldEnd, 4, 4, s_PaletteBaelog, fmt::format("{}/{}", szOutDir, "baelog_old"));
	// extractGfx(FileRom, s_ulOffsOlafOldStart, s_ulOffsOlafOldEnd, 4, 4, s_PaletteOlaf, fmt::format("{}/{}", szOutDir, "olaf_old"));

	// Read asset TOC
	fmt::print("List of assets at 0x{:06X}:\n", 0x050000);
	FileRom.seekg(0x050000, std::ios::beg);
	std::vector<tAssetTocEntry> vAssetToc;
	for(uint32_t i = 0; i < 0x155; ++i) {
		// Read offset
		uint32_t ulOffs;
		FileRom.read(reinterpret_cast<char*>(&ulOffs), sizeof(ulOffs));
		uint32_t ulOffsEntryNext = FileRom.tellg();
		auto OffsCpu = ulOffs + 0x8A8000;
		auto OffsRom = snesAddressToRomOffset(OffsCpu);

		// Update deduced size on previous entry
		if(i > 0) {
			uint32_t ulSizeInRom = OffsRom - vAssetToc[i - 1].ulOffs;
			fmt::print(FMT_STRING(", size: {:5d}"), ulSizeInRom);
			vAssetToc[i - 1].ulSizeInRom = ulSizeInRom;

			// Check if decompression would make any sense
			uint16_t uwSizeDecompressed;
			FileRom.seekg(vAssetToc[i - 1].ulOffs, std::ios::beg);
			FileRom.read(
				reinterpret_cast<char*>(&uwSizeDecompressed),
				sizeof(uwSizeDecompressed)
			);
			if(uwSizeDecompressed < ulSizeInRom) {
				fmt::print(
					FMT_STRING(", uncompressed (decompressed would be smaller)"),
					uwSizeDecompressed, ulSizeInRom
				);
				vAssetToc[i - 1].isUncompressed = true;
			}
			fmt::print("\n");
		}
		vAssetToc.push_back({
			.ulOffs = OffsRom, .ulSizeInRom = 0,
			.isUncompressed = false
		});

		FileRom.seekg(ulOffsEntryNext, std::ios::beg);

		fmt::print(FMT_STRING("raw: 0x{:06X}, rom: 0x{:08X}"), ulOffs, OffsRom);
	}
	fmt::print("\n");

	try {
		for(auto &TocEntry: vAssetToc) {
			std::string szAssetName = fmt::format(FMT_STRING("_compressed_{:08X}"), TocEntry.ulOffs);
			std::vector<uint8_t> vAssetContents;
			if(!TocEntry.isUncompressed) {
				try {
					vAssetContents = extractCompressedAsset(FileRom, TocEntry.ulOffs);
				}
				catch(const std::exception &Exc) {
					fmt::print("ERR: Exception while decoding asset at {:08X}: '{}'\n", TocEntry.ulOffs, Exc.what());
					TocEntry.isUncompressed = true;
					szAssetName = fmt::format(FMT_STRING("_faildec_{:08X}"), TocEntry.ulOffs);
				}
			}
			else {
				szAssetName = fmt::format(FMT_STRING("_uncompressed_{:08X}"), TocEntry.ulOffs);
			}
			if(TocEntry.isUncompressed) {
					if(TocEntry.ulSizeInRom) {
						vAssetContents = extractUncompressedAsset(FileRom, TocEntry.ulOffs, TocEntry.ulSizeInRom);
					}
			}
			if(vAssetContents.size() != 0) {
				std::ofstream FileOut;
				std::string szOutPath;
				std::string szExtension = "dat";
				if(vAssetContents[0] == 0x00 && vAssetContents[1] == 0x01 && vAssetContents[2] == 0xF8 && vAssetContents[3] == 0x00) {
					szExtension = "1F8";
				}
				auto Asset = mOffsToFileName.find(TocEntry.ulOffs);
				if(Asset != mOffsToFileName.end()) {
					szAssetName = Asset->second;
				}
				szOutPath = fmt::format(FMT_STRING("{}/{}.{}"), szOutDir, szAssetName, szExtension);
				FileOut.open(szOutPath,	std::ios::binary);
				FileOut.write(reinterpret_cast<char*>(vAssetContents.data()), vAssetContents.size());
				FileOut.close();
			}
		}
	}
	catch(const std::exception &Exc) {
		fmt::print("Super failure: '{}'!\n", Exc.what());
		return EXIT_FAILURE;
	}

	fmt::print("All done!\n");
	return EXIT_SUCCESS;
}
