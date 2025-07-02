#ifndef ZOO_UART_H
#define ZOO_UART_H

#include <Arduino.h>
#include <SoftwareSerial.h>

class ZooUART {
public:
    ZooUART(uint8_t rxPin, uint8_t txPin, int senderId);

    void begin(long baud = 9600);
    void loop();
    String createMessage(bool ping, bool global, int address, int value, int sender);
    void sendMessage(String message);

    // User-implementable callbacks
    void onGlobalPing(void (*cb)());
    void onDirectPing(void (*cb)(int sender));
    void onGlobalMessage(void (*cb)(int sender, int value));
    void onDirectMessage(void (*cb)(int sender, int value));

    // State variables
    bool deviceChange = false;
    bool globalPing = false;
    bool directPing = false;
    int globalMessage = 0;
    int directMessage = 0;
    int directMessageSender = 0;

private:
    SoftwareSerial softSerial;
    int mySenderId;
    String node_recivedString = "";

    // Callback pointers
    void (*globalPingCb)() = nullptr;
    void (*directPingCb)(int) = nullptr;
    void (*globalMessageCb)(int, int) = nullptr;
    void (*directMessageCb)(int, int) = nullptr;

    void handleMessageFromNode(String message);
};

#endif