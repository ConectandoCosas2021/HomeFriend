#include "thing_libreria.h"
#include <ArduinoJson.h>
#include "Esp32MQTTClient.h"
#include <ESP32Servo.h>


DynamicJsonDocument incoming_message(1024);

WiFiClient espClient;
PubSubClient client(espClient);
DHTesp dht;
#define DHT_PIN 2
#define MSG_BUFFER_SIZE  50

/*#define pdTICKS_TO_MS( xTicks )   ( ( uint32_t ) ( xTicks ) * 1000 / configTICK_RATE_HZ )
void bring_resources();*/


int gradosActual = 0;
unsigned long lastMsg = 0;
char msg[MSG_BUFFER_SIZE];

int value = 0;
boolean estado = false;
int cm = 0; //cm del sensor de ultra sonido
int count_on=0;
void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }//end for
  Serial.println();

  //convert topic to string to parse
  String _topic = String(topic);  
  if (_topic.startsWith("v1/devices/me/rpc/request/")) {
    //We are in a request, check request number
    String _number = _topic.substring(26);

    //Read JSON Object
    deserializeJson(incoming_message, payload);
    String metodo = incoming_message["method"];
    String parametro = incoming_message["param"];
    if (metodo == "setBlueLed") {  //Check device status. Expects a response to the same topic number with status=true.
        bool valor = incoming_message["params"];
        setBlueLed(valor);
    }
    if (metodo == "setGrados") {  //Check device status. Expects a response to the same topic number with status=true.
      int grados = incoming_message["params"];
      setGrados(grados,gradosActual);
    }    
  }
}//end callback

////////////////////////////////////////////////////////////////////////////////////
///////                            SERVO                                     ///////
////////////////////////////////////////////////////////////////////////////////////  
#define SERVO_1 27

unsigned long empezo = 0;
Servo servoN1;
Servo servo1;

  

void setGrados(int grados, int &gradosActual){  
  servo1.attach(SERVO_1, 500, 2400);
  Serial.println(grados);
  if(gradosActual<grados){
    for(int i=gradosActual; i < grados; i++){     
      servo1.write(i);       
      while (millis() - empezo < 20) {            
      }
      empezo = millis(); 
    }
  }
  else{
    for(int i=gradosActual; i > grados; i--){    
      servo1.write(i);       
      while (millis() - empezo < 20) {            
      }
      empezo = millis(); 
    }
  }
  gradosActual = grados;
  servo1.detach(); //Hacemos esto para que deje de vibrar
}

//////////////////////SETUP////////////////////////////////////////////////
const char* mqtt_server = "demo.thingsboard.io";
void setup() {
  servo1.write(gradosActual);
  servo1.setPeriodHertz(50); // standard 50 hz servo
  pinMode(2, OUTPUT);     //(LED AZUL)
  Serial.begin(115200);
  /////
  dht.setup(DHT_PIN, DHTesp::DHT11);
  /////
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
  setup_wifi();
  
} 

void loop(){
  
  if (!client.connected()) {//conexion a thingsboard
    reconnect(client);
  }
  client.loop();
  
  //cm = readUltrasonicDistance();
  cm = 22; //Esto hay que sacarlo cuando metamos el sensor de distancia  

  
  if ( millis() - lastMsg > 5000) { //TODO Poner millis
    lastMsg = millis();
    TempAndHumidity lastValues = dht.getTempAndHumidity();//Tomo la temperatura   
    int temp = (int)lastValues.temperature;
    int hum = (int)lastValues.humidity;
    Serial.print("temperatura ");Serial.println(temp);
    Serial.print("humedad ");Serial.println(hum);
    
    if (temp==2147483647 || hum==2147483647){    
      Serial.println("No se tomo bien la temperatura");
    }
    else{
        snprintf (msg, MSG_BUFFER_SIZE, "{'temperatura': %ld}", temp);
        client.publish("v1/devices/me/telemetry", msg);
        snprintf (msg, MSG_BUFFER_SIZE, "{'humedad': %ld}", hum);
        client.publish("v1/devices/me/telemetry", msg);
        snprintf (msg, MSG_BUFFER_SIZE, "{'prendido': %ld}", count_on);
        client.publish("v1/devices/me/telemetry", msg);
        count_on++;
      }
    }
}//end loop
