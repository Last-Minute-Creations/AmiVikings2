/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "state_game.hpp"
#include <ace/managers/viewport/tilebuffer.h>
#include <ace/managers/viewport/simplebuffer.h>
#include <ace/managers/system.h>
#include <ace/managers/game.h>
#include <ace/managers/key.h>
#include <ace/managers/blit.h>
#include <ace/managers/bob.h>
#include <ace/utils/palette.h>
#include <ace/utils/chunky.h>
#include <ace/utils/font.h>
#include <ace/utils/string.h>
#include <lmc/array.hpp>
#include <entity/entity_erik.hpp>
#include "assets.hpp"
#include "tile.hpp"
#include "steer.hpp"
#include "hud.hpp"
#include "player_controller.hpp"

using namespace Lmc;

#define MAIN_BPP 6
#define BOB_COUNT 4

static void substatePlayLoop(void);
static void substateInventoryLoop(void);
static void substatePauseCreate(void);
static void substatePauseLoop(void);
static void substateMessageCreate(void);
static void substateMessageLoop(void);

static tView *s_pView;
static tVPort *s_pVpMain;
static tTileBufferManager *s_pBufferMain;

static tBitMap *s_pTileset;
static tBob s_pBobs[BOB_COUNT];
// static tFont *s_pFont;
// static tTextBitMap *s_pTextTile;
static auto s_sGameSubstatePlay = tState::empty()
	.withLoop(substatePlayLoop);
static auto s_sGameSubstateInventory = tState::empty()
	.withLoop(substateInventoryLoop);
static tState s_sGameSubstateMessage = tState::empty()
	.withCreate(substateMessageCreate)
	.withLoop(substateMessageLoop);
static tState s_sGameSubstatePause = tState::empty()
	.withCreate(substatePauseCreate)
	.withLoop(substatePauseLoop);
tStateManager *s_pGameSubstateMachine;
static tPlayerIdx s_eControllingPlayer;

static tBitMap *s_pFont;
static tBitMap *s_pFontMask;
static UWORD s_uwPendingMessageId;
static UWORD s_uwPendingMessageBgColor;

