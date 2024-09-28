/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef LMC_ENUM_VALUE_HPP
#define LMC_ENUM_VALUE_HPP

namespace Lmc {

template<typename t_tEnum>
	requires __is_enum(t_tEnum)
constexpr auto enumValue(t_tEnum eEnum) {
	return static_cast<__underlying_type(t_tEnum)>(eEnum);
}

} // namespace lmc

#endif // LMC_ENUM_VALUE_HPP
