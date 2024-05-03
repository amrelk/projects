//
// Created by evelyn on 4/22/24.
//

#pragma once

#ifdef USE_FLASH

#include "common.hpp"


namespace ev24::flash {
    u32 readWord(u32 index);
    bool erase();
    bool writeWord(u32 index, u32 word);
    u32* getAddress(u32 index);
} // ev24::flash


#endif
