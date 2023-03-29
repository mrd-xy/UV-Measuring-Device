#include <Arduino.h>
#include <SPI.h>
#include "Adafruit_ILI9341esp.h"
#include "Adafruit_GFX.h"
#include "XPT2046.h"
#include "Wire.h"
#include "Adafruit_VEML6070.h"
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>

#include <Fonts\FreeSans9pt7b.h>
#include <Fonts\FreeSans12pt7b.h>
#include <Fonts\FreeSans18pt7b.h>
#include <Fonts\FreeSans24pt7b.h>

//WiFi Settings
#define APSSID "ESP8266"
#define APPSK  "12345678"
const int ap_channel = 6;
const boolean ap_hidden = false;
IPAddress local_ip(192,169,6,100);
IPAddress gateway(192,169,6,254);
IPAddress netmask(255,255,255,0);
ESP8266WebServer server(80);


//Tasten details
#define BUTTON_X 120
#define BUTTON_Y 35
#define BUTTON_W 220
#define BUTTON_H 35
#define BUTTON_SPACING_X 0
#define BUTTON_SPACING_Y 15
#define BUTTON_TEXTSIZE 2

//Farben Hinzufügen
#define sehrhell 0xe5f3
#define hell 0xedb1
#define mittel 0xd52f
#define hellbraun 0xac29
#define braun 0x72a4
#define dunkelbraun 0x4141

//Variabeln definieren
char typ = 1;
float k;
int neu;
float eigen; 
float erst;
char zähler = 0;
int st;
int mi;
float mz;
float maxzeit;
int stunden;
int minuten;
int wert;
int uvindex;

// Tasten Beschriftung
char buttonlabels[6][100] = {"sehr hell", "hell","mittel","hellbraun","braun","schwarz"};
uint16_t buttoncolors[6] = {sehrhell,
                             hell, 
                             mittel,
                             hellbraun,
                             braun,
                             dunkelbraun};
Adafruit_GFX_Button buttons[6];

Adafruit_VEML6070 uv = Adafruit_VEML6070(); 

Adafruit_ILI9341 tft = Adafruit_ILI9341( /*TFT_CS*/ 15,  /*TFT_DC*/ 2);
XPT2046 touch(/*cs=*/ 4, /*irq=*/ 16);  

void handleRoot() 
{server.send(200, "text/html",String(String(wert)+ "/" + String(uvindex) + "/" + String(k) + "/" + String(millis()) + "/" + String(minuten) + "/" + String(stunden)));}

void setup() {

  delay(200);
  Serial.begin(115200);
  delay(200);
  SPI.setFrequency(ESP_SPI_FREQ);
  Wire.begin(0,5);
  
  tft.begin();
  touch.begin(tft.width(), tft.height());  // Must be done before setting rotation
  tft.setRotation(2);  // can be 0, 1, 2, 3
  Serial.print("tftx ="); Serial.print(tft.width()); Serial.print(" tfty ="); Serial.println(tft.height());
  tft.fillScreen(ILI9341_BLACK);

  delay(200);
   
  // Display einrichten
  touch.setCalibration(1832, 262, 264, 1782);

touch.setRotation(touch.ROT180);
  // Tasten erstellen
  for (uint8_t row=0; row<6; row++) {
    uint8_t col=0;
      buttons[col + row].initButton(&tft, BUTTON_X+col*(BUTTON_W+BUTTON_SPACING_X), 
                 BUTTON_Y+row*(BUTTON_H+BUTTON_SPACING_Y),    // x, y, w, h, outline, fill, text
                  BUTTON_W, BUTTON_H, ILI9341_WHITE, buttoncolors[col+row], ILI9341_WHITE,
                  buttonlabels[col + row], BUTTON_TEXTSIZE); 
      buttons[col + row].drawButton();
    }
    
 //WiFi start
  WiFi.softAPConfig(local_ip, gateway, netmask);
  WiFi.softAP(APSSID, APPSK, ap_channel, ap_hidden);
 server.on("/", handleRoot);
 server.begin();
 Serial.println(WiFi.localIP()); 
}




float minuszeit(){
  if (uvindex == 0){mz = 0;}
  else {mz = ((maxzeit)/(((maxzeit/uvindex)*3600)/4));
  }
  return mz;  
}

int stunden1(){
  if (uvindex == 0){st = 24;}
  else {st = (k / uvindex);
  }
  return st;
}

int minuten1(){
  if (uvindex == 0){mi = 0;}
  else {mi = ((k / uvindex) - stunden1())*60;
  }
  return mi;
}

 
int getUVI() {
 char uvi = (wert < (12 * 187*4)) ? (wert + 1) / (187*4) : 11;
  return uvi;}


  
