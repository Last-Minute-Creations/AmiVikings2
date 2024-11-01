/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef INC_DIALOGUE_MSG_POS_FLAG_HPP
#define INC_DIALOGUE_MSG_POS_FLAG_HPP

#include <ace/types.h>
#include <ace/macros.h>

enum class tMsgPosFlag: UBYTE {
	UP = 0,
	DOWN = BV(0),
	LEFT = 0,
	RIGHT = BV(1),
	CAM = BV(7),
};

static tMsgPosFlag operator | (tMsgPosFlag eLeft, tMsgPosFlag eRight)
{
	return tMsgPosFlag(Lmc::enumValue(eLeft) | Lmc::enumValue(eRight));
}

static tMsgPosFlag operator & (tMsgPosFlag eLeft, tMsgPosFlag eRight)
{
	return tMsgPosFlag(Lmc::enumValue(eLeft) & Lmc::enumValue(eRight));
}

#endif // INC_DIALOGUE_MSG_POS_FLAG_HPP
