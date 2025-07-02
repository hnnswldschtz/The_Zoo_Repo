# The Zoo Repo


## Using esp8266 as a external WIFI/espNow antenna: 
Send and receive data from esp8266 (nodeMCu 1.0) with Arduino Uno as mainboard and control logic.

Three things needed: 

1. connect both devices via UART (rx->tx; tx->rx) and power (GND,3v3)
2. upload and run `ESP8266_antenna_uart.ino` on ESPboard
install zooUART Library to Arduino IDE
3. include `#include <ZooUART.h>` into your sketch and make use of it



### 1. Connect
- Data:
    - connect __Arduino pin 10 (rx) to D2 on ESPboard__
    - connect __Arduino 11 (tx) to D1 on ESPboard__ (nodeMCu 1.0)
- Power:
    - connect __GND Arduino <-> to GND EspBoard__
    - connect __3v3 on arduino to 3v3 on espboard__ 


### 2. Installation

__On ESPBoard__ (NodeMCU 1.0), 
- upload and run: `ESP8266_antenna_uart.ino`

__On Arduino__
- download *zooUART* folder and copy it into the library folder: ~/documents/Arduino/libraries (or similar in windows)
- Restart the Arduino IDE.
- Include the library in your sketch:
~~~ c++
#include <ZooUART.h>
~~~

### library usage in your Arduino sketch: ###

### Example
```c++
include <ZooUART.h>


ZooUART zoo(10, 11, 4); // init with RX pin, TXpin, your ID


// function definitions (give what ever name you want)
// need to be registered as callbacks in setup();

void myGlobalMessageHandler(int sender, int value) {
    // put whatever you need when a global message is incoming 
    Serial.print("Global Msg from ");
    Serial.print(sender);
    Serial.print(": ");
    Serial.println(value);
}

void myDirectMessageHandler(int sender, int value) {
     // put whatever you need when a direct message to you is incoming 
    Serial.print("Direct Msg from ");
    Serial.print(sender);
    Serial.print(": ");
    Serial.println(value);
}

void setup() {
    Serial.begin(9600);
    zoo.begin();
    //register and init callback handlers here (your functions!)
    zoo.onGlobalMessage(myGlobalMessageHandler);
    zoo.onDirectMessage(myDirectMessageHandler);
}

void loop() {
    zoo.watch();
    // To send: zoo.sendMessage(zoo.createMessage(...));
    // Send a direct message to address 3, with value 123, from this device
    // because of an IF THEN or switch statement example: Button press
    String msg = zoo.createMessage(0, 0, 3, 123, 4); // ping, global, address, value, sender
    zoo.sendMessage(msg); // send the message through the esp antenna to the zoo. 
}
```

### Library Reference 
Constructor: 
~~~c++ 
ZooUART zoo(rxPin, txPin, senderId);
~~~

- rxPin: Arduino pin for receiving UART data
- txPin: Arduino pin for transmitting UART data
- senderId: Unique ID (0-9) for this (your) device

Methods:

~~~c++ 
void begin(long baud = 9600);
~~~
Initializes the SoftwareSerial port. Default baud rate is 9600.
~~~c++ 
void watch();
~~~
Call this in your main loop() to process incoming messages.
~~~ c++ 
String createMessage(bool ping, bool global, int address, int value, int sender);
~~~
Creates a formatted message string.

- `ping`: true for ping message, false otherwise
- `global`: true for global message, false for direct
- `address`: Target address (ignored for global)
- `value`: Value to send (ignored for ping)
- `sender`: Sender ID

<br>Callback Registration.\
Register functions to handle incoming messages:
~~~ c++ 
    void onGlobalPing(void (*cb)());
    void onDirectPing(void (*cb)(int sender));
    void onGlobalMessage(void (*cb)(int sender, int value));
    void onDirectMessage(void (*cb)(int sender, int value));
~~~


#### Using esp8266 standalone (without Arduino UNO as host)
#### send and receive data from esp8266 as mainboard with all control logic:
    
- use 
`ESP8266_standalone.ino` on esp8266 (NodeMCU 1.0) as a base for your code


to create and send messages use:
``` c++

createAndSendMsgFromString("0062235");  
    //EXAMPLE private msg to #6, val 223 from #5 
    // Ping   Glob  addr  val   sender
    // 0      0     6     223   5

```
to handle incoming messages use and modify according to your needs:
``` c++
void handleGlobalMessage(int sender, int value){
  
  // put your code here (remove existing content if you like )
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
  // put your code here (remove existing content if you like )  
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
```
    


