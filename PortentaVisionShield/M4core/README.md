###  June 2022


FOMO Vision on M4 works but can be tricky.









.




.






### 17 months ago!





Problem. My Impulse does not work on the M4 innerr core. Also my sound impulse does not work.

First lets test if the issue is an arduino issue. Need to get the camera and microphone working on the inner core.

Using [m4-microphoneTest.ino](m4-microphoneTest.ino) which shows colors depending on the highest frquency red low, green med, high blue


Looks like the M4 core mcrophone test failed. Might be easy to fix

```
'SCB_InvalidateDCache_by_Addr' was not declared in this scope

     SCB_InvalidateDCache_by_Addr((uint32_t *)&recordPDMBuf[start], AUDIO_IN_PDM_BUFFER_SIZE * 2);

     ^~~~~~~~~~~~~~~~~~~~~~~~~~~~

C:\Users\me.windows\AppData\Local\Arduino15\packages\arduino\hardware\mbed\1.3.1\libraries\PDM\src\stm32\PDM.cpp:142:5: error: 'SCB_CleanDCache_by_Addr' was not declared in this scope

     SCB_CleanDCache_by_Addr((uint32_t*)_doubleBuffer.data(), AUDIO_IN_PDM_BUFFER_SIZE * 2);

     ^~~~~~~~~~~~~~~~~~~~~~~


```



Running another color test [m4-camera-color-test.ino](m4-camera-color-test.ino) this time using the camera I got these issues

```
error: 'SCB_InvalidateDCache_by_Addr' was not declared in this scope

     SCB_InvalidateDCache_by_Addr((uint32_t*)user_buffer, GetSize(CameraCurrentResolution));

     ^~~~~~~~~~~~~~~~~~~~~~~~~~~~
```





