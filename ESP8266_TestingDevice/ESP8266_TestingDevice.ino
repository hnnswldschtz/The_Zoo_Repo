// 3 at usb-serial001

#include <espnow.h>
#include "ESP8266WiFi.h"

//Started SoftwareSerial at RX and TX pin of ESP8266/NodeMCU

uint8_t receiverAddress[][6] = {
{0x50, 0x02, 0x91, 0x75, 0x14, 0x16},
{0xBC, 0xFF, 0x4D, 0xCA, 0xBF, 0xB1},
{0x84, 0xCC, 0xA8, 0x98, 0x98, 0xA7},
{0xBC, 0xFF, 0x4D, 0xC9, 0xB2, 0xB7},
{0xBC, 0xFF, 0x4D, 0xC4, 0x20, 0x7B},
{0x24, 0xD7, 0xEB, 0xEA, 0xC5, 0x2B},
{0xEC, 0x64, 0xC9, 0xF1, 0x13, 0x5C},
{0xEC, 0x64, 0xC9, 0xF0, 0x59, 0x29},
{0xF4, 0xCF, 0xA2, 0xEB, 0xFC, 0x18},
{0xA0, 0x20, 0xA6, 0x13, 0x11, 0x60},
{0x5C, 0xCF, 0x7F, 0x6C, 0xD8, 0x15} 
};
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
    int address;    //if not global, should have address 0-9
    bool isPing;    //true of message is ping, if not check value(payload)
    int value;      //value of payload 0-255
    int sender;     //our device ID 0-9
    
} customMessage;

customMessage myReceivedMessage;  //receive message
customMessage myMessageToSend;   //send message

String arduino_recivedString;

bool deviceChange = false;
bool globalPing = false;
bool directPing = false;
int globalMessage = 0;
int directMessage = 0;
int directMessageSender = 0;
bool state = false;





/**
 * @brief Callback function for sending messages.
 * 
 * This function is called when a message is sent.
 * It takes the MAC address of the recipient and the status of the send operation as parameters.
 * 
 * @param macAddr The MAC address of the recipient.
 * @param status The status of the send operation (0 for success, non-zero for error).
 */
void messageSent(uint8_t *macAddr, uint8_t status) {
    Serial.print("Send status: ");
    if(status == 0){
        Serial.println("Success");
    }
    else{
        Serial.println("Error");
    }
}

/**
 * @brief Callback function for receiving messages.
 * 
 * This function is called when a message is received.
 * It takes the MAC address of the sender, the incoming data, and the length of the data as parameters.
 * 
 * @param macAddr The MAC address of the sender.
 * @param incomingData The incoming data.
 * @param len The length of the incoming data.
 */

void messageReceived(uint8_t* macAddr, uint8_t* incomingData, uint8_t len){
    memcpy(&myReceivedMessage, incomingData, sizeof(myReceivedMessage));

    // Print the MAC address of the sender
    Serial.printf("Incoming Message from: %02X:%02X:%02X:%02X:%02X:%02X \n\r", 
            macAddr[0], macAddr[1], macAddr[2], macAddr[3], macAddr[4], macAddr[5]);    
    // Print the received message
    Serial.print("Message: ");
//Sending data to arduino using serial connection
    String data;

    data += String(myReceivedMessage.isPing);
    data += String(myReceivedMessage.isGlobal);
    
    if(myReceivedMessage.isGlobal){
      data += "-";
    }else{
      data += String(myReceivedMessage.address);
    }
    if(myReceivedMessage.isPing){
      data += "000";
    }else{
      data += String(myReceivedMessage.value / 100 % 10);
      data += String(myReceivedMessage.value / 10 % 10);
      data += String(myReceivedMessage.value % 10);
    }
    data += String(myReceivedMessage.sender);
    data +="\n";
    //first char isPing 0 or 1
    //second char isGlobal 0 or 1
    //third char if global insert any char (wont be read later anyway) else int 0-6 ADDRESS
    //4th 5th and 6th if Ping insert any chars (wont be read later anyway) else int 000-255 VALUE
    //7th int of my device 0-6

    //EXAMPLE "01x1230"
    Serial.print("complete string:    ");
    Serial.println(data);
}


/**
 * @brief Handles the message received from Arduino.
 * 
 * This function is responsible for processing the message received from Arduino.
 * 
 * @param message The message received from Arduino.
 */
