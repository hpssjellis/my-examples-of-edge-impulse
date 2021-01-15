/* 
 *  By Jeremy Elis @rocksetta
 *  Hopefully this sketch is still at
 *  https://github.com/hpssjellis/my-examples-of-edge-impulse/blob/main/OV7670-Camera/nanoCamOV7670.ino

*/

#define EI_DSP_IMAGE_BUFFER_STATIC_SIZE 128
/* Includes ---------------------------------------------------------------- */
//  Hopefully my included library is at the github repository
//  https://github.com/hpssjellis/my-examples-of-edge-impulse/tree/main/OV7670-Camera
#include <ov7670-08-detect-micro-restored_inference.h>

#include <Arduino_OV767X.h>




/*defines so that the newer OV7670 Camera module works.*/

//  - OV7670 camera module:
//  - 3.3 connected to 3.3
//  - GND connected GND
//  - SIOC/SCL connected to A5
//  - SIOD/SDA connected to A4

#define OV7670_VSYNC 8
#define OV7670_HREF  A1
#define OV7670_PLK   A0
#define OV7670_XCLK  9
#define OV7670_D0    0  // If green tint switch D0 with D1
#define OV7670_D1    1  // If green tint switch D1 with D0
#define OV7670_D2    2
#define OV7670_D3    3
#define OV7670_D4    4
#define OV7670_D5    5
#define OV7670_D6    6
#define OV7670_D7    7





// raw frame buffer from the camera
#define FRAME_BUFFER_COLS           160
#define FRAME_BUFFER_ROWS           120
uint16_t frame_buffer[FRAME_BUFFER_COLS * FRAME_BUFFER_ROWS] = { 0 };

// cutout that we want (this does not do a resize, which would also be an option, but you'll need some resize lib for that)
#define CUTOUT_COLS                 EI_CLASSIFIER_INPUT_WIDTH
#define CUTOUT_ROWS                 EI_CLASSIFIER_INPUT_HEIGHT
const int cutout_row_start = (FRAME_BUFFER_ROWS - CUTOUT_ROWS) / 2;
const int cutout_col_start = (FRAME_BUFFER_COLS - CUTOUT_COLS) / 2;

// helper methods to convert from rgb -> 565 and vice versa this one not used
uint16_t rgb_to_565(uint8_t r, uint8_t g, uint8_t b) {
    return ((r >> 3) << 11) | ((g >> 2) << 5) | (b >> 3);
}

// funciton converts from RGB565b to RGB888 and is used.
void r565_to_rgb(uint16_t color, uint8_t *r, uint8_t *g, uint8_t *b) {
    *r = (color & 0xF800) >> 8;
    *g = (color & 0x07E0) >> 3;
    *b = (color & 0x1F) << 3;
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
        uint16_t pixelTemp = frame_buffer[(frame_buffer_row * FRAME_BUFFER_COLS) + frame_buffer_col];

        // This line needed to switch big and little endians
        uint16_t pixel = (pixelTemp>>8) | (pixelTemp<<8);

        uint8_t r, g, b;
        r565_to_rgb(pixel, &r, &g, &b);

        // then convert to out_ptr format
        float pixel_f = (r << 16) + (g << 8) + b;
        //float pixel_f = (r << 16) | (g << 8) | b;
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
    // put your setup code here, to run once:
    Serial.begin(115200);
    //while (!Serial); // Don't trap the serial port unless debugging

    if (!Camera.begin(QQVGA, RGB565, 5)) { //Resolution, Byte Format, FPS 1 or 5
       Serial.println("Failed to initialize camera!");
       while (1);
    }
    Serial.println("Edge Impulse Inferencing Demo");
}

/**
 * @brief      Arduino main function
 */
void loop()
{
    ei_printf("Edge Impulse standalone inferencing (Arduino)\n");
    ei_impulse_result_t result = { 0 };

    // Get 160 x 120 RGB565 data from OV7670 Camera 
    Camera.readFrame((uint8_t*)frame_buffer);

    // Set up pointer to look after data, crop it and convert it to RGB888
    signal_t signal;
    signal.total_length = CUTOUT_COLS * CUTOUT_ROWS;
    signal.get_data = &cutout_get_data;

    // Feed signal to the classifier
    EI_IMPULSE_ERROR res = run_classifier(&signal, &result, false /* debug */);

    // Returned error variable "res" while data object.array in "result" 
    ei_printf("run_classifier returned: %d\n", res);
    if (res != 0) return;

    // print the predictions
    ei_printf("Predictions ");
    ei_printf("(DSP: %d ms., Classification: %d ms., Anomaly: %d ms.)",
        result.timing.dsp, result.timing.classification, result.timing.anomaly);
    ei_printf(": \n");

    // Print short form result data
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
    #if EI_CLASSIFIER_HAS_ANOMALY == 1
        ei_printf("    anomaly score: %.3f\n", result.anomaly);
    #endif



/*   unbracket to grab an image of what the board sees   */

/*
    for (size_t ix = 0; ix < signal.total_length; ix++) {
        float value[1];
        signal.get_data(ix, 1, value);

        ei_printf("0x%06x", (int)value[0]);
        if (ix != signal.total_length - 1) {
          ei_printf(", ");
        }
    }
*/
    Serial.println();
    delay(7000);
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
