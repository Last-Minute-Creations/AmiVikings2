/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef INC_LMC_SPAN_HPP
#define INC_LMC_SPAN_HPP

#include <stdint.h>

namespace Lmc {

template<typename t_tData>
struct tSpan {
	const t_tData *pData;
	uint16_t uwSize;

	constexpr tSpan(const t_tData *pData, uint16_t uwSize):
		pData(pData),
		uwSize(uwSize)
	{
	}

	template<typename t_tContainer>
	constexpr tSpan(const t_tContainer &Container):
		pData(Container.pData),
		uwSize(Container.uwSize)
	{
	}
};

} // namespace Lmc

#endif // INC_LMC_SPAN_HPP
