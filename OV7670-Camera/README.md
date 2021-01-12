Started Jan 4th, 2021

All files to use the OV7670 camera with the Nano33 BLE SENSE and hopefully later the Porttenta Vision Shield



interesting


    int r = ((p >> 11) & 0x1f) << 3;
    int g = ((p >> 5) & 0x3f) << 2;
    int b = ((p >> 0) & 0x1f) << 3;
