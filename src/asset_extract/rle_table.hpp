/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include <array>
#include <cstdint>

class tRleTable {
public:
	void writeValue(std::uint8_t ubValue) {
		Buffer[uwWritePos] = ubValue;
		if(++uwWritePos >= Buffer.size()) {
			uwWritePos = 0;
		}
	}

	std::uint8_t readValue(std::uint16_t *pReadPos) {
		std::uint8_t ubReadValue = Buffer[*pReadPos];
		if(++*pReadPos >= Buffer.size()) {
			*pReadPos = 0;
		}
		return ubReadValue;
	}

private:
	std::array<std::uint8_t, 0x1000> Buffer = {0}; // decompression depends on buffer being initially zeroed out
	std::uint16_t uwWritePos = 0;
};
