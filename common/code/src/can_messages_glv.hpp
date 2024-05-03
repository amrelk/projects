#pragma once

#include "can_messages.hpp"

#define TELEM_CANMSG(name, channelID) CANMSG(name, id(TELEM, channelID), { u16 data[4]; })

#define DEVICE(name, deviceID) CANMSG(name, id(STATUS, deviceID), u8 flags;)

namespace can {

// all standard packet IDs start with 3 bits of namespace
// TELEM  packet IDs are followed by the data channel ID
// STATUS packet IDs are followed by the device ID

// LOW, MEDIUM, HIGH_FREQ_TELEM, HIGH, and CRITICAL are arbitrarily assigned

enum CanIDNamespace {
    LOW = 0b111, // we can drop these and we don't care

    // everything else is higher priority, a dropped packet is considered a critical failure

    TELEM = 0b110, // 50hz and 50/(2^n) hz telemetry packets. majority of the bus utilization
    // telem packets are always u16[4] at

    STATUS = 0b101, // reports the status of a specific device. Always a "Status" packet
    // these are sent when the bottom 6 LSB of the device ID match the current tick
    // so this entire namespace only takes up one packet in each tick
    // ~0.8hz lol
    // each bit is individual in a status packet. The first 4 are standardized. the rest of the bits are per-device
    // bit 1 - unhealthy flag.
    // bit 2 - missing can messages flag
    // bit 3 - unable to send can messages flag
    // bit 4 - not enough cpu time flag
    // bit 5-8 per-device flag

    NORMAL = 0b100, // timing guarantee of "transmitted within one tick"

    HIGH_FREQ_TELEM = 0b011, // at >50hz, this is for stuff like throttle, brake pedal, and wheel speed during LC

    HIGH = 0b010, // misc

    CRITICAL = 0b000, // what CarHeartbeat goes under
};

constexpr CanID_t id(CanIDNamespace canIDNamespace, u8 id) { return (canIDNamespace << 8 | id); }


enum CarStatus : u8 { OK = 0, NOT_OK, DRIVER_DECEASED, ON_FIRE };

// sent at 50hz
CANMSG(CarHeartbeat, id(CRITICAL, 0), {
    u32 tick; // starts at zero, strictly increasing. should last 3 years
    CarStatus status;
})

CANMSG(CarSettings, id(HIGH, 0), {
    u32 time;     // epoch seconds
    u8 frequency; // nominally '50' for '50hz'. This is how ofter MsgCarHeartbeat will be sent out
    bool atComp;
})

CANMSG(RTDStateChange, id(HIGH, 1), {
    bool isActive; // true if Ready To Drive (RTD)
})

CANMSG(PedalParams, id(HIGH, 2), {
    u16 max_traction_torque;
    u16 max_regen_torque;
    u16 max_battery_discharge_current;
    u16 max_battery_charge_current;
})

CANMSG(Throttle, id(HIGH_FREQ_TELEM, 0), { u32 throttle; })

CANMSG(Brake, id(HIGH_FREQ_TELEM, 1), { u32 brake; })

TELEM_CANMSG(IMU_YAW, 0)
TELEM_CANMSG(IMU_PITCH, 1)
TELEM_CANMSG(IMU_ROLL, 2)
TELEM_CANMSG(IMU_X, 3)
TELEM_CANMSG(IMU_y, 4)
TELEM_CANMSG(IMU_Z, 5)
TELEM_CANMSG(MAG_X, 6)
TELEM_CANMSG(MAG_Y, 7)
TELEM_CANMSG(MAG_Z, 8)
TELEM_CANMSG(BRAKE_TRAVEL, 9)
TELEM_CANMSG(THROTTLE_TRAVEL, 10)
TELEM_CANMSG(VBUS, 11)
TELEM_CANMSG(IBUS, 12)
TELEM_CANMSG(MOTOR_RPM, 13)
TELEM_CANMSG(GPS_LAT, 14)
TELEM_CANMSG(GPS_LON, 15)
TELEM_CANMSG(STEERING_WHEEL_ANGLE, 16)
TELEM_CANMSG(SUSPENSION_FR, 17)
TELEM_CANMSG(SUSPENSION_FL, 18)
TELEM_CANMSG(SUSPENSION_BR, 19)
TELEM_CANMSG(SUSPENSION_BL, 20)
TELEM_CANMSG(WHEELSPEED_FR, 21)
TELEM_CANMSG(WHEELSPEED_FL, 22)
TELEM_CANMSG(WHEELSPEED_BR, 23)
TELEM_CANMSG(WHEELSPEED_BL, 24)
TELEM_CANMSG(DRIVER_HEARTRATE, 25)


CANMSG(InverterHC1Demands, EXT(0x19107171), {
    i16 torqueRequest;
    u16 controlWord;
    i16 torqueLimit;
    u8 checksum;
    u8 seqCounter;
})

CANMSG(InverterHC2Demands, EXT(0x19117171), {
    i16 torqueRegenLimit;
    i16 speedLimitForwardRPM;
    i16 speedLimitReverseRPM;
    u8 checksum;
    u8 seqCounter;
})

CANMSG(InverterHC3BatteryDemands, EXT(0x19127171), {
    i16 maxDischargeI;
    i16 maxRegenI;
    i16 dcVoltageLink; // must be zero
    u8 checksum;
    u8 seqCounter;
})

CANMSG(InverterHS1TorqueFeedback, EXT(0x1918FF71), {
    i16 torqueMeasured;
    i16 speedMeasured;
    i16 dcLinkCurrent;
    u8 checksum;
    u8 seqCounter;
})

} // namespace can
