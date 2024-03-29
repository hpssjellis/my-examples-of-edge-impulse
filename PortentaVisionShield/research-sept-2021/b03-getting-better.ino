/*
 * 
 * Must use portenta camerera explained in this PR
 * https://github.com/arduino/ArduinoCore-mbed/pull/122
 * Should be implemented with MBED version greater than 1.3.1
 * Until then needs the 2 main library folders Portenta_Camera and Himax_HM01B0
 * 
 * 
 */


 
#include <Arduino.h>  // only needed for https://platformio.org/

#include <U8g2lib.h>
 
#ifdef U8X8_HAVE_HW_SPI
#include <SPI.h>
#endif
#ifdef U8X8_HAVE_HW_I2C
#include <Wire.h>
#endif



// works
U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE);   //(rotation, clock, data [, reset])




#define EI_DSP_IMAGE_BUFFER_STATIC_SIZE 128


/* Includes ---------------------------------------------------------------- */


//#include <ov7670-detect-microcontroller_inference.h>
#include <ov7670-08-detect-micro-restored_inference.h>


#include "camera.h"

CameraClass myCam;




// raw frame buffer from the camera
#define FRAME_BUFFER_COLS          320   // 160
#define FRAME_BUFFER_ROWS          240   // 120
//uint16_t frame_buffer[FRAME_BUFFER_COLS * FRAME_BUFFER_ROWS] = { 0 };

uint8_t frame_buffer[320*240] __attribute__((aligned(32)));

// cutout that we want (this does not do a resize, which would also be an option, but you'll need some resize lib for that)
#define CUTOUT_COLS                 EI_CLASSIFIER_INPUT_WIDTH
#define CUTOUT_ROWS                 EI_CLASSIFIER_INPUT_HEIGHT
const int cutout_row_start = (FRAME_BUFFER_ROWS - CUTOUT_ROWS) / 2;
const int cutout_col_start = (FRAME_BUFFER_COLS - CUTOUT_COLS) / 2;

/**
 * This function is called by the classifier to get data
 * We don't want to have a separate copy of the cutout here, so we'll read from the frame buffer dynamically
 */
int cutout_get_data(size_t offset, size_t length, float *out_ptr) {
    // so offset and length naturally operate on the *cutout*, so we need to cut it out from the real framebuffer
    size_t bytes_left = length;
    size_t out_ptr_ix = 0;

    // read byte for byte
    while (bytes_left != 0) {
        // find location of the byte in the cutout
        size_t cutout_row = floor(offset / CUTOUT_COLS);
        size_t cutout_col = offset - (cutout_row * CUTOUT_COLS);

        // then read the value from the real frame buffer
        size_t frame_buffer_row = cutout_row + cutout_row_start;
        size_t frame_buffer_col = cutout_col + cutout_col_start;

        // grab the value and convert to r/g/b
        uint8_t pixel = frame_buffer[(frame_buffer_row * FRAME_BUFFER_COLS) + frame_buffer_col];


        //uint8_t pixel = (pixelTemp>>8) | (pixelTemp<<8);
        //uint8_t pixel = 255-pixelTemp;
        
        uint8_t r = pixel;
        uint8_t g = pixel;
        uint8_t b = pixel;

        // then convert to out_ptr format
        float pixel_f = (r << 16) + (g << 8) + b;
        out_ptr[out_ptr_ix] = pixel_f;

        // and go to the next pixel
        out_ptr_ix++;
        offset++;
        bytes_left--;
    }

    // and done!
    return 0;
}






/**
 * @brief      Arduino setup function
 */
void setup()
{
  
    u8g2.begin();  // For OLED
    // put your setup code here, to run once:
    Serial.begin(115200);
     // Serial.begin(921600);
     pinMode(LEDB, OUTPUT);   
     digitalWrite(LEDB, HIGH); 
     
     pinMode(LEDG, OUTPUT);   
     digitalWrite(LEDG, HIGH); 
     
     pinMode(LEDR, OUTPUT);   
     digitalWrite(LEDR, HIGH); 

  // Init the cam
  myCam.begin(CAMERA_R320x240, 30);

  // Skip 60 frames
 // myCam.skip_frames(frame_buffer, 30);

    Serial.println("Edge Impulse Inferencing Demo");
}

/**
 * @brief      Arduino main function
 */
