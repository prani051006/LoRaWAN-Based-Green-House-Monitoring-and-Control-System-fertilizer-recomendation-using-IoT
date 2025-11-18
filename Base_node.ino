/*.
 
  Base - green house
  
*/
#include <WiFi.h>
#include <HTTPClient.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <ThingSpeak.h>  // Include the ThingSpeak library

#include <LiquidCrystal_I2C.h>

#include <SPI.h>              // include libraries
#include <LoRa.h>
#include <Wire.h>

// ThingSpeak credentials
unsigned long channelID = 2834230;  // Replace with your ThingSpeak Channel ID
const char* apiKey = "I1MJATXZKPJM09FW";        // Replace with your ThingSpeak Write API Key

WiFiClient client;

const char* ssid = "iot";
const char* password = "qwertyuiop!";

String serverNamewrite = "https://dreampixelz.in/25/iot25/write.php";
String serverNamereadwt = "https://dreampixelz.in/25/iot25/readwt.php";


int tv,hv,htv,lv,smv, apv;
int vv,iv,pv;
int psv,fsv,lsv,hsv,pfv=0,lfv=0,hfv=0;

int wt=0;

const int buz = 4;

String message ;

#define ss 5
#define rst 14
#define dio0 2

// Tracks the time since last event fired
unsigned long previousMillis = 0;
unsigned long int previoussecs = 0;
unsigned long int currentsecs = 0;
unsigned long currentMillis = 0;
int interval = 1 ; // updated every 1 second
int Secs = 0;

byte msgCount = 0;            // count of outgoing messages
byte Node = 0xAB;
byte Aut = 0x9C;



// set the LCD number of columns and rows
int lcdColumns = 16;
int lcdRows = 2;

// set LCD address, number of columns and rows
// if you don't know your display address, run an I2C scanner sketch
LiquidCrystal_I2C lcd(0x27, lcdColumns, lcdRows);  

String incoming = "";
unsigned long last = 0, lastTime =0, lastTime1 =0;
unsigned long timerDelay = 60000;
unsigned long timerDelay1 = 2000;

