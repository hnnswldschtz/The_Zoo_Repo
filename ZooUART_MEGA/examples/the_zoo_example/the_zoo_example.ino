#include <ZooUART_MEGA.h>

 // init with RX pin, TXpin, your ID
ZooUART zoo(10); 

// function definitions 
// define your own handlers here, give what ever name you want, but reflect that in setup()
// because they need to be initalized in setup();
// define whatever you want in these functions

//called whenever a global message is received
void GlobalMessageHandler(int sender, int value) {
    Serial.print("Global Msg from ");
    Serial.print(sender);
    Serial.print(": ");
    Serial.println(value);
}
// called whenever a direct message is received
void DirectMessageHandler(int sender, int value) {
    Serial.print("Direct Msg from ");
    Serial.print(sender);
    Serial.print(": ");
    Serial.println(value);
}


void setup() {
    Serial.begin(9600);
    zoo.begin();
    //set up and register callback handlers here
    zoo.onGlobalMessage(GlobalMessageHandler);
    zoo.onDirectMessage(DirectMessageHandler);
}


void loop() {
    zoo.watch(); //must be called in loop to check for incoming messages
    // If you want to send messages, you can do so here or in response to some event. ( e.g. button press, sensor reading, etc.)
    // Send a direct message to address 3, with value 123, from this device
    String msg = zoo.createMessage(false, false, 0, 255, 3); // ping, global, address, value, sender
    zoo.sendMessage(msg);

  
}