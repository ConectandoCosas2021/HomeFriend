#include "stream_library.h"

void set_camara_config(camera_config_t &configuracion){
  configuracion.ledc_channel = LEDC_CHANNEL_0;
  configuracion.ledc_timer = LEDC_TIMER_0;
  configuracion.pin_d0 = Y2_GPIO_NUM;
  configuracion.pin_d1 = Y3_GPIO_NUM;
  configuracion.pin_d2 = Y4_GPIO_NUM;
  configuracion.pin_d3 = Y5_GPIO_NUM;
  configuracion.pin_d4 = Y6_GPIO_NUM;
  configuracion.pin_d5 = Y7_GPIO_NUM;
  configuracion.pin_d6 = Y8_GPIO_NUM;
  configuracion.pin_d7 = Y9_GPIO_NUM;
  configuracion.pin_xclk = XCLK_GPIO_NUM;
  configuracion.pin_pclk = PCLK_GPIO_NUM;
  configuracion.pin_vsync = VSYNC_GPIO_NUM;
  configuracion.pin_href = HREF_GPIO_NUM;
  configuracion.pin_sscb_sda = SIOD_GPIO_NUM;
  configuracion.pin_sscb_scl = SIOC_GPIO_NUM;
  configuracion.pin_pwdn = PWDN_GPIO_NUM;
  configuracion.pin_reset = RESET_GPIO_NUM;
  configuracion.xclk_freq_hz = 20000000;
  configuracion.pixel_format = PIXFORMAT_JPEG;//esta es la del stream

  // Frame parameters
  //  config.frame_size = FRAMESIZE_UXGA;
  configuracion.frame_size = FRAMESIZE_QVGA;
  configuracion.jpeg_quality = 12;//este es el del stream
  

  configuracion.fb_count =2;//esto estaba en 2 originalmente
  
  }
 
void set_camara_config2(camera_config_t &configuracion){
  configuracion.ledc_channel = LEDC_CHANNEL_0;
  configuracion.ledc_timer = LEDC_TIMER_0;
  configuracion.pin_d0 = Y2_GPIO_NUM;
  configuracion.pin_d1 = Y3_GPIO_NUM;
  configuracion.pin_d2 = Y4_GPIO_NUM;
  configuracion.pin_d3 = Y5_GPIO_NUM;
  configuracion.pin_d4 = Y6_GPIO_NUM;
  configuracion.pin_d5 = Y7_GPIO_NUM;
  configuracion.pin_d6 = Y8_GPIO_NUM;
  configuracion.pin_d7 = Y9_GPIO_NUM;
  configuracion.pin_xclk = XCLK_GPIO_NUM;
  configuracion.pin_pclk = PCLK_GPIO_NUM;
  configuracion.pin_vsync = VSYNC_GPIO_NUM;
  configuracion.pin_href = HREF_GPIO_NUM;
  configuracion.pin_sscb_sda = SIOD_GPIO_NUM;
  configuracion.pin_sscb_scl = SIOC_GPIO_NUM;
  configuracion.pin_pwdn = PWDN_GPIO_NUM;
  configuracion.pin_reset = RESET_GPIO_NUM;
  configuracion.xclk_freq_hz = 20000000;
  
  configuracion.pixel_format =PIXFORMAT_RGB565;//esta es la de motin detection
  // Frame parameters
  //  config.frame_size = FRAMESIZE_UXGA;
  configuracion.frame_size = FRAMESIZE_QVGA;  
  configuracion.jpeg_quality = 30;
  configuracion.fb_count =1;//esto estaba en 2 originalmente
  
  }