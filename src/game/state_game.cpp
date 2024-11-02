/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "state_game.hpp"
#include <ace/generic/screen.h>
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
#include <lmc/span.hpp>
#include <entity/entity_erik.hpp>
#include <dialogue/dialogue.hpp>
#include "assets.hpp"
#include "tile.hpp"
#include "steer.hpp"
#include "hud.hpp"
#include "player_controller.hpp"

using namespace Lmc;

#define MAIN_BPP 6
#define BOB_COUNT 4

// BUDGET:
// tiles: 700 16x16 6bpp: 131KB
// viking: 70KB one-side, 140KB two-side, 3x: 421KB, masked: 842KB
// items: 13,5KB
// font: 7,5KB

static void substatePlayLoop(void);
static void substateInventoryLoop(void);
static void substatePauseCreate(void);
static void substatePauseLoop(void);
static void substateDialogueCreate(void);
static void substateDialogueLoop(void);

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
static tState s_sGameSubstateDialogue = tState::empty()
	.withCreate(substateDialogueCreate)
	.withLoop(substateDialogueLoop);
static tState s_sGameSubstatePause = tState::empty()
	.withCreate(substatePauseCreate)
	.withLoop(substatePauseLoop);
tStateManager *s_pGameSubstateMachine;
static tPlayerIdx s_eControllingPlayer;

static tBitMap *s_pFont;
static tBitMap *s_pFontMask;
static UWORD s_uwPendingDialogueId;
static UWORD s_uwPendingMessageBgColor;
static UWORD s_uwMapTileWidth;
static UWORD s_uwMapTileHeight;
static tActiveDialogue s_ActiveDialogue;
static tUwRect s_LastMessageFrameRect;

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
		CharToGlyphIndex.pData[i] = 0;
	}
	for(UBYTE i = 0; i < sizeof(GlyphIndexToChar); ++i) {
		CharToGlyphIndex.pData[static_cast<UBYTE>(GlyphIndexToChar[i])] = i;
	}

	return CharToGlyphIndex;
}

