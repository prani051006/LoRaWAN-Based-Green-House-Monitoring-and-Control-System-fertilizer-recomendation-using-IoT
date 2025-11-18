/*
  Sensor Node - ard - lora
*/

#include <SPI.h>              // include libraries
#include <LoRa.h>


#define ss 10
#define rst 9
#define dio0 2


byte msgCount = 0;            // count of outgoing messages
byte Node = 0xAA;
byte Aut = 0x9C;


// Tracks the time since last event fired
unsigned long previousMillis = 0;
unsigned long int previoussecs = 0;
unsigned long int currentsecs = 0;
unsigned long currentMillis = 0;
int interval = 1 ; // updated every 1 second
int Secs = 0;


String dt ;

String incoming = "";

int tv,hv,htv,lv,smv, apv;
int vv,iv,pv;
int psv,fsv,lsv,hsv,pfv=0,lfv=0,hfv=0;
int wt=0,ak=0;

// Timer variables
unsigned long lastTime = 0;
unsigned long timerDelay = 20000;

void setup()
{
 
  Serial.begin(9600);
   
  //setup LoRa transceiver module
  LoRa.setPins(ss, rst, dio0);
  
  //replace the LoRa.begin(---E-) argument with your location's frequency 
  //433E6 for Asia
  //866E6 for Europe
  //915E6 for North America
  while (!LoRa.begin(433E6)) {
 
    delay(500);
  }
 
                     delay(2000);
  
}

void loop()
{

  
  // parse for a packet, and call onReceive with the result:
  onReceive(LoRa.parsePacket());
   incoming = "";
  
  

  
 if (Serial.available()){
   dt = Serial.readString();

  send_dt();
  }



  if ( (millis() - lastTime) > timerDelay) {

 lastTime = millis();
 
 }
 
 
}






void send_dt()
{
 sendMessage(dt, Node, Aut);
  dt = "";
  delay(2000);

}


void sendMessage(String outgoing, byte ReqNode, byte Node) {
  LoRa.beginPacket();                   // start packet
  LoRa.write(ReqNode);                  // add destination address
  LoRa.write(Node);                     // add sender address
  LoRa.write(msgCount);                 // add message ID
  LoRa.write(outgoing.length());        // add payload length
  LoRa.print(outgoing);                 // add payload
  LoRa.endPacket();                     // finish packet and send it
  msgCount++;                           // increment message ID
}




void onReceive(int packetSize) {
 
  if (packetSize == 0) return;          // if there's no packet, return
  // read packet header bytes:
  int recipient = LoRa.read();          // recipient address
  byte sender = LoRa.read();            // sender address
     
  byte incomingMsgId = LoRa.read();     // incoming msg ID
  byte incomingLength = LoRa.read();    // incoming msg length
  while (LoRa.available()) {
    incoming += (char)LoRa.read();
  }
 //  Serial.println(incoming);
  if (incomingLength != incoming.length()) {   // check length for error
//    Serial.println("error: message length does not match length");
    ;
    incoming = "";
    return;                             // skip rest of function
  }
  
  


    String wts = getValue(incoming, ',', 0); // wt
      
   
    wt = wts.toInt();
   
    incoming = "";
   ak=1;

   String message = message + ak + "," + wt ;
 
   String msg = message;
 Serial.println(msg);
  message = "";
  
 
ak=0;
}

String getValue(String data, char separator, int index)
{
  int found = 0;
  int strIndex[] = { 0, -1 };
  int maxIndex = data.length() - 1;
  for (int i = 0; i <= maxIndex && found <= index; i++) {
    if (data.charAt(i) == separator || i == maxIndex) {
      found++;
      strIndex[0] = strIndex[1] + 1;
      strIndex[1] = (i == maxIndex) ? i + 1 : i;
    }
  }
  return found > index ? data.substring(strIndex[0], strIndex[1]) : "";
}