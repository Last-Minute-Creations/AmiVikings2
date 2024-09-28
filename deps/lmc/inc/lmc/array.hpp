/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef INC_LMC_ARRAY_HPP
#define INC_LMC_ARRAY_HPP

#include <stdint.h>

namespace Lmc {

template<typename t_tValue, uint16_t t_uwSize>
struct tArray {
	t_tValue Data[t_uwSize];
	static const uint16_t uwSize = t_uwSize;
};

} // namespace lmc

#endif // INC_LMC_ARRAY_HPP
