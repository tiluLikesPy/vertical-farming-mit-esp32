#include <Arduino.h>
#include "DHT.h"
#include <Adafruit_Sensor.h>

#include "WiFi.h"
#include "ESPAsyncWebServer.h" 


// Netzwerk daten
const char*ssid = "Tims hotspot";
const char* password = "Passwort ist privat";



// Definierung der Pins
#define DHTPIN 15
#define DHTTYPE DHT22

#define PIN_RED_LED 2
#define PIN_GREEN_LED 0

const int relay1 = 32;
const int relay2 = 33;
const int relay3 = 21;
const int relay4 = 22;


const char* ntpServer = "pool.ntp.org";
const long  gmtOffset_sec = 0;
const int   daylightOffset_sec = 3600;

// definiert die Uhrzeiten, wann das licht an ist (Wert -1 für die aktuelle Uhrzeit, z.b. wenn timeForLight gleich 8, entspricht das 7 Uhr in der Wirklichkeit)
const int timeForLight= 7 || 8 || 9 || 10 || 11 || 12 || 13 || 14 || 15 || 16 || 17 || 18 || 19;

DHT dht(DHTPIN, DHTTYPE);

AsyncWebServer server(80);

// diese Funktion "definiert" und "startet" die wichtigsten Prozesse
void setup() {
    Serial.begin(115200);
    Serial.println(F("DHTxx test!"));

    dht.begin();


    pinMode(PIN_RED_LED, OUTPUT);
    pinMode(PIN_GREEN_LED, OUTPUT);

    pinMode(relay1, OUTPUT);
    pinMode(relay2, OUTPUT);
    pinMode(relay3, OUTPUT);
    pinMode(relay4, OUTPUT);

    // sobald eine Wlan Verbindung aufgebaut ist, wird die ip in der Konsole ausgegeben
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
      delay(1000);
      Serial.println("Connecting to WiFi..");
    }

    Serial.println(WiFi.localIP());


    configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
    

    digitalWrite(relay4, HIGH);
}

// diese Funktion läuft dauerhaft und führt die unten angeführten Funktionen aus. Sie wird alle 30 min ausgefüht
void loop() {


  readDHT();
  humidityControllLED();
  controllThingWithHumidity();

  lightControll();

  delay(1800000);
}

// diese FUnktion steuert anhand der Uhrzeit die Beleuchtung ( zwischen 6 und 18 Uhr sind die lampen an )
void lightControll(){
  struct tm timeinfo;

  if(timeinfo.tm_hour != timeForLight){
    Serial.println("Schlafenszeit");
    digitalWrite(relay3, HIGH);
  }
  else{
    digitalWrite(relay3, LOW);
    Serial.println("Lichter an");
    Serial.println(timeinfo.tm_hour);
  }
}



//diese Funktion steuert die relays 1 und 2 abhängig von der Luftfeuchtigkeit (für genauere Erklärungen siehe Funktionsweiße)
void controllThingWithHumidity(){
  float h = dht.readHumidity();
  if(h > 50){
    digitalWrite(relay1, HIGH);
    digitalWrite(relay2, LOW);
  }
  else if(h < 40) {
    digitalWrite(relay1, LOW);
    digitalWrite(relay2, HIGH);
  }
  else{
    digitalWrite(relay1, HIGH);
    digitalWrite(relay2, HIGH);
  }
}




// diese Funktion steuert die leds anhand der Luftfeuchtigkeit (siehe Funktionsweiße)
void humidityControllLED() {
  float h = dht.readHumidity();
  if (h>50){
    digitalWrite(PIN_RED_LED, HIGH);
    digitalWrite(PIN_GREEN_LED, LOW);
  }
  else {
    digitalWrite(PIN_RED_LED, LOW);
    digitalWrite(PIN_GREEN_LED, HIGH);
  }
}


// diese Funktion gibt die Daten aus die vom DHT22 übermittelt werden. Außerdem werden diese daten auch über eine schnittstellen an einen angeschlossenen pc übermittelt
void readDHT() {
  delay(2000);
  float h = dht.readHumidity();

  float t = dht.readTemperature();

  float f = dht.readTemperature(true);

  if (isnan(h) || isnan(t) || isnan(f)) {
    Serial.println(F("Failed to read from DHT sensor!"));
    return;
  }


  float hif = dht.computeHeatIndex(f, h);

  float hic = dht.computeHeatIndex(t, h, false);

  //luftfeuchtigkeit
  Serial.print(F("Humidity: "));
  Serial.print(h);
  //temperatur
  Serial.print(F("%  Temperature: "));
  Serial.print(t);
  Serial.print(F("°C "));

  //temperatur abhängig von der Luftfeuchtigkeit
  Serial.print(F("   Heat index: "));
  Serial.print(hic);
  Serial.println(F("°C "));
}

// relay1 aktivieren/deactivieren
void activateR1(){
  digitalWrite(relay1, HIGH);
}

void stopR1(){
  digitalWrite(relay1, LOW);
}

// relay2 aktivieren/deactivieren
void activateR2(){
  digitalWrite(relay2, HIGH);
}

void stopR2(){
  digitalWrite(relay2, LOW);
}
