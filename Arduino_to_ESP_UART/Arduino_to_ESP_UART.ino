#include<SoftwareSerial.h>

// check Wiring to connect to ESP board
SoftwareSerial Arduino_SoftSerial(10,11); //RX (first one) connect to D2 on ESPboard , TX (second one) connect to D1 use any free pins on your arduino
bool deviceChange = false;
bool globalPing = false;
bool directPing = false;
int globalMessage = 0;
int directMessage = 0;
int directMessageSender = 0;
bool state = false;


// SET HERE YOUR SENDER ID!!!!
#define MY_SENDER_ID 4

/* sender IDs:

1
2
3
4
5
6
7
8
9
10

*/

void setup(){
  //Serial.begin(9600);
 //------------------ copy--------------------  
  Arduino_SoftSerial.begin(9600); // needs to be 9600
  // ----------------------------------------
}


String node_recivedString ="";
void loop()
{   

//-------------- Mainloop call for RECEIVING MESSAGES FROM ESP (copy to your loop) ------------
  
  bool dataFlag = false;
  while(Arduino_SoftSerial.available()>0){
    char c = Arduino_SoftSerial.read();
    node_recivedString += c;
    if (c == '\n'){
      dataFlag=true;
      break;
    }
  }
  if (dataFlag){
      handleMessageFromNode(node_recivedString);
      dataFlag = false;
      node_recivedString="";
    }
  
//--------------------------------------------------------------------------


}


void handleMessageFromNode(String message){
  
  //ReadingMessage from network as int variables
  //------------------------------------------------------------
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

// Serial.println(message); 
// for (int i=0; i<7;i++){
//  Serial.print(message[i]); 
// }
Serial.println(); 
  //handling message 
  //------------------------------------------------------------

  if(isPing && isGlobal){
    //global ping
    handleGlobalPing();
  }else if(isPing){
    //prvate ping
    handleDirectPing(sender);
  }else if(isGlobal){
     //global message 
    handleGlobalMessage(sender, value);
  }else{
    //privte MEssage. one on one 
    handleDirectMessage(sender, value);
  }
  deviceChange = true;
}


//SENDING MESSAGES TO ESP
/**
 * Creates a message with the specified parameters.
 * * use this function to create messages to the network
 *
 * @param ping    A boolean indicating if the message is a ping message.
 * @param global  A boolean indicating if the message is a global message.
 * @param address The address of the message.
 * @param value   The value of the message.
 * @param sender  The sender of the message.
 * @return        The created message as a string.
 */
String createMessage(bool ping, bool global, int address, int value, int sender){

   // to be used in main loop like this:  Arduino_SoftSerial.println(createMessage(false, false, 0, 123, MY_SENDER_ID));
    
    String dataToESP;

    dataToESP += String(ping);
    dataToESP += String(global);

    //adding address if necessary
    if(global){
      dataToESP += "-";
    }else{
      dataToESP += String(address);
    }

    //adding value if necessary
    if(ping){
      dataToESP += "000";
    }else{
      dataToESP += String(value / 100 % 10);
      dataToESP += String(value / 10 % 10);
      dataToESP += String(value % 10);
    }

    //adding my sender id
    dataToESP += String(sender);

    //finishing message
    dataToESP +="\n";

    return dataToESP;
          
      //triggered if message is received from arduino
      //message is in format "01x1230"

      //first char isPing 0 or 1
      //second char isGlobal 0 or 1
      //third char if global insert any char (wont be read later anyway) else int 0-6
      //4th 5th and 6th if Ping insert any chars (wont be read later anyway) else int 000-255
      //7th int of my device 0-6

      //EXAMPLE "0031232" private msg
      // Ping   Glob  addr  val   sender
      // 0      0     3     123   2
      
      //EXAMPLE "11xyyy2" global ping 
      // Ping   Glob  addr  val   sender
      // 1      1     x     yyy   2
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

    }

    void handleDirectMessage(int sender, int value){
      directMessage = value;
      directMessageSender = sender; 

    }

  





