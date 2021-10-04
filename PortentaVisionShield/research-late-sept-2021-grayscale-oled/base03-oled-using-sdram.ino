/*
 * 
 *
 * Purchase here https://www.waveshare.com/1.5inch-OLED-Module.htm about $29 USD
 *
 * Note u8g2 is an arduino search caplble library, easy to install
 *
 *  using 128 x 64 seeeduino OLED
 *  black GND 
 *  red 3v3      so do not need the protective resistors below
 *  //white SDA    D11 // on portenta
 *  //yellow SCL   D12 // on portenta
 *  
 *  FOR the GRAYSCALE Waveshare OLED
 *   blue  DIN (mosi) D8
 *   yellow (sck) D9 
 *   orange (cs) D7
 *   green (dc)  D6
 *   white (reset) not needed but D14 if you did
 *
 * another reference here 
 * https://learn.adafruit.com/adafruit-gfx-graphics-library/graphics-primitives
 *
 */


#include <Arduino.h>  // only needed for https://platformio.org/
#include <SDRAM.h>
#include <Adafruit_SSD1327.h>

// Used for software SPI
#define OLED_CLK D9  //yellow wire
#define OLED_MOSI D8 // blue wire 

// Used for software or hardware SPI
#define OLED_CS D7  // orange wire
#define OLED_DC D6   // green wire 

// Used for I2C or SPI
#define OLED_RESET -1

// hardware SPI
Adafruit_SSD1327 display(128, 128, &SPI, OLED_DC, OLED_RESET, OLED_CS);


#include "camera.h"

CameraClass cam;

SDRAMClass mySDRAM;

uint8_t *sdram_frame_buffer;
//uint8_t frame_buffer[320*320];

void setup() {
  //Serial.begin(921600);  
  Serial.begin(115200);  
 
  //mySDRAM.begin();
  
  mySDRAM.begin(SDRAM_START_ADDRESS);

  
  sdram_frame_buffer = (uint8_t *)mySDRAM.malloc(320 * 320 * sizeof(uint8_t));
 
  // Init the cam QVGA, 30FPS
  cam.begin(CAMERA_R320x320, 30);

    if ( ! display.begin(0x3D) ) {
     Serial.println("Unable to initialize OLED");
     while (1) yield();
  }    
    display.setTextSize(1);
    display.setTextColor(SSD1327_WHITE);

    display.setRotation(0);
    display.setCursor(0,0);

    
}

void loop() {
    display.clearDisplay();                 // clear the internal memory

  
   //  display.drawPixel(random(128), random(128), random(512)); 
  // Wait until the receiver acknowledges
  // that they are ready to receive new data
  //while(Serial.read() != 1){};
  
  // Grab frame and write to serial
  if (cam.grab(sdram_frame_buffer) == 0) {
     //Serial.write(frame_buffer, 320*320);     


    for (int x=0; x < 320; x++){     // FRAME_BUFFER_COLS = 320
       for (int y=0; y < 320; y++){       //FRAME_BUFFER_ROWS = 320

          uint16_t myGRAY = sdram_frame_buffer[(y * 320) + x] ;  
        //  int myGRAY = frame_buffer[(y * 320) + x] ;  

        
          int myGrayMap = map(myGRAY, 0, 255, 0, 15);  
          int xMap = map(x, 0, 320, 0, 127);
          int yMap = map(y, 0, 320, 0, 127);
     //     display.drawPixel(xMap, yMap, myGRAY); 
           display.drawPixel(xMap, yMap, (int)myGrayMap);   // grayscale 0-16, 128x128  //128 x 64
         // delayMicroseconds(10);
      } 
    }     
  }

      display.drawRect(5,5,120,120, SSD1327_WHITE );

    display.setCursor(10,10);
    display.println("Rocksetta");

    
  display.display();
        
 // delay(100); 
}
