/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef AMILOSTVIKINGS2_ASSET_EXTRACT_ROM_CONSTEXPR_STD_HPP
#define AMILOSTVIKINGS2_ASSET_EXTRACT_ROM_CONSTEXPR_STD_HPP

namespace AmiLostVikings2::AssetExtract::ConstexprStd {

constexpr int strcmp(const char *szA, const char *szB) {
	// https://en.cppreference.com/w/c/string/byte/strcmp

	// It's ok to check only bounds of szA since if szB is shorter then its null
	// terminator won't be the same as szA's char on same pos.
	while(*szA) {
		// If chars are not the same, return the difference.
		if(*szA != *szB) {
			return *szA - *szB;
		}
		++szA;
		++szB;
	}

	// szB is longer or equal to szA - return the difference.
	return *szA - *szB;
}

} // namespace namespace AmiLostVikings2::AssetExtract

#endif // AMILOSTVIKINGS2_ASSET_EXTRACT_ROM_CONSTEXPR_STD_HPP
