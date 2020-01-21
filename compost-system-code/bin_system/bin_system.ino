#define BLYNK_PRINT Serial

#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>

char auth[] = "WLRKPVpWpJe9hJWNb0AmQSrdNQjM8gTB";

char ssid[] = "Gbridge";
char pass[] = "gokulraja";

// defines pins numbers
const int trigPin = D1;  
const int echoPin = D2;  
const int ledPin  = D0;
// defines variables
long duration;
int distance;

const int bin_height = 60;
uint32_t timestamp;

void setup() {
  Serial.begin(115200);
  Blynk.begin(auth, ssid, pass);
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  pinMode(ledPin, OUTPUT);
}

void loop() {
  
  Blynk.run();
  
  if ((millis() - timestamp) > 5000)
  {
    timestamp = millis();
     
    digitalWrite(trigPin, LOW);
    delayMicroseconds(2);
    digitalWrite(trigPin, HIGH);
    delayMicroseconds(10);
    digitalWrite(trigPin, LOW);
    
    duration = pulseIn(echoPin, HIGH);
    distance= duration*0.034/2;
    
    Serial.print("Distance - ");
    Serial.println(distance);
  
    int bin_storage = map(distance, 5, bin_height, 100,  0);
    bin_storage = constrain(bin_storage, 0 , 100);
    Serial.print("storage  - ");
    Serial.println(bin_storage);

    digitalWrite(ledPin, !(bin_storage >= 90));
    
    Blynk.virtualWrite(V0, bin_storage);
  }
}
