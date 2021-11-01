#ifndef MY_LIBRARY_H
#define MY_LIBRARY_H
#include <stdint.h>



void capture();
void save();
void start_camera(); //inicia la camara
void hay_movimiento(); // detecta si hay movimieto
void set_motion_config(); //setea la config de la librerias para detectar movmiento

void procces_buffer( uint8_t* buffer);//toma un buffer con la imagen y lo procesa
#endif
