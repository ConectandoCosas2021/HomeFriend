#include "thing_libreria.h"
#include <ArduinoJson.h>
#include "Esp32MQTTClient.h"
#include <ESP32Servo.h>

DynamicJsonDocument incoming_message(1024);

WiFiClient espClient;
PubSubClient client(espClient);
DHTesp dht;
Servo myservo;
TaskHandle_t Task1;
#define DHT_PIN 21
#define MSG_BUFFER_SIZE  50
#define LDR_PIN 22
#define GAS_PIN 18

/*#define pdTICKS_TO_MS( xTicks )   ( ( uint32_t ) ( xTicks ) * 1000 / configTICK_RATE_HZ )
void bring_resources();*/

////////////Variables de micrófono/////////////
#define SAMPLES 512
int dB_raw = 0;
int dB_filtrado = 0;
double promedio = 0;
double vRaw[SAMPLES]; //vector de muestras sin filtrar
double vFiltrado[SAMPLES]; //vector de muestras filtradas

///////////Variable de gato////////////////
boolean gato_anterior = false; // Guardo el estado previo del gato (si no cambia no mando mensaje)
boolean gato_cerca = false; // Estado actual del gato
boolean activarJuguete = false;

///////////Variable de luz////////////////
boolean hubo_Luz = false;

///////////Variable de gas////////////////
boolean hubo_Gas = false;

int gradosActual = 0;
unsigned long lastMsg = 0;
unsigned long sound_time = 0;
unsigned long giro_time = 0;
char msg[MSG_BUFFER_SIZE];

int value = 0;
boolean estado = false;
int cm = 0; //cm del sensor de ultra sonido
int count_on=0;
boolean mic_enable= true;
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
    if (metodo == "micEnable") {  //Check device status. Expects a response to the same topic number with status=true.
      mic_enable = incoming_message["params"];      
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
      while (millis() - empezo < 10) {            
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
  dht.setup(DHT_PIN, DHTesp::DHT22);
  /////
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
  setup_wifi();

    xTaskCreatePinnedToCore(
                    Task1code,   /* Task function. */
                    "Task1",     /* name of task. */
                    10000,       /* Stack size of task */
                    NULL,        /* parameter of the task */
                    1,           /* priority of the task */
                    &Task1,      /* Task handle to keep track of created task */
                    0);          /* pin task to core 0 */                  
  delay(500); 
} 

void loop(){
  
  if (!client.connected()) {//conexion a thingsboard
    reconnect(client);
  }
  client.loop();
  //Mediciones de micrófono

  if ( millis() - sound_time > 20) {     //TODO Poner millis
    sound_time = millis();
    filtroPasaBanda(vFiltrado, 0.45, 0.7);  //alphaLO = 0.45; alphaHI = 0.7  
    filtroPasaBanda(vRaw, 0.02, 0.5);       //alphaLO = 0.02; alphaHI = 0.5
    dB_raw = (int)20*log10(average(vRaw, promedio)) + 20;
    dB_filtrado = (int)20*log10(average(vFiltrado, promedio)+1) + 20;
    
    
    if( dB_filtrado > 60 && !activarJuguete){ //Si gato_anterior está en false y dB_filtrado > 50
      Serial.println(dB_filtrado);   
      giro_time= millis();
      gato_cerca= true;                       //El gato está cerca
      }  
  }   
  if ( millis() - lastMsg > 5000) { //TODO Poner millis
    lastMsg = millis();
    TempAndHumidity lastValues = dht.getTempAndHumidity();//Tomo la temperatura   
    int temp = (int)lastValues.temperature;
    int hum = (int)lastValues.humidity;
    hubo_Gas = digitalRead(GAS_PIN); // Si es HIGH se detectó gas
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
    } 
    snprintf (msg, MSG_BUFFER_SIZE, "{'hubo_Gas': %ld}", hubo_Gas); // Envío gas
    client.publish("v1/devices/me/telemetry", msg);

    snprintf (msg, MSG_BUFFER_SIZE, "{'dB': %ld}", dB_raw);
    client.publish("v1/devices/me/telemetry", msg);
 
    DynamicJsonDocument resp(256);
    resp["gato_cerca"] = gato_cerca;
    char buffer[256];
    serializeJson(resp, buffer);
    client.publish("v1/devices/me/attributes", buffer);
    Serial.print("Publish message gato_cerca : ");
    Serial.println(gato_cerca); 
           
    if(gato_cerca && mic_enable ){ // Si el gato está cerca, mando el mensaje
      activarJuguete = true; //Habilita el giro del servo (core 0)          
    }
    gato_cerca=false;   
      
    snprintf (msg, MSG_BUFFER_SIZE, "{'hubo_Luz': %ld}", hubo_Luz);
    Serial.print("imprimio luz"); Serial.println(hubo_Luz);
    client.publish("v1/devices/me/telemetry", msg);
    hubo_Luz = false;

    snprintf (msg, MSG_BUFFER_SIZE, "{'prendido': %ld}", count_on);
    client.publish("v1/devices/me/telemetry", msg);
    count_on++;  
    }
}//end loop
//Uso esta funcion para detectar si el gato cerca
boolean gatoPresente( int dB_filtrado){

  if( dB_filtrado > 55){ //Si gato_anterior está en false y dB_filtrado > 50
    return true; //El gato está cerca
    }
}

void Task1code( void * pvParameters ){
  for(;;){
    if(activarJuguete){
      if (!myservo.attached()) {
        myservo.setPeriodHertz(50); // standard 50 hz servo
        myservo.attach(33, 1000, 2000); // Attach the servo after it has been detatched
        }
      myservo.write(0);
      if ( millis() - giro_time > 10000) {
         myservo.write(90);
         myservo.detach(); // Turn the servo off for a while
         activarJuguete = false;
      }     
    }    
    
    vTaskDelay(10);
    //Intensidad lumínica
    if(digitalRead(LDR_PIN)==LOW){
      hubo_Luz = true;
      }
  }
}
