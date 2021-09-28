Just another folder as my other one was getting full. This one uses the GRAYSCALE 128x128 1.5 inch waveshare OLED and the new 2.5.2 
verison of Portenta software that allows memory sharing between the cores but does not increase the heap meaning presently our models can't be much bigger


looking into squish instead of cutout ways to look at the image


https://www.waveshare.com/1.5inch-oled-module.htm





Note: I tried this one on 320x320 Arduino Portenta Camera and it failed

b44-BAD-gray-128x128-oled-320x320-camera.ino

I filed an issue with arduino mbed core

https://github.com/arduino/ArduinoCore-mbed

https://github.com/arduino/ArduinoCore-mbed/issues/336