void createAndSendMsgFromString(String message){ //handleMessageFromArduino
      
      //triggered if message is received from arduino
      //message is in format "01x1230"

      //first char isPing 0 or 1
      //second char isGlobal 0 or 1
      //third char if global insert any char (wont be read later anyway) else int 1-9 for adresses
      //4th 5th and 6th if Ping insert any chars (wont be read later anyway) else int 000-255
      //7th int of my device 1-9

      //EXAMPLE "0031232" private msg
      // Ping   Glob  addr  val   sender
      // 0      0     3     123   2
      
      //EXAMPLE "11xyyy2" global ping 
      // Ping   Glob  addr  val   sender
      // 1      1     x     yyy   2

    bool isPing = (message[0] == '0') ? false : true;
    bool isGlobal = (message[1] == '0') ? false : true;
    int address;
    if(isGlobal){
      address = -1;
    }else{
      address = (message[2]- '0');
    }
    int value;
    if(isPing){
      value = 0;
    }else{
      value = 100*(message[3]- '0') + 10*(message[4]- '0') + (message[5]- '0');
    }
    int sender = message[6] - '0';

    myMessageToSend.isPing = isPing;
    myMessageToSend.isGlobal = isGlobal;
    myMessageToSend.address = address;
    myMessageToSend.value = value;
    myMessageToSend.sender = sender;
    //debugging
    Serial.print("\nisPing: ");
    Serial.println(myMessageToSend.isPing);
    Serial.print("isGlobal: ");
    Serial.println(myMessageToSend.isGlobal);
    Serial.print("address: ");
    Serial.println(myMessageToSend.address);
    Serial.print("value: ");
    Serial.println(myMessageToSend.value);
    Serial.print("sender: ");
    Serial.println(myMessageToSend.sender);
  
  //SEND MESSAGE 
    //if (address>=0){ // if address is not global
      esp_now_send(receiverAddress[address], (uint8_t *) &myMessageToSend, sizeof(myMessageToSend));
   //} else { //0 is global message->adress all devices
      //for (int i=0; i<sizeof(receiverAddress) / sizeof(receiverAddress[0]);i++){
       // esp_now_send(receiverAddress[i], (uint8_t *) &myMessageToSend, sizeof(myMessageToSend));
   //   }
    }
}

void handleMessageFromNetwork(String message){
  
    //ReadingMessage from network as int variables
    //------------------------------------------------------------
    bool isPing = (!myReceivedMessage.isPing) ? false : true;
    bool isGlobal = (!myReceivedMessage.isGlobal) ? false : true;
    int address;

    if(isGlobal){
        address = -1;
    }else{
      address = (myReceivedMessage.address);
    }
    int value;

    if(isPing){
        value = 0;
    }else{
        value = myReceivedMessage.value;
    }
    int sender = myReceivedMessage.sender;

    // Serial.println(message); 
    // for (int i=0; i<7;i++){
    //  Serial.print(message[i]); 
    // }
    


    //handling message 
    //------------------------------------------------------------

    if(isPing && isGlobal){
      handleGlobalPing();
    }else if(isPing){
      handleDirectPing(sender);
    }else if(isGlobal){
      handleGlobalMessage(sender, value);
    }else{
      handleDirectMessage(sender, value);
    }
    deviceChange = true;
}

    //use this functions to trigger your actions

void handleGlobalPing(){
  globalPing = true;
}

void handleDirectPing(int sender){
  directPing = true;

}

void handleGlobalMessage(int sender, int value){
  
  globalMessage = value;  
  directMessageSender = sender; 
  Serial.print("Incoming global Message from: "); 
  Serial.print(sender); 
  Serial.print(" value: "); 
  Serial.println(value); 
  digitalWrite(13, HIGH);
  delay(100);
    digitalWrite(13, LOW);
}

void handleDirectMessage(int sender, int value){
  directMessage = value;
  directMessageSender = sender; 
  Serial.print("Incoming direct Message from: "); 
  Serial.print(sender); 
  Serial.print(" value: "); 
  Serial.println(value); 
  digitalWrite(13, HIGH);
  delay(100);
    digitalWrite(13, LOW);
}


void setup(){

    //serial connection to computer
    Serial.begin(9600);
    // delay(1000); // uncomment if your serial monitor is empty
    //handles serial connection to arduino via wire
    //NodeMcu_SoftSerial.begin(9600);

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
    for (int i=0;i<sizeof(receiverAddress) / sizeof(receiverAddress[0]);i++) {
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

if(true){
    for (int k=0; k<250; k+=50){
        myMessageToSend.sender = random(0,9);
      for (int i=0; i<sizeof(receiverAddress) / sizeof(receiverAddress[0]); i++){
        myMessageToSend.isPing = 0;
        myMessageToSend.isGlobal = 0;
        myMessageToSend.address = i;
        myMessageToSend.value = k;
        
        esp_now_send(receiverAddress[myMessageToSend.address], (uint8_t *) &myMessageToSend, sizeof(myMessageToSend));
        Serial.print("sending random data to: ");
        Serial.print(myMessageToSend.address);
        Serial.print(" with value: ");
        Serial.println(myMessageToSend.value);
        delay (10);
      }
      delay(10000);
    }
    
  }

}
