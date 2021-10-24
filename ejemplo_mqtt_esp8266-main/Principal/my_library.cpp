
#include "my_library.h"

#define CAMERA_MODEL_AI_THINKER
// include the relevant libraries
#include <FS.h>
#include <SPIFFS.h>
#include <EloquentArduino.h>
#include <eloquentarduino/io/serial_print.h>
#include <eloquentarduino/vision/camera/ESP32Camera.h>
#include <eloquentarduino/vision/io/writers/JpegWriter.h>
#include <eloquentarduino/vision/io/decoders/Red565RandomAccessDecoder.h>
#include <eloquentarduino/vision/processing/downscaling/Downscaler.h>
#include <eloquentarduino/vision/processing/MotionDetector.h>
using namespace Eloquent::Vision;


// define the source format and the downscaled dimensions
#define FRAME_SIZE FRAMESIZE_QVGA
/**##define PIXFORMAT PIXFORMAT_JPEG incompatible con stream
*/
#define PIXFORMAT PIXFORMAT_RGB565

#define W 320
#define H 240
#define w 32
#define h 24

 //podriamos ver de setearlos online
#define DIFF_THRESHOLD 15
#define MOTION_THRESHOLD 0.10

// delete the second definition if you want to turn on code benchmarking
//#define timeit(label, code) { uint32_t start = millis(); code; uint32_t duration = millis() - start; eloquent::io::print_all("It took ", duration, " millis for ", label); }
#define timeit(label, code) code;

using namespace Eloquent::Vision;

camera_fb_t *frame;


IO::Decoders::Red565RandomAccessDecoder decoder;
Processing::Downscaling::Center<W / w, H / h> strategy;
Processing::Downscaling::Downscaler<W, H, w, h> downscaler(&decoder, &strategy);
Processing::MotionDetector<w, h> motion;
IO::Writers::JpegWriter<W, H> jpegWriter;

 void start_camera(){ //la camara que se inicializa aca tiene distitnas configs hay que entender com omierda hacer
Camera::ESP32Camera camera(PIXFORMAT);//esto va afuera
camera.begin(FRAME_SIZE);

 }

void set_motion_config(){
    motion.setDiffThreshold(DIFF_THRESHOLD);     // set how much a pixel value should differ to be considered as a change    
    motion.setMotionThreshold(MOTION_THRESHOLD); // set how many pixels (in percent) should change to be considered as motion    
    motion.setDebounceFrames(5);                 // prevent consecutive triggers  
  }
  
void hay_movimiento(){
  if (motion.triggered()) {
        Serial.println("Motion detected");
        // uncomment to save to disk
        // save();
        delay(1000);
    }
  
  }
// void capture() {
//     uint8_t downscaled[w * h];
   
    //  timeit("capture frame", frame = camera.capture()); // capture image
    
    //  timeit("downscale", downscaler.downscale(frame->buf, downscaled)); // scale image from size H * W to size h * w    
//     timeit("motion detection", motion.detect(downscaled)); // detect motion on the downscaled image
//     eloquent::io::print_all(motion.changes(), " pixels changed"); //detecata si hubo cambio puede ir en otra llamada
// }
void procces_buffer(uint8_t* buffer) {
    uint8_t downscaled[w * h];
   float promedio=0;
   // timeit("capture frame", frame = camera.capture()); // capture image
    
    timeit("downscale", downscaler.downscale(buffer, downscaled)); // scale image from size H * W to size h * w    
    timeit("motion detection", motion.detect(downscaled));             // detect motion on the downscaled image
    eloquent::io::print_all(motion.changes(), " pixels changed");      //detecata si hubo cambio puede ir en otra llamada
    promedio= motion.detect_bright(downscaled);           // detect motion on the downscaled image
    Serial.println(promedio);
}

/**
void save() {
    File imageFile = SPIFFS.open("/capture.jpg", "wb");
    uint8_t quality = 30;

    eloquent::io::print_all("The image will be saved as /capture.jpg");
    jpegWriter.write(imageFile, frame->buf, PIXFORMAT, quality);
    imageFile.close();
    eloquent::io::print_all("Saved");
}
*/
