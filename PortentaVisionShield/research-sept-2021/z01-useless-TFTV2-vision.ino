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




#include <Adafruit_GFX.h>
#include <Adafruit_ILI9341.h>
#include <PortentaTouchScreen.h>


// PortentaTouchScreen ts(XP, YP, XM, YM, 300);  // original
// PortentaTouchScreen ts(A3, A2, A1, A0, 300);  // Yun
// Must be:       digital, analog/digital, analog/digital, digital, resistor across x plate ~300 ohms
// Touchscreen          X+, X-, Y+, Y-, resistor 
PortentaTouchScreen ts(D5, A6, A7, D4, 300);  // on Portenta

// Set for Portenta
// SPI settings  MISO (CIPO) D10  // on Portenta
//               MOSI (COPI) D9
//               SCK         D8
#define TFT_CS D7
#define TFT_DC D6
Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC);




#define EI_DSP_IMAGE_BUFFER_STATIC_SIZE 128


/* Includes ------
---------------------------------------------------------- */
//#include <ei-portenta-17-detect-micro-110x110-grayscale_inferencing.h>

//#include <ei-ov7670-16-detect-micro-100x100-grayscale_inferencing.h>

#include <ei-ov7670-15-detect-micro-96x96-grayscale_inferencing.h>


#include "camera.h"

CameraClass myCam;

// raw frame buffer from the camera
#define FRAME_BUFFER_COLS          320   // 160
#define FRAME_BUFFER_ROWS          240   // 120
//uint16_t frame_buffer[FRAME_BUFFER_COLS * FRAME_BUFFER_ROWS] = { 0 };

uint8_t frame_buffer[320*240] __attribute__((aligned(32)));

// globals for the rectangle
int x1Map, x2Map, y1Map, y2Map;


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
  
  tft.begin();

  tft.fillScreen(ILI9341_BLUE);
  // origin = left,top landscape (USB left upper)
  tft.setRotation(1); 
  
  tft.setTextColor(ILI9341_WHITE);
  tft.setTextSize(2);
  tft.setCursor(100, 20);  //before print set x,y  
  tft.println("Rocksetta");  // write something to the internal memory

 // tft.setCursor(100, 20);  //before print set x,y
 // tft.println("Working!");
    
    int x1 = (int)cutout_col_start;
    int x2 = (int)CUTOUT_COLS;
    int y1 = (int)cutout_row_start;  
    int y2 = (int)CUTOUT_ROWS; 

    //   map cutout of the 320 x 240 model to OLED 128 x 64 screen
    x1Map = x1; //map(x1, 0, 320, 0, 127);  
    x2Map = x2; //map(x2, 0, 320, 0, 127);
    y1Map = y1; //map(y1, 0, 240, 0, 63);
    y2Map = y2; //map(y2, 0, 240, 0, 63);

    Serial.begin(115200);
     pinMode(LEDB, OUTPUT);   
     digitalWrite(LEDB, HIGH); // internal LED HIGH = off
     
     pinMode(LEDG, OUTPUT);   
     digitalWrite(LEDG, HIGH); 
     
     pinMode(LEDR, OUTPUT);   
     digitalWrite(LEDR, HIGH); 

     myCam.begin(CAMERA_R320x240, 30); 
   
    ei_printf("Edge Impulse Inferencing Demo by Rocksetta\n");
}

/**
 * @brief      Arduino main function
 */
void loop()
{

    ei_printf("Edge Impulse standalone inferencing (Arduino)\n");
    ei_impulse_result_t result = { 0 };
 
   int myCamResult =  myCam.grab(frame_buffer); // myCamResult should be zero 

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



/*   unbracket to grab image of what the board sees on serial print  */

/*
    for (size_t ix = 0; ix < features_signal.total_length; ix++) {
        float value[1];
        features_signal.get_data(ix, 1, value);

        ei_printf("0x%06x", (int)value[0]);
        if (ix != features_signal.total_length - 1) {
          ei_printf(", ");
        }
    }

*/

    // tft.fillScreen(ILI9341_BLUE);
    
      for (int x=0; x < FRAME_BUFFER_COLS; x++){     // FRAME_BUFFER_COLS = 320
        for (int y=0; y < FRAME_BUFFER_ROWS; y++){       //FRAME_BUFFER_ROWS = 240
          
          uint8_t myGRAY = frame_buffer[(y * (int)FRAME_BUFFER_COLS) + x];  
             if (myGRAY > 70){  // if brightish then put pixel on OLED
            // rgb = grey << 16 | grey << 8 | grey
            // Y = .2126 * R^gamma + .7152 * G^gamma + .0722 * B^gamma
            //  int rgb = grey * 0x00010101; 
            //int xMap = map(x, 0, 320, 0, 128);
           // int yMap = map(y, 0, 240, 0, 64);
           
           // uint16_t myRGB565 = myGRAY << 16 | myGRAY << 8 | myGRAY;

            tft.drawPixel(x, y, ILI9341_WHITE);   //128 x 64, unit_16 color GREY to RGB565

            // u8g2.drawPixel(x,y);
          }
      } 
   }

 //  tft.drawRect(x1Map-1, y1Map-1,   x2Map+1, y2Map+1, 10000);
 

  // tft.setCursor(100, 20);  //before print set x,y  
 //  tft.println("Rocksetta");  // write something to the internal memory


}

/**
 * @brief      Printf function uses vsnprintf and output using Arduino Serial
 *
 * @param[in]  format     Variable argument list
 */
void ei_printf(const char *format, ...) {

  #ifdef CORE_CM4  // do essentially no printing
  #else   // for all other microcontrollers continue to print
    static char print_buf[1024] = { 0 };

    va_list args;
    va_start(args, format);
    int r = vsnprintf(print_buf, sizeof(print_buf), format, args);
    va_end(args);

    if (r > 0) {
        Serial.write(print_buf);
    }
  #endif
}
