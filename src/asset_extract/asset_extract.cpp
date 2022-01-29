/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "logging.h"
#include "fs.h"
#include "rgb.h"
#include "bitmap.h"
#include <fstream>
#include <filesystem>
#include <map>

struct tAssetDef {
	std::string AssetName;
	std::optional<bool> isCompressed;
	std::function<void(
		const std::vector<uint8_t> &vDataUnprocessed, const std::string &PathOut
	)> onExtract;
};

void handleExtractFont(
	const std::vector<uint8_t> &vDataUnprocessed, const std::string &PathOut
);

void handleExtractFrames(
	const std::vector<uint8_t> &vDataUnprocessed, const std::string &PathOut,
	const tPalette &Palette, uint32_t ulFirstFrameIdx
);

auto handleExtractFramePart(
	const tPalette &Palette, uint32_t ulFristFrame
) {
	return [&Palette, ulFristFrame](const std::vector<uint8_t> &vDataUnprocessed, const std::string &PathOut) {
		handleExtractFrames(vDataUnprocessed, PathOut, Palette, ulFristFrame);
	};
};

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

static const std::map<uint32_t, tAssetDef> s_mOffsToFileName = {
	{0xED14B, tAssetDef {.AssetName = "logo_lv2_a", .isCompressed = {}, .onExtract = nullptr}},
	{0xEDB38, tAssetDef {.AssetName = "logo_lv2_b", .isCompressed = {}, .onExtract = nullptr}},
	{0xE2583, tAssetDef {.AssetName = "logo_interplay", .isCompressed = {}, .onExtract = nullptr}},
	{0xE4D50, tAssetDef {.AssetName = "logo_gameover_a", .isCompressed = {}, .onExtract = nullptr}},
	{0xE3863, tAssetDef {.AssetName = "logo_blizzard", .isCompressed = {}, .onExtract = nullptr}},
	{0xC5BF1, tAssetDef {.AssetName = "bomb", .isCompressed = {}, .onExtract = nullptr}},
	{0xE9F06, tAssetDef {.AssetName = "enemy_tomator", .isCompressed = {}, .onExtract = nullptr}},
	{0xCAD87, tAssetDef {.AssetName = "enemy_poof", .isCompressed = {}, .onExtract = nullptr}},
	{0xCA8FC, tAssetDef {.AssetName = "enemy_spikes", .isCompressed = {}, .onExtract = nullptr}},
	{0xCB35F, tAssetDef {.AssetName = "enemy_jelly", .isCompressed = {}, .onExtract = nullptr}},
	{0xCBA0E, tAssetDef {.AssetName = "enemy_fish", .isCompressed = {}, .onExtract = nullptr}},
	{0xCCE03, tAssetDef {.AssetName = "enemy_vamp", .isCompressed = {}, .onExtract = nullptr}},
	{0xCC4D1, tAssetDef {.AssetName = "enemy_skeleton", .isCompressed = {}, .onExtract = nullptr}},
	{0xCDBE8, tAssetDef {.AssetName = "enemy_knight", .isCompressed = {}, .onExtract = nullptr}},
	{0xCEEA8, tAssetDef {.AssetName = "enemy_wizard", .isCompressed = {}, .onExtract = nullptr}},
	{0xCFB5F, tAssetDef {.AssetName = "enemy_pirate", .isCompressed = {}, .onExtract = nullptr}},
	{0xD0484, tAssetDef {.AssetName = "enemy_corsair", .isCompressed = {}, .onExtract = nullptr}},
	{0xD3582, tAssetDef {.AssetName = "enemy_vine", .isCompressed = {}, .onExtract = nullptr}},
	{0xF1420, tAssetDef {.AssetName = "enemy_roboknight", .isCompressed = {}, .onExtract = nullptr}},
	{0xD2328, tAssetDef {.AssetName = "enemy_monkey", .isCompressed = {}, .onExtract = nullptr}},
	{0xD1273, tAssetDef {.AssetName = "enemy_montezuma", .isCompressed = {}, .onExtract = nullptr}},
	{0xD3CBD, tAssetDef {.AssetName = "enemy_robot", .isCompressed = {}, .onExtract = nullptr}},
	{0xD4780, tAssetDef {.AssetName = "enemy_xeno", .isCompressed = {}, .onExtract = nullptr}},
	{0xC9A18, tAssetDef {.AssetName = "obstacle_spike_ball_underwater", .isCompressed = {}, .onExtract = nullptr}},
	{0xC9E5A, tAssetDef {.AssetName = "obstacle_spike_ball_underwater_2", .isCompressed = {}, .onExtract = nullptr}}, // other palette
	{0xC6730, tAssetDef {.AssetName = "obstacle_spike_ball_underwater_3", .isCompressed = {}, .onExtract = nullptr}}, // other palette
	{0xC95E7, tAssetDef {.AssetName = "obstacle_spear_up", .isCompressed = {}, .onExtract = nullptr}},
	{0xC9580, tAssetDef {.AssetName = "obstacle_spear_left", .isCompressed = {}, .onExtract = nullptr}},
	{0xC8143, tAssetDef {.AssetName = "obstacle_drill_up", .isCompressed = {}, .onExtract = nullptr}},
	{0xCA6CD, tAssetDef {.AssetName = "obstacle_zap", .isCompressed = {}, .onExtract = nullptr}},
	{0xCA5F5, tAssetDef {.AssetName = "obstacle_zap2", .isCompressed = {}, .onExtract = nullptr}},
	{0xEE368, tAssetDef {.AssetName = "obstacle_zap3", .isCompressed = {}, .onExtract = nullptr}},
	{0xC681D, tAssetDef {.AssetName = "obstacle_spike_up", .isCompressed = {}, .onExtract = nullptr}},
	{0xECD65, tAssetDef {.AssetName = "obstacle_burning_rope", .isCompressed = {}, .onExtract = nullptr}},
	{0x506BF, tAssetDef {.AssetName = "font", .isCompressed = {}, .onExtract = handleExtractFont}},
	{0xC76D1, tAssetDef {.AssetName = "door_bolted", .isCompressed = {}, .onExtract = nullptr}},
	{0xC87E6, tAssetDef {.AssetName = "door_skull", .isCompressed = {}, .onExtract = nullptr}},
	{0xC7D97, tAssetDef {.AssetName = "bounce_bone", .isCompressed = {}, .onExtract = nullptr}},
	{0xC92DC, tAssetDef {.AssetName = "dunno1", .isCompressed = {}, .onExtract = nullptr}},
	{0xC8431, tAssetDef {.AssetName = "dunno2", .isCompressed = {}, .onExtract = nullptr}},
	{0xC8C13, tAssetDef {.AssetName = "dunno3", .isCompressed = {}, .onExtract = nullptr}},
	{0xC9C87, tAssetDef {.AssetName = "dunno_ball", .isCompressed = {}, .onExtract = nullptr}},
	{0xC63EB, tAssetDef {.AssetName = "dunno_magic", .isCompressed = {}, .onExtract = nullptr}},
	{0xC8D32, tAssetDef {.AssetName = "dunno_ball2", .isCompressed = {}, .onExtract = nullptr}},
	{0xCA894, tAssetDef {.AssetName = "dunno4", .isCompressed = {}, .onExtract = nullptr}},
	{0xE9D06, tAssetDef {.AssetName = "dunno_water1", .isCompressed = {}, .onExtract = nullptr}},
	{0xECB65, tAssetDef {.AssetName = "dunno_water2", .isCompressed = {}, .onExtract = nullptr}},
	{0xE9906, tAssetDef {.AssetName = "dunno_water3", .isCompressed = {}, .onExtract = nullptr}},
	{0xE9B06, tAssetDef {.AssetName = "dunno_water4", .isCompressed = {}, .onExtract = nullptr}},
	{0xEC965, tAssetDef {.AssetName = "dunno_water5", .isCompressed = {}, .onExtract = nullptr}},
	{0xC7B68, tAssetDef {.AssetName = "gas", .isCompressed = {}, .onExtract = nullptr}},
	{0xC5711, tAssetDef {.AssetName = "items_keys", .isCompressed = {}, .onExtract = nullptr}},
	{0xEFAEA, tAssetDef {.AssetName = "cutscene_screens", .isCompressed = {}, .onExtract = nullptr}},
	{0xF02D0, tAssetDef {.AssetName = "cutscene_fist", .isCompressed = {}, .onExtract = nullptr}},
	{0xF212C, tAssetDef {.AssetName = "cutscene_lolipop", .isCompressed = {}, .onExtract = nullptr}},
	{0xC8F07, tAssetDef {.AssetName = "items_skull_voodoo", .isCompressed = {}, .onExtract = nullptr}},
	{0xC55DF, tAssetDef {.AssetName = "fart", .isCompressed = {}, .onExtract = nullptr}},
	{0xC981D, tAssetDef {.AssetName = "tile_block_crush", .isCompressed = {}, .onExtract = nullptr}},
	{0xCA410, tAssetDef {.AssetName = "items_pcb_battery_disk_burger", .isCompressed = {}, .onExtract = nullptr}},
	{0xE6E1B, tAssetDef {.AssetName = "npc_witch", .isCompressed = {}, .onExtract = nullptr}},
	{0xE92F5, tAssetDef {.AssetName = "npc_time_machine", .isCompressed = {}, .onExtract = nullptr}},
	{0xE6FFD, tAssetDef {.AssetName = "npc_mage", .isCompressed = {}, .onExtract = nullptr}},
	{0xE7479, tAssetDef {.AssetName = "npc_gypsy", .isCompressed = {}, .onExtract = nullptr}},
	{0xE783B, tAssetDef {.AssetName = "npc_shaman", .isCompressed = {}, .onExtract = nullptr}},
	{0xE7C0E, tAssetDef {.AssetName = "npc_connor", .isCompressed = {}, .onExtract = nullptr}},
	{0xE822F, tAssetDef {.AssetName = "npc_kid", .isCompressed = {}, .onExtract = nullptr}},
	{0xC65E6, tAssetDef {.AssetName = "interact_switch", .isCompressed = {}, .onExtract = nullptr}},
	{0xC75DE, tAssetDef {.AssetName = "platform_grass", .isCompressed = {}, .onExtract = nullptr}},
	{0xC90FB, tAssetDef {.AssetName = "platform_wood", .isCompressed = {}, .onExtract = nullptr}},
	{0xCA0F7, tAssetDef {.AssetName = "platform_future", .isCompressed = {}, .onExtract = nullptr}},
	{0xC6538, tAssetDef {.AssetName = "interact_button", .isCompressed = {}, .onExtract = nullptr}},
	{0xEE50E, tAssetDef {.AssetName = "bubbles", .isCompressed = {}, .onExtract = nullptr}},
	{0xC7F04, tAssetDef {.AssetName = "tile_elevator_updown1", .isCompressed = {}, .onExtract = nullptr}},
	{0xC64C1, tAssetDef {.AssetName = "cursor_hammer", .isCompressed = {}, .onExtract = nullptr}},
	{0xE4D1C, tAssetDef {.AssetName = "cursor_password", .isCompressed = {}, .onExtract = nullptr}},
	{0xC85CC, tAssetDef {.AssetName = "tile_corner1", .isCompressed = {}, .onExtract = nullptr}},
	{0xC951F, tAssetDef {.AssetName = "tile_corner2", .isCompressed = {}, .onExtract = nullptr}},
	{0xF2368, tAssetDef {.AssetName = "tile_corner3", .isCompressed = {}, .onExtract = nullptr}},
	{0xCA3A2, tAssetDef {.AssetName = "tile_elevator_updown2", .isCompressed = {}, .onExtract = nullptr}},
	{0x509B9, tAssetDef {.AssetName = "hud_border", .isCompressed = {}, .onExtract = nullptr}},
	{0x50DAD, tAssetDef {.AssetName = "hud_cursor_up", .isCompressed = {}, .onExtract = nullptr}},
	{0xC56BB, tAssetDef {.AssetName = "hud_cursor_down", .isCompressed = {}, .onExtract = nullptr}},
	{0x5275B, tAssetDef {.AssetName = "hud_hpbar", .isCompressed = {}, .onExtract = nullptr}},
	{0x50DDB, tAssetDef {.AssetName = "hud_portrait_erik_active", .isCompressed = {}, .onExtract = nullptr}},
	{0x50F5B, tAssetDef {.AssetName = "hud_portrait_baelog_active", .isCompressed = {}, .onExtract = nullptr}},
	{0x510DB, tAssetDef {.AssetName = "hud_portrait_olaf_active", .isCompressed = {}, .onExtract = nullptr}},
	{0x5125B, tAssetDef {.AssetName = "hud_portrait_fang_active", .isCompressed = {}, .onExtract = nullptr}},
	{0x513DB, tAssetDef {.AssetName = "hud_portrait_scorch_active", .isCompressed = {}, .onExtract = nullptr}},
	{0x5155B, tAssetDef {.AssetName = "hud_portrait_erik_inactive", .isCompressed = {}, .onExtract = nullptr}},
	{0x516DB, tAssetDef {.AssetName = "hud_portrait_baelog_inactive", .isCompressed = {}, .onExtract = nullptr}},
	{0x5185B, tAssetDef {.AssetName = "hud_portrait_olaf_inactive", .isCompressed = {}, .onExtract = nullptr}},
	{0x51B5B, tAssetDef {.AssetName = "hud_portrait_scorch_inactive", .isCompressed = {}, .onExtract = nullptr}},
	{0x519DB, tAssetDef {.AssetName = "hud_portrait_fang_inactive", .isCompressed = {}, .onExtract = nullptr}},
	{0x51CDB, tAssetDef {.AssetName = "hud_portrait_erik_dead", .isCompressed = {}, .onExtract = nullptr}},
	{0x51E5B, tAssetDef {.AssetName = "hud_portrait_baelog_dead", .isCompressed = {}, .onExtract = nullptr}},
	{0x51FDB, tAssetDef {.AssetName = "hud_portrait_olaf_dead", .isCompressed = {}, .onExtract = nullptr}},
	{0x5215B, tAssetDef {.AssetName = "hud_portrait_scorch_dead", .isCompressed = {}, .onExtract = nullptr}},
	{0x522DB, tAssetDef {.AssetName = "hud_portrait_fang_dead", .isCompressed = {}, .onExtract = nullptr}},
	{0x5281B, tAssetDef {.AssetName = "hud_items", .isCompressed = {}, .onExtract = nullptr}},
	{0x5245B, tAssetDef {.AssetName = "hud_portrait_unk1", .isCompressed = {}, .onExtract = nullptr}},
	{0x525DB, tAssetDef {.AssetName = "hud_portrait_unk2", .isCompressed = {}, .onExtract = nullptr}},
	{0xC5418, tAssetDef {.AssetName = "baelog_hand", .isCompressed = {}, .onExtract = nullptr}},
	{0xC6038, tAssetDef {.AssetName = "push_block", .isCompressed = {}, .onExtract = nullptr}},
	{0xC9D76, tAssetDef {.AssetName = "projectile_banana", .isCompressed = {}, .onExtract = nullptr}},
	{0xC8E04, tAssetDef {.AssetName = "projectile_rock", .isCompressed = {}, .onExtract = nullptr}},
	{0xCF971, tAssetDef {.AssetName = "projectile_magic", .isCompressed = {}, .onExtract = nullptr}},
	{0xC5ABE, tAssetDef {.AssetName = "projectile_fireball", .isCompressed = {}, .onExtract = nullptr}},
	{0xCA7B3, tAssetDef {.AssetName = "projectile_dunno1", .isCompressed = {}, .onExtract = nullptr}},
	{0xC8D9B, tAssetDef {.AssetName = "projectile_dunno2", .isCompressed = {}, .onExtract = nullptr}},
	{0xC9F3C, tAssetDef {.AssetName = "item_keycards", .isCompressed = {}, .onExtract = nullptr}},
	{0xE6CC1, tAssetDef {.AssetName = "item_machine_parts", .isCompressed = {}, .onExtract = nullptr}},
	{0xE6D9E, tAssetDef {.AssetName = "item_capacitor", .isCompressed = {}, .onExtract = nullptr}},
	{0xCA001, tAssetDef {.AssetName = "interact_keyslots", .isCompressed = {}, .onExtract = nullptr}},
	{0xC5BA1, tAssetDef {.AssetName = "tile_pipe", .isCompressed = {}, .onExtract = nullptr}},
	{0x8CA18, tAssetDef {.AssetName = "frames_erik", .isCompressed = {}, .onExtract = handleExtractFramePart(s_PaletteErik, 0)}},
	{0x94A18, tAssetDef {.AssetName = "frames_erik", .isCompressed = {}, .onExtract = handleExtractFramePart(s_PaletteErik, 64)}},
	{0x98218, tAssetDef {.AssetName = "frames_baelog", .isCompressed = {}, .onExtract = handleExtractFramePart(s_PaletteBaelog, 0)}},
	{0xA0018, tAssetDef {.AssetName = "frames_baelog", .isCompressed = {}, .onExtract = handleExtractFramePart(s_PaletteBaelog, 63)}},
	{0xA3018, tAssetDef {.AssetName = "frames_fang", .isCompressed = {}, .onExtract = handleExtractFramePart(s_PaletteFang, 0)}},
	{0xAA218, tAssetDef {.AssetName = "frames_fang", .isCompressed = {}, .onExtract = handleExtractFramePart(s_PaletteFang, 57)}},
	{0xAD418, tAssetDef {.AssetName = "frames_scorch", .isCompressed = {}, .onExtract = handleExtractFramePart(s_PaletteScorch, 0)}},
	{0xB3818, tAssetDef {.AssetName = "frames_scorch", .isCompressed = {}, .onExtract = handleExtractFramePart(s_PaletteScorch, 50)}},
	{0xB6818, tAssetDef {.AssetName = "frames_olaf", .isCompressed = {}, .onExtract = handleExtractFramePart(s_PaletteOlaf, 0)}},
	{0xBE618, tAssetDef {.AssetName = "frames_olaf", .isCompressed = {}, .onExtract = handleExtractFramePart(s_PaletteOlaf, 63)}},
	{0xC1C18, tAssetDef {.AssetName = "frames_special", .isCompressed = {}, .onExtract = nullptr}},
	{0xD78E4, tAssetDef {.AssetName = "continue_chars", .isCompressed = {}, .onExtract = nullptr}},
	{0xE55FD, tAssetDef {.AssetName = "continue_valkyrie", .isCompressed = {}, .onExtract = nullptr}},
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

//-------------------------------------------------------------- TILE EXTRACTING

void extractGfxTiles(
	std::vector<uint8_t> vDataRaw, uint8_t ubBpp,
	const std::vector<tMergeRule> &vMergeRules, const tPalette &Palette,
	const std::string &Path
)
{
	// Read every tile
	uint32_t ulTileCnt = uint32_t(vDataRaw.size()) / (8 * ubBpp); // number of SNES 8x8 tiles
	std::map<uint32_t, std::shared_ptr<tRawTile>> mTiles;
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
		Merged.toPng(fmt::format("{}/{}.png", Path, Rule.m_Name));
	}

	// Dump all unmerged
	for(const auto &[TileIdx, RawTile]: mTiles) {
		if(!RawTile->m_isUsed) {
			RawTile->m_Tile.toPng(fmt::format("{}/unused-{}.png", Path, TileIdx));
		}
	}
}

