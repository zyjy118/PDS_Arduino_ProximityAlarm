#include <WiFiNINA.h>
#include <Arduino.h>
#include "Firebase_Arduino_WiFiNINA.h"
#include "arduino_secrets.h"
#define FIREBASE_HOST "insert URL here"  //<databaseName>.firebaseio.com or <databaseName>.<region>.firebasedatabase.app
#define FIREBASE_AUTH "insert database secret here"

char WIFI_SSID[] = SECRET_SSID;        
char WIFI_PASSWORD[] = SECRET_PASS;   

//Define firebase and dht objects
FirebaseData firebaseData;
String path = "/Dist_sensor";
int timestamp;

// defines pins numbers
const int trigPin = 7;
const int echoPin = 6;
const int buzzer = 8;
                            
// defines variables
long duration;
int distance;
int safetyDistance;

void printWifi() {
  Serial.println("----------------------------------------");
  Serial.println("Board Information:");
  // print your board's IP address:
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);
  Serial.println();
  Serial.println("Network Information:");
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());
  // print the received signal strength:
  long rssi = WiFi.RSSI();
  Serial.print("signal strength (RSSI):");
  Serial.println(rssi);
  byte encryption = WiFi.encryptionType();
  Serial.print("Encryption Type:");
  Serial.println(encryption, HEX);
  Serial.println();
  Serial.println("----------------------------------------");
}

void setup()
{
  // Serial Setup and wait for serial
  Serial.begin(9600);
  while (!Serial); // Waits for serial 

  //Buzzer code
  pinMode(trigPin, OUTPUT); // Sets the trigPin as an Output
  pinMode(echoPin, INPUT); // Sets the echoPin as an Input
  pinMode(buzzer, OUTPUT);
  Serial.begin(9600); // Starts the serial communication  

  // WiFi setup
  int status = WL_IDLE_STATUS;
  while (status != WL_CONNECTED) {
    Serial.println("Connecting to Wi-Fi");
    status = WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    delay(1000);
  }
  printWifi();
  // Firebase setup
  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH, SECRET_SSID, SECRET_PASS);
  Firebase.reconnectWiFi(true);
}

void loop() {
  Serial.println("check if dist less than 20");
  Serial.println("----------------------------------------");
  
  //Buzzer code
  // Clears the trigPin
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
   
  // Sets the trigPin on HIGH state for 10 micro seconds
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(5);
  digitalWrite(trigPin, LOW);
   
  // Reads the echoPin, returns the sound wave travel time in microseconds
  duration = pulseIn(echoPin, HIGH);
   
  // Calculating the distance
  distance= duration*0.034/2;
  safetyDistance = distance;
  if (safetyDistance <= 20)   // You can change safety distance here eg. 20 , 40 , 60 , 80 , 100, all in cm
  {
    digitalWrite(buzzer, HIGH);    
    timestamp=WiFi.getTime(); //get the time stamp. time in seconds since 1 Jan 1970 
    Serial.println(timestamp);
    if (Firebase.setString(firebaseData, path + "/data" + "/"+String(timestamp) + "/Trip_distance" , String(safetyDistance))) {
    } else {
      Serial.println(firebaseData.errorReason());
    }
    tone(buzzer, 1000); // Send 1KHz sound signal
    delay(200);        // sound duration
  }
  else{
    digitalWrite(buzzer, LOW);
    noTone(buzzer);     // Stop sound
    delay(200);        // sound duration
  }
  // Prints the distance on the Serial Monitor
  Serial.print("Distance: ");
  Serial.println(distance);
  delay(100);
  }
