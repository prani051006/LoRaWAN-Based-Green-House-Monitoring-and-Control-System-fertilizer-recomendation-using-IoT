/*
  Sensor Node - green house
*/

#include <LiquidCrystal_I2C.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include "DHT.h"
#include <Wire.h>
#include <BH1750.h>
#include <Adafruit_INA219.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BMP280.h>



String message,aks,wts ;

String dt = "";

#define BMP280_I2C_ADDRESS  0x76  // Change to 0x77 if needed

Adafruit_BMP280 bmp;  // Create BMP280 object

#define ONE_WIRE_BUS 15
OneWire oneWire(ONE_WIRE_BUS);  

// Pass oneWire reference to DallasTemperature library
DallasTemperature sensors(&oneWire);


#define DHTPIN 16

BH1750 lightMeter;
Adafruit_INA219 ina219;

const int pump = 26;
const int fan = 25;
const int heater = 27;
const int led = 12;

int sm = 34;   // Connect soil sensor analog measure pin to D34 pin

 
#define DHTTYPE DHT22
DHT dht(DHTPIN, DHTTYPE);


// set the LCD number of columns and rows
int lcdColumns = 16;
int lcdRows = 2;

// set LCD address, number of columns and rows
// if you don't know your display address, run an I2C scanner sketch
LiquidCrystal_I2C lcd(0x27, lcdColumns, lcdRows);  


int tv,hv,htv,lv,smv, apv;
int vv,iv,pv;
int psv,fsv,lsv,hsv,pfv=0,lfv=0,hfv=0;
int wt=0,ak=0,sd=0;

// Timer variables
unsigned long lastTime = 0;
unsigned long timerDelay = 60000;

void setup()
{
 
  Serial.begin(9600);
  pinMode(15, INPUT_PULLUP);
  pinMode(pump, OUTPUT);
  pinMode(fan, OUTPUT);
  pinMode(heater, OUTPUT);
  pinMode(led, OUTPUT);

  digitalWrite(pump, LOW); 
  digitalWrite(fan, LOW); 
  digitalWrite(heater, LOW); 
  digitalWrite(led, LOW); 

  
  dht.begin();
  sensors.begin();  
  Wire.begin();
  lightMeter.begin();

  if (! ina219.begin()) {
   // Serial.println("Failed to find INA219 chip");
    while (1) { delay(10); }
  }

    if (!bmp.begin(BMP280_I2C_ADDRESS)) {
      //  Serial.println("Could not find BMP280! Check wiring.");
        while (1);
    }

    // Configure BMP280 settings
    bmp.setSampling(Adafruit_BMP280::MODE_NORMAL,
                    Adafruit_BMP280::SAMPLING_X2,  // Temperature Oversampling
                    Adafruit_BMP280::SAMPLING_X16, // Pressure Oversampling
                    Adafruit_BMP280::FILTER_X16,   // Filtering
                    Adafruit_BMP280::STANDBY_MS_500); // Standby time

   
  lcd.init();               
  lcd.backlight();

  
  lcd.clear();                    //LoRa Based Green House Monitoring and Control System Using IoT
  lcd.setCursor( 0, 0);
  lcd.print("LoRa Based Green");
  lcd.setCursor( 0, 1);
  lcd.print("House Monitoring");delay(3000);  

    lcd.clear();                    //LoRa Based Green House Monitoring and Control System Using IoT
  lcd.setCursor( 0, 0);
  lcd.print("  SENSOR NODE   ");
  delay(3000);  
}

void loop()
{

    readSen();  

  if ( (millis() - lastTime) > timerDelay) {

    if(sd==0){sd=1;
       send_dt();delay(3000);
    }

  
 lastTime = millis();
 
 }

 
 if (Serial.available()){
   dt = Serial.readString();
  
   aks = getValue(dt, ',', 0); // ak
    wts = getValue(dt, ',', 1); // wt 
    ak = aks.toInt();
    wt = wts.toInt();
    dt = "";
     lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("WT:");
    lcd.print(wt);
    lcd.setCursor(0,1);
    if(wt==1){
    lcd.print("STORM");}
     if(wt==2){
    lcd.print("SUNNY");}

   delay(3000);

    send_dt(); delay(3000);
    
  }

 
 
}

