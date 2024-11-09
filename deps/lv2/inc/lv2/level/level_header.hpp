/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef INC_LV2_LEVEL_LEVEL_HEADER_HPP
#define INC_LV2_LEVEL_LEVEL_HEADER_HPP

#include <stdint.h>

namespace lv2::level {

struct tLevelHeader {
	uint8_t ubUnk0; // Always 00?
	uint8_t ubUnk1; // Always 01?
	uint8_t ubUnk2; // Always F8?
	uint8_t ubUnk3; // Always 00?
	uint8_t ubUnk4;
	uint8_t ubMusic; // 0: menu, 1-5: worlds, 6: game over, 7: spaceship, 8: outro, 9: no music, A+: no audio
	uint8_t ubUnk6;
	uint8_t ubUnk7;
	uint8_t ubUnk8;
	uint8_t ubUnk9;
	uint8_t ubUnk10;
	uint16_t uwNextLevelHeaderIdx;
	uint8_t ubUnk13;
	uint8_t ubUnk14;
	uint8_t ubUnk15;
	uint8_t ubUnk16;
	uint8_t ubUnk17;
	uint8_t ubUnk18;
	uint8_t ubUnk19;
	uint8_t ubUnk20;
	uint8_t ubUnk21;
	uint8_t ubUnk22;
	uint8_t ubUnk23;
	uint8_t ubUnk24;
	uint8_t ubUnk25;
	uint8_t ubUnk26;
	uint8_t ubUnk27;
	uint8_t ubUnk28;
	uint8_t ubUnk29;
	uint8_t ubUnk30;
	uint16_t uwTileWidth;
	uint16_t uwTileHeight;
	uint8_t ubUnk35;
	uint16_t uwTilemapFileIndex; // 16x16 tile arrangement used to construct a level.
	uint16_t uwTilesetFileIndex; // 8x8 tiles used to construct 16x16 tiles.
	uint16_t uwTiledefFileIndex; // How to construct required 16x16 tiles from 8x8 ones.
	uint16_t uwBackgroundWidth;
	uint16_t uwBackgroundHeight;
	uint8_t ubUnk46;
	uint16_t uwBackgroundFileIndex; // Set to FFFF for no background
};

} // namespace lv2::level

#endif // INC_LV2_LEVEL_LEVEL_HEADER_HPP
