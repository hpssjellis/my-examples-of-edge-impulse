# My Silicon Labs Thunderboard2 stuff here

##### By @rocksetta
##### Use at you own risk

## explanatioin of the above .bin files

When I first tried the ThunderBoard2 I was very imprerssed with the out-of-the-box Android (and IOS) BLE phone connection to the board. Very fancy to get up and running immediately. I then tried installing a few of my recent ei- libraries, see above files and then I was shocked that I could not reset the board. After a few days of installing software and about 7 Gb later (I was also installing the full NRF-Connect) I found out that in the demos you can install directly to the thunderboard the default app called (Bluetooth-SoC-Thunderboard Ssense 2), but they don't actually give you the file. (Probably some reason for that) and when you dig for it it has the special format .s37. This is a bit of a waste of the amazing pop-up folder that allows drag and drop installation of files.

So I tried a blinky program to make sure my board was working, then dragged the .s37 file to the popup folder the Thunderboard makes and it reset the thunderboard back to factory setting that work with the app.

I then tried putting it on this github and you can download .bin files but not .s37 files. I then zipped it and you can download that. I then changed the file name from .s37 to .bin and it still installs changing blinky back to the default app. Not sure why SiliconLabs does not give this .bin file out (Probably a reason) as I was very upset when I thought I had basically bricked my board. The installation of the Silicon labs Simplicity setup is a large install and completely filled up almost making my old desktop un-useable. 

I hope this bin is helpful to others who also have that sense of panick that you have bricked a new board.

Drag this file [thunderboard-default.bin](thunderboard-default.bin) to your Thunderboard2 pop up folder to reset the board.




## Main link

https://www.silabs.com/development-tools/thunderboard/thunderboard-sense-two-kit

## Cloud compile  arm mbed

https://account.mbed.com/login



Useful locations when using Simplicity Studio

## Simplicity Studion main link
https://www.silabs.com/developers/simplicity-studio


## MBED hardware  installation location on windows 
C:\Users\USERNAME\AppData\Local\Arduino15\packages\arduino\hardware\mbed\1.3.2





## Where your .s37 demo files are generated
C:\SiliconLabs\SimplicityStudio\v5\developer\sdks\gecko_sdk_suite\v3.1\app\bluetooth\demos
