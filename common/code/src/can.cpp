#ifndef EV24_EXCLUDE_CAN


#include "can.hpp"

namespace can {
etl::map<CAN_HandleTypeDef *, Bus *, NUM_CAN_BUS> bus_map;


void Bus::add_callback(can::Callback callback) {
    if(callback_table.contains(callback.id)) { ev24::init_panic("Callback for that ID already exists"); }
    if(callback_table.full()) { ev24::init_panic("Callback table FULL"); }
    callback_table[callback.id] = callback.callback;
}

bool Bus::put_message_in_mailbox(CanMsg_t msg) {
    CAN_TxHeaderTypeDef header;
    u32 mailbox;
    if((msg.id & U32_MSB) == U32_MSB) {
        header.IDE = CAN_ID_EXT;
        header.ExtId = msg.id;
    } else {
        header.IDE = CAN_ID_STD;
        header.StdId = msg.id;
    }
    header.RTR = CAN_RTR_DATA;
    header.DLC = msg.len;
    return HAL_OK == HAL_CAN_AddTxMessage(hcan, &header, msg.data, &mailbox);
}

bool Bus::send_message(CanMsg_t msg) {
    __disable_irq();
    if(tx_queue.empty() && HAL_CAN_GetTxMailboxesFreeLevel(hcan) > 0) {
        bool succ = put_message_in_mailbox(msg);
        __enable_irq();
        return succ;
    }
    if(tx_queue.full()) {
        __enable_irq();
        return false;
    }
    tx_queue.push(msg);
    __enable_irq();
    return true;
}

void Bus::msg_pending_callback(u32 fifo) {
    CAN_RxHeaderTypeDef header;
    u8 data[8] = { 0 }; // technically shouldn't ever be read, but we want to avoid issues where it is
    HAL_CAN_GetRxMessage(hcan, fifo, &header, data);
    if(header.IDE == CAN_ID_STD && callback_table.contains((CanID_t) header.StdId)) {
        callback_table[(CanID_t) header.StdId](data);
    }
    u8 namespace_num = header.StdId >> 8;
    //if(namespace_callbacks[namespace_num] != nullptr) { namespace_callbacks[namespace_num](data, header.StdId & 0xF); }
}

void Bus::tx_complete_callback(u32 fifo) {
    // TODO do we want to do something with this fifo?
    __disable_irq(); // we don't want someone else adding a message in before we can
    if(!tx_queue.empty()) {
        CanMsg_t msg = tx_queue.top();
        tx_queue.pop();
        put_message_in_mailbox(msg);
    }
    __enable_irq();
}

void Bus::init() {
    bus_map[hcan] = this;

    // activate the callbacks

    HAL_CAN_ActivateNotification(hcan, CAN_IT_RX_FIFO0_MSG_PENDING);
    HAL_CAN_ActivateNotification(hcan, CAN_IT_RX_FIFO1_MSG_PENDING);
    HAL_CAN_ActivateNotification(hcan, CAN_IT_TX_MAILBOX_EMPTY);
    // filter nothing in hardware, we do all the filtering in software
    // In the future, we could set up some custom filters on some devices (maybe sensor boards?) to save cpu cycles.
    // But for now, an interrupt at 6khz is not an issue at all
    CAN_FilterTypeDef filter = {
        .FilterMaskIdHigh = 0x0000,
        .FilterMaskIdLow = 0x0000,
        .FilterFIFOAssignment = 0,
        .FilterBank = 0,
        .FilterMode = CAN_FILTERMODE_IDMASK,
        .FilterScale = CAN_FILTERSCALE_16BIT,
        .FilterActivation = ENABLE,
    };
    HAL_CAN_ConfigFilter(hcan, &filter);
    HAL_StatusTypeDef e = HAL_CAN_Start(hcan);
    if(e != HAL_OK) { ev24::init_panic("HAL_CAN_Start didn't return HAL-OK"); }
}

} // namespace can
void HAL_CAN_TxMailbox0CompleteCallback(CAN_HandleTypeDef *can) { can::bus_map[can]->tx_complete_callback(0); }
void HAL_CAN_TxMailbox1CompleteCallback(CAN_HandleTypeDef *can) { can::bus_map[can]->tx_complete_callback(1); }
void HAL_CAN_TxMailbox2CompleteCallback(CAN_HandleTypeDef *can) { can::bus_map[can]->tx_complete_callback(2); }

void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *can) { can::bus_map[can]->msg_pending_callback(0); }
void HAL_CAN_RxFifo1MsgPendingCallback(CAN_HandleTypeDef *can) { can::bus_map[can]->msg_pending_callback(1); }

#endif
