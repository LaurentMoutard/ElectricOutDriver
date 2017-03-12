
/*
  Read & write information between Arduino Uno and Web Server
  by L.Moutard
  This example code is in the public domain :-)
  modified 05 mars 2017
*/

#include <Arduino.h>
#include <ArduinoJson.h>
#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <ESP8266HTTPClient.h>
#include <SoftwareSerial.h>

//SoftwareSerial mySerial(13, 15); //RX-D7, TX-D8
// Serial Port used for the comm with Arduino Uno
SoftwareSerial mySerial(14, 12); //RX-D5, TX- D6

ESP8266WiFiMulti WiFiMulti;

String payload;// Received and pushed to the WebServer
String jsonToServer, recupFromServer, recupFromUno;
int      programmeEnCours, delaiEnCours;

unsigned long previousMillis = 0; // last time update
long intervalDInterrogation = 5000; // // Interval of request to the Web Server



void clignote(int nbrClignotements, int duree) { // blinking (nbr of blinks, duration of light)
  for (int i = 0; i < nbrClignotements; i++) {
    digitalWrite(LED_BUILTIN, LOW);   // Turn the LED on
    digitalWrite(D1, HIGH);   // Turn on the another LED on D1
    delay(duree);
    digitalWrite(LED_BUILTIN, HIGH);// Turn the LED off
    digitalWrite(D1, LOW);   // Turn OFF the another LED on D1
    delay(duree);
  }
}

// send Json to server
void sendToServer() {

  // Connect to the WiFi BOX
  if ((WiFiMulti.run() == WL_CONNECTED)) {

    clignote(2, 50);

    Serial.print("Send to netWork : ");

    HTTPClient http;

    // sendJson
    Serial.println("[HTTP] begin...");

    // URL to set the data on Server
    http.begin("http://yourDomain/yourFolder/set.php");

    Serial.print("[HTTP] POST ... payload:");
    Serial.println(jsonToServer);

    // HTTP header
    http.addHeader("Content-type", "application/json");
    int httpCode = http.POST(jsonToServer);   //Send the request
    payload = http.getString(); //Get the response payload

    Serial.println(httpCode);   //Print HTTP return code
    Serial.println(payload);    //Print request response payload

    http.end();  //Close connection
  }
  else {
    Serial.print("DECONNECTE");
    clignote(1, 1000);
  }
}

// get Json data from server
String getFromServer() {
  if ((WiFiMulti.run() == WL_CONNECTED)) {

    HTTPClient http;
    Serial.println("[HTTP] begin...");
    
    // URL to set the data on Server
    http.begin("http://yourDomain/yourFolder/get.php");

    Serial.println("[HTTP] GET...");
    // launch connexion and send HTTP headers
    int httpCode = http.GET();

    // httpCode is <=0 on error
    if (httpCode > 0) {
      if (httpCode == HTTP_CODE_OK) {
        // retrieve string data
        String payload = http.getString();
        //      Serial.println("[HTTP] GET... payload:");
        //      Serial.println(payload);
        return payload;
      }
    }
    else {
      Serial.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
      return ("");
    }
    http.end();
  }
  else {
    clignote(1, 1000);
  }

}

void setup() {
  // Serial Init
  Serial.begin(9600);
  mySerial.begin(9600);

  pinMode(LED_BUILTIN, OUTPUT);     // Initialize the LED_BUILTIN pin as an output
  pinMode(D1, OUTPUT);     // Initialize the D1 pin as an output
  clignote(1, 500);

  // Begin WIFI connexion
  Serial.print("WiFi Connexion : ");
  ESP.wdtEnable(15000);
  // You can try sevral boxes
  //  WiFiMulti.addAP("SSID", "PWD");
  //  WiFiMulti.addAP("SSID2", "PWD2");


  //  ESP.wdtEnable(15000);
  while (WiFiMulti.run() != WL_CONNECTED) { // Wait for connexion OK
    delay(50);
    Serial.print(".");
    clignote(1, 100);
  }
  Serial.println("");  // parameters when connected
  Serial.println("WiFi connecte");
  Serial.print("Adresse IP du module EPC: ");
  Serial.println(WiFi.localIP()); // IP adress
  Serial.print("Adresse IP de la box : ");
  Serial.println(WiFi.gatewayIP());
  clignote(10, 100);

}


void loop() {
  // Request the server and send to Uno
  unsigned long currentMillis = millis(); // currentTime To know if intervalDInterrogation is passed

  if (currentMillis - previousMillis > intervalDInterrogation) {
    previousMillis = currentMillis;

    recupFromServer = getFromServer(); // Receive information from Web Server
    Serial.print("json récupéré : ");
    Serial.println(recupFromServer);

    DynamicJsonBuffer jsonBuffer;
    JsonObject& root = jsonBuffer.parseObject(recupFromServer);
    if (root.success()) // if it's a well formed Json
    {
      // send to softwareSerial
      clignote(10, 50);
      root.printTo(mySerial);
      mySerial.write('\n');
    }
    //    sendToServer(); // send values received from Arduino Uno to Web Server
    // must be done each intervalDInterrogation
    sendToServer(); // send values received from Arduino Uno to Web Server

  }
  // Request Uno and send to server
  // Read from softwareSerial
  while (mySerial.available() > 0) {
    recupFromUno = mySerial.readStringUntil('\n');
  }
  Serial.print("Received from Uno : ");
  Serial.println(recupFromUno);

  DynamicJsonBuffer jsonBuffer2;
  JsonObject& root2 = jsonBuffer2.parseObject(recupFromUno);
  if (root2.success())// if it's a well formed Json
  {
    jsonToServer = recupFromUno; // jsonToServer is always a well formede Uno
  }
  else {
    Serial.println("parseObject() failed");
  }
}

