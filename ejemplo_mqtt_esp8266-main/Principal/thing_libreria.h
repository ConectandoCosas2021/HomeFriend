//#ifndef THING_LIBRERIA_H
#define THING_LIBRERIA_H


#include <WiFi.h>
#include <PubSubClient.h>
//#include <Servo.h>
#include <DHTesp.h>
//#include <ThingsBoard.h>

void setBlueLed(bool value);
void setup_wifi();
void reconnect(PubSubClient &client);
void setGrados(int grados, int &gradosActual);
void filtroPasaBanda(double vSamples[]);
double average(double vSamples[], double promedio);
