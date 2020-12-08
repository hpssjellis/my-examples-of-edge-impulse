Until a fix is on the Arduino MBED folder for Windows this file needs to be in the same folder at the platform.txt file


Problem the file needs to go in a hidden folder from your windows accouunt.

Easiest way to find it is to go to your login name and type 

```
\AppData
```

if that works then follow the directories to your mbed version

```
\AppData\Local\Arduino15\packages\arduino\hardware\mbed\1.3.0
```

If you look in that file you should already have a platform.txt file


put the above platform.local.txt file in the same  folder.


That will allow longer than 32k filename string until a new version of arduino solves the problem.



Here is what the entire path looks like 
```
C:\Users\YOUR_USERNAME\AppData\Local\Arduino15\packages\arduino\hardware\mbed\1.3.0
```




