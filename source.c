// ---------------------------------------------------------------- //
// Hashtrip Counter Device
// Written by Rizky Andre Wibisono
// Using Arduino IDE 1.8.12
// Using WeMos D1 Mini (esp8266)
// Using HC-SR04 Module
// Tested on 16 June 2020
// ---------------------------------------------------------------- //

#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266HTTPClient.h>
#include <Wire.h>
#include <string>

/* Set these to your desired credentials. */
const char *ssid = "KALIWUNGU ";
const char *password = "Kaliwungu101";

//Web/Server address to read/write from
//const char *host = "192.168.43.82"; //IP address of server
String url_api = "http://192.168.43.82:8000/api/";
;
String slug = "";
String postData;

// ultrasound pin
#define trigPin D5
#define echoPin D6
// defines ultrasound variables
long duration;
int distance;

// LED indicator
#define ledPin D2

void setup()
{
  pinMode(ledPin, OUTPUT); // sets the ledPin as an output

  pinMode(trigPin, OUTPUT); // Sets the trigPin as an Output
  pinMode(echoPin, INPUT);  // Sets the echoPin as an Input
  delay(1000);

  Serial.begin(9600);

  WiFi.mode(WIFI_OFF); //Prevents reconnection issue (taking too long to connect)
  delay(1000);
  WiFi.mode(WIFI_STA); //This line hides the viewing of ESP as wifi hotspot

  WiFi.begin(ssid, password); //Connect to your WiFi router
  Serial.println("");

  Serial.print("Connecting to ");
  Serial.print(ssid);
  // Wait for connection
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }

  //If connection successful show IP address in serial monitor
  Serial.println("");
  Serial.println("Connected");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
}

void loop()
{
  digitalWrite(ledPin, LOW);

  if (WiFi.status() != WL_CONNECTED)
  {
    WiFi.disconnect();
    WiFi.mode(WIFI_STA);
    Serial.print("Reconnecting to ");
    Serial.println(ssid);
    WiFi.begin(ssid, password);

    while (WiFi.status() != WL_CONNECTED)
    {
      delay(500);
      Serial.print(".");
    }
    Serial.println("");
    Serial.println("Connected");
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
  }

  // look for a person
  // Clears the trigPin
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  // Sets the trigPin on HIGH state for 10 micro seconds
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  // Reads the echoPin, returns the sound wave travel time in microseconds
  duration = pulseIn(echoPin, HIGH);
  // Calculating the distance
  distance = duration * 0.034 / 2;
  Serial.print("Distance: ");
  Serial.println(distance);

  if (distance > 15)
  {
    Serial.println("No one is passing");
    delay(500);
    return; //goto start of loop, no one is passing
  }

  Serial.println("someone is passing");
  digitalWrite(ledPin, HIGH);

  HTTPClient http;
  //POST Data
  postData = "slug=" + slug;
  Serial.println("sending data to server . .");
  http.begin(url_api);
  http.addHeader("Content-Type", "application/x-www-form-urlencoded");
  int httpCode = http.POST(postData); //Send the request
  delay(10);
  String payload = http.getString(); //Get the response payload

  if (payload == "success")
  {
    Serial.println("Counter updated succesfully");
  }
  else
  {
    Serial.println("Sending data failed!");
  }

  delay(500);
  Serial.println(".");
  postData = "";
  http.end(); //Close connection
}