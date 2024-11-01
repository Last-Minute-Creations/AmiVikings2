/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef INC_DIALOGUE_DIALOGUE_HPP
#define INC_DIALOGUE_DIALOGUE_HPP

#include <lmc/enum_value.hpp>
#include <dialogue/boxed_message.hpp>
#include <dialogue/msg_pos_flag.hpp>

struct tDialogueEntry;
struct tActiveDialogue;

const tDialogueEntry *dialogueGetChain(UWORD uwIndex);
const tBoxedMessage &dialogueGetMessage(UWORD uwIndex);

struct tDialogueEntry {

	static constexpr UWORD uwInvalidMessageIndex = 65535;

	UWORD m_uwMessageIndex;
	UBYTE m_ubActorIndex;
	tMsgPosFlag m_eCornerFlags;
	tWCoordYX m_ActorOffset; ///< From entity's centerpoint.

	constexpr tDialogueEntry():
		m_uwMessageIndex(uwInvalidMessageIndex)
	{}

	constexpr tDialogueEntry(
		UWORD uwMessageIndex,
		UBYTE m_ubActorIndex,
		tWCoordYX ActorOffset,
		tMsgPosFlag eCornerFlags
	):
		m_uwMessageIndex(uwMessageIndex),
		m_ubActorIndex(m_ubActorIndex),
		m_eCornerFlags(eCornerFlags),
		m_ActorOffset(ActorOffset)
	{}

	constexpr const tBoxedMessage &getMessage() const {
		return dialogueGetMessage(m_uwMessageIndex);
	}

	constexpr bool isEnd() const {
		return m_uwMessageIndex == uwInvalidMessageIndex;
	}
};

struct tActiveDialogue {
	const tDialogueEntry * m_pEntries;
	UWORD m_uwCurrentEntryIndex;

	constexpr tActiveDialogue() = default;

	constexpr tActiveDialogue(const tDialogueEntry * pEntries):
		m_pEntries(pEntries),
		m_uwCurrentEntryIndex(0)
	{}

	constexpr const tDialogueEntry &getCurrentEntry() const {
		return m_pEntries[m_uwCurrentEntryIndex];
	}

	constexpr bool tryAdvance() {
		if(getCurrentEntry().isEnd()) {
			return false;
		}

		++m_uwCurrentEntryIndex;
		bool isEnd = getCurrentEntry().isEnd();
		return !isEnd;
	}
};

#endif // INC_DIALOGUE_DIALOGUE_HPP