void readSen(){
  
   hv = dht.readHumidity();
   tv = dht.readTemperature();

   smv = analogRead(sm);
   smv = constrain(map(smv, 0, 4095, 100, 0), 0, 100);

  
  if(tv>-40 && tv<126 && hv>=0 && hv<=100){  
  
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
  
  }
    apv = bmp.readPressure() / 100.0;  // Convert Pa to hPa (millibars)
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("P:");
    lcd.print(apv);lcd.print("hpa");
    delay(3000);

 
  // Read s p temperature from DS18B20
  
  sensors.requestTemperatures();// Send the command to get temperatures 
  htv=sensors.getTempCByIndex(0); //Stores Value in Variable
   
  lv = lightMeter.readLightLevel();

    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("HT:");
    lcd.print(htv); lcd.print(char(223)); lcd.print("C");lcd.print("  ");
    lcd.setCursor(0,1);
    lcd.print("L:");
    lcd.print(lv);lcd.print("Lux "); delay(3000);

  
  iv = ina219.getCurrent_mA();
  vv = ina219.getBusVoltage_V();
  pv = ina219.getPower_mW();
 

  
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("V:");
    lcd.print(vv);lcd.print("V");  
    lcd.setCursor(9,0);
    lcd.print("I:");
    lcd.print(iv);lcd.print("mA ");
    lcd.setCursor(0,1);
    lcd.print("P:");
    lcd.print(pv);lcd.print("mW");  
    delay(3000);

    


 
  if (smv<40) {
    
     digitalWrite(pump, HIGH); 
    psv=1;
     
  }
  if (smv>70) {
    psv=0;
     digitalWrite(pump, LOW); 
    psv=0;
     
  }
  


 if (lv<1000) {
    
     digitalWrite(led, HIGH); 
    lsv=1;
     
  }
  else if (lv>5000) {
    
     digitalWrite(led, LOW); 
    lsv=0;
     
  }
  
 else;




 if (tv>35 || hv>70 || wt==2) {

    if (tv>27 && hv<60) {
    
     digitalWrite(heater, LOW); 
    hsv=0;
     
  }
     digitalWrite(fan, HIGH); 
    fsv=1;
     
  }
  else if (tv<32 && hv<60 && wt==0) {
    
     digitalWrite(fan, LOW); 
    fsv=0;
     
  }
  
 else;



 if (tv<20 || hv>70 || wt==1) {

    if (tv<32 && hv<60 ) {
    
     digitalWrite(fan, LOW); 
    fsv=0;
     
  }
     digitalWrite(heater, HIGH); 
    hsv=1;
     
  }
  else if (tv>27 && hv<60 && wt==0) {
    
     digitalWrite(heater, LOW); 
    hsv=0;
     
  }
  
 else;


  
 if(psv==1){  
  if(pv>500)pfv=0;
  else pfv=1;
 }
 else pfv=0;

 
 if(lsv==1){ 
  if(lv>1000)lfv=0;
  else lfv=1;
 }
 else lfv=0;


 if(hsv==1){
   if(htv>(tv+2))hfv=0;
  else hfv=1;
 }
 else hfv=0;
 
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("PS:");
    lcd.print(psv);
    lcd.setCursor(9,0);
    lcd.print("LS:");
    lcd.print(lsv);
    lcd.setCursor(0,1);
    lcd.print("FS:");
    lcd.print(fsv);
    lcd.setCursor(9,1);
    lcd.print("HS:");
    lcd.print(hsv);
    delay(3000);

    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("PF:");
    lcd.print(pfv);
    lcd.setCursor(9,0);
    lcd.print("LF:");
    lcd.print(lfv);
    lcd.setCursor(9,1);
    lcd.print("HF:");
    lcd.print(hfv);
    delay(3000);
    
    
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
  lcd.clear();
  lcd.setCursor( 0, 0);
  lcd.print("  Sending Data.. ");
  delay(1000);
  
 if(tv>-40 && tv<126 && hv>=0 && hv<=100){  
 
 // Serial.print(hp);Serial.print(",");Serial.println(sp);
  message = message + tv + "," + hv+ "," + smv + "," + htv + "," + lv+  "," + apv + "," + psv + "," + fsv + "," + lsv + "," + hsv + "," + pfv + "," + lfv + "," + hfv ;
  String msg = message;
 Serial.println(msg);
  message = "";
  
  }
  delay(2000);
}

