#include "camera.h"

CameraClass cam;

uint8_t fb[320*240] __attribute__((aligned(32)));
//int myMax = 324 * 244;
int myMax = 320 * 240;

//byte data1[myMax * 1] = { 0 };  // 1 bytes per pixel
void setup() {

 // Serial.begin(115200);
    pinMode(LEDR, OUTPUT);
    pinMode(LEDG, OUTPUT);
    pinMode(LEDB, OUTPUT);
    digitalWrite(LEDR, HIGH); // high means off
    digitalWrite(LEDG, HIGH);
    digitalWrite(LEDB, HIGH);
  //while (!Serial);

  // put your setup code here, to run once:
  cam.begin(320, 240);
 // cam.start(5000);
   cam.skip_frames(fb, 60);
  //cam.testPattern(true);
}

void loop() {
  // put your main code here, to run repeatedly:
 // if (Serial) {
   // Serial.write(cam.grab(), 324 * 244);
  //}




   // uint8_t* myImage = cam.grab();
    int myImage = cam.grab(fb);
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

 // Serial.println("sizeof(&fb)");
 // Serial.println(sizeof(&fb));
 // Serial.println();

float myTotal = 0;
float myAverage = 0;

   for(int i=0; i < myMax; i++){
    myTotal += fb[i];

    

/*
    
  //  if (i % 3 == 0){
   //   if (i==0){ Serial.print("0x");  } else { Serial.print(", 0x"); }
  // }
    char hexChar[1];

    sprintf(hexChar, "%01x", fb[i]);
    Serial.print("0x");
    Serial.print(hexChar);
    Serial.print(hexChar);
    Serial.print(hexChar);
    if (i != myMax - 1) {
      Serial.print(", ");
    }

*/

    
   }
  myAverage = myTotal/myMax;  // basically 20 = dark and about 150 = bright
  
 // Serial.println("myAverage");
 // Serial.println(myAverage);

   digitalWrite(LEDR, HIGH); // high means off
   digitalWrite(LEDG, HIGH);
   digitalWrite(LEDB, HIGH);

   if(myAverage < 60){    // dark
     digitalWrite(LEDR, LOW);
   }

   if(myAverage  >= 60 &&  myAverage < 110){   
     digitalWrite(LEDG, LOW);  // medium intensity
   }

   if(myAverage  >= 110){   // bright light
      digitalWrite(LEDB, LOW);
   }

 
 // delay(100);
 // cam.skip_frames(fb, 3);
  
}
