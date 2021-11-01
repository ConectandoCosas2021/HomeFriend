#ifndef STREAM_LIBRARY_H
#define STREAM_LIBRARY_H

#define CAMERA_MODEL_AI_THINKER
#include <WiFi.h>
#include <WebServer.h>
#include <WiFiClient.h>
#include "src/OV2640.h"
//#define CAMERA_MODEL_AI_THINKER
#include "camera_pins.h"

#define SSID1 "perdedores"
#define PWD1  "somos perdedores 1375"




void set_camara_config(camera_config_t &config);
void set_camara_config2(camera_config_t &config);

#endif
