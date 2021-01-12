/*
  OV767X - Camera Capture Raw Bytes

  This sketch reads a frame from the OmniVision OV7670 camera
  and writes the bytes to the Serial port. Use the Procesing
  sketch in the extras folder to visualize the camera output.


enum
{
  YUV422 = 0,
  RGB444 = 1,
  RGB565 = 2,
  // SBGGR8 = 3
  GRAYSCALE = 4
};

enum
{
  VGA = 0,  // 640x480
  CIF = 1,  // 352x240
  QVGA = 2, // 320x240
  QCIF = 3,  // 176x144
  QQVGA = 4,  // 160x120
};

  int begin(int resolution, int format, int fps); // Supported FPS: 1, 5, 10, 15, 30

*/

#include <Arduino_OV767X.h>



//  - OV7670 camera module:
//  - 3.3 connected to 3.3
//  - GND connected GND
//  - SIOC/SCL connected to A5
//  - SIOD/SDA connected to A4

#define OV7670_VSYNC 8
#define OV7670_HREF  A1
#define OV7670_PLK   A0
#define OV7670_XCLK  9
#define OV7670_D0    0  // If green tint switch D0 and D1
#define OV7670_D1    1  // If green tint switch D1 and D0
#define OV7670_D2    2
#define OV7670_D3    3
#define OV7670_D4    4
#define OV7670_D5    5
#define OV7670_D6    6
#define OV7670_D7    7


const int myStartX = 0;
const int myStartY = 0;
const int myEndX = 80;  // 96
const int myEndY =  80;  // 96

const int myCroppedWidth = myEndX - myStartX;
const int myCroppedHeight = myEndY - myStartY;
const int myBytesPerPixel = 2;  // RGB565 = 2 Bytes

const int original_width = 160;  
const int original_height = 120;

int bytesPerFrame;

byte data[original_width * original_height * myBytesPerPixel]; // Original image 160 x 120 x 2 bytes RGB565 
byte myCroppedData[myCroppedWidth * myCroppedHeight * myBytesPerPixel];  // Cropped image ? x ? x 2 bytes RGB565 
uint16_t frame_buffer[myCroppedWidth * myCroppedHeight * myBytesPerPixel] = { 0 };
float myRGB888[myCroppedWidth * myCroppedHeight * 3] = { 0 };


void setup() {

  Serial.begin(115200);
  while (!Serial);

  if (!Camera.begin(QQVGA, RGB565, 5)) { //Resolution, Byte Format, FPS 1 or 5
    Serial.println("Failed to initialize camera!");
    while (1);
  }

  //bytesPerFrame = Camera.width() * Camera.height() * Camera.bytesPerPixel(); // test original with 
  bytesPerFrame = myCroppedWidth * myCroppedHeight * myBytesPerPixel;          // crop with

  // Camera.testPattern();
}




void r565_to_rgb(uint16_t color, uint8_t *r, uint8_t *g, uint8_t *b) {
 *r = ((color >> 11) & 0x1f) << 3;
 *g = ((color >> 5) & 0x3f) << 2;
 *b = ((color >> 0) & 0x1f) << 3;
}


 // *r = byte(((color & 0xF800) >> 11) << 3);
 // *g = byte(((color & 0x7E0) >> 5) << 2);
 // *b = byte(((color & 0x1F)) << 3);
//}
  
     // *r = ((((color >> 3) & 0x1F) * 527) + 23) >> 6;
      //*g = ((((((color & 0xE0) >> 5) | ((color & 0x03) << 3)) & 0x3F) * 259) + 33) >> 6;
     // *b = (((color & 0x1F) * 527) + 23) >> 6;
//}

  //  *r = (((color & 0xF800) >> 11)*255)/31;
  //  *g = (((color & 0x07E0) >> 5)*255)/63;
  //  *b = (((color & 0x1F)))*255/31;
//}

 //   *r = (color & 0xF800) >> 8;
 //   *g = (color & 0x07E0) >> 3;
 //   *b = (color & 0x1F) << 3;
//}

//*r = ((((color >> 11) & 0x1F) * 527) + 23) >> 6;
//*g = ((((color >> 5) & 0x3F) * 259) + 33) >> 6;
//*b = (((color & 0x1F) * 527) + 23) >> 6;

//}

// *r = ((color >> 11) & 0x1f) << 3;
// *g = ((color >> 5) & 0x3f) << 2;
// *b = ((color >> 0) & 0x1f) << 3;
//  }




void my565to888(){

        memcpy(frame_buffer, myCroppedData, sizeof(frame_buffer));
        
        int index2 = 0;
        for (int y =0; y < myEndY; y++) {
        for (int x = 0; x < myEndX; x++){  
            // loop through each pixel and convert it to a float
            // myCroppedData[(y*myEndY)+x]; 
           uint16_t pixel = frame_buffer[(y*myEndY)+x];

           uint8_t r, g, b;
           r565_to_rgb(pixel, &r, &g, &b);
          // float pixel_f;
           float pixel_f = (r << 16) + (g << 8) + b;
       //  float pixel_f = (r << 16) | (g << 8) | b;
           myRGB888[index2++] = pixel_f;
                
        }
      }
  
  }
  



  
void myCrop() {
      
      int index = 0;

      // loop through rows and columns grabbing wanted bytes
      for (int y = myStartY; y < myEndY; y++) {
        for (int x = myStartX; x < (myEndX * myBytesPerPixel); x++){  
           myCroppedData[index++] = data[(y*original_width * myBytesPerPixel)+x]; 
        }
      }
 
}


void loop() {
  Camera.readFrame(data);

  myCrop();
  my565to888();

  // Serial.write(data, bytesPerFrame);        // test original with 
  //Serial.write(myCroppedData, bytesPerFrame);  // crop with

  int myfloatSize = myCroppedWidth * myCroppedHeight;   // not *3 since floats not bytes

  
    Serial.println("sizeof(frame_buffer)");
    Serial.println(sizeof(frame_buffer));


    
    Serial.println("myfloatSize");  
    Serial.println(myfloatSize);   
    Serial.println();   
    Serial.println();

    
    Serial.println();
    for (size_t ix = 0; ix < myfloatSize; ix++) {
        float value[1];
       // signal.get_data(ix, 1, value);
       value[0] = myRGB888[ix];
        char myHex[8];
        sprintf(myHex, "0x%06x", (int)value[0]);
        Serial.print(myHex);
        delayMicroseconds(1);
        if (ix != myfloatSize - 1) {
          Serial.print(", ");
        }
    }

    Serial.println();
    Serial.println();
    Serial.println();
    Serial.println("Waiting for 10 seconds ...");
    delay(10000);
}
