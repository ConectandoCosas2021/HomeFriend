/*
 Basic ESP8266 MQTT example
 This sketch demonstrates the capabilities of the pubsub library in combination
 with the ESP8266 board/library.
 It connects to an MQTT server then:
  - publishes "hello world" to the topic "outTopic" every two seconds
  - subscribes to the topic "inTopic", printing out any messages
    it receives. NB - it assumes the received payloads are strings not binary
  - If the first character of the topic "inTopic" is an 1, switch ON the ESP Led,
    else switch it off
 It will reconnect to the server if the connection is lost using a blocking
 reconnect function. See the 'mqtt_reconnect_nonblocking' example for how to
 achieve the same result without blocking the main loop.
 To install the ESP8266 board, (using Arduino 1.6.4+):
  - Add the following 3rd party board manager under "File -> Preferences -> Additional Boards Manager URLs":
       http://arduino.esp8266.com/stable/package_esp8266com_index.json
  - Open the "Tools -> Board -> Board Manager" and click install for the ESP8266"
  - Select your ESP8266 in "Tools -> Board"
*/

#include <ArduinoJson.h>
#include <WiFi.h>
#include "Esp32MQTTClient.h"
#include <PubSubClient.h>

#include <ESP_Mail_Client.h>
#define SMTP_HOST "smtp.gmail.com"
#define SMTP_PORT 465
/* The sign in credentials */
#define AUTHOR_EMAIL "homefriend2021@gmail.com"
#define AUTHOR_PASSWORD "HomE2021#$"
/* Recipient's email*/
#define RECIPIENT_EMAIL "luispsn1497@gmail.com"
/* The SMTP Session object used for Email sending */
SMTPSession smtp;
/* Callback function to get the Email sending status */
void smtpCallback(SMTP_Status status);

//#include <Servo.h>
//int grados = 0; // grados de giro
//Servo servo_4; // nombre del servo
// Update these with values suitable for your network.

DynamicJsonDocument incoming_message(1024);

const char* ssid = "La Comarca";
const char* password = "Wifipass1";
const char* mqtt_server = "demo.thingsboard.io";
const char* token = "WCw44NcuFq0tT0eVRQ5l";

WiFiClient espClient;
PubSubClient client(espClient);
unsigned long lastMsg = 0;
#define MSG_BUFFER_SIZE	(50)
char msg[MSG_BUFFER_SIZE];
char msg2[MSG_BUFFER_SIZE];

