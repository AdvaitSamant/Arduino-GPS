// library we need to include
#include <Adafruit_SSD1306.h>
#include <splash.h>
#include <Adafruit_GFX.h>
#include <Adafruit_GrayOLED.h>
#include <Adafruit_SPITFT.h>
#include <Adafruit_SPITFT_Macros.h>
#include <gfxfont.h>
#include <Wire.h>
#include <TinyGPS++.h>
#include <TinyGPSPlus.h>
#include <SoftwareSerial.h>

// Defining the RX and TX Pins of GSM and GPS
#define RX_Pin 6
#define TX_Pin 5
static const int RXPin = 13, TXPin = 12;
static const uint32_t GPSBaud = 9600;

//Defining the width and height of the OLED Panel
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

//Declaration for an SSD display connected to I2C (SDA,SCL pins)
Adafruit_SSD1306 display(SCREEN_WIDTH,SCREEN_HEIGHT,&Wire,-1);

//RX,TX for Neo-6M GPS module
SoftwareSerial ss(RXPin,TXPin);

//RX,TX for GSM SIM 800L module
SoftwareSerial gsm(RX_Pin,TX_Pin);

//Tiny GPS Module 
TinyGPSPlus gps;

void setup()
{
  //OLED Panel setup
  Serial.begin(115200);
  display.clearDisplay();
  display.setTextSize(3);
  display.setTextColor(WHITE);

  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3c))
  {
    //Address 0x3D for 128x64
    Serial.println(F("SSD1306 allocation failed!!!"));
  }

  display.setCursor(0,20);

  // Display Static Text
  display.clearDisplay();
  display.println("GPS PetLocator");
  display.display();
  
  // Baud Rate for Arudino
  Serial.begin(9600);

  //Baud rate for GPS
  ss.begin(9600);

  //Baud rate for GSM
  gsm.begin(9600);

  //Delay which allowes the module to initialize
  delay(1000);

  //Code For Gsm SIM SMS
  gsm.println("AT+CMGF=1:"); //Sets GSM mode to SMS
  delay(100);
  gsm.println("AT+CMGS=\"+916350479764\"");
  delay(100);
  gsm.println("Initializing The GPS Location: Waiting For The GPS Signal!!!");
  delay(100);
  gsm.write(26);
  delay(1000);
}

void loop()
{
  //Checks if GPS Location of the pet is obtained
  while (ss.available() > 0)
  {
    
    if(gps.encode(ss.read()))
    {
      //Extract latitude and latitude
      float lt = (gps.location.lat(),4);
      float ln = (gps.location.lng(),4);

      //Display GPS Loaction of the Pet On Serial Monitor 
      Serial.print("Latitude : ");
      Serial.print(gps.location.lat(),6);
      Serial.print("Longitude : ");
      Serial.print(gps.location.lng(),6);

      //Display Static Text
      display.clearDisplay();
      display.println(String(lt)+ " "+String(ln));
      display.display();

      //Sending GPS Location Of The Pet To The Owner Via SMS
      gsm.println("AT+CMGF=1:"); //Sets GSM mode to SMS
      delay(100); //Delays for 1 sec
      gsm.println("AT+CMGS=\"+916350479764\""); //Sets what number to message
      delay(100); //Delays for 1 sec
      gsm.println("Current GPS Location: \n Latitude:" + String(lt)+ "\nLongitude"+String(ln)); //Sends the SMS to the owner
      delay(100); //Delays for 1 sec
      gsm.write(26); //Delays for 1 sec
      delay(1000); //Delays For 10 secs

      display.setCursor(0,20);

      //Makes The GSM Module Wait For 30 Secs Before Making It Send Another SMS
      delay(30000);
    }
    else
    {
      display.setCursor(0,20);

      //Display Static Text
      display.clearDisplay();
      display.println("NO GPS DATA");
      display.display();
    }
  }
}
