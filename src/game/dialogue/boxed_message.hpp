/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef INC_DIALOGUE_BOXED_MESSAGE_HPP
#define INC_DIALOGUE_BOXED_MESSAGE_HPP

#include <ace/types.h>

struct tBoxedMessage {
	tUbCoordYX m_Size;
	const char *m_szText;

	constexpr tBoxedMessage(tUbCoordYX Size, const char *szMsg):
		m_Size(Size),
		m_szText(szMsg)
	{}

	constexpr tBoxedMessage(const char *szMsg):
		m_szText(szMsg)
	{
		UBYTE ubMaxWidth = 0;
		UBYTE ubWidth = 0;
		UBYTE ubHeight = 1;

		const char *pChar = szMsg;
		while(*pChar != '\0') {
			if(*pChar == '\r') {
				if(ubWidth > ubMaxWidth) {
					ubMaxWidth = ubWidth;
				}
				ubWidth = 0;
				++ubHeight;
			}
			else {
				++ubWidth;
			}
			++pChar;
		}

		if(ubWidth > ubMaxWidth) {
			ubMaxWidth = ubWidth;
		}

		m_Size.ubX = ubWidth + 2;
		m_Size.ubY = ubHeight + 2;
	}
};

#endif // INC_DIALOGUE_BOXED_MESSAGE_HPP