// [y][x]
static const UBYTE s_pTilesStrt[][32] = {
	{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
	{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
	{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 3, 2, 3, 1, 1, 1, 1, 1},
	{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 4, 5, 4, 5, 1, 1, 6, 7, 1},
	{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 8, 9, 8, 9, 1, 1, 10, 11, 1},
	{12, 13, 14, 15, 16, 17, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 18, 19, 20, 21, 20, 21, 22, 23, 24, 25, 26},
	{27, 28, 29, 30, 31, 32, 19, 33, 34, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 35, 36, 37, 36, 37, 38, 37, 36, 39, 40, 36},
	{37, 36, 36, 37, 36, 37, 36, 37, 36, 41, 42, 43, 1, 1, 1, 1, 1, 1, 44, 45, 46, 47, 48, 49, 50, 49, 48, 50, 51, 52, 53, 48},
	{50, 49, 50, 48, 50, 49, 50, 50, 48, 54, 55, 56, 57, 58, 14, 15, 1, 18, 59, 60, 61, 48, 50, 62, 63, 64, 65, 66, 67, 68, 69, 48},
	{48, 50, 48, 62, 51, 70, 71, 53, 48, 48, 50, 72, 41, 73, 29, 30, 19, 35, 74, 48, 50, 75, 50, 49, 76, 77, 78, 50, 50, 79, 69, 62},
	{50, 66, 53, 50, 69, 68, 80, 81, 52, 63, 64, 48, 54, 55, 37, 37, 36, 47, 49, 62, 65, 71, 66, 53, 48, 50, 51, 70, 82, 65, 83, 48},
	{84, 85, 86, 87, 71, 52, 48, 50, 63, 88, 77, 50, 75, 48, 50, 50, 62, 78, 50, 48, 50, 89, 51, 67, 50, 49, 69, 49, 90, 91, 92, 48},
	{93, 49, 48, 94, 89, 50, 49, 48, 76, 77, 50, 68, 81, 53, 49, 48, 50, 49, 49, 50, 49, 69, 81, 70, 53, 50, 95, 96, 97, 50, 49, 50},
};

consteval auto generateCharToGlyph() {
	const char GlyphIndexToChar[] = {
		'\x00', '\x01', '\x02', '\x03', '\x04', '\x05', '\x06', '\x07',
		'\x08', '\x09', '\x0A', '\x0B', '\x0C',    ' ',    ' ',    ' ',
		   ' ',    '!',    '"', '\x0D', '\x0E', '\x0F', '\x10',   '\'',
			 '(',    ')', '\x11',    '+',    ',',    '-',    '.',    '/',
			 '0',    '1',    '2',    '3',    '4',    '5',    '6',    '7',
			 '8',    '9',    ':',    ';',	'\x12', '\x13', '\x14',    '?',
		'\x15',    'A',    'B',    'C',    'D',    'E',    'F',    'G',
		   'H',    'I',    'J',    'K',	   'L',    'M',    'N',    'O',
			 'P',    'Q',    'R',    'S',    'T',    'U',    'V',    'W',
			 'X',    'Y',    'Z', '\x16', '\x17', '\x18', '\x19', '\x1A'
	};

	UBYTE ubMaxChar = 0;
	for(UBYTE i = 0; i < sizeof(GlyphIndexToChar); ++i) {
		if(GlyphIndexToChar[i] > ubMaxChar) {
			ubMaxChar = GlyphIndexToChar[i];
		}
	}

	tArray<UBYTE, 91> CharToGlyphIndex;
	for(UBYTE i = 0; i < CharToGlyphIndex.uwSize; ++i) {
		CharToGlyphIndex.Data[i] = 0;
	}
	for(UBYTE i = 0; i < sizeof(GlyphIndexToChar); ++i) {
		CharToGlyphIndex.Data[static_cast<UBYTE>(GlyphIndexToChar[i])] = i;
	}

	return CharToGlyphIndex;
}

static void loadMap(void) {
	tileReset();

	for(UBYTE y = 0; y < 13; ++y) {
		for(UBYTE x = 0; x < 32; ++x) {
			UBYTE ubTileIdx = s_pTilesStrt[y][x] - 1;
			s_pBufferMain->pTileData[x][y] = ubTileIdx;
			if(
				(35 <= ubTileIdx && ubTileIdx <= 39) || ubTileIdx == 54 ||
				ubTileIdx == 71 || ubTileIdx == 46 || ubTileIdx == 73 ||
				ubTileIdx == 59
			) {
				tileSetType(x, y, tTile::FloorFlat);
			}
			else if(ubTileIdx == 40) {
				tileSetType(x, y, tTile::FloorRamp22DownA);
			}
			else if(ubTileIdx == 41 || ubTileIdx == 72) {
				tileSetType(x, y, tTile::FloorRamp22DownB);
			}
			else if(ubTileIdx == 55) {
				tileSetType(x, y, tTile::FloorRamp45Down);
			}
			else if(ubTileIdx == 34 || ubTileIdx == 58) {
				tileSetType(x, y, tTile::FloorRamp45Up);
			}
			else if(ubTileIdx == 44) {
				tileSetType(x, y, tTile::FloorRamp22UpA);
			}
			else if(ubTileIdx == 45) {
				tileSetType(x, y, tTile::FloorRamp22UpB);
			}
			else {
				tileSetType(x, y, tTile::Empty);
			}
		}
	}
	cameraReset(s_pBufferMain->pCamera, 0, 0, 32*16, 13 * 16, 1);
}

static void onTileDraw(
	UWORD uwTileX, UWORD uwTileY,
	tBitMap *pBitMap, UWORD uwBitMapX, UWORD uwBitMapY
) {
	// const UBYTE *pTileHeightmap = tileGetHeightmap(uwTileX, uwTileY);
	// for(UBYTE x = 0; x < 16; ++x) {
	// 	chunkyToPlanar(8, uwBitMapX + x, uwBitMapY + pTileHeightmap[x], pBitMap);
	// }
	// UBYTE ubTileIdx = s_pTilesStrt[uwTileY][uwTileX] - 1;
	// if(ubTileIdx) {
	// 	char szTileIdx[4];
	// 	stringDecimalFromULong(ubTileIdx, szTileIdx);
	// 	fontDrawStr(
	// 		s_pFont, pBitMap, uwBitMapX + 8, uwBitMapY + 8, szTileIdx, 8,
	// 		FONT_COOKIE | FONT_SHADOW | FONT_CENTER, s_pTextTile
	// 	);
	// }
}

static void drawGlyphAt(UBYTE ubGlyphIndex, UWORD uwPosX, UWORD uwPosY) {
	UWORD uwGlyphX = (ubGlyphIndex & 1) * 8;
	UWORD uwGlyphY = (ubGlyphIndex / 2) * 8;
	blitCopyMask(s_pFont, uwGlyphX, uwGlyphY, s_pBufferMain->pScroll->pFront, uwPosX, uwPosY, 8, 8, s_pFontMask->Planes[0]);
}

static void drawTextLineAt(const char *szText, UWORD uwPosX, UWORD uwPosY) {
	static const auto CharToGlyph = generateCharToGlyph();

	UBYTE ubTextLength = strlen(szText);
	for(UBYTE ubX = 0; ubX < ubTextLength; ++ubX) {
		drawGlyphAt(CharToGlyph.Data[(UBYTE)szText[ubX]], uwPosX + ubX * 8, uwPosY);
	}
}

static void drawMessageFrameAt(UBYTE ubBgColor, UBYTE ubBlockPosX, UBYTE ubBlockPosY, UBYTE ubLineCount, const char** pText) {
	// TODO: character indicator: 10, 11
	// 234
	// 5 6
	// 789
	UWORD uwPosX = ubBlockPosX * 8;
	UWORD uwPosY = ubBlockPosY * 8;
	UBYTE ubTextWidth = strlen(pText[0]);
	blitRect(s_pBufferMain->pScroll->pFront, uwPosX + 4, uwPosY + 6, (ubTextWidth + 2) * 8 - 4 - 3, (ubLineCount + 2) * 8 - 6 - 5, ubBgColor);
	drawGlyphAt(2, uwPosX, uwPosY);
	drawGlyphAt(7, uwPosX, uwPosY + (ubLineCount + 1) * 8);
	for(UBYTE i = 1; i < ubTextWidth + 1; ++i) {
		drawGlyphAt(3, uwPosX + i * 8, uwPosY);
		drawGlyphAt(8, uwPosX + i * 8, uwPosY + (ubLineCount + 1) * 8);
	}
	for(UBYTE i = 1; i < ubLineCount + 1; ++i) {
		drawGlyphAt(5, uwPosX, uwPosY + i * 8);
		drawGlyphAt(6, uwPosX + (ubTextWidth + 1) * 8, uwPosY + i * 8);
	}
	drawGlyphAt(4, uwPosX + (ubTextWidth + 1) * 8, uwPosY);
	drawGlyphAt(9, uwPosX + (ubTextWidth + 1) * 8, uwPosY + (ubLineCount + 1) * 8);

	for(UBYTE ubY = 0; ubY < ubLineCount; ++ubY) {
		drawTextLineAt(pText[ubY], uwPosX + 1 * 8, uwPosY + (ubY + 1) * 8);
	}
}

static void stateGameCreate(void) {
	UWORD uwCopHudSize = simpleBufferGetRawCopperlistInstructionCount(HUD_BPP);
	UWORD uwCopMainStartSize = tileBufferGetRawCopperlistInstructionCountStart(MAIN_BPP);
	UWORD uwCopMainBreakSize = tileBufferGetRawCopperlistInstructionCountBreak(MAIN_BPP);
	UWORD uwCopListSize = uwCopHudSize + uwCopMainStartSize + uwCopMainBreakSize;
	s_pView = viewCreate(0,
		TAG_VIEW_GLOBAL_PALETTE, 1,
		TAG_VIEW_COPLIST_MODE, COPPER_MODE_RAW,
		TAG_VIEW_COPLIST_RAW_COUNT, uwCopListSize,
		TAG_VIEW_WINDOW_HEIGHT, 224,
	TAG_END);

	s_pTileset = bitmapCreateFromFile("data/tiles.bm", 0);
	s_pFont = bitmapCreateFromFile("data/font.bm", 0);
	s_pFontMask = bitmapCreateFromFile("data/font_mask.bm", 0);

	hudCreate(s_pView);

	s_pVpMain = vPortCreate(0,
		TAG_VPORT_BPP, MAIN_BPP,
		TAG_VPORT_VIEW, s_pView,
	TAG_END);

	s_pBufferMain = tileBufferCreate(0,
		TAG_TILEBUFFER_BITMAP_FLAGS, BMF_CLEAR | BMF_INTERLEAVED,
		TAG_TILEBUFFER_IS_DBLBUF, 1,
		TAG_TILEBUFFER_BOUND_TILE_X, 40,
		TAG_TILEBUFFER_BOUND_TILE_Y, 40,
		TAG_TILEBUFFER_REDRAW_QUEUE_LENGTH, 15,
		TAG_TILEBUFFER_TILE_SHIFT, 4,
		TAG_TILEBUFFER_TILESET, s_pTileset,
		TAG_TILEBUFFER_VPORT, s_pVpMain,
		TAG_TILEBUFFER_COPLIST_OFFSET_START, uwCopHudSize,
		TAG_TILEBUFFER_COPLIST_OFFSET_BREAK, uwCopHudSize + uwCopMainStartSize,
		TAG_TILEBUFFER_CALLBACK_TILE_DRAW, onTileDraw,
	TAG_END);

	s_pGameSubstateMachine = stateManagerCreate();

	assetsGlobalCreate();
	// s_pFont = fontCreate("data/uni54.fnt");
	// s_pTextTile = fontCreateTextBitMap(320, 8);
	playerControllerReset();
	playerControllerSetSteerPresets(tSteerKind::Preset1, tSteerKind::Null);

	bobManagerCreate(
		s_pBufferMain->pScroll->pFront, s_pBufferMain->pScroll->pBack,
		s_pBufferMain->pScroll->uwBmAvailHeight
	);


	for(UBYTE i = 0; i < BOB_COUNT; ++i) {
		bobInit(
			&s_pBobs[i], 32, 32, 1,
			bobCalcFrameAddress(g_pBobBmErik, 0),
			bobCalcFrameAddress(g_pBobBmErikMask, 0),
			32 + 48 * (i + 1), 32
		);
	}

	s_uwPendingMessageId = 0;
	s_uwPendingMessageBgColor = 0;

	// Init entities
	s_eControllingPlayer = tPlayerIdx::First;
	entityManagerReset();
	tEntity *pPlayerEntity1 = entityManagerSpawnEntity(tEntityKind::Erik, 32, 32, 16, 16, 0, 0);
	tEntity *pPlayerEntity2 = entityManagerSpawnEntity(tEntityKind::Erik, 64, 32, 16, 16, 0, 0);
	entityManagerSpawnEntity(tEntityKind::InfoBox, 128, 104, 8, 8, 0x0024, 0x0106);

	playerControllerSetVikingEntity(0, pPlayerEntity1);
	playerControllerSetVikingEntity(1, pPlayerEntity2);
	playerControllerSetVikingEntity(2, 0);

	playerControllerSetDefaultSelection();

	bobReallocateBgBuffers();

	systemUnuse();

	loadMap();
	hudReset();

	tileBufferRedrawAll(s_pBufferMain);
	viewLoad(s_pView);
	stateChange(s_pGameSubstateMachine, &s_sGameSubstatePlay);
}

static void stateGameLoop(void) {
	stateProcess(s_pGameSubstateMachine);

	systemIdleBegin();
	vPortWaitUntilEnd(s_pVpMain);
	systemIdleEnd();
}

static void stateGameDestroy(void) {
	viewLoad(0);
	systemUse();
	bobManagerDestroy();
	assetsGlobalDestroy();
	// fontDestroy(s_pFont);
	// fontDestroyTextBitMap(s_pTextTile);
	bitmapDestroy(s_pTileset);
	bitmapDestroy(s_pFont);
	bitmapDestroy(s_pFontMask);
	hudDestroy();
	viewDestroy(s_pView);
}

#pragma region substates

static void substatePlayLoop(void) {
	for(
		tPlayerIdx ePlayerIdx = (tPlayerIdx)0;
		enumValue(ePlayerIdx) < enumValue(tPlayerIdx::Count);
		ePlayerIdx = static_cast<tPlayerIdx>(enumValue(ePlayerIdx) + 1)
	) {
		tSteer *pSteer = playerControllerGetSteer(ePlayerIdx);
		steerUpdate(pSteer);

		if(steerUse(pSteer, tSteerAction::Inventory)) {
			s_eControllingPlayer = ePlayerIdx;
			stateChange(s_pGameSubstateMachine, &s_sGameSubstateInventory);
			return;
		}
		if(steerUse(pSteer, tSteerAction::Pause)) {
			s_eControllingPlayer = ePlayerIdx;
			stateChange(s_pGameSubstateMachine, &s_sGameSubstatePause);
			return;
		}
		if(s_uwPendingMessageId != 0) {
			s_eControllingPlayer = ePlayerIdx;
			stateChange(s_pGameSubstateMachine, &s_sGameSubstateMessage);
			return;
		}

		tEntity *pActiveEntity = hudProcessPlay(ePlayerIdx, pSteer);
		if(pActiveEntity) {
			if(ePlayerIdx == tPlayerIdx::First) {
				cameraCenterAt(
					s_pBufferMain->pCamera,
					pActiveEntity->sBob.sPos.uwX, pActiveEntity->sBob.sPos.uwY
				);
			}
		}
	}

	bobBegin(s_pBufferMain->pScroll->pBack);
	tileBufferQueueProcess(s_pBufferMain);

	// for(UBYTE i = 0; i < BOB_COUNT; ++i) {
	// 	bobPush(&s_pBobs[i]);
	// }
	entityManagerProcess();
	bobPushingDone();
	bobEnd();

	viewProcessManagers(s_pView);
	copProcessBlocks();
}

static void substateInventoryLoop(void) {
	tSteer *pSteer = playerControllerGetSteer(s_eControllingPlayer);
	steerUpdate(pSteer);

	if(steerUse(pSteer, tSteerAction::Inventory)) {
		stateChange(s_pGameSubstateMachine, &s_sGameSubstatePlay);
		return;
	}

	hudProcessInventory(s_eControllingPlayer, pSteer);
}

static void substateMessageCreate()
{
	UBYTE ubPosX = ((256/8 - 16)/2);
	UBYTE ubPosY = (4);

	static const char* pText[] = {
		"CAN YOU TAKE US ",
		"TO THE BIG SHINY",
		"METAL THING THAT",
		"BROUGHT US HERE?",
	};
	drawMessageFrameAt(s_uwPendingMessageBgColor, ubPosX, ubPosY, 4, pText);
	s_uwPendingMessageId = 0;
}

static void substateMessageLoop()
{
	tSteer *pSteer = playerControllerGetSteer(s_eControllingPlayer);
	steerUpdate(pSteer);

	if(steerUse(pSteer, tSteerAction::Ability1)) {
		stateChange(s_pGameSubstateMachine, &s_sGameSubstatePlay);
		return;
	}
}

static UBYTE s_ubPauseBlinkCooldown;
static UBYTE s_isPauseBlinkDraw;
static UBYTE s_isPauseYesSelected;

#define GIVE_UP_X ((256/8 - 8)/2)
#define GIVE_UP_Y (4)
#define GIVE_UP_YES_OFFS_X 1
#define GIVE_UP_YES_OFFS_Y 3
#define GIVE_UP_NO_OFFS_X (GIVE_UP_YES_OFFS_X + 6)
#define GIVE_UP_NO_OFFS_Y GIVE_UP_YES_OFFS_Y

static void substatePauseCreate(void) {
	// static const char* pText[] = {
	// 	"TRY AGAIN?",
	// 	"          ",
	// 	" YES   NO ",
	// };
	static const char* pText[] = {
		"GIVE UP?",
		"        ",
		"YES   NO",
	};
	drawMessageFrameAt(0, GIVE_UP_X, GIVE_UP_Y, 3, pText);
	s_ubPauseBlinkCooldown = 25;
	s_isPauseBlinkDraw = 0;
	s_isPauseYesSelected = 1;
}

static void updateYesNoBlink(UBYTE isYes, UBYTE isDraw) {
	if(isDraw) {
		if(isYes) {
			drawTextLineAt(
				"YES",
				(GIVE_UP_X + GIVE_UP_YES_OFFS_X) * 8,
				(GIVE_UP_Y + GIVE_UP_YES_OFFS_Y) * 8
			);
		}
		else {
			drawTextLineAt(
				"NO",
				(GIVE_UP_X + GIVE_UP_NO_OFFS_X) * 8,
				(GIVE_UP_Y + GIVE_UP_NO_OFFS_Y) * 8
			);
		}
	}
	else {
		if(isYes) {
			blitRect(
				s_pBufferMain->pScroll->pFront,
				(GIVE_UP_X + GIVE_UP_YES_OFFS_X) * 8,
				(GIVE_UP_Y + GIVE_UP_YES_OFFS_Y) * 8,
				strlen("YES") * 8, 8, 0
			);
		}
		else {
			blitRect(
				s_pBufferMain->pScroll->pFront,
				(GIVE_UP_X + GIVE_UP_NO_OFFS_X) * 8,
				(GIVE_UP_Y + GIVE_UP_NO_OFFS_Y) * 8,
				strlen("NO") * 8, 8, 0
			);
		}
	}
}

static void substatePauseLoop(void) {
	tSteer *pSteer = playerControllerGetSteer(s_eControllingPlayer);
	steerUpdate(pSteer);

	if(steerUse(pSteer, tSteerAction::Pause)) {
		stateChange(s_pGameSubstateMachine, &s_sGameSubstatePlay);
		return;
	}
	if(steerUse(pSteer, tSteerAction::Left)) {
		updateYesNoBlink(s_isPauseYesSelected, 1);
		s_isPauseYesSelected = 1;
		s_ubPauseBlinkCooldown = 1;
		s_isPauseBlinkDraw = 0;
	}
	else if(steerUse(pSteer, tSteerAction::Right)) {
		updateYesNoBlink(s_isPauseYesSelected, 1);
		s_isPauseYesSelected = 0;
		s_ubPauseBlinkCooldown = 1;
		s_isPauseBlinkDraw = 0;
	}
	else if(steerUse(pSteer, tSteerAction::Ability1)) {
		if(s_isPauseYesSelected) {
			gameExit();
			return;
		}
		else {
			stateChange(s_pGameSubstateMachine, &s_sGameSubstatePlay);
			return;
		}
	}

	if(--s_ubPauseBlinkCooldown == 0) {
		s_ubPauseBlinkCooldown = 25;
		updateYesNoBlink(s_isPauseYesSelected, s_isPauseBlinkDraw);
		s_isPauseBlinkDraw = !s_isPauseBlinkDraw;
	}
}

#pragma endregion

tState g_sStateGame = tState::empty()
	.withCreate(stateGameCreate)
	.withLoop(stateGameLoop)
	.withDestroy(stateGameDestroy);

void gameSetPendingMessage(UWORD uwMessageId, UWORD uwBgColor)
{
	s_uwPendingMessageId = uwMessageId;
	s_uwPendingMessageBgColor = uwBgColor;
}