static void loadMap(void) {
	tileReset();

	systemUse();
	auto *pFileTilemap = fileOpen("data/tilemap_w1_a0.dat", "rb");
	fileRead(pFileTilemap, &s_uwMapTileWidth, sizeof(s_uwMapTileWidth));
	fileRead(pFileTilemap, &s_uwMapTileHeight, sizeof(s_uwMapTileHeight));

	for(UBYTE y = 0; y < s_uwMapTileHeight; ++y) {
		for(UBYTE x = 0; x < s_uwMapTileWidth; ++x) {
			constexpr UBYTE ubTileIndexMaskSize = 10;
			constexpr UWORD uwTileIndexMask = (1 << ubTileIndexMaskSize) - 1;

			UWORD uwTileData;
			fileRead(pFileTilemap, &uwTileData, sizeof(uwTileData));
			s_pBufferMain->pTileData[x][y] = uwTileData & uwTileIndexMask;

			UBYTE ubAttribute = uwTileData >> ubTileIndexMaskSize;
			tileSetAttribute(x, y, ubAttribute);
		}
	}
	fileClose(pFileTilemap);
	systemUnuse();

	cameraReset(s_pBufferMain->pCamera, 0, 0, s_uwMapTileWidth*16, s_uwMapTileHeight * 16, 1);
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

static void drawTextAt(const char *szText, UWORD uwStartPosX, UWORD uwStartPosY) {
	static const auto CharToGlyph = generateCharToGlyph();

	UWORD uwPosX = uwStartPosX;
	UWORD uwPosY = uwStartPosY;
	for(const char *pCurrent = szText; *pCurrent != '\0'; ++pCurrent) {
		if(*pCurrent == '\r' || *pCurrent == '\n') {
			uwPosX = uwStartPosX;
			uwPosY += 8;
		}
		else {
			drawGlyphAt(CharToGlyph.pData[(UBYTE)*pCurrent], uwPosX, uwPosY);
			uwPosX += 8;
		}
	}
}

static void undrawMessageFrame() {
	blitCopy(
		s_pBufferMain->pScroll->pBack, s_LastMessageFrameRect.uwX, s_LastMessageFrameRect.uwY,
		s_pBufferMain->pScroll->pFront, s_LastMessageFrameRect.uwX, s_LastMessageFrameRect.uwY,
		s_LastMessageFrameRect.uwWidth, s_LastMessageFrameRect.uwHeight, MINTERM_COOKIE
	);
}

static void drawMessageFrame(
	UBYTE ubBgColor,
	tUwCoordYX WorldPosition,
	const tBoxedMessage &Message
) {
	// Save relevant portion in backbuffer
	s_LastMessageFrameRect.uwX = WorldPosition.uwX;
	s_LastMessageFrameRect.uwY = WorldPosition.uwY;
	s_LastMessageFrameRect.uwWidth = Message.m_Size.ubX * 8;
	s_LastMessageFrameRect.uwHeight = Message.m_Size.ubY * 8;
	blitCopy(
		s_pBufferMain->pScroll->pFront, s_LastMessageFrameRect.uwX, s_LastMessageFrameRect.uwY,
		s_pBufferMain->pScroll->pBack, s_LastMessageFrameRect.uwX, s_LastMessageFrameRect.uwY,
		s_LastMessageFrameRect.uwWidth, s_LastMessageFrameRect.uwHeight, MINTERM_COOKIE
	);

	// 234
	// 5 6
	// 789
	// TODO: character direction indicator: 10, 11

	blitRect(
		s_pBufferMain->pScroll->pFront,
		WorldPosition.uwX + 4, WorldPosition.uwY + 6,
		Message.m_Size.ubX * 8 - 4 - 3, (Message.m_Size.ubY) * 8 - 6 - 5, ubBgColor
	);
	drawGlyphAt(2, WorldPosition.uwX, WorldPosition.uwY);
	drawGlyphAt(7, WorldPosition.uwX, WorldPosition.uwY + (Message.m_Size.ubY - 2 + 1) * 8);
	for(UBYTE i = 1; i < Message.m_Size.ubX - 1; ++i) {
		drawGlyphAt(3, WorldPosition.uwX + i * 8, WorldPosition.uwY);
		drawGlyphAt(8, WorldPosition.uwX + i * 8, WorldPosition.uwY + (Message.m_Size.ubY - 2 + 1) * 8);
	}
	for(UBYTE i = 1; i < Message.m_Size.ubY - 2 + 1; ++i) {
		drawGlyphAt(5, WorldPosition.uwX, WorldPosition.uwY + i * 8);
		drawGlyphAt(6, WorldPosition.uwX + (Message.m_Size.ubX - 1) * 8, WorldPosition.uwY + i * 8);
	}
	drawGlyphAt(4, WorldPosition.uwX + (Message.m_Size.ubX - 1) * 8, WorldPosition.uwY);
	drawGlyphAt(9, WorldPosition.uwX + (Message.m_Size.ubX - 1) * 8, WorldPosition.uwY + (Message.m_Size.ubY - 2 + 1) * 8);

	drawTextAt(Message.m_szText, WorldPosition.uwX + 8, WorldPosition.uwY + 8);
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
		TAG_VIEW_WINDOW_WIDTH, 256,
		TAG_VIEW_WINDOW_START_X, SCREEN_XOFFSET + (SCREEN_PAL_WIDTH - 256) / 2,
	TAG_END);

	s_pTileset = bitmapCreateFromFile("data/tiles_w1.bm", 0);
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
		TAG_TILEBUFFER_BOUND_TILE_X, TILE_MAP_SIZE,
		TAG_TILEBUFFER_BOUND_TILE_Y, TILE_MAP_SIZE,
		TAG_TILEBUFFER_REDRAW_QUEUE_LENGTH, 15,
		TAG_TILEBUFFER_TILE_SHIFT, 4,
		TAG_TILEBUFFER_TILESET, s_pTileset,
		TAG_TILEBUFFER_VPORT, s_pVpMain,
		TAG_TILEBUFFER_COPLIST_OFFSET_START, uwCopHudSize,
		TAG_TILEBUFFER_COPLIST_OFFSET_BREAK, uwCopHudSize + uwCopMainStartSize,
		TAG_TILEBUFFER_CALLBACK_TILE_DRAW, onTileDraw,
		TAG_TILEBUFFER_MAX_TILESET_SIZE, 1024,
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
			bobCalcFrameAddress(g_pBobBmErikLeft, 0),
			bobCalcFrameAddress(g_pBobBmErikLeftMask, 0),
			32 + 48 * (i + 1), 32
		);
	}

	s_uwPendingDialogueId = 0;
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
	stateManagerDestroy(s_pGameSubstateMachine);
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
		ePlayerIdx < tPlayerIdx::Count;
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
		if(s_uwPendingDialogueId != 0) {
			s_eControllingPlayer = ePlayerIdx;
			s_ActiveDialogue = tActiveDialogue(dialogueGetChain(s_uwPendingDialogueId));
			s_uwPendingDialogueId = 0;
			stateChange(s_pGameSubstateMachine, &s_sGameSubstateDialogue);
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

static tUwCoordYX EntryToWorldPosition(const tDialogueEntry &Entry) {
	auto &EntityBob = entityManagerGetEntityFromIndex(Entry.m_ubActorIndex).sBob;
	tUwCoordYX Pos;
	if(Entry.m_eCornerFlags == tMsgPosFlag::CAM) {
		Pos.ulYX = s_pBufferMain->pCamera->uPos.ulYX;
	}
	else {
		Pos.uwX = EntityBob.sPos.uwX + EntityBob.uwWidth / 2;
		Pos.uwY = EntityBob.sPos.uwY + EntityBob.uwHeight / 2;
		if((Entry.m_eCornerFlags & tMsgPosFlag::RIGHT) == tMsgPosFlag::RIGHT) {
			Pos.uwX -= Entry.getMessage().m_Size.ubX * 8;
		}
		if((Entry.m_eCornerFlags & tMsgPosFlag::DOWN) == tMsgPosFlag::DOWN) {
			Pos.uwY -= Entry.getMessage().m_Size.ubY * 8;
		}
	}
	Pos.uwX += Entry.m_ActorOffset.wX;
	Pos.uwY += Entry.m_ActorOffset.wY;
	return Pos;
}

static void substateDialogueShowCurrent() {
	auto &Entry = s_ActiveDialogue.getCurrentEntry();
	drawMessageFrame(s_uwPendingMessageBgColor, EntryToWorldPosition(Entry), Entry.getMessage());
}

static void substateDialogueCreate()
{
	substateDialogueShowCurrent();
}

static void substateDialogueLoop()
{
	tSteer *pSteer = playerControllerGetSteer(s_eControllingPlayer);
	steerUpdate(pSteer);

	if(
		steerUse(pSteer, tSteerAction::Ability1) ||
		steerUse(pSteer, tSteerAction::Interact)
	) {
		undrawMessageFrame();
		if(s_ActiveDialogue.tryAdvance()) {
			substateDialogueShowCurrent();
		}
		else {
			stateChange(s_pGameSubstateMachine, &s_sGameSubstatePlay);
			return;
		}
	}
}

static UBYTE s_ubPauseBlinkCooldown;
static UBYTE s_isPauseBlinkDraw;
static UBYTE s_isPauseYesSelected;

#define GIVE_UP_X ((256 - 8 * 8)/2)
#define GIVE_UP_Y (4 * 8)
#define GIVE_UP_YES_OFFS_X (1 * 8)
#define GIVE_UP_YES_OFFS_Y (3 * 8)
#define GIVE_UP_NO_OFFS_X (GIVE_UP_YES_OFFS_X + 6 * 8)
#define GIVE_UP_NO_OFFS_Y GIVE_UP_YES_OFFS_Y

static void substatePauseCreate(void) {
	// static const auto Message = tBoxedMessage("TRY AGAIN?\r\r YES   NO ");
	static const auto Message = tBoxedMessage("GIVE UP?\r\rYES   NO");

	tUwCoordYX Pos = {.uwY = GIVE_UP_Y, .uwX = GIVE_UP_X};
	Pos.ulYX += s_pBufferMain->pCamera->uPos.ulYX;
	drawMessageFrame(0, Pos, Message);
	s_ubPauseBlinkCooldown = 25;
	s_isPauseBlinkDraw = 0;
	s_isPauseYesSelected = 1;
}

static void updateYesNoBlink(UBYTE isYes, UBYTE isDraw) {
	const char *szMsg;
	tUwCoordYX Pos = {.uwY = GIVE_UP_Y, .uwX = GIVE_UP_X};
	Pos.ulYX += s_pBufferMain->pCamera->uPos.ulYX;
	if(isYes) {
		Pos.uwX += GIVE_UP_YES_OFFS_X;
		Pos.uwY += GIVE_UP_YES_OFFS_Y;
		szMsg = "YES";
	}
	else {
		Pos.uwX += GIVE_UP_NO_OFFS_X;
		Pos.uwY += GIVE_UP_NO_OFFS_Y;
		szMsg = "NO";
	}

	if(isDraw) {
		drawTextAt(szMsg, Pos.uwX, Pos.uwY);
	}
	else {
		blitRect(
			s_pBufferMain->pScroll->pFront,
			Pos.uwX, Pos.uwY, strlen(szMsg) * 8, 8, 0
		);
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
			undrawMessageFrame();
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

void gameSetPendingDialogue(UWORD uwDialogueId, UWORD uwBgColor)
{
	s_uwPendingDialogueId = uwDialogueId;
	s_uwPendingMessageBgColor = uwBgColor;
}