void extractGfxTiles(
	std::ifstream &FileRom, uint32_t ulOffsStart, uint32_t ulOffsEnd, uint8_t ubBpp,
	const std::vector<tMergeRule> &vMergeRules, const tPalette &Palette,
	const std::string &Path
)
{
	// Read the asset contents
	FileRom.seekg(ulOffsStart, std::ios::beg);
	auto Size = ulOffsEnd - ulOffsStart;
	std::vector<uint8_t> vDataRaw(Size, 0);
	FileRom.read(reinterpret_cast<char*>(vDataRaw.data()), Size);

	// Pass them for processing
	extractGfxTiles(vDataRaw, ubBpp, vMergeRules, Palette, Path);
}

void extractGfxTiles(
	std::ifstream &FileRom, uint32_t ulOffsStart, uint32_t ulOffsEnd, uint8_t ubBpp,
	uint8_t ubTileWidth, uint8_t ubTileHeight, const tPalette &Palette,
	const std::string &DirName
)
{
	uint16_t uwTilesPerFrame = ubTileHeight * ubTileWidth;
	uint32_t ulTileCnt = ((ulOffsEnd - ulOffsStart) / (8 * ubBpp)) / uwTilesPerFrame;
	std::vector<tMergeRule> vRules;
	for(uint32_t i = 0; i < ulTileCnt; ++i) {
		vRules.push_back(tMergeRule(ubTileWidth, ubTileHeight, fmt::format("{}", i), i * uwTilesPerFrame));
	}
	extractGfxTiles(FileRom, ulOffsStart, ulOffsEnd, ubBpp, vRules, Palette, DirName);
}

