#define CAYENNE_PRINT Serial     // Comment this out to disable prints and save space
#include <CayenneMQTTESP8266.h> // Change this to use a different communication device. See Communications examples.

// WiFi network info.
char ssid[] = "Gbridge";
char wifiPassword[] = "gokulraja";

// Cayenne authentication info. This should be obtained from the Cayenne Dashboard.
char username[] = "4f753c40-35d9-11ea-b73d-1be39589c6b2";
char password[] = "a8250f860dab66015680534bd3de42b0bd038fd3";
char clientID[] = "0553a0c0-35de-11ea-8221-599f77add412";

// defines pins numbers
const int trigPin = D1;  
const int echoPin = D2;  

// defines variables
long duration;
int distance;

const int bin_height = 60;
uint32_t timestamp;

void setup() {
  Serial.begin(115200);
  Cayenne.begin(username, password, clientID, ssid, wifiPassword);
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
}

void loop() {
  
  Cayenne.loop();
  
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
  
    Cayenne.virtualWrite(1, timestamp/1000);
  }
}
