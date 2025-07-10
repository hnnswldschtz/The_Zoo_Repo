#include <ZooUART_MEGA.h>

ZooUART zoo(4); // RX, TX, Sender ID


// function definitions (give what ever name you want)
// need to be initalized in setup();

void GlobalPingHandler() {
    Serial.println("Global Ping!");
}

void DirectPingHandler(int sender) {
    Serial.print("Direct Ping from ");
    Serial.println(sender);
}

void GlobalMessageHandler(int sender, int value) {
    Serial.print("Global Msg from ");
    Serial.print(sender);
    Serial.print(": ");
    Serial.println(value);
}

void DirectMessageHandler(int sender, int value) {
    Serial.print("Direct Msg from ");
    Serial.print(sender);
    Serial.print(": ");
    Serial.println(value);
}

void setup() {
    Serial.begin(9600);
    zoo.begin(9600);
    //set up and init callback handlers here
    zoo.onGlobalPing(GlobalPingHandler);
    zoo.onDirectPing(DirectPingHandler);
    zoo.onGlobalMessage(GlobalMessageHandler);
    zoo.onDirectMessage(DirectMessageHandler);
}

void loop() {
    zoo.watch();
    // To send: zoo.sendMessage(zoo.createMessage(...));
    // Send a direct message to address 3, with value 123, from this device
    String msg = zoo.createMessage(false, false, 3, 123, 4); // ping, global, address, value, sender
    zoo.sendMessage(msg);
}