void loop() {
  delay(100);
  erst = millis();
  Serial.println(typ);
  
  if (typ == 1){
  uint16_t x, y;
  if (touch.isTouching()){
    touch.getPosition(x, y);
    Serial.print("touch");Serial.print(x);Serial.println(y);}


  // Schauen, on eine Taste gedrückt wurde
  for (uint8_t b=0; b<6; b++) {
    if (buttons[b].contains(x, y)) {
      typ=10;
      neu = b ;
      buttons[b].drawButton(true);
    } else {
      buttons[b].press(false);
    }
  }
 } 
 else if(typ == 10){ 
 
 uv.begin(VEML6070_4_T);
 
delay(300); Serial.print("neu4: "); Serial.println(neu);
  if(neu == 0) {
   eigen = 0.166666 ; Serial.println(eigen, 4);
  } else if(neu == 1) {
    eigen = 0.25 ; Serial.println(eigen, 4);
  } else if(neu == 2) {
    eigen = 0.416666 ; Serial.println(eigen, 4);
  }else if(neu == 3) {
    eigen = 0.75 ; Serial.println(eigen, 4);
  }else if(neu == 4) {
    eigen = 1 ; Serial.println(eigen, 4);
  }else if(neu == 5) {
    eigen = 1.5 ; Serial.println(eigen, 4);
  }else { Serial.println("Fehler"); 
  }

maxzeit = eigen*8;
  
tft.fillScreen(ILI9341_BLACK);
  
  tft.setFont(&FreeSans12pt7b); 
  tft.setCursor(15, 30);
  tft.setTextColor(ILI9341_YELLOW); tft.setTextSize(1);
  if(neu == 0) {
   tft.println("Hauttyp: sehr hell");
  } else if(neu == 1) {
    tft.println("Hauttyp: hell");
  } else if(neu == 2) {
    tft.println("Hauttyp: mittel");
  }else if(neu == 3) {
    tft.println("Hauttyp: hellbraun");
  }else if(neu == 4) {
    tft.println("Hauttyp: braun");
  }else if(neu == 5) {
    tft.println("Hauttyp: schwarz");
  }else { tft.println("Fehler"); 
  }


  tft.setFont(&FreeSans18pt7b); 
  tft.setCursor(15, 80);
  tft.setTextColor(ILI9341_GREEN);
  tft.setTextSize(1);
  tft.println("Messwert"); 
  
  tft.setCursor(15, 165);
  tft.println("UV-Index"); 
  
  tft.setCursor(15, 240);
  tft.println("Rest Zeit"); 
  
  typ = 100;
  
 }
 else if(typ == 100) {  for ( k = maxzeit; k > -1; k = k - minuszeit()){

wert = uv.readUV();
uvindex = getUVI();
minuten = minuten1();
stunden = stunden1();

    
 Serial.print("UV light level: "); Serial.println(wert);
 Serial.print("UV-Index: "); Serial.println(uvindex);
 Serial.print("maxzeit: "); Serial.println(maxzeit);
 Serial.print("Minuszeit: "); Serial.println(minuszeit());
 Serial.print("Zeit: "); Serial.println(k);
 Serial.print("Stunden: ");Serial.println(stunden);
 Serial.print("Minuten: ");Serial.println(minuten);

  tft.fillRect(5,85,190,40,ILI9341_BLACK); 
  tft.setFont(&FreeSans18pt7b); 
  tft.setTextColor(ILI9341_GREEN);
  tft.setCursor(30, 120);
  tft.println(wert);

  tft.fillRect(0,170,190,40,ILI9341_BLACK); 
  tft.setCursor(30, 200);
  tft.setTextSize(1);
  tft.println(uvindex);
  
   tft.fillRect(0,250,190,40,ILI9341_BLACK); 
  tft.setCursor(30, 280);
  tft.setTextSize(1);
  String pe= "h  ";
  String pz="h ";
  String nu="";
  String minu="min";
  if (minuten<10){tft.println(nu + stunden + pe + minuten + minu);
  }else {tft.println(nu + stunden + pz + minuten + minu);}
  server.handleClient();

tft.fillRect(195,70,40,210,ILI9341_DARKGREY); 
  if ((k / maxzeit)>= 0.4){
  tft.fillRect(200,(75+(200-((k / maxzeit)*200))),30,((k / maxzeit)*200),ILI9341_GREEN);
  }
  else if ((k / maxzeit)>= 0.1){
    tft.fillRect(200,(75+(200-((k / maxzeit)*200))),30,((k / maxzeit)*200),ILI9341_YELLOW);
  }
  else if ((k / maxzeit)> 0){
    tft.fillRect(200,(75+(200-((k / maxzeit)*200))),30,((k / maxzeit)*200),ILI9341_RED);
  } 
  else { 
  if (zähler >= 2){
    tft.fillScreen(ILI9341_RED);
    delay(1000);
    tft.fillScreen(ILI9341_BLACK);
    tft.setFont(&FreeSans18pt7b); 
    tft.setCursor(15, 80);
    tft.setTextColor(ILI9341_RED);
    tft.setTextSize(1);
    tft.println("Messwert"); 
  
    tft.setCursor(15, 165);
    tft.println("UV-Index"); 
  
    tft.setCursor(15, 240);
    tft.println("Rest Zeit"); 
    Serial.print("Fertig");
    zähler = 0;
    k = 0;
    }
    else {zähler++;
    k = 0;
    }
  }   
  Serial.print("Rechenzeit:"); Serial.println(millis()-erst);
  Serial.print("erst: "); Serial.println(millis());
  delay(4000-(millis()-erst));
  erst = millis();
  Serial.print("zweit: "); Serial.println(erst);
}
  } else { tft.println("Fehler"); 
  }
}
