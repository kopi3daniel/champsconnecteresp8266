// librerias modulo Wifi Acces Point
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>

#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128     // OLED display width, in pixels
#define SCREEN_HEIGHT 64     // OLED display height, in pixels
#define OLED_RESET -1        // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C  ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// Definir las credenciales de acceso
const char *ssid ="Champs connect";
const char *password ="1234567890";

// Variables para lectura del DHT 11
float t;
float h;
float f;
float hh,hhh;

#define transistor D6
ESP8266WebServer server(80);

//Creamos el objeto LCD
//LiquidCrystal_I2C lcd(0x27,16,2);

#include "DHT.h"

#define DHTPIN D5     // Digital pin connected to the DHT sensor
// Feather HUZZAH ESP8266 note: use pins 3, 4, 5, 12, 13 or 14 --
// Pin 15 can work but DHT must be disconnected during program upload.

// Uncomment whatever type you're using!
#define DHTTYPE DHT11   // DHT 11
//#define DHTTYPE DHT22   // DHT 22  (AM2302), AM2321
//#define DHTTYPE DHT21   // DHT 21 (AM2301)

DHT dht(DHTPIN, DHTTYPE);

void setup() {

  Serial.begin(9600);

  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;)
      ;  // Don't proceed, loop forever
  }

  
  display.display();
  delay(2000);  // Pause for 2 seconds
  display.display();
  display.clearDisplay();
  display.display();
  display.setTextSize(1);  // taille du texte
  display.setTextColor(WHITE);
  display.setCursor(0, 0);  // position du curseur colonne / ligne
  display.println("Champs connect ...");
  display.setTextSize(1);
  display.setCursor(0,25);
  display.print("Chargement");
  display.display();
  for (int i = 0 ; i < 73 ; i++ ) {
    display.setCursor(60 + i, 20);
    display.display();
    display.print(".");
    display.display();
    delay(1000);
  }
  display.println();
  display.clearDisplay();
  display.display();
  display.display();
  delay(3000);

  pinMode(A0,INPUT);

  dht.begin();

  // configuramo el modo acces Point
  WiFi.softAP(ssid, password);
  server.on("/", handleRoot);
  server.begin();
  // inciar la LCD 
//  display.init();  
//  display.backlight(); // Enciende luz de fondo
  display.setCursor(0, 0);  // position du curseur colonne / ligne
  display.println("Champs connect ...");
  display.setTextSize(1);
  //display.clearDisplay();
  display.display();
  display.println(" ");
  IPAddress myIP = WiFi.softAPIP();
  display.print("IP: ");
  display.print(myIP);
  display.display();
  
  pinMode (transistor , OUTPUT);
}

void loop() {

  LecturaTH();
  server.handleClient();



}

// creamos la funcion Estructura HTML
void handleRoot(){
  server.send(200,"text/html","<html><head><meta name='viewport' content='width=device-width, user-scalable=no'>"
  "<meta http-equiv='refresh' content='5' charset='UTF-8'><title>Champs connect</title>"
  "<style>body {font-family: monospace; background-color: rgb(20,21, 38);  }"
  "h1 {  color: white; text-align: center;  margin: 50px; font-size: 18px; }"
  ".barraTemp{ height: 200px; width: 200px; margin: auto;"
  "border-radius: 50%; overflow: hidden; } .cont{ height: 100px; width: 200px;"
  "} .circulo_centro { width: 150px; height: 150px; background-color: rgb(20,21, 38);"
  "border-radius: 50%; margin: auto; position: relative; margin: -175px auto 50px;"
  "} .rectangulo{ width: 200px; height: 100px; background-color: rgb(20,21, 38);"
  "position: relative; margin: -125px auto 50px; } </style> </head>"
  "<body> <h1>Champs connect matanda</h1>  <h1>Temperature ambiante</h1>"
  
  "<div class='barraTemp' style='transform: rotate("+String(map(t,0,100,0,180))+"deg);'>"
  "<div class='cont' style='background-color: rgb(143, 136, 136);'></div>"
  "<div class='cont' style='background-color: rgb(253,0,0);'></div>"
  "</div><div class='circulo_centro'></div><div class='rectangulo'></div>"
  "<h1 style='font-size: 22px; position: relative; "
  
  "margin: -125px auto 50px;'>"+String(t)+" °C</h1>"
  
  "<h1>Humidite ambiante</h1> <div class='barraTemp' style='transform: rotate("+String(map(h,0,100,0,180))+"deg);'>"
  "<div class='cont' style='background-color: rgb(143, 136, 136);'></div>"
  "<div class='cont' style='background-color: rgb(253,0,0);'></div> "
  "</div> <div class='circulo_centro'></div> <div class='rectangulo'></div>"
  "<h1 style='font-size: 22px; position: relative; margin: -125px auto 50px;'>"+String(h)+" %</h1>"

  "<h1>Humidite sol</h1> <div class='barraTemp' style='transform: rotate("+String(map(hh,0,100,0,180))+"deg);'>"
  "<div class='cont' style='background-color: rgb(143, 136, 136);'></div>"
  "<div class='cont' style='background-color: rgb(253,0,0);'></div> "
  "</div> <div class='circulo_centro'></div> <div class='rectangulo'></div>"
  "<h1 style='font-size: 22px; position: relative; margin: -125px auto 50px;'>"+String(hh)+" %</h1>"
  
  "</body></html>");
   
  
 }

// funcion de lectura de temperatura y humedad
void LecturaTH(){

  h = dht.readHumidity();
  t = dht.readTemperature();
  f = dht.readTemperature(true);

  // Check if any reads failed and exit early (to try again).
  if (isnan(h) || isnan(t) || isnan(f)) {
    Serial.println(F("Failed to read from DHT sensor!"));
    return;
  }

  // Compute heat index in Fahrenheit (the default)
  float hif = dht.computeHeatIndex(f, h);
  // Compute heat index in Celsius (isFahreheit = false)
  float hic = dht.computeHeatIndex(t, h, false);

  Serial.print(F("Humidity: "));
  Serial.print(h);
 
  Serial.print(F("%  Temperature: "));
  Serial.print(t);
  Serial.print(F("°C "));
  Serial.print(f);
 
  Serial.print(F("°F  Heat index: "));
  Serial.print(hic);
  Serial.print(F("°C "));
  Serial.print(hif);
  Serial.println(F("°F"));  
  hh=analogRead(A0)/10.24;
 // hhh=(hh*(3.3/1023))*30;
  if(hh<40){
    digitalWrite(transistor,HIGH);
  }
  else{
     digitalWrite(transistor,LOW);
  }
  display.display();
 //  display.setTextSize(1.8,1.8);  // taille du texte
  display.setTextColor(WHITE);
  display.println(" ");
  display.setCursor(0,30);
  display.print("T:");
  display.print(t);
  display.println(" ");
  display.display();
  display.display();
  display.print("H:");
  display.print(h);
  display.display();
  display.display();
  display.println(" ");
  display.display();
  display.display();
  display.print("H sol:");
  display.print(hh);
  display.display();
  display.display();
}
