#include <espnow.h>
#include "ESP8266WiFi.h"



uuint8_t receiverAddress[][6] = {
{0x50, 0x02, 0x91, 0x75, 0x14, 0x16},
{0xBC, 0xFF, 0x4D, 0xCA, 0xBF, 0xB1},
{0x84, 0xCC, 0xA8, 0x98, 0x98, 0xA7},
{0xBC, 0xFF, 0x4D, 0xC9, 0xB2, 0xB7},
{0xBC, 0xFF, 0x4D, 0xC4, 0x20, 0x7B},
{0x24, 0xD7, 0xEB, 0xEA, 0xC5, 0x2B},
{0xEC, 0x64, 0xC9, 0xF1, 0x13, 0x5C},
{0xEC, 0x64, 0xC9, 0xF0, 0x59, 0x29},
{0xF4, 0xCF, 0xA2, 0xEB, 0xFC, 0x18},
{0xA0, 0x20, 0xA6, 0x13, 0x11, 0x60}};
/*
0 USBC
1 USBC
2 USBC
3 USBC
4 USBC
5 MicroUSB
6 MicroUSB
7 MicroUSB
8 MicroUSB
9 old version
*/

typedef struct customMessage {
    bool isGlobal;  //anotates if message is directed at some other device specifically or directly to a specific device
    int address;    //if not global, should have address 0-10
    bool isPing;    //true of message is ping, if not check value(payload)
    int value;      //value of payload 0-255
    int sender;     //our device ID 0-10
} customMessage;


customMessage myReceivedMessage;
customMessage myMessageToSend;

void messageReceived(uint8_t* macAddr, uint8_t* incomingData, uint8_t len){
    memcpy(&myReceivedMessage, incomingData, sizeof(myReceivedMessage));
    Serial.printf("Incoming Message from: %02X:%02X:%02X:%02X:%02X:%02X \n\r", 
            macAddr[0], macAddr[1], macAddr[2], macAddr[3], macAddr[4], macAddr[5]);
    Serial.print("isGlobal: ");
    Serial.println(myReceivedMessage.isGlobal);
    Serial.print("address: ");
    Serial.println(myReceivedMessage.address);
    Serial.print("isPing: ");
    Serial.println(myReceivedMessage.isPing);
    Serial.print("value: ");
    Serial.println(myReceivedMessage.value);
    Serial.print("sender: ");
    Serial.println(myReceivedMessage.sender);
    Serial.println(); 

    // char textMsg[] = "Thanks for the data!";
    // memcpy(&myMessageToBeSent.text, textMsg, sizeof(textMsg));
    // myMessageToBeSent.runTime = millis()/1000;
    // esp_now_send(receiverAddress, (uint8_t *) &myMessageToBeSent, sizeof(myMessageToBeSent));    
}    

void setup(){
    Serial.begin(9600);
    // delay(1000); // uncomment if your serial monitor is empty
    WiFi.mode(WIFI_STA);
    
    if (esp_now_init() == 0) {
        Serial.println("ESPNow Init success");
    }
    else {
        Serial.println("ESPNow Init fail");
        return;
    }

    esp_now_set_self_role(ESP_NOW_ROLE_COMBO);

    // check length of receiverAddress structure
    //iterate through all the possible addresses and add them as peers
    for (int i=0;i<sizeof(receiverAddress) / sizeof(receiverAddress[0]); ;i++) {
      uint8_t result = esp_now_add_peer(receiverAddress[i], ESP_NOW_ROLE_COMBO, 0, NULL, 0);    
      if(result != 0){
        Serial.println("Failed to add peer");
      }
    }
    
    //call back functions for sending and receiving messages
    esp_now_register_send_cb(messageSent);  
    esp_now_register_recv_cb(messageReceived); 

}
 
void loop(){
  //TESTING SEND TO Device ESP
    if(true){
      myMessageToSend.isPing = random(0,2);
      myMessageToSend.isGlobal = random(0,2);
      myMessageToSend.address = random(1,6);
      myMessageToSend.value = random(0,255);
      myMessageToSend.sender = random(0,6);
      esp_now_send(receiverAddress[myMessageToSend.address], (uint8_t *) &myMessageToSend, sizeof(myMessageToSend));
      Serial.println("sending random data");
    }
    delay(10000);
}
