#include "camera.h"

CameraClass cam;


//int myMax = 324 * 244;
int myMax = 320 * 240;

//byte data1[myMax * 1] = { 0 };  // 1 bytes per pixel
void setup() {

  Serial.begin(115200);
  //while (!Serial);

  // put your setup code here, to run once:
  cam.begin(324, 244);
  cam.start(5000);
  //cam.testPattern(true);
}

void loop() {
  // put your main code here, to run repeatedly:
 // if (Serial) {
   // Serial.write(cam.grab(), 324 * 244);
  //}




   // uint8_t* myImage = cam.grab();
    uint8_t* myImage = cam.grab();
/*
    for (size_t i = 0; ix < signal.total_length; ix++) {
        float value[1];
        signal.get_data(ix, 1, value);

        ei_printf("0x%06x", (int)value[0]);
        if (ix != signal.total_length - 1) {
          ei_printf(", ");
        }
    }
*/

  Serial.println("sizeof(&myImage)");
  Serial.println(sizeof(&myImage));
  Serial.println();


   for(int i=0; i < myMax; i++){
  //  if (i % 3 == 0){
   //   if (i==0){ Serial.print("0x");  } else { Serial.print(", 0x"); }
  // }
    char hexChar[1];

    sprintf(hexChar, "%01x", myImage[i]);
    Serial.print("0x");
    Serial.print(hexChar);
    Serial.print(hexChar);
    Serial.print(hexChar);
    if (i != myMax - 1) {
      Serial.print(", ");
    }
   }

  Serial.println();
  
}