void setup()
{
 
  Serial.begin(115200);

   
     
  pinMode(buz, OUTPUT);
  

  digitalWrite(buz, LOW); 
   

  //setup LoRa transceiver module
  LoRa.setPins(ss, rst, dio0);
  
  //replace the LoRa.begin(---E-) argument with your location's frequency 
  //433E6 for Asia
  //866E6 for Europe
  //915E6 for North America
  while (!LoRa.begin(433E6)) {
    Serial.print(".");
    delay(500);
  }
  Serial.println("LoRa Initializing OK!");
       
 lcd.init();               
  lcd.backlight();

   lcd.clear(); 
   lcd.setCursor( 0, 0);
   lcd.print("Connecting WiFi");
   
  WiFi.begin(ssid, password);
  Serial.println("Connecting");
  while(WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to WiFi network with IP Address: ");
  Serial.println(WiFi.localIP());

   lcd.clear(); 
   lcd.setCursor( 0, 0);
   lcd.print("WiFi - Connected");delay(3000);  

    // Initialize ThingSpeak
  ThingSpeak.begin(client);
    
  
  lcd.clear();                    //LoRa Based Green House Monitoring and Control System Using IoT
  lcd.setCursor( 0, 0);
  lcd.print("LoRa Based Green");
  lcd.setCursor( 0, 1);
  lcd.print("House Monitoring");delay(3000);  

    

  read_wt();delay(2000);

  lcd.clear();                    //LoRa Based Green House Monitoring and Control System Using IoT
  lcd.setCursor( 0, 0);
  lcd.print("   BASE NODE   ");
  delay(3000);  
}

void loop()
{

       // parse for a packet, and call onReceive with the result:
  onReceive(LoRa.parsePacket());
   incoming = "";


}


void sendMessage(String outgoing, byte ReqNode, byte Node2) {
  LoRa.beginPacket();                   // start packet
  LoRa.write(ReqNode);              // add destination address
  LoRa.write(Node2);             // add sender address
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
   Serial.println(incoming);
  if (incomingLength != incoming.length()) {   // check length for error
    Serial.println("error: message length does not match length");
    ;
    incoming = "";
    return;                             // skip rest of function
  }
  
 
  lcd.clear();    
  lcd.setCursor(0,0);
  lcd.print("RECEIVING DATA...");    delay(3000); 
  
  // if message is for this device, or broadcast, print details:
  Serial.println("Received from: 0x" + String(sender, HEX));
  Serial.println("Sent to: 0x" + String(recipient, HEX));
  Serial.println("Message ID: " + String(incomingMsgId));
   Serial.println("Message length: " + String(incomingLength));
   Serial.println("Message: " + incoming);
  Serial.println("RSSI: " + String(LoRa.packetRssi()));
   Serial.println("Snr: " + String(LoRa.packetSnr()));
   Serial.println();



    String ts = getValue(incoming, ',', 0); // tv
    String hs = getValue(incoming, ',', 1); // hv
    String sms = getValue(incoming, ',', 2); // smv
    String hts = getValue(incoming, ',', 3); // htv
    String ls = getValue(incoming, ',', 4); // lv
    String aps = getValue(incoming, ',', 5); // apv
    String pss = getValue(incoming, ',', 6); // psv
    String fss = getValue(incoming, ',', 7); // fsv
    String lss = getValue(incoming, ',', 8); // lsv
    String hss = getValue(incoming, ',', 9); // hsv
    String pfs = getValue(incoming, ',', 10); // pfv
    String lfs = getValue(incoming, ',', 11); // lfv
    String hfs = getValue(incoming, ',', 12); // hfv

    
    // tv,hv,ch4v,cov,fsv,fcv,lsv;
   
    tv = ts.toInt();
    hv = hs.toInt();
    smv = sms.toInt();
    htv = hts.toInt();
    lv = ls.toInt();
    apv = aps.toInt();
    psv = pss.toInt();
    fsv = fss.toInt();
    lsv = lss.toInt();
    hsv = hss.toInt();
    pfv = pfs.toInt();
    lfv = lfs.toInt();
    hfv = hfs.toInt();    
    
    incoming = "";
   

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("  Sensor Node   ");delay(2000);
    

  
  
  if(tv>10 && tv<126 && hv>=20 && hv<=100){  
  
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("T:");
    lcd.print(tv); lcd.print(char(223)); lcd.print("C");lcd.print("  ");
    lcd.setCursor(9,0);
    lcd.print("H:");
    lcd.print(hv);lcd.print("% ");
    lcd.setCursor(0,1);
    lcd.print("SM:");
    lcd.print(smv);lcd.print("% ");
    delay(3000);

    if(tv>35){ digitalWrite(buz, HIGH);   lcd.clear();
       lcd.setCursor(0,0);
    lcd.print("Temp. High");delay(1500); digitalWrite(buz, LOW); 
    }
    if(tv<20){ digitalWrite(buz, HIGH);   lcd.clear();
       lcd.setCursor(0,0);
    lcd.print("Temp. Low");delay(1500); digitalWrite(buz, LOW); 
    }
    if(hv>80){ digitalWrite(buz, HIGH);   lcd.clear();
       lcd.setCursor(0,0);
    lcd.print("Humi. High");delay(1500); digitalWrite(buz, LOW); 
    }
    if(hv<40){ digitalWrite(buz, HIGH);   lcd.clear();
       lcd.setCursor(0,0);
    lcd.print("Humi. Low");delay(1500); digitalWrite(buz, LOW); 
    }

     if(smv>80){ digitalWrite(buz, HIGH);   lcd.clear();
       lcd.setCursor(0,0);
    lcd.print("S.Moi. High");delay(1500); digitalWrite(buz, LOW); 
    }
    if(smv<30){ digitalWrite(buz, HIGH);   lcd.clear();
       lcd.setCursor(0,0);
    lcd.print("S.Moi. Low");delay(1500); digitalWrite(buz, LOW); 
    }
    
    
    
    
  
  
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("P:");
    lcd.print(apv);lcd.print("hpa");
    lcd.setCursor(9,0);
    lcd.print("HT:");
    lcd.print(htv); lcd.print(char(223)); lcd.print("C");lcd.print("  ");
    lcd.setCursor(0,1);
    lcd.print("L:");
    lcd.print(lv);lcd.print("Lux "); delay(3000);

     
    if(lv<500){ digitalWrite(buz, HIGH);   lcd.clear();
       lcd.setCursor(0,0);
    lcd.print("Light Int Low");delay(1500); digitalWrite(buz, LOW); 
    }


    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("PS:");
    lcd.print(psv);
    lcd.setCursor(9,0);
    lcd.print("FS:");
    lcd.print(fsv);
    lcd.setCursor(0,1);
    lcd.print("LS:");
    lcd.print(lsv);
    lcd.setCursor(9,1);
    lcd.print("HS:");
    lcd.print(hsv);    
    delay(3000);

    

    
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("PFS:");
    lcd.print(pfv);
    lcd.setCursor(0,1);
    lcd.print("LFS:");
    lcd.print(lfv);
    lcd.setCursor(9,1);
    lcd.print("HFS:");
    lcd.print(hfv);    
    delay(3000);

     if(pfv==1){ digitalWrite(buz, HIGH);   lcd.clear();
       lcd.setCursor(0,0);
    lcd.print("Pump Failure");delay(1500); digitalWrite(buz, LOW); 
    }
    if(lfv==1){ digitalWrite(buz, HIGH);   lcd.clear();
       lcd.setCursor(0,0);
    lcd.print("Light Failure");delay(1500); digitalWrite(buz, LOW); 
    }
     if(hfv==1){ digitalWrite(buz, HIGH);   lcd.clear();
       lcd.setCursor(0,0);
    lcd.print("Heater Failure");delay(1500); digitalWrite(buz, LOW); 
    }

    send_dt(); delay(3000);
  }
  
    read_wt(); delay(5000);

    send_wt();delay(2000);

  
  
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



void send_dt()
{
  
  //Send Data

  
   
    //Check WiFi connection status
    if(WiFi.status()== WL_CONNECTED){
      HTTPClient http;
  lcd.clear();    
  lcd.setCursor(0,0);
  lcd.print("Sending Dt Cld...");    
      String serverPath = serverNamewrite + "?t=" + tv + "&h=" + hv + "&sm=" + smv + "&ht=" + htv + "&l=" + lv + "&ap=" + apv + "&ps=" + psv + "&fs=" + fsv + "&ls=" + lsv + "&hs=" + hsv + "&pf=" + pfv + "&lf=" + lfv + "&hf=" + hfv;
      
      // Your Domain name with URL path or IP address with path
      http.begin(serverPath.c_str());
      
      // Send HTTP GET request
      int httpResponseCode = http.GET();
      
      if (httpResponseCode>0) {
        Serial.print("HTTP Response code: ");
        Serial.println(httpResponseCode);
        String payload = http.getString();
        Serial.println(payload);
      }
      else {
        Serial.print("Error code: ");
        Serial.println(httpResponseCode);
      }
      // Free resources
      http.end();
    }
    else {
      Serial.println("WiFi Disconnected");
    }
   
    delay(3000);

 lcd.clear();    
  lcd.setCursor(0,0);
  lcd.print("Sending TGS Cld...");    
    // Set values for fields
  ThingSpeak.setField(1, pfv);
  ThingSpeak.setField(2, lfv);
  ThingSpeak.setField(3, hfv);

  // Send data to ThingSpeak
  int status = ThingSpeak.writeFields(channelID, apiKey);

  if (status == 200) {
   // Serial.println("Data sent to ThingSpeak successfully!");
  } else {
   // Serial.print("Error sending data. HTTP Response Code: ");
  //  Serial.println(status);
  }
 delay(2000);
   
}


void read_wt()
{


  lcd.clear();    
  lcd.setCursor(0,0);
  lcd.print("Rd WT Dt Cld...");   
  
    //Receive Data
 
    //Check WiFi connection status
    if(WiFi.status()== WL_CONNECTED){
      HTTPClient http;

      String serverPath = serverNamereadwt;
      
      // Your Domain name with URL path or IP address with path
      http.begin(serverPath.c_str());
      
      // Send HTTP GET request
      int httpResponseCode = http.GET();
      
      if (httpResponseCode>0) {
        Serial.print("HTTP Response code: ");
        Serial.println(httpResponseCode);
        String payload = http.getString();
        wt = payload.toInt();
        Serial.println(payload);

        lcd.setCursor(0,1);
  lcd.print("WT:");  
   lcd.setCursor(3,1);
  lcd.print(wt);  
  delay(2000); 
        
      }
      else {
        Serial.print("Error code: ");
       // Serial.println(httpResponseCode);
      }
      // Free resources
      http.end();
    }
    else {
      Serial.println("WiFi Disconnected");
    }
   


}




void send_wt()
{
  lcd.clear();
  lcd.setCursor( 0, 0);
  lcd.print("  Sending wt.. ");
  delay(1000);
  message = message + wt + "," + wt;
  String msg = message;
  sendMessage(msg, Node, Aut);
  message = "";
  delay(2000);
   lcd.clear();                   
  lcd.setCursor( 0, 0);
  lcd.print("   BASE NODE   ");
}