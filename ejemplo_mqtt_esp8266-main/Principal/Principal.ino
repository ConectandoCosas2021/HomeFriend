#include "thing_libreria.h"
#include <ArduinoJson.h>
#include "Esp32MQTTClient.h"
#include <FS.h>
#include <SPIFFS.h>
#include "stream_library.h"
#include "my_library.h"

TaskHandle_t Task2;//LOOP INFINITO EN CORE 0
DynamicJsonDocument incoming_message(1024);
WiFiClient espClient;
PubSubClient client(espClient);
WebServer server(80);
OV2640 cam;
DHTesp dht;
#define MSG_BUFFER_SIZE  (50)
#define CAMERA_MODEL_AI_THINKER
#define timeit(label, code) code;
#define DHT_PIN 2

void bring_resources();



unsigned long lastMsg = 0;
char msg[MSG_BUFFER_SIZE];
char msg2[MSG_BUFFER_SIZE];

int value = 0;
boolean estado = false;
int cm = 0; //cm del sensor de ultra sonido

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
     }
     }//end callback

const char HEADER[] = "HTTP/1.1 200 OK\r\n" \
                      "Access-Control-Allow-Origin: *\r\n" \
                      "Content-Type: multipart/x-mixed-replace; boundary=123456789000000000000987654321\r\n";
const char BOUNDARY[] = "\r\n--123456789000000000000987654321\r\n";
const char CTNTTYPE[] = "Content-Type: image/jpeg\r\nContent-Length: ";
const int hdrLen = strlen(HEADER);
const int bdrLen = strlen(BOUNDARY);
const int cntLen = strlen(CTNTTYPE);

 
void handle_jpg_stream(void) //loop infinito esto va a ir a un core
{
  char buf[32]; //creo que aca se guarda la imagen
  int s;        //tamano de la imagen

  WiFiClient client = server.client();

  client.write(HEADER, hdrLen);
  client.write(BOUNDARY, bdrLen);
  bool entro=false;
  bool primera=true;
  while (client.connected())
  { entro=true;
    if (primera){
      esp_camera_deinit();//des inicializa la camara
      camera_config_t config;
      set_camara_config(config);
      cam.init(config);
      primera=false;
      }
    cam.run(); //saca una nueva foto, el fb queda guardado en la variable de clase
    s = cam.getSize();
    client.write(CTNTTYPE, cntLen);
    sprintf( buf, "%d\r\n\r\n", s );
    client.write(buf, strlen(buf));
    client.write((char *)cam.getfb(), s);
    client.write(BOUNDARY, bdrLen);
    Serial.print("stream en core :");
    Serial.println(xPortGetCoreID());//core en el que se ejecuta
  }
  if (entro){
    esp_camera_deinit();//des inicializa la camara
    camera_config_t config; //crea una config nueva
    set_camara_config2(config); //configura para el motion sensor
    Serial.println("incia la camara");
    cam.init(config);//inica la camara
    }
}

const char JHEADER[] = "HTTP/1.1 200 OK\r\n" \
                       "Content-disposition: inline; filename=capture.jpg\r\n" \
                       "Content-type: image/jpeg\r\n\r\n";
const int jhdLen = strlen(JHEADER);
//////////////////////////////////
void handle_jpg(void)
{
  WiFiClient client = server.client();
  cam.run();
  if (!client.connected()) return;
  client.write(JHEADER, jhdLen);
  client.write((char *)cam.getfb(), cam.getSize());
}
/////////////////////////////////
void handleNotFound()
{
  String message = "Server is running!\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET) ? "GET" : "POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  server.send(200, "text / plain", message);
}
//////////////////////SETUP////////////////////////////////////////////////
const char* mqtt_server = "demo.thingsboard.io";
void setup() {
  pinMode(2, OUTPUT);     //(LED AZUL)
  Serial.begin(115200);
  /////
  dht.setup(DHT_PIN, DHTesp::DHT11);
  /////
  SPIFFS.begin(true);
  delay(1000);
//  camera_config_t config;
//  set_camara_config(config);
//  cam.init(config);
  set_motion_config();
  //////wifi 2////
  IPAddress ip;

  WiFi.mode(WIFI_STA);
  WiFi.begin(SSID1, PWD1);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(F("."));
  }
  ip = WiFi.localIP();//local IP del server
  Serial.println(F("WiFi connected"));
  Serial.println("");
  Serial.println(ip);
  Serial.print("Stream Link: http://");
  Serial.print(ip);
  Serial.println("/mjpeg/1");

  server.on("/mjpeg/1", HTTP_GET, handle_jpg_stream);
  server.on("/jpg", HTTP_GET, handle_jpg);
  server.onNotFound(handleNotFound);
  server.begin();
  ///////////////////////////
  //setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
  iniciar_stmp();    
 /////////////////////////////////////////////////////////////////   

  camera_config_t config;
  set_camara_config2(config);
  Serial.println("incia la camara");
  cam.init(config);

///
    xTaskCreatePinnedToCore(
    Task2code,   /* Task function. */
    "Task2",     /* name of task. */
    10000,       /* Stack size of task */
    NULL,        /* parameter of the task */
    1,           /* priority of the task */
    &Task2,      /* Task handle to keep track of created task */
    0);          /* pin task to core 1 */
  delay(500);
}

void Task2code( void * pvParameters ) {// core 0
  TickType_t tiempodormir=xTaskGetTickCount(); 
  for (;;) { // infinite loop
    Serial.print("task2 :");
  Serial.println(xPortGetCoreID());//core en el que se ejecuta
  server.handleClient();
  cam.run();//guarda la foto en el buffer     
  procces_buffer(cam.getfb()); //camputra frames
  hay_movimiento(); //detecta si hay o no movimiento  
    
  }
  }

void loop(){
  
  if (!client.connected()) {//conexion a thingsboard
    reconnect(client);
  }
  client.loop();
//  server.handleClient();
  
//  cam.run();//guarda la foto en el buffer     
//  procces_buffer(cam.getfb()); //camputra frames
//  hay_movimiento(); //detecta si hay o no movimiento  
  
//  delay(30);//fps estaba en 30
  
  //cm = readUltrasonicDistance();
  cm = 22; //Esto hay que sacarlo cuando metamos el sensor de distancia
  TempAndHumidity lastValues = dht.getTempAndHumidity();//setup de last values
  //Serial.print((int) lastValues.temperature); Serial.println(" C"); 
  //Serial.print((int)lastValues.humidity); Serial.println(" %");
  unsigned long now = millis();
  if (now - lastMsg > 2000) {
    lastMsg = now;
    snprintf (msg, MSG_BUFFER_SIZE, "{'valor': %ld}", cm);
    client.publish("v1/devices/me/telemetry", msg);
    // #TODO aca tenemos que hacer un if not nan envio (lastValues.temperature y lastValues.humidity)   
      snprintf (msg, MSG_BUFFER_SIZE, "{'temperatura': %ld}", (int)lastValues.temperature);
      client.publish("v1/devices/me/telemetry", msg);
      snprintf (msg, MSG_BUFFER_SIZE, "{'humedad': %ld}", (int)lastValues.humidity);
      client.publish("v1/devices/me/telemetry", msg);
    }
}//end loop
