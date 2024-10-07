/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef INC_AMILOSTVIKINGS2_ASSET_EXTRACT_ROM_METADATA_HPP
#define INC_AMILOSTVIKINGS2_ASSET_EXTRACT_ROM_METADATA_HPP

#include <algorithm>
#include <array>
#include <cstdint>
#include <span>
#include "constexpr_std.hpp"

namespace AmiLostVikings2::AssetExtract {

struct tRomMetadata {

	template<std::size_t t_PakFileCount>
	constexpr tRomMetadata(
		std::uint32_t ulPakAddress,
		std::uint16_t uwLastPakFileSize,
		const std::array<const char *, t_PakFileCount> &vPakFileEntries
	):
		m_ulPakAddress(ulPakAddress),
		m_uwLastPakFileSize(uwLastPakFileSize),
		m_PakFileEntries((const char **)vPakFileEntries.data(), vPakFileEntries.size())
	{}

	std::int16_t getAssetIndexByName(const char *szName) const {
		auto Pos = std::find_if(
			m_PakFileEntries.begin(), m_PakFileEntries.end(),
			[szName](const char *szEntry) {
				return szEntry != nullptr && !ConstexprStd::strcmp(szEntry, szName);
			}
		);
		if(Pos == m_PakFileEntries.end()) {
			return -1;
		}
		return static_cast<std::int16_t>(Pos - m_PakFileEntries.begin());
	}

	std::uint32_t m_ulPakAddress;
	std::uint16_t m_uwLastPakFileSize;
	std::span<const char*> m_PakFileEntries;
};

extern const tRomMetadata g_PalRomMetadata;

} // namespace AmiLostVikings2::AssetExtract


#endif // INC_AMILOSTVIKINGS2_ASSET_EXTRACT_ROM_METADATA_HPP
