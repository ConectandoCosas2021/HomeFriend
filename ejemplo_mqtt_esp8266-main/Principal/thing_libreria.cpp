#include "thing_libreria.h"

const int laserPin = 12;
//const int triggerPin = 15;
//const int echoPin = 14;
const char* ssid = "perdedores";
const char* password = "somos perdedores 1375";
const char* token = "WCw44NcuFq0tT0eVRQ5l"; //Fijarse nuevo token
////////////////////// setup wifi///////////////////////////////////////
void setup_wifi() {

  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  randomSeed(micros());

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

/*
long readUltrasonicDistance(){
  pinMode(triggerPin, OUTPUT);  // Clear the trigger
  digitalWrite(triggerPin, LOW);
  delayMicroseconds(2);
  // Sets the trigger pin to HIGH state for 10 microseconds
  digitalWrite(triggerPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(triggerPin, LOW);
  pinMode(echoPin, INPUT);
  // Reads the echo pin, and returns the sound wave travel time in microseconds
  return 0.01723*pulseIn(echoPin, HIGH);
}
*/
////////////prende apaga led/////////////////////////////////////
void setBlueLed(bool value){
  pinMode(laserPin, OUTPUT);
  Serial.println(value);
  if (value){
  digitalWrite(laserPin, HIGH);
  Serial.println("Estoy en HIGH");
} else {
  Serial.println("Estoy en LOW");
  digitalWrite(laserPin, LOW);
}
}
////////////END prende apaga led/////////////////////////////////////

////////////Reconectarse a thingsboard/////////////////////////////////////
void reconnect(PubSubClient &client) {//reconectarse a thingsboard
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect - client.connect(DEVICE_ID, TOKEN, TOKEN)
    if (client.connect("NODEMCU Nuevo", token, token )) {
      Serial.println("connected");
      // Once connected, publish an announcement...
      //client.publish("outTopic", "hello world");
      // ... and resubscribe to rpc topic
      client.subscribe("v1/devices/me/rpc/request/+");
    } else {
      Serial.println("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}
