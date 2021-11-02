#include "thing_libreria.h"

const int triggerPin = 15;
const int echoPin = 14;
SMTPSession smtp;
const char* ssid = "perdedores";
const char* password = "somos perdedores 1375";
const char* token = "WCw44NcuFq0tT0eVRQ5l";
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
////////////////////MAIL/////////////////////////////////////////
void setup_session(ESP_Mail_Session &session){
  session.server.host_name = SMTP_HOST;
  session.server.port = SMTP_PORT;
  session.login.email = AUTHOR_EMAIL;
  session.login.password = AUTHOR_PASSWORD;
  session.login.user_domain = "";
}

void setup_message(SMTP_Message &message){
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

}


void iniciar_stmp(){
  
  smtp.debug(1);  
  smtp.callback(smtpCallback);  
  ESP_Mail_Session session;
  setup_session(session); 
  SMTP_Message message;
  setup_message(message);  
  /* Connect to server with the session config */
  if (!smtp.connect(&session))
    return;
  /* Start sending Email and close the session */
  if (!MailClient.sendMail(&smtp, &message))
    Serial.println("Error sending Email, " + smtp.errorReason());
    //Fin setup para Mail
  }

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
/////////////////////////////////////////////////////////////
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
////////////prende apaga led/////////////////////////////////////
void setBlueLed(bool value){
  if (value){
  digitalWrite(2, HIGH);
} else {
  digitalWrite(2, LOW);
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
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}
