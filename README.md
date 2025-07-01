# The Zoo Repo
## Code and documentation for The Zoo project 

### Using esp8266 as a external WIFI/espNow antenna:
Send and receive data from esp8266 (nodeMCu 1.0) with Arduino Uno as mainboard and control logic:

#### connect both devices via UART (rx->tx; tx->rx) and power (GND,3v3):

Data: 
    On arduino connect pin 10 (rx) to D2 on ESPboard and 
    11 (tx) on arduino to D1 on espboard (nodeMCu 1.0)
Power: 
    connect GND Arduino <-> to GND EspBoard and 
    connect 3v3 on arduino to 3v3 on espboard to supply power from Arduino to espBoard. 


### Installation

__On ESPBoard__ (NodeMCU 1.0), 
- upload and run: `ESP8266_antenna_uart.ino`

__On Arduino__
- download zooUART folder and copy it into ~/documents/Arduino/libraries
- Restart the Arduino IDE.
- Include the library in your sketch:

    `#include <ZooUART.h>`


### library usage in your Arduino sketch:
Constructor
    ZooUART zoo(rxPin, txPin, senderId);
- rxPin: Arduino pin for receiving UART data
- txPin: Arduino pin for transmitting UART data
- senderId: Unique ID (0-9) for this device

Methods:

    void begin(long baud = 9600);
Initializes the SoftwareSerial port. Default baud rate is 9600.

    void loop();
Call this in your main loop() to process incoming messages.

    String createMessage(bool ping, bool global, int address, int value, int sender);
Creates a formatted message string.

- `ping`: true for ping message, false otherwise
- `global`: true for global message, false for direct
- `address`: Target address (ignored for global)
- `value`: Value to send (ignored for ping)
- `sender`: Sender ID

Callback Registration
Register functions to handle incoming messages:

    void onGlobalPing(void (*cb)());
    void onDirectPing(void (*cb)(int sender));
    void onGlobalMessage(void (*cb)(int sender, int value));
    void onDirectMessage(void (*cb)(int sender, int value));




### Using esp8266 standalone
send and receive data from esp8266 as mainboar with all control logic:
    
    upload and run 
        ESP8266_standalone.ino
    on esp8266 (NodeMCU 1.0)
    


