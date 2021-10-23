#ifndef THING_LIBRERIA_H
#define THING_LIBRERIA_H

#include <ESP_Mail_Client.h>
#include <WiFi.h>
#include <PubSubClient.h>
//#include <Servo.h>

#define SMTP_HOST "smtp.gmail.com"
#define SMTP_PORT 465

/* The sign in credentials */
#define AUTHOR_EMAIL "homefriend2021@gmail.com"
#define AUTHOR_PASSWORD "HomE2021#$"
#define RECIPIENT_EMAIL "luispsn1497@gmail.com"


void setup_wifi(); //incializa el wifi
long readUltrasonicDistance();//leer del ultrasonido
void setBlueLed(bool value); //prende un led
void  setup_message(SMTP_Message &message);
void setup_session(ESP_Mail_Session &session);
void reconnect(PubSubClient &client); 
#endif
