# The Zoo Repo
## Code and documentation for The Zoo project 

### Using esp8266 as a external WIFI/espNow antenna:
    run Arduino_to_ESP_UART.ino on arduino
    run ESP8266_antenna_uart.ino on esp8266 

connect both devices via UART (rx->tx; tx->rx)
RX (pin10) connect to D2 on ESPboard , TX (second one) connect to D1 use any free pins on your arduino
on arduino pin 10 (rx) to D2 on ESPboard and 11 (tx) to D1 on espboard (nodeMCu 1.0)

Now send and receive data from esp8266 with Arduino Uno as mainboard and control logic 

### Using esp8266 standalone
    run ESP8266_standalone.ino on esp8266 (NodeMCU 1.0)
    
Now send and receive data from esp8266 with all control logic

