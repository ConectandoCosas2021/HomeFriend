#include "thing_libreria.h"

const int laserPin = 12;
//const int triggerPin = 15;
//const int echoPin = 14;
const char* ssid = "La Comarca";
const char* password = "Wifipass1";
const char* token = "H27rHId7IM9zxemfkjXb"; //Fijarse nuevo token

//Variables y constantes para micrófono y filtro pasa banda
//#define alphaLO 0.025
//#define alphaHI 0.7
#define SAMPLES 512
int adc_raw = 0;
double adc_lo = 0;
double adc_hi = 0;
double adc_filtrado = 0;
long sum = 0;

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

////////////Micrófono y filtro///////////////////////////
void filtroPasaBanda(double vSamples[], double alphaLO, double alphaHI){
   for ( int i = 0 ; i <SAMPLES; i ++){
   adc_raw = analogRead(35);
   adc_lo = (alphaLO*adc_raw) + ((1-alphaLO)*adc_lo);
   adc_hi = (alphaHI*adc_raw) + ((1-alphaHI)*adc_hi);
   vSamples[i] = adc_hi - adc_lo; //Guardamos señal filtrada
   }
}
double average(double vSamples[],double promedio){
  sum = 0;
  for ( int i = 0 ; i <SAMPLES; i ++)  
  { 
    sum = sum + abs(vSamples[i]);
  } 
  promedio = sum/SAMPLES;
  return promedio;
}

//////////////////////////////////////////////////////////////
