#include "ZooUART.h"

ZooUART::ZooUART(uint8_t rxPin, uint8_t txPin, int senderId)
    : softSerial(rxPin, txPin), mySenderId(senderId) {}

void ZooUART::begin(long baud) {
    softSerial.begin(baud);
}

void ZooUART::loop() {
    bool dataFlag = false;
    while (softSerial.available() > 0) {
        char c = softSerial.read();
        node_recivedString += c;
        if (c == '\n') {
            dataFlag = true;
            break;
        }
    }
    if (dataFlag) {
        handleMessageFromNode(node_recivedString);
        dataFlag = false;
        node_recivedString = "";
    }
}

String ZooUART::createMessage(bool ping, bool global, int address, int value, int sender) {
    String dataToESP;
    dataToESP += String(ping);
    dataToESP += String(global);

    if (global) {
        dataToESP += "-";
    } else {
        dataToESP += String(address);
    }

    if (ping) {
        dataToESP += "000";
    } else {
        dataToESP += String(value / 100 % 10);
        dataToESP += String(value / 10 % 10);
        dataToESP += String(value % 10);
    }

    dataToESP += String(sender);
    dataToESP += "\n";
    return dataToESP;
}

void ZooUART::handleMessageFromNode(String message) {
    if (message.length() < 7) return; // basic check

    bool isPing = (message[0] == '0') ? false : true;
    bool isGlobal = (message[1] == '0') ? false : true;
    int address = isGlobal ? -1 : (message[2] - '0');
    int value = isPing ? 0 : 100 * (message[3] - '0') + 10 * (message[4] - '0') + (message[5] - '0');
    int sender = message[6] - '0';

    if (isPing && isGlobal) {
        globalPing = true;
        if (globalPingCb) globalPingCb();
    } else if (isPing) {
        directPing = true;
        if (directPingCb) directPingCb(sender);
    } else if (isGlobal) {
        globalMessage = value;
        directMessageSender = sender;
        if (globalMessageCb) globalMessageCb(sender, value);
    } else {
        directMessage = value;
        directMessageSender = sender;
        if (directMessageCb) directMessageCb(sender, value);
    }
    deviceChange = true;
}

// Callback setters
void ZooUART::onGlobalPing(void (*cb)()) { globalPingCb = cb; }
void ZooUART::onDirectPing(void (*cb)(int)) { directPingCb = cb; }
void ZooUART::onGlobalMessage(void (*cb)(int, int)) { globalMessageCb = cb; }
void ZooUART::onDirectMessage(void (*cb)(int, int)) { directMessageCb = cb; }