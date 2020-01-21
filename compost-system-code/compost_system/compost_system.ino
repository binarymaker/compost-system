#include <OneWire.h>
#include <DallasTemperature.h>
#include <Wire.h>
#include <Servo.h>
#include "DHTesp.h"
#include <EEPROM.h>

#define EEPROM_SIZE 1

#define BLYNK_PRINT Serial

#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>
#include <Time.h> // https://github.com/PaulStoffregen/Time
#include <TimeLib.h>
#include <WidgetRTC.h>

char auth[] = "WEBa7nYzxqZAisMBplKRRxfDNcUEZLCA";

char ssid[] = "Gbridge";
char pass[] = "gokulraja";

#define DHTpin     15
#define DS18B20pin  4
#define floatPin   32
#define pumpPin    33
#define servoPin    5
#define soliMoiPin 34
#define methenePin 35

DHTesp dht;
//Temperature
OneWire oneWire(DS18B20pin);
DallasTemperature sensors(&oneWire);

Servo servoMotor;
WidgetLCD lcd(V10);
BlynkTimer timer;
WidgetRTC rtc;

uint32_t delaytime;
tmElements_t compost_start;


BLYNK_CONNECTED() {
  // Synchronize time on connection
  rtc.begin();
}

void compost_time()
{
  tmElements_t T2;
  
  T2.Hour = hour();
  T2.Minute = minute();
  T2.Second = second();
  T2.Day = day();
  T2.Month = month();
  T2.Year = year() - 1970; // because Year is offset from 1970

  String currentTime = String(hour()) + ":" + minute() + ":" + second();
  String currentDate = String(day()) + " " + month() + " " + year();
  Serial.print("Current time: ");
  Serial.print(currentTime);
  Serial.print(" ");
  Serial.print(currentDate);
  Serial.println();
    
  // convert T1 and T2 to seconds since 1/1/1970
  time_t T1sec = makeTime( compost_start );
  time_t T2sec = makeTime( T2 );

  // differences in seconds
  int32_t diff = T2sec - T1sec;

  // show difference in hours, minutes and seconds
  
  uint32_t hours = (diff / 3600);
  uint8_t minutes = (diff / 60) % 60;
  uint8_t seconds = diff % 60;
  
  float days = hours / 24.0 ;
  
  Serial.print( days );
  Serial.print( " days(s), " );
  Serial.print( hours );
  Serial.print( " hour(s), " );
  Serial.print( minutes );
  Serial.print( " minute(s) and " );
  Serial.print( seconds );
  Serial.println( " second(s) before T2." );

  Blynk.virtualWrite(V5, days);
}


void setup() {
  Serial.begin(115200);

  EEPROM.begin(EEPROM_SIZE);
  Blynk.begin(auth, ssid, pass);
  lcd.clear();

  pinMode(floatPin, INPUT_PULLUP); //External pull good
  pinMode(pumpPin, OUTPUT);
  sensors.begin();
  dht.setup(DHTpin, DHTesp::DHT11);
  servoMotor.attach(servoPin);

  setSyncInterval(5 * 60); // Sync interval in seconds (5 minutes)
  
   compost_start.Hour   = EEPROM.read(0);
   compost_start.Minute = EEPROM.read(1);
   compost_start.Second = EEPROM.read(2);
   compost_start.Day    = EEPROM.read(3);
   compost_start.Month  = EEPROM.read(4);
   compost_start.Year   = EEPROM.read(5);
}

void loop() {

  Blynk.run();

  if (millis() - delaytime > 5000)
  {
    delaytime = millis();
    
    compost_time();
    
    sensors.requestTemperatures();
    float compost_tempC = sensors.getTempCByIndex(0);
    Serial.print("compost temperature - ");
    Serial.print(compost_tempC);
    Serial.println("ºC");
    Blynk.virtualWrite(V3, compost_tempC);

    float compost_moisture = analogRead(soliMoiPin);
    compost_moisture = map(compost_moisture, 0, 4095, 0, 100);
    Serial.print("compost moisture    - ");
    Serial.print(compost_moisture);
    Serial.println("%");
    Blynk.virtualWrite(V2, compost_moisture);

    float compost_methane = analogRead(methenePin);
    compost_methane = map(compost_methane, 0,  4095, 0, 1000);
    Serial.print("compost methane     - ");
    Serial.print(compost_methane);
    Serial.println("ppm");
    Blynk.virtualWrite(V4, compost_methane);

    float temperature = dht.getTemperature();
    float humidity = dht.getHumidity();
    Serial.print("Ambient temperature - ");
    Serial.print(temperature);
    Serial.println("ºC");
    Serial.print("Ambient humidity    - ");
    Serial.print(humidity);
    Serial.println();
    Blynk.virtualWrite(V0, temperature);
    Blynk.virtualWrite(V1, humidity);

    int is_water_tank_empty = digitalRead(floatPin);
    Serial.print("Water tank empty    - ");
    Serial.print(is_water_tank_empty);
    Serial.println();

    if (is_water_tank_empty == 0)
    {
      if (compost_moisture > 70)
      {
        digitalWrite(pumpPin, HIGH);
        lcd.print(0, 1, "WATER PUMP ON  ");
        Serial.println("-> WATER PUMP ON  ");
      }
      else
      {
        digitalWrite(pumpPin, LOW);
        lcd.print(0, 1, "WATER PUMP OFF  ");
        Serial.println("-> WATER PUMP OFF  ");
      }
    }
    else
    {
      lcd.print(0, 1, "WATER TANK EMPTY");
      Serial.println("-> WATER TANK EMPTY");
    }

    if (compost_tempC > 50)
    {
      servoMotor.write(180);
      lcd.print(0, 0, "VENDILATION ON  ");
      Serial.println("-> VENDILATION ON ");
    }
    else
    {
      servoMotor.write(0);
      lcd.print(0, 0, "VENDILATION OFF ");
      Serial.println("-> VENDILATION OFF ");
    }


    Serial.println("----------------------------------");
    delay(dht.getMinimumSamplingPeriod());
  }
}

BLYNK_WRITE(V6)
{
  int i = param.asInt();
  
  if (i == 1)
  {
    EEPROM.write(0, hour());
    EEPROM.write(1, minute());
    EEPROM.write(2, second());
    EEPROM.write(3, day());
    EEPROM.write(4, month());
    EEPROM.write(5, year() - 1970);
    EEPROM.commit();
  }
  
}
