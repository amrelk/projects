//
// Created by evelyn on 4/22/24.
//
#ifdef USE_FLASH
#include "flash.hpp"
#include "main.h"

extern u32 _user_store_start;

namespace ev24::flash {
    u32 readWord(u32 index) {
        return *(&_user_store_start + index);
    }

    bool erase() {
        if (HAL_OK != HAL_FLASH_Unlock()) return false;
        u32 pe;
        FLASH_EraseInitTypeDef e {
            .TypeErase = FLASH_TYPEERASE_PAGES,
            .PageAddress = (u32)&_user_store_start,
            .NbPages = 1
        };
        if (HAL_OK != HAL_FLASHEx_Erase(&e, &pe)) return false;
        if (HAL_OK != HAL_FLASH_Lock()) return false;
        return true;
    }

    bool writeWord(u32 index, u32 word) {
        if (HAL_OK != HAL_FLASH_Unlock()) return false;
        if (HAL_OK != HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, (u32)&_user_store_start + 4*index, word)) return false;
        if (HAL_OK != HAL_FLASH_Lock()) return false;
        return true;
    }

    u32* getAddress(u32 index) {
        return (&_user_store_start + index);
    }
} // ev24::flash
#endif