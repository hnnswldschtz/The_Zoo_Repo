#include <espnow.h>
#include "ESP8266WiFi.h"

uint8_t receiverAddress[][6] = {
{0x0, 0x0, 0x0, 0x0, 0x0, 0x0},
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
1 USBC
2 USBC
3 USBC
4 USBC
5 USBC
6 MicroUSB
7 MicroUSB
8 MicroUSB
9 MicroUSB
10 old version
*/

typedef struct messageToBeSent {
    char text[64];
    int intVal;
    float floatVal;
} messageToBeSent;

typedef struct receivedMessage {
    char text[64];
} receivedMessage;

messageToBeSent myMessageToBeSent; 
receivedMessage myReceivedMessage; 

void messageSent(uint8_t *macAddr, uint8_t status) {
    Serial.print("Send status: ");
    if(status == 0){
        Serial.println("Success");
    }
    else{
        Serial.println("Error");
    }
}

void messageReceived(uint8_t* macAddr, uint8_t* incomingData, uint8_t len){
    memcpy(&myReceivedMessage, incomingData, sizeof(myReceivedMessage));
    // Find sender index in receiverAddress array
    int senderIdx = -1;
    for (int i = 0; i < sizeof(receiverAddress)/6; i++) {
        bool match = true;
        for (int j = 0; j < 6; j++) {
            if (macAddr[j] != receiverAddress[i][j]) {
                match = false;
                break;
            }
        }
        if (match) {
            senderIdx = i;
            break;
        }
    }
    if (senderIdx >= 0) {
        Serial.print("Incoming Message from device ");
        Serial.print(senderIdx);
        Serial.print(": ");
    } else {
        Serial.print("Incoming Message from unknown MAC: ");
        for (int j = 0; j < 6; j++) {
            if (j > 0) Serial.print(":");
            Serial.printf("%02X", macAddr[j]);
        }
        Serial.print(": ");
    }
    Serial.println(myReceivedMessage.text);

    // --- New logic for D3 control ---
    if (strcmp(myReceivedMessage.text, "D3=ON") == 0) {
        digitalWrite(D3, HIGH);
        Serial.println("D3 set HIGH");
    } else if (strcmp(myReceivedMessage.text, "D3=OFF") == 0) {
        digitalWrite(D3, LOW);
        Serial.println("D3 set LOW");
    }
    // --- End new logic ---

    Serial.println();
}

// Helper: Convert "aa:bb:cc:dd:ee:ff" to uint8_t array
bool parseMac(const char* macStr, uint8_t* macArr) {
    int values[6];
    if (sscanf(macStr, "%x:%x:%x:%x:%x:%x", 
        &values[0], &values[1], &values[2], 
        &values[3], &values[4], &values[5]) == 6) {
        for (int i = 0; i < 6; ++i) macArr[i] = (uint8_t)values[i];
        return true;
    }
    return false;
}

void setup(){
    Serial.begin(115200);
    WiFi.mode(WIFI_STA);

    pinMode(D3, OUTPUT); // <-- Add this line to set D3 as output

    if (esp_now_init() == 0) {
        Serial.println("ESPNow Init success");
    }
    else {
        Serial.println("ESPNow Init fail");
        return;
    }

    esp_now_set_self_role(ESP_NOW_ROLE_COMBO);^
    esp_now_register_send_cb(messageSent);  
    esp_now_register_recv_cb(messageReceived); 

    // check length of receiverAddress structure
    //iterate through all the possible addresses and add them as peers
    for (int i=0;i<sizeof(receiverAddress) / sizeof(receiverAddress[0]);i++) {
      uint8_t result = esp_now_add_peer(receiverAddress[i], ESP_NOW_ROLE_COMBO, 0, NULL, 0);    
      if(result != 0){
        Serial.println("Failed to add peer");
      }
    }

    Serial.println("Enter message in format: index;message");
}
 
void loop(){
    if (Serial.available()) {
        String input = Serial.readStringUntil('\n');
        input.trim();
        int sep = input.indexOf(';');
        if (sep > 0) {
            String idxStr = input.substring(0, sep);
            String msgStr = input.substring(sep + 1);

            int idx = idxStr.toInt();
            if (idx >= 0 && idx < (sizeof(receiverAddress)/6)) {
                // Prepare and send message
                snprintf(myMessageToBeSent.text, sizeof(myMessageToBeSent.text), "%s", msgStr.c_str());
                myMessageToBeSent.intVal = 0;
                myMessageToBeSent.floatVal = 0.0;
                esp_now_send(receiverAddress[idx], (uint8_t *) &myMessageToBeSent, sizeof(myMessageToBeSent));
                Serial.println("Message sent!");
            } else {
                Serial.println("Invalid index!");
            }
        } else {
            Serial.println("Invalid input format! Use: index;message");
        }
    }
}