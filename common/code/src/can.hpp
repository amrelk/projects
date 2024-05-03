
#pragma once

#include "can.h"
#include "can_messages.hpp"
#include "common.hpp"
#include "etl/map.h"
#include "etl/priority_queue.h"
#include "etl/vector.h"


#ifdef STM32F042x6
#define NUM_CAN_BUS 1
#elif STM32F413xx
#define NUM_CAN_BUS 3
#elif STM32G031xx
#error No CAN Bus on this chip, lol
#else
#error Unknown how many CAN busses there are on this chip
#endif

namespace can {
struct Callback {
    CanID_t id;
    void (*callback)(u8 *buf);
};


class Bus;
extern etl::map<CAN_HandleTypeDef *, Bus *, NUM_CAN_BUS> bus_map;

class Bus {
public:
    explicit Bus(CAN_HandleTypeDef *hcan) : hcan(hcan){};

    void init();

    template<typename T>
    void register_callback(void (*callback)(T *)) {
        add_callback(Callback{ msgId<T>, (void (*)(u8 *)) callback });
    };

    template<typename T>
    void register_namespace_callback(u8 namespace_bits, void (*callback)(T *, u8)) {
        if(namespace_bits > 0b111) { ev24::init_panic("namespace bits out of range"); }
        namespace_callbacks[namespace_bits] = (void (*)(u8 *, CanID_t)) callback;
    }

    template<CanMsg T>
    bool send_message(T message) {
        CanMsg_t msg{
            .id = msgId<T>,
            .len = sizeof(T),
        };
        memcpy(msg.data, (char *) (&message), sizeof(T));
        return send_message(msg);
    }

    template<CanMsg T>
    bool send_namespaced_message_with_id(T message, u8 id) {
        CanMsg_t msg{ .id = (CanID_t) (msgId<T> | id), .len = sizeof(T) };
        memcpy(msg.data, (char *) (&message), sizeof(T));
        return send_message(msg);
    }

    void msg_pending_callback(u32 fifo);
    void tx_complete_callback(u32 fifo);

    struct CanMsg_t {
        CanID_t id;
        u8 data[8];
        u8 len;
        // the only thing we need to define here for etl::priority_queue to work
        bool operator<(const CanMsg_t &other) const { return other.id < id; }
    };


private:
    bool put_message_in_mailbox(CanMsg_t msg);
    void add_callback(Callback callback);
    bool send_message(CanMsg_t msg);

    CAN_HandleTypeDef *hcan;
    etl::map<CanID_t, void (*)(u8 *), 10> callback_table;
    etl::priority_queue<CanMsg_t, 20> tx_queue;
    void (*namespace_callbacks[8])(u8 *, CanID_t id) = { nullptr }; // 0b111 = 7
};

} // namespace can