void loop()
{


    u8g2.clearBuffer();                   // clear the internal memory
    u8g2.setFont(u8g2_font_ncenB08_tr); 
    //u8g2.drawPixel(random(128), random(64));


    //make a box for the cutout
    // using these:        (int)cutout_col_start, (int)CUTOUT_COLS, (int)cutout_row_start, (int)CUTOUT_ROWS,  
  
    u8g2.drawLine((int)cutout_col_start,0,                        (int)CUTOUT_COLS, 0);                      // horizontal top
    u8g2.drawLine((int)cutout_col_start,(int)cutout_row_start,    (int)CUTOUT_COLS, (int)cutout_row_start);  // horizontal bottom
    u8g2.drawLine((int)CUTOUT_COLS,0,                             (int)CUTOUT_COLS,(int)CUTOUT_ROWS);        // vertical right
    u8g2.drawLine(0,0,                                            0,(int)CUTOUT_ROWS);                       // vertical left
    
   //u8g2.drawLine((int)cutout_row_start,0,  (int)CUTOUT_ROWS, 0);                                          // horizontal top
   // u8g2.drawLine((int)cutout_row_start,(int)cutout_col_start,  (int)CUTOUT_ROWS, (int)cutout_col_start);  // horizontal bottom
   // u8g2.drawLine((int)cutout_row_start,0,   (int)cutout_row_start,(int)CUTOUT_ROWS);                      // vertical right
   // u8g2.drawLine(0,0,    0,(int)CUTOUT_ROWS);                                                             // vertical left

    u8g2.drawCircle(90,20, random(15)); // Circle center x,y,  random radius
    u8g2.drawStr(20,20,"Edge Impulse");  // write something to the internal memory
   


    ei_printf("Edge Impulse standalone inferencing (Arduino)\n");



    ei_impulse_result_t result = { 0 };


     // if (Serial) {
    // Grab frame and write to serial
   // if (cam.grab(frame_buffer) == 0) {
    //  Serial.write(frame_buffer, 320*240);
   // }
  //}
     // potentially need to check for above framebuffer == 0
     
   int myCamResult =  myCam.grab(frame_buffer); // myCamResult should be zero 

  //  int myCamResult;
  //  do {
  //      myCamResult =  myCam.grab(frame_buffer);
   // } while (myCamResult != 0);
  
 // myCam.skip_frames(frame_buffer, 60);

    // the features are stored into flash, and we don't want to load everything into RAM
    signal_t features_signal;
    features_signal.total_length = CUTOUT_COLS * CUTOUT_ROWS;
    features_signal.get_data = &cutout_get_data;

    // invoke the impulse
    EI_IMPULSE_ERROR res = run_classifier(&features_signal, &result, false /* debug */);
    ei_printf("run_classifier returned: %d\n", res);

    if (res != 0) return;

    // print the predictions
    ei_printf("Predictions ");
    ei_printf("(DSP: %d ms., Classification: %d ms., Anomaly: %d ms.)",
        result.timing.dsp, result.timing.classification, result.timing.anomaly);
    ei_printf(": \n");
    ei_printf("[");
    for (size_t ix = 0; ix < EI_CLASSIFIER_LABEL_COUNT; ix++) {
        ei_printf("%.5f", result.classification[ix].value);
#if EI_CLASSIFIER_HAS_ANOMALY == 1
        ei_printf(", ");
#else
        if (ix != EI_CLASSIFIER_LABEL_COUNT - 1) {
            ei_printf(", ");
        }
#endif
    }
#if EI_CLASSIFIER_HAS_ANOMALY == 1
    ei_printf("%.3f", result.anomaly);
#endif
    ei_printf("]\n");

    // human-readable predictions
    for (size_t ix = 0; ix < EI_CLASSIFIER_LABEL_COUNT; ix++) {
        ei_printf("    %s: %.5f\n", result.classification[ix].label, result.classification[ix].value);
        
    }
    
      digitalWrite(LEDB, HIGH);   
      digitalWrite(LEDG, HIGH);   
      digitalWrite(LEDR, HIGH); 
        
    if (result.classification[0].value>= 0.3){
      digitalWrite(LEDB, LOW);
      delay(300);
    } 
    
    if (result.classification[1].value>= 0.3){
      digitalWrite(LEDG, LOW);
      delay(300);
    } 
    
    if (result.classification[2].value>= 0.3){
      digitalWrite(LEDR, LOW);
      delay(300);
    }
    
#if EI_CLASSIFIER_HAS_ANOMALY == 1
    ei_printf("    anomaly score: %.3f\n", result.anomaly);
#endif



/*   unbracket to grab an image of what the board sees   */


    for (size_t ix = 0; ix < features_signal.total_length; ix++) {
        float value[1];
        features_signal.get_data(ix, 1, value);

        ei_printf("0x%06x", (int)value[0]);
        if (ix != features_signal.total_length - 1) {
          ei_printf(", ");
        }
    }

    Serial.println();

    for (int x=0; x < FRAME_BUFFER_COLS; x++){  // FRAME_BUFFER_COLS = 320
      for (int y=0; y < FRAME_BUFFER_ROWS; y++){ //FRAME_BUFFER_ROWS = 240
          //frame_buffer[FRAME_BUFFER_COLS * FRAME_BUFFER_ROWS]
          uint8_t myGRAY = frame_buffer[(x * y) + x];
          if (myGRAY > 60){
             u8g2.drawPixel(x, y);
          }
      } 
   }



    // send data to OLED
    u8g2.sendBuffer();  
        
   // delay(100);
}

/**
 * @brief      Printf function uses vsnprintf and output using Arduino Serial
 *
 * @param[in]  format     Variable argument list
 */
void ei_printf(const char *format, ...) {
    static char print_buf[1024] = { 0 };

    va_list args;
    va_start(args, format);
    int r = vsnprintf(print_buf, sizeof(print_buf), format, args);
    va_end(args);

    if (r > 0) {
        Serial.write(print_buf);
    }
}
