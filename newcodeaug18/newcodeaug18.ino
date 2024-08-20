#include <Arduino.h>
#include "MHZ19.h"   
#include <Adafruit_GFX.h> // Core graphics library
#include <Adafruit_ST7735.h> // Hardware-specific library for ST7735
#include <SPI.h>
#include <Fonts/Roboto_11.h>



//mhz19 sensor
#define RX_PIN 10                                          // Rx pin which the MHZ19 Tx pin is attached to
#define TX_PIN 11                                          // Tx pin which the MHZ19 Rx pin is attached to
#define BAUDRATE 9600                                      // Device to MH-Z19 Serial baudrate (should not be changed)
MHZ19 myMHZ19;                                             // Constructor for library
HardwareSerial mySerial2(2);                                // (Uno example) create device to MH-Z19 serial

//dht11 sensor
/*
#define DHTTYPE DHT11 
int dhtPin = 11; //change for later
DHT dht(DHTPin, DHTTYPE); //Initialize Dht sensor 
*/  

float Temperature;
float Humidity;
 
// These pins will also work for the 1.8" TFT shield
 
//ESP32-WROOM
#define TFT_DC 21 //A0
#define TFT_CS 18 //CS
#define TFT_MOSI 22 //SDA
#define TFT_CLK 23 //SCK
#define TFT_RST 5
#define TFT_MISO 0
 
Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_MOSI, TFT_CLK, TFT_RST);

unsigned long getDataTimer = 0;

#define backGroundColor ST77XX_BLACK


char prevText[100] = "";
//DEBOUNCE
unsigned long lastDebounceDelay = 0; 
unsigned long debounceDelay = 30;

float p = 3.1415926;

bool cleared = 0; 

int prevC02;
int prevTemp; 
char prevChar[20] = "";

void setup(void) {
  Serial.begin(9600);
  Serial.print(F("AIR QUALITY SENSOR STARTING"));

  // Use this initializer if using a 1.8" TFT screen:
  tft.initR(INITR_BLACKTAB);      // Init ST7735S chip, black tab
  tft.fillScreen(backGroundColor);

  
  //MHZ19 
  mySerial2.begin(BAUDRATE);                               // (Uno example) device to MH-Z19 serial start   
  myMHZ19.begin(mySerial2);                                // *Serial(Stream) refence must be passed to library begin(). 
  myMHZ19.autoCalibration();

  tft.setRotation(3);
  tft.setFont(&Roboto_11);
  
  //Dht11 sensor
  /*
  pinMode(DHTPin, INPUT);
  dht.begin();
  */
  delay(1000);

  // tft print function
}

void loop() {
  if (millis() - getDataTimer >= 1000)
    {   
        dataPage();
        getDataTimer = millis();
    }
  delay(10); //prevent wasted resources
}

void dataPage(){
  int C02;
  int Temp;
  C02 = CO2();
  Temp = temp();
  writeText(22, 32, 1, ST77XX_GREEN, "C02 Level:");
  writeText(115, 32, 1, ST77XX_GREEN, "ppm");
  if (prevC02 != C02) {
    eraseNumber(85, 32, 1, prevC02);
  }
  writeNumber(85, 32, 1, ST77XX_GREEN, C02);
  Serial.println(C02);

  if (prevTemp != Temp) {
    eraseNumber(0, 0, 1, prevTemp);
  }
  writeNumber(0, 0, 1, ST77XX_GREEN, Temp);

  prevC02 = C02;
  prevTemp = Temp;

}

void mainPage() {
  int AQ = 0;
  AQ = airQuality();
  Serial.println(AQ);
  switch(AQ) {
    case 0: 
      writeText(0, 0, 1, ST77XX_GREEN, "NO DATA");
    case 1:
      writeText(0, 0, 1, ST77XX_GREEN, "GOOD");
      
    case 2: 
      writeText(0, 0, 1, ST77XX_GREEN, "MEDIOCRE");
      
    case 3: 
      writeText(0, 0, 1, ST77XX_GREEN, "POOR");
      
    default: 
      writeText(0, 0, 1, ST77XX_GREEN, "NO DATA");
  }
}

int airQuality() {
  Serial.println("AQ"); 
  if (CO2() < 980) {
    return 1; 
    Serial.println("1"); 
  }
  else if (CO2() > 980 && CO2() < 1900) {
    return 2; 
    Serial.println("2"); 
  }
  else if (CO2() > 1900) {
    return 3; 
    Serial.println("3"); 
  }
  else {
    return 0; 
  }
}

int CO2() {
  int CO2; 
  CO2 = myMHZ19.getCO2();    
  Serial.println(CO2);                         // Request CO2 (as ppm)   
  //Serial.print("CO2 (ppm): ");                                          // Request Temperature (as Celsius)
  //Serial.print("Temperature (C): ");                  
  return CO2;
}

int temp(){
  float temp;
  temp = myMHZ19.getTemperature();
  return temp;

}

void eraseNumber(int x, int y, uint8_t size, int num) { // x, y, text size, value (integer)
  tft.setCursor(x,y);
  tft.setTextSize(size);
  tft.setTextColor(backGroundColor);
  tft.print(num);
}

void writeNumber(int x, int y, uint8_t size, int8_t color, int num) {// x, y, text size, color, value (integer)
  Serial.println(size);
  tft.setCursor(x,y);
  tft.setTextSize(size);
  tft.setTextColor(color);
  tft.print(num);
}

void writeText(int x, int y, float size, int8_t color, char text[]) {
  tft.setCursor(x,y);
  tft.setTextSize(size);
  tft.setTextColor(color);
  tft.print(text);

  
}
