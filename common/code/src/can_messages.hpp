#pragma once

#include "typedefs.hpp"

#define CANMSG(name, id, fields)                   \
    struct __attribute__((__packed__)) Msg##name   \
        fields                                     \
    ;                                              \
    template<>                                     \
    constexpr CanID_t msgId<Msg##name> = id;       \
    template<>                                     \
    constexpr const char *msgName<Msg##name> = #name;

constexpr u32 U32_MSB = 1 << 31;

#define EXT(ID) (ID | U32_MSB)

namespace can {

// if the U32_MSB is 1, then it is an extended ID. if the MSB is 0, then it's a standard ID
typedef u32 CanID_t;

template<typename T>
constexpr CanID_t msgId = 0;

template<typename T>
constexpr const char *msgName = "";

template<typename T>
concept CanMsg = requires {
    sizeof(T) <= 8;
    msgId<T> != 0;
    sizeof(T) != 0; // TEMPORARY
};

template<CanMsg T>
u8 *pack(T *msg) {
    return (u8 *) msg;
}

template<CanMsg T>
T *unpack(u8 *buf) {
    return (T *) buf;
}

struct CanMsg_t {
    CanID_t id;
    u8 data[8];
    u8 len;
};

} // namespace can
