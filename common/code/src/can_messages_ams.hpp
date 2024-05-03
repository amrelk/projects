
#pragma once

#include "can_messages.hpp"

namespace can {

// VOLTAGE_DATA, TEMP_DATA_ONE, and TEMP_DATA_TWO follow this format for ID:

// <3 bit namespace><4 bit segment ID><4 bit array index>

enum CanIDNamespace {
    LOW = 0b111, // warnings, faults

    VOLTAGE_DATA = 0b110,
    TEMP_DATA_ONE = 0b101,
    TEMP_DATA_TWO = 0b100,

    OTHER = 0b011, // segment heartbeat

    COMMANDS = 0b010, // commands from AMS to segments
    AMS_HEARTBEAT = 0b000  // AMS heartbeat to segments
};

constexpr CanID_t id(CanIDNamespace canIDNamespace, u8 id) { return (canIDNamespace << 8 | id); }


CANMSG(SegmentConfig, id(COMMANDS, 0x1), {
    u16 upperVoltageCutoff; // in 100µVs. should be 4200000 nominally
    u16 lowerVoltageCutoff; // in 100µVs. Should be 2500000 nominally
})

CANMSG(SegmentHeartbeat, id(OTHER, 0x1), {
    u16 voltage_mv; // in millivolts. At the last zero crossing if there was one within 1s, otherwise just a reading
    bool happy; // means nothing to the computer, is exclusively human-facing data
    u8 segmentID; // segment ID, should be stored on the segment EEPROM.
       // The segments are not going to be told where they physically are - that is the job of the AMS controller
    bool shouldFault; // this is a true/false "should the car fault". This will immediately fault the car if true.
    bool isBalancing; // if the segment is balancing or not
})

CANMSG(SegmentHeartbeatTwo, id(OTHER, 0x2), {
    u8 segmentID;
    u8 die_temp; // in c
})


CANMSG(RequestForVoltages, id(COMMANDS, 0x2), { u8 segmentID; })

// array index = n means this is transmitting the (n * 2) and (n * 2 + 1)th cell voltage
CANMSG(VoltageData, id(VOLTAGE_DATA, 0x0), { u16 voltages[2]; })


CANMSG(RequestForTempData, id(COMMANDS, 0x3), { u8 segmentID; })
CANMSG(TempDataOne, id(TEMP_DATA_ONE, 0x0), { i16 temps[2]; })
CANMSG(TempDataTwo, id(TEMP_DATA_TWO, 0x0), { i16 temps[2]; }) // the 17 and 18th thermistor. index should always be zero


CANMSG(Balance, id(COMMANDS, 0x4), { u16 voltage; })

CANMSG(Warning, id(LOW, 0x1), { u8 id; u8 e; u8 msg[7]; })
CANMSG(Fault, id(LOW, 0x2), { u8 id; u8 e; u8 msg[7]; })

CANMSG(AMSHeartBeat, id(AMS_HEARTBEAT, 0x1), {})

} // namespace can
