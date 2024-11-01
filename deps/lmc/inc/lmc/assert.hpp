/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef INC_LMC_ASSERT_HPP
#define INC_LMC_ASSERT_HPP

#include <ace/managers/log.h>

#if defined(GAME_DEBUG)
#define LMC_ASSERT(expression)\
	do{if consteval{(void)(1/(!!(expression)));}else{if(!(expression)) {logWrite("ERR: " #expression);}}}while(0)
#else
#define LMC_ASSERT(expression) do{}while(0)
#endif

#endif // INC_LMC_ASSERT_HPP
