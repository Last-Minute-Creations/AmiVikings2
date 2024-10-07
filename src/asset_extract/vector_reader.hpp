/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef INC_AMILOSTVIKINGS2_ASSET_EXTRACT_FILE_STREAM_HPP
#define INC_AMILOSTVIKINGS2_ASSET_EXTRACT_FILE_STREAM_HPP

#include <vector>

namespace AmiLostVikings2::AssetExtract {

class tVectorReader {
	const std::vector<std::uint8_t> &m_vData;
	std::size_t m_ReadPos;

public:
	constexpr tVectorReader(const std::vector<std::uint8_t> &vData):
		m_vData(vData),
		m_ReadPos(0)
	{
	}

	constexpr std::uint8_t readU8()
	{
		std::uint8_t ubValue = m_vData[m_ReadPos++];
		return ubValue;
	};

	constexpr std::uint16_t readU16()
	{
		std::uint16_t uwValue = 0;
		uwValue |= m_vData[m_ReadPos++];
		uwValue |= m_vData[m_ReadPos++] << 8;
		return uwValue;
	};

	template<typename t_tValue>
	constexpr void readTo(t_tValue &Value) {
		Value = 0;
		for(auto i = 0; i < sizeof(t_tValue); ++i) {
			std::uint8_t ubPart = readU8();
			Value |= (ubPart << (8 * i));
		}
	}
};

} // namespace AmiLostVikings2::AssetExtract

#endif // INC_AMILOSTVIKINGS2_ASSET_EXTRACT_FILE_STREAM_HPP