//------------------------------------------------------ ASSET PROCESS CALLBACKS

void handleExtractFont(
	const std::vector<uint8_t> &vDataUnprocessed, const std::string &PathOut
) {
	std::filesystem::create_directories(PathOut);

	tPalette Palette(std::vector<tRgb> {tRgb(0x000000), tRgb(0x555555), tRgb(0xAAAAAA), tRgb(0xFFFFFF)});
	std::vector<tMergeRule> vMergeRules;
	for(uint8_t i = 0; i < 80; ++i) {
		vMergeRules.push_back(tMergeRule(1, 1, fmt::format(FMT_STRING("{}"), i), i));
	}
	extractGfxTiles(vDataUnprocessed, 2, vMergeRules, Palette, PathOut);
}

void handleExtractFrames(
	const std::vector<uint8_t> &vDataUnprocessed, const std::string &PathOut,
	const tPalette &Palette, uint32_t ulFirstFrameIdx
) {
	std::filesystem::create_directories(PathOut);
	uint32_t ulFrameByteSize = (32 * 32 * 4) / 8; // w * h * bpp / bitsInByte
	uint32_t ulFrameCount = uint32_t(vDataUnprocessed.size()) / ulFrameByteSize;

	std::vector<tMergeRule> vMergeRules;
	for(uint8_t i = 0; i < ulFrameCount; ++i) {
		vMergeRules.push_back(tMergeRule(
			4, 4, fmt::format(FMT_STRING("{}"), ulFirstFrameIdx + i), i * 16
		));
	}
	extractGfxTiles(vDataUnprocessed, 4, vMergeRules, Palette, PathOut);
}

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

