/* Edge Impulse Arduino examples
 * Copyright (c) 2020 EdgeImpulse Inc.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

/* Includes ---------------------------------------------------------------- */
#define EI_DSP_IMAGE_BUFFER_STATIC_SIZE 128
#include <ov7670-detect-microcontroller_inference.h>
#include "camera.h"



CameraClass cam;


//int myMax = 324 * 244;
int myMax = 320 * 240;




// raw frame buffer from the camera
#define FRAME_BUFFER_COLS           320
#define FRAME_BUFFER_ROWS           240
//byte data[FRAME_BUFFER_COLS * FRAME_BUFFER_ROWS * 2] = { 0 };  // 2 bytes per pixel
//uint16_t frame_buffer[FRAME_BUFFER_COLS * FRAME_BUFFER_ROWS] = { 0 };

    uint8_t* frame_buffer;


// cutout that we want (this does not do a resize, which would also be an option, but you'll need some resize lib for that)
#define CUTOUT_COLS                 EI_CLASSIFIER_INPUT_WIDTH
#define CUTOUT_ROWS                 EI_CLASSIFIER_INPUT_HEIGHT
const int cutout_row_start = (FRAME_BUFFER_ROWS - CUTOUT_ROWS) / 2;
const int cutout_col_start = (FRAME_BUFFER_COLS - CUTOUT_COLS) / 2;

// helper methods to convert from rgb -> 565 and vice versa
uint16_t rgb_to_565(uint8_t r, uint8_t g, uint8_t b) {
    return ((r >> 3) << 11) | ((g >> 2) << 5) | (b >> 3);
}

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

   // Serial.println("inside cutout_get_data sizeof(frame_buffer) : " + String(sizeof(frame_buffer)) );

    
  //  Serial.println("inside CUTOUT_COLS: " + String(CUTOUT_COLS) );
   // Serial.println("inside CUTOUT_ROWS: " + String(CUTOUT_ROWS) );
  // Serial.println("inside FRAME_BUFFER_COLS: " + String(FRAME_BUFFER_COLS) );
   // Serial.println("inside FRAME_BUFFER_ROWS: " + String(FRAME_BUFFER_ROWS) );

    
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

      //  uint8_t r, g, b;
      //  r565_to_rgb(pixel, &r, &g, &b);

        // then convert to out_ptr format
       // float pixel_f = pixel + pixel + pixel;  // 1 byte to 3 byte float hopefully
      //  float pixel_f = (r << 16) + (g << 8) + b;
        float pixel_f = (pixel << 16) + (pixel << 8) + pixel;
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
    while (!Serial);
     
    cam.begin(324, 244);
    cam.start(5000);

    
    //if (!Camera.begin(QQVGA, RGB565, 5)) { //Resolution, Byte Format, FPS 1 or 5
    //   Serial.println("Failed to initialize camera!");
    //   while (1);
   // }
    Serial.println("Edge Impulse Inferencing Demo");
}

/**
 * @brief      Arduino main function
 */
void loop()
{
    ei_printf("Edge Impulse standalone inferencing (Arduino)\n");



///    if (sizeof(features) / sizeof(float) != EI_CLASSIFIER_DSP_INPUT_FRAME_SIZE) {
///        ei_printf("The size of your 'features' array is not correct. Expected %lu items, but had %lu\n",
///            EI_CLASSIFIER_DSP_INPUT_FRAME_SIZE, sizeof(features) / sizeof(float));
///        delay(1000);
///        return;
///    }

    ei_impulse_result_t result = { 0 };

    Serial.println("Loading Image");
  //  Camera.readFrame(data);



     frame_buffer = cam.grab();

  
   // Camera.readFrame((uint8_t*)frame_buffer);


    
    
   // Serial.println("Copying Byte image array to uint16 image array");
   // memcpy(frame_buffer, data, sizeof(frame_buffer));
    
   // Serial.println("sizeof(data) : " + String(sizeof(data)) );
  //  Serial.println("sizeof(frame_buffer) : " + String(sizeof(frame_buffer)) );
  //  Serial.println("CUTOUT_COLS: " + String(CUTOUT_COLS) );
  //  Serial.println("CUTOUT_ROWS: " + String(CUTOUT_ROWS) );

    signal_t signal;
    signal.total_length = CUTOUT_COLS * CUTOUT_ROWS;
    
    Serial.println("Cropping Image");
    signal.get_data = &cutout_get_data;





///    // the features are stored into flash, and we don't want to load everything into RAM
///    signal_t features_signal;
///    features_signal.total_length = sizeof(features) / sizeof(features[0]);
///    features_signal.get_data = &raw_feature_get_data;

    // invoke the impulse
///    EI_IMPULSE_ERROR res = run_classifier(&features_signal, &result, false /* debug */);

    Serial.println();
    EI_IMPULSE_ERROR res = run_classifier(&signal, &result, false /* debug */);
    
    Serial.println();
    ei_printf("run_classifier returned: %d\n", res);
   
    Serial.println("Classifier returned");
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
#if EI_CLASSIFIER_HAS_ANOMALY == 1
    ei_printf("    anomaly score: %.3f\n", result.anomaly);
#endif




    Serial.println();
    Serial.println();
    Serial.println();
    for (size_t ix = 0; ix < signal.total_length; ix++) {
        float value[1];
        signal.get_data(ix, 1, value);

        ei_printf("0x%06x", (int)value[0]);
        if (ix != signal.total_length - 1) {
          ei_printf(", ");
        }
    }




    Serial.println();
    Serial.println();
    Serial.println("Waiting 20 seconds");
    delay(20000);    
    Serial.println("Picture in 2 seconds");
    delay(2000);
    Serial.println("Picture Now");
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