int value = 0;
boolean estado = false;
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

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
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
    
    /*if (metodo=="checkStatus") {  //Check device status. Expects a response to the same topic number with status=true.
      
      char outTopic[128];
      ("v1/devices/me/rpc/response/"+_number).toCharArray(outTopic,128);
      
      DynamicJsonDocument resp(256);
      resp["status"] = true;
      char buffer[256];
      serializeJson(resp, buffer);
      client.publish(outTopic, buffer);
    }*/
    if (metodo == "setBlueLed") {  //Check device status. Expects a response to the same topic number with status=true.
      bool valor = incoming_message["params"];
      setBlueLed(valor);
  }
  if(metodo == "setLeft"){
  //  girarIzquierda();
  }
  if(metodo == "setRight"){
   // girarDerecha();
  }
   /*deserializeJson(incoming_message, payload);
  
  String dispositivo = incoming_message["params"][0];
  bool valor = incoming_message["params"][1];
  Serial.print("El dispositivo es: "); Serial.println(dispositivo);
  Serial.print("El valor es: "); Serial.println(valor);
  
  if (dispositivo == "1" and valor){
    digitalWrite(2, HIGH);
  } else {
    digitalWrite(2, LOW);
  }*/
}
}
void reconnect() {
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
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

int trigger = 15;
int echo = 14;
int cm = 0;
long readUltrasonicDistance(int triggerPin, int echoPin){
  pinMode(triggerPin, OUTPUT);  // Clear the trigger
  digitalWrite(triggerPin, LOW);
  delayMicroseconds(2);
  // Sets the trigger pin to HIGH state for 10 microseconds
  digitalWrite(triggerPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(triggerPin, LOW);
  pinMode(echoPin, INPUT);
  // Reads the echo pin, and returns the sound wave travel time in microseconds
  return pulseIn(echoPin, HIGH);
}
 void setBlueLed(bool value){
    if (value){
    digitalWrite(2, HIGH);
  } else {
    digitalWrite(2, LOW);
  }
}
  void girarIzquierda(){
  
}


void setup() {
  pinMode(2, OUTPUT);     //(LED AZUL)
  Serial.begin(115200);
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);

  //pinMode(3, INPUT);
  //servo_4.attach(4, 500, 2500);

  //pinMode(2, INPUT);

  //Setup para Mail
   /** Enable the debug via Serial port
   * none debug or 0
   * basic debug or 1
  */
  smtp.debug(1);

  /* Set the callback function to get the sending results */
  smtp.callback(smtpCallback);

  /* Declare the session config data */
  ESP_Mail_Session session;

  /* Set the session config */
  session.server.host_name = SMTP_HOST;
  session.server.port = SMTP_PORT;
  session.login.email = AUTHOR_EMAIL;
  session.login.password = AUTHOR_PASSWORD;
  session.login.user_domain = "";

  /* Declare the message class */
  SMTP_Message message;

  /* Set the message headers */
  message.sender.name = "HOMEFRIEND";
  message.sender.email = AUTHOR_EMAIL;
  message.subject = "HomeFriend sistema de alertas";
  message.addRecipient("Luis", RECIPIENT_EMAIL);

  /*Send HTML message*/
  String htmlMsg = "<div style=\"color:#2f4468;\"><h1>HomeFriend</h1><p>- Este es un mensaje enviado desde la ESP32 de prueba</p></div>";
  message.html.content = htmlMsg.c_str();
  message.html.content = htmlMsg.c_str();
  message.text.charSet = "us-ascii";
  message.html.transfer_encoding = Content_Transfer_Encoding::enc_7bit;
  
  /* Set the custom message header */
  //message.addHeader("Message-ID: <abcde.fghij@gmail.com>");

  /* Connect to server with the session config */
  if (!smtp.connect(&session))
    return;

  /* Start sending Email and close the session */
  if (!MailClient.sendMail(&smtp, &message))
    Serial.println("Error sending Email, " + smtp.errorReason());
    //Fin setup para Mail
}

void loop(){

  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  cm = 0.01723*readUltrasonicDistance(trigger, echo);
  // Si los centimetros son menores a 30 notificar
  if(cm < 30){
   // mandarMail();
  }
  
  unsigned long now = millis();
  if (now - lastMsg > 200) {
    lastMsg = now;
    snprintf (msg, MSG_BUFFER_SIZE, "{'valor': %ld}", cm);
   // Serial.print("Publish message: ");
   // Serial.println(msg);
    client.publish("v1/devices/me/telemetry", msg);
    
  }
}

/* Callback function to get the Email sending status */
void smtpCallback(SMTP_Status status){
  /* Print the current status */
  Serial.println(status.info());

  /* Print the sending result */
  if (status.success()){
    Serial.println("----------------");
    ESP_MAIL_PRINTF("Message sent success: %d\n", status.completedCount());
    ESP_MAIL_PRINTF("Message sent failled: %d\n", status.failedCount());
    Serial.println("----------------\n");
    struct tm dt;

    for (size_t i = 0; i < smtp.sendingResult.size(); i++){
      /* Get the result item */
      SMTP_Result result = smtp.sendingResult.getItem(i);
      time_t ts = (time_t)result.timestamp;
      localtime_r(&ts, &dt);

      ESP_MAIL_PRINTF("Message No: %d\n", i + 1);
      ESP_MAIL_PRINTF("Status: %s\n", result.completed ? "success" : "failed");
      ESP_MAIL_PRINTF("Date/Time: %d/%d/%d %d:%d:%d\n", dt.tm_year + 1900, dt.tm_mon + 1, dt.tm_mday, dt.tm_hour, dt.tm_min, dt.tm_sec);
      ESP_MAIL_PRINTF("Recipient: %s\n", result.recipients);
      ESP_MAIL_PRINTF("Subject: %s\n", result.subject);
    }
    Serial.println("----------------\n");
  }
}
