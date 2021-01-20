/*
Colors for higher sounds

  Circuit:
  - Arduino Nano 33 BLE board

  This example code is in the public domain.
*/

#include <PDM.h>

// buffer to read samples into, each sample is 16-bits
short sampleBuffer[256];

// number of samples read
volatile int samplesRead;
int myMax;

void setup() {
  //Serial.begin(9600);
  //while (!Serial);
    pinMode(LEDR, OUTPUT);
    pinMode(LEDG, OUTPUT);
    pinMode(LEDB, OUTPUT);
    digitalWrite(LEDR, HIGH); // high means off
    digitalWrite(LEDG, HIGH);
    digitalWrite(LEDB, HIGH);

  // configure the data receive callback
  PDM.onReceive(onPDMdata);

  // optionally set the gain, defaults to 20
  // PDM.setGain(30);

  // initialize PDM with:
  // - one channel (mono mode)
  // - a 16 kHz sample rate
  if (!PDM.begin(1, 16000)) {
   // Serial.println("Failed to start PDM!");
    while (1);
  }
}

void loop() {
  // wait for samples to be read
  if (samplesRead) {

    // print samples to the serial monitor or plotter


myMax = 0;
    
    for (int i = 0; i < samplesRead; i++) {
    //  Serial.println(sampleBuffer[i]);
    if (sampleBuffer[i]> myMax){myMax = sampleBuffer[i];}
    digitalWrite(LEDR, HIGH); // high means off
    digitalWrite(LEDG, HIGH);
    digitalWrite(LEDB, HIGH);

    }
        if(myMax < 2000){    // low pitch
         digitalWrite(LEDR, LOW);
      }

          if(myMax  >= 2000 &&  myMax < 10000){   
         digitalWrite(LEDG, LOW);  // medium pitch
      }


          if(myMax  >= 10000){   // high pitch
         digitalWrite(LEDB, LOW);
      }
   delay(100);
    // clear the read count
    samplesRead = 0;
  }
}

void onPDMdata() {
  // query the number of bytes available
  int bytesAvailable = PDM.available();

  // read into the sample buffer
  PDM.read(sampleBuffer, bytesAvailable);

  // 16-bit, 2 bytes per sample
  samplesRead = bytesAvailable / 2;
}
