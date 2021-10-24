#include "thing_libreria.h"
#include <ArduinoJson.h>
#include "Esp32MQTTClient.h"




DynamicJsonDocument incoming_message(1024);
WiFiClient espClient;
PubSubClient client(espClient);

#define MSG_BUFFER_SIZE  (50)

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

/* The SMTP Session object used for Email sending */
//SMTPSession smtp;

/* Callback function to get the Email sending status */
//void smtpCallback(SMTP_Status status);
const char* mqtt_server = "demo.thingsboard.io";
void setup() {
  pinMode(2, OUTPUT);     //(LED AZUL)
  Serial.begin(115200);
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
  iniciar_stmp();    
 /////////////////////////////////////////////////////////////////   
}

void loop(){
  if (!client.connected()) {//conexion a thingsboard
    reconnect(client);
  }
  client.loop();
  cm = readUltrasonicDistance();
  unsigned long now = millis();
  if (now - lastMsg > 200) {
    lastMsg = now;
    snprintf (msg, MSG_BUFFER_SIZE, "{'valor': %ld}", cm);
    client.publish("v1/devices/me/telemetry", msg);    
  }
}//end loop

/* Callback function to get the Email sending status */
//void smtpCallback(SMTP_Status status){
//  /* Print the current status */
//  Serial.println(status.info());
//
//  /* Print the sending result */
//  if (status.success()){
//    Serial.println("----------------");
//    ESP_MAIL_PRINTF("Message sent success: %d\n", status.completedCount());
//    ESP_MAIL_PRINTF("Message sent failled: %d\n", status.failedCount());
//    Serial.println("----------------\n");
//    struct tm dt;
//
//    for (size_t i = 0; i < smtp.sendingResult.size(); i++){
//      /* Get the result item */
//      SMTP_Result result = smtp.sendingResult.getItem(i);
//      time_t ts = (time_t)result.timestamp;
//      localtime_r(&ts, &dt);
//
//      ESP_MAIL_PRINTF("Message No: %d\n", i + 1);
//      ESP_MAIL_PRINTF("Status: %s\n", result.completed ? "success" : "failed");
//      ESP_MAIL_PRINTF("Date/Time: %d/%d/%d %d:%d:%d\n", dt.tm_year + 1900, dt.tm_mon + 1, dt.tm_mday, dt.tm_hour, dt.tm_min, dt.tm_sec);
//      ESP_MAIL_PRINTF("Recipient: %s\n", result.recipients);
//      ESP_MAIL_PRINTF("Subject: %s\n", result.subject);
//    }
//    Serial.println("----------------\n");
//  }
//}