//--------------------------------------------------------- EXTRACT: LV1 VIKINGS

static const uint32_t s_ulOffsErikOldStart = 0xC4818;
static const uint32_t s_ulOffsErikOldEnd = 0xC4C18;

static const uint32_t s_ulOffsBaelogOldStart = 0xC4C18;
static const uint32_t s_ulOffsBaelogOldEnd = 0xC5018;

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

	uint32_t ulEndPos = uint32_t(FileRom.tellg());
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

	// extractGfxTiles(FileRom, s_ulOffsHudStart, s_ulOffsHudEnd, s_vMergeRulesHud, s_PaletteHud, fmt::format("{}/{}", szOutDir, "hud"));
	// extractGfxTiles(FileRom, s_ulOffsEffectStart, s_ulOffsEffectEnd, 4, 4, s_PaletteEffect, fmt::format("{}/{}", szOutDir, "effect"));
	// extractGfxTiles(FileRom, s_ulOffsErikOldStart, s_ulOffsErikOldEnd, 4, 4, s_PaletteErik, fmt::format("{}/{}", szOutDir, "erik_old"));
	// extractGfxTiles(FileRom, s_ulOffsBaelogOldStart, s_ulOffsBaelogOldEnd, 4, 4, s_PaletteBaelog, fmt::format("{}/{}", szOutDir, "baelog_old"));
	// extractGfxTiles(FileRom, s_ulOffsOlafOldStart, s_ulOffsOlafOldEnd, 4, 4, s_PaletteOlaf, fmt::format("{}/{}", szOutDir, "olaf_old"));

	// Read asset TOC
	fmt::print("List of assets at 0x{:06X}:\n", 0x050000);
	FileRom.seekg(0x050000, std::ios::beg);
	std::vector<tAssetTocEntry> vAssetToc;
	for(uint32_t i = 0; i < 0x155; ++i) {
		// Read offset
		uint32_t ulOffs;
		FileRom.read(reinterpret_cast<char*>(&ulOffs), sizeof(ulOffs));
		uint32_t ulOffsEntryNext = uint32_t(FileRom.tellg());
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
			decltype(tAssetDef::onExtract) onExtract = nullptr;
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
				bool isWrittenByCallback = false;
				if(
					vAssetContents[0] == 0x00 && vAssetContents[1] == 0x01 &&
					vAssetContents[2] == 0xF8 && vAssetContents[3] == 0x00
				) {
					szExtension = "1F8";
				}
				auto AssetPair = s_mOffsToFileName.find(TocEntry.ulOffs);
				if(AssetPair != s_mOffsToFileName.end()) {
					szAssetName = AssetPair->second.AssetName;
					if(AssetPair->second.onExtract != nullptr) {
						AssetPair->second.onExtract(vAssetContents, fmt::format(FMT_STRING("{}/{}"), szOutDir, szAssetName));
						isWrittenByCallback = true;
					}
				}
				if(!isWrittenByCallback) {
					szOutPath = fmt::format(FMT_STRING("{}/{}.{}"), szOutDir, szAssetName, szExtension);
					FileOut.open(szOutPath,	std::ios::binary);
					FileOut.write(reinterpret_cast<char*>(vAssetContents.data()), vAssetContents.size());
					FileOut.close();
				}
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
