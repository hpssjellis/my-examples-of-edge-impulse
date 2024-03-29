/*
 * 
 * Must use portenta with Vision camerera and Grove OLED 
 * Should be implemented with MBED version greater than 1.4.2
 * Until then needs the 2 main library folders Portenta_Camera and Himax_HM01B0
 * 
 * 
 *
 * Purchase here https://wiki.seeedstudio.com/Grove-OLED_Display_0.96inch/ about $20 USD
 *
 * Note u8g2 is an arduino search caplble library, easy to install
 *
 *  using 128 x 64 seeeduino OLED
 *  black GND 
 *  red 3v3      so do not need the protective resistors below
 *  white SDA    D11 // on portenta
 *  yellow SCL   D12 // on portenta
 *
 * another reference here 
 * https://github.com/olikraus/u8g2/wiki/u8g2reference#setfont
 *
 */

 
 
#include <Arduino.h>  // only needed for https://platformio.org/

#include <U8g2lib.h>


// works
U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE);   //(rotation, clock, data [, reset])


#define EI_DSP_IMAGE_BUFFER_STATIC_SIZE 128


/* Includes ---------------------------------------------------------------- */



//#include <ei-portenta-38-try-96x96-full_inferencing.h>



#include <ei-portenta-37-try-120x120-full_inferencing.h>


//#include <ei-portenta-36-try-160x160-filters4_inferencing.h>


//#include <ei-portenta-35-try-160x160-regular_inferencing.h>



//#include <ei-portenta-34-try-96x96-from-scratch_inferencing.h>


//#include <ei-portenta-26-240x240-try_inferencing.h>


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

   int x1Map, x2Map, y1Map, y2Map;


void print_memory_info() {
    // allocate enough room for every thread's stack statistics
    int cnt = osThreadGetCount();
    mbed_stats_stack_t *stats = (mbed_stats_stack_t*) malloc(cnt * sizeof(mbed_stats_stack_t));
 
    cnt = mbed_stats_stack_get_each(stats, cnt);
    for (int i = 0; i < cnt; i++) {
        ei_printf("Thread: 0x%lX, Stack size: %lu / %lu\r\n", stats[i].thread_id, stats[i].max_size, stats[i].reserved_size);
    }
    free(stats);
 
    // Grab the heap statistics
    mbed_stats_heap_t heap_stats;
    mbed_stats_heap_get(&heap_stats);
    ei_printf("Heap size: %lu / %lu bytes\r\n", heap_stats.current_size, heap_stats.reserved_size);
}







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
    u8g2.setFont(u8g2_font_ncenB08_tr); 
    // put your setup code here, to run once:

   // int x1 = (int)cutout_col_start;
   // int x2 = (int)CUTOUT_COLS;
   // int y1 = (int)cutout_row_start;  
   // int y2 = (int)CUTOUT_ROWS; 

    //   map cutout of the 320 x 240 model to OLED 128 x 64 screen
    x1Map = map((int)cutout_col_start, 0, 320, 0, 127);  
    x2Map = map((int)CUTOUT_COLS, 0, 320, 0, 127);
    y1Map = map((int)cutout_row_start, 0, 240, 0, 63);
    y2Map = map((int)CUTOUT_ROWS, 0, 240, 0, 63);

    Serial.begin(115200);
     
    pinMode(LEDR, OUTPUT); 
    pinMode(LEDG, OUTPUT);   
    pinMode(LEDB, OUTPUT);  
      

    // Init the cam
    myCam.begin(CAMERA_R320x240, 30);

   // Serial.println("Edge Impulse Inferencing Demo");
}

/**
 * @brief      Arduino main function
 */
void loop()
{

    u8g2.clearBuffer();                   // clear the internal memory

    ei_printf("Edge Impulse standalone inferencing (Arduino)\n");
    ei_printf("CUTOUT_COLS : %d\n", CUTOUT_COLS );
    ei_printf("CUTOUT_ROWS : %d\n", CUTOUT_ROWS );
    ei_printf("FRAME_BUFFER_COLS : %d\n", FRAME_BUFFER_COLS );
    ei_printf("FRAME_BUFFER_ROWS : %d\n", FRAME_BUFFER_ROWS );
    ei_printf("sizeof(frame_buffer) : %d\n", sizeof(frame_buffer) );
    print_memory_info();

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
    digitalWrite(LEDB, HIGH);   //on board LED's are turned off by HIGH    
    digitalWrite(LEDG, HIGH);   
    digitalWrite(LEDR, HIGH); 
        
    if (result.classification[0].value>= 0.3){  //BLUE on first object
      digitalWrite(LEDB, LOW);
      delay(300);
    } 
    
    if (result.classification[1].value>= 0.3){  // green on second object
      digitalWrite(LEDG, LOW);
      delay(300);
    } 
    
    if (result.classification[2].value>= 0.3){   // red on third object
      digitalWrite(LEDR, LOW);
      delay(300);
    }
    
#if EI_CLASSIFIER_HAS_ANOMALY == 1
    ei_printf("    anomaly score: %.3f\n", result.anomaly);
#endif



/*   unbracket to grab an image of what the board sees   */

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
    Serial.println();

      for (int x=0; x < FRAME_BUFFER_COLS; x++){     // FRAME_BUFFER_COLS = 320
        for (int y=0; y < FRAME_BUFFER_ROWS; y++){       //FRAME_BUFFER_ROWS = 240
          //frame_buffer[FRAME_BUFFER_COLS * FRAME_BUFFER_ROWS]
          
          uint8_t myGRAY = frame_buffer[(y * (int)FRAME_BUFFER_COLS) + x];  
          if (myGRAY > 100){  // if brightish then put pixel on OLED 0 to 255

            int xMap = map(x, 0, 320, 0, 128);
            int yMap = map(y, 0, 240, 0, 64);
            u8g2.drawPixel(xMap, yMap);   //128 x 64
          }
      } 
   }



    u8g2.setDrawColor(2);  // XOR OLED  
    u8g2.drawFrame(x1Map, y1Map,   x2Map, y2Map);

    u8g2.setDrawColor(1);               // regular OLED
    u8g2.drawStr(20,10,"Rocksetta");    // write something to the internal memory

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

 #ifdef CORE_CM7  //The M4 core does not print
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
