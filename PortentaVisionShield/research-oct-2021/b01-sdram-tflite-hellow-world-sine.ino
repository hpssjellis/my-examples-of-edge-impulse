/*
 * b01_makerML_hello_world.ino
 * simplifying TensorflowLite Micro Machine Learning for all makers
 * 
 *  Original working file at (or install portenta-pro-community-solutions)
 *  https://github.com/hpssjellis/portenta-pro-community-solutions/tree/main/examples/e-portenta-machine-learning/e-b-portenta-rocksetta-ml/e-b-a-rocksetta-hello-ml-sine
 * 
 * By Jeremy Ellis 
 * Twitter @rocksetta
 * Website https://rocksetta.com
 * created Aug 20th, 2020 
 * Github    https://github.com/hpssjellis/my-examples-for-the-arduino-portentaH7/tree/master/m09-Tensoflow
 *  
 *  
 *  Hashtags
 *  #TensorflowLiteMicro
 *  #TensorflowLiteForArduino
 *  #TinyMY
 *  #MakerML
 */





#include "Arduino.h"
#include <TensorFlowLite.h>
//#include "model.h"
#include "rocksetta.h"

// Needed for SDRAM assignment on Arduino Portenta
#include <SDRAM.h>
#define ALIGN_PTR(p,a)   ((p & (a-1)) ?(((uintptr_t)p + a) & ~(uintptr_t)(a-1)) : p)
SDRAMClass mySDRAM;


unsigned int model_tflite_len = 2640;


// define SDRAM pointer
unsigned char *sdram_mem;
unsigned char *sdram_tflite; // 32-byte aligned




/*================= Start Maker Area ======================================*/


int myCounter = 0;
int myLed = LED_BUILTIN;   //or 7 or 5 or LED_BUILTIN or LEDB

void setup() {
  
   Serial.begin(115200);
   mySDRAM.begin(SDRAM_START_ADDRESS);   

   // setup SDRAM memory block
   sdram_mem = (unsigned char *) SDRAM.malloc(4 + 32 /*alignment*/);
   sdram_tflite = (unsigned char *)ALIGN_PTR((uintptr_t)sdram_mem, 32);


   memcpy(sdram_tflite, "0x18/x00/x00/x00/x54/x46/x4c/x33/x00/x00/x0e/x000x18/x00/x04/x00/x08/x00/x0c/x00/x10/x00/x14/x000x0e/x00/x00/x00/x03/x00/x00/x00/x10/x0a/x00/x000xb8/x05/x00/x00/xa0/x05/x00/x00/x04/x00/x00/x000x0b/x00/x00/x00/x90/x05/x00/x00/x7c/x05/x00/x000x24/x05/x00/x00/xd4/x04/x00/x00/xc4/x00/x00/x000x74/x00/x00/x00/x24/x00/x00/x00/x1c/x00/x00/x000x14/x00/x00/x00/x0c/x00/x00/x00/x04/x00/x00/x000x54/xf6/xff/xff/x58/xf6/xff/xff/x5c/xf6/xff/xff0x60/xf6/xff/xff/xc2/xfa/xff/xff/x04/x00/x00/x000x40/x00/x00/x00/xd0/x37/xed/xbd/xb6/x38/xf9/x3e0xc6/x9d/x00/x3f/x1a/xb0/x05/xbe/xc6/x49/xd9/xbe0x55/x62/x8b/xbe/xfa/xa9/x86/x3e/xc4/x4d/x9c/x3e0x85/x7c/x8d/xbe/x44/xf3/x2c/xbe/xb5/x07/x56/x3e0xb3/x84/x49/x3e/x60/x35/xa8/xbc/x60/xbc/xcf/xbe0x23/x52/xff/x3e/xc6/xaf/x39/xbe/x0e/xfb/xff/xff0x04/x00/x00/x00/x40/x00/x00/x00/x00/x00/x00/x000x36/xec/xe2/x3d/x87/xea/x53/xbf/x00/x00/x00/x000x00/x00/x00/x00/x00/x00/x00/x00/x17/xef/xfb/x3e0x5a/x6f/x2b/xbe/x00/x00/x00/x00/x00/x00/x00/x000x1b/x1f/x12/xbf/xb4/xef/xec/xbd/x00/x00/x00/x000x00/x00/x00/x00/x2e/x32/x8f/xbf/x00/x00/x00/x000x5a/xfb/xff/xff/x04/x00/x00/x00/x00/x04/x00/x000xdd/xec/x8d/xbe/x42/xef/x55/x3d/xdd/xdd/x18/x3e0x6b/x54/xaa/x3e/xa5/xa5/xa6/x3e/xf4/x12/xe7/xbd0x06/x6a/x4b/xbe/xf7/x32/x14/x3e/xa4/xe2/xb0/xbe0x0c/x83/xe1/x3d/x88/xf6/xf1/x3e/x6b/x71/x0e/x3c0xde/xed/x77/x3e/x92/x91/x23/x3e/x08/xb4/x1b/xbe0x9d/x9b/xa8/xbe/x77/x35/x9e/xbe/x42/x29/x20/xbf0x07/x20/x45/x3e/x3e/x92/xc3/xbe/x36/xca/x10/xbe0xef/x96/xc0/xbe/x8c/xa8/xcf/x3d/xe5/xbe/xfc/x3d0x44/x5a/xf8/x3d/x92/x68/xd4/xbe/x3d/x18/x8f/x3e0xae/x9a/x45/x3d/x1b/x8b/xb8/xbe/x40/x3d/x8c/xbd0x2f/x90/x2d/xbf/x8a/x81/x1a/x3e/x04/x8f/x5c/xbe0x4c/xb8/xe1/xbd/x62/x66/x59/xbe/xe6/xb4/xb4/xbe0xe6/x74/x3f/x3e/xfc/x40/xaf/x3d/x25/x72/x50/x3e0x47/xec/xcc/x3e/x86/x9e/x70/x3e/x2a/x3b/x67/x3e0xad/xf5/xcf/xbc/x30/x4d/x0d/x3e/xfc/xb9/xd0/x3d0xcf/xa8/xc0/xbe/x7c/x8e/xe8/x3e/x18/x76/x14/xbe0x76/x3f/x84/xbe/x4c/xd6/x46/x3d/x49/x07/xca/x3e0x90/x87/xa0/xbd/xa5/x7d/xdd/xbe/xe5/x4a/xa5/x3e0x0d/x6e/xff/xbd/xbc/x53/xb3/xbe/x57/x58/x92/x3e0x29/x9e/x91/x3e/x24/x25/xd5/x3e/xe5/x06/x07/xbe0x2a/xd4/xb8/xbe/xcc/xe4/x02/xbe/x68/x7b/x95/x3e0xc1/x45/xcc/xbe/x4b/xb3/x82/x3e/x25/xae/x00/xbf0x29/xdc/xb4/xbe/xfe/x09/x60/x3e/x0f/x43/xc7/x3e0xc4/xf8/xcd/xbd/x1d/x74/xc1/xbe/xf4/xc8/x5c/x3e0xe6/xaf/x0b/x3e/x98/x01/xa6/xbd/x96/xb4/x90/xbe0x78/x41/xc3/xbe/xfd/x30/xc1/x3e/x15/x7f/xcb/x3e0xb3/x97/x0a/x3e/x97/x4d/xa9/x3e/x2f/x97/xa5/x3e0x24/x1c/xea/x3d/x6a/x7e/x39/x3e/x83/x3b/x61/xbe0xd8/x55/x6d/x3d/xe1/x22/xd9/x3e/xc0/x09/x88/xbe0x42/x55/xdb/xbe/xfa/x71/x73/x3e/x0d/x35/x88/xbe0xf1/x67/xc9/x3e/xa0/x1f/x76/xbc/x81/xe0/xa0/x3e0xc0/x11/x1c/x3d/x04/x30/x6a/xbe/x2a/x9c/x4c/x3e0x1f/x99/xad/x3e/x3f/xe9/x5a/x3d/xee/xaa/xce/x3e0xd1/x44/xc6/x3e/x9b/xaa/x57/xbe/x43/x1f/x8f/xbe0x34/x8a/x9a/xbd/xbd/x92/x6e/x3e/xc1/xc1/x3c/xbe0x1f/x30/x43/xbe/x45/xe6/xb5/xbc/xce/xa5/x93/xbe0x56/xea/x1a/xbe/x47/x05/xab/x3e/xc0/xd0/xcd/x3e0x25/x60/x8d/x3e/xb6/xf2/x50/x3e/x93/x4c/x09/x3d0xca/x36/x9d/x3e/xcf/xd3/xa0/xbe/xfa/x86/x00/x3e0xe0/xfb/x8c/xbd/xb2/x8a/xbf/x3e/xc3/x70/xa9/xbe0x21/x43/x84/x3e/x5a/x16/x16/x3e/xc4/x0b/x28/x3d0xb9/x09/x51/xbe/xda/xcc/x04/xbe/xd9/x4b/x8a/x3e0x8f/x22/x93/x3d/x72/xfe/x2d/xbe/x8e/x0d/x13/xbe0xa1/x47/xe4/xbe/x03/x52/xe0/x3e/xd2/xdc/x5c/xbe0x63/xab/x83/x3e/x14/x07/x92/x3d/xe8/xa4/x7e/x3d0x16/xc6/x92/x3e/xd4/x76/x95/xbe/xf0/xdc/xc4/xbd0xec/x5e/x74/x3e/x0c/x08/xc7/x3e/x66/xc0/x9c/xbe0x6c/xf6/xdd/xbd/x77/x7e/x74/x3e/xc0/xfb/xdd/x3b0x7e/x00/x08/xbe/xa0/x2e/x9f/xbc/xe0/xc5/xbf/x3c0x22/xcc/x95/xbe/x00/x90/x80/x3a/x48/x39/x5c/xbd0x02/x1c/x06/xbe/x60/x32/xdd/xbc/x9c/xfb/x99/xbe0xb0/xc2/x0d/xbd/x28/x33/x42/xbd/x90/x1f/x82/xbd0x09/xd4/xd9/x3e/xa4/x79/xf0/x3d/xe6/x5b/x21/xbe0x6d/xa2/xbe/xbe/xae/x26/xc9/xbe/x76/x50/xeb/x3d0x5e/x01/x86/x3e/x67/xbd/xdc/x3e/x78/x38/x8a/xbe0x74/xc3/xec/x3d/x70/xc9/xd0/x3e/xc3/xfe/x5a/xbd0xd8/x69/x31/xbd/x3b/xdb/x7f/xbe/x12/x35/x58/xbe0xb0/x3a/x81/x3e/xcc/xa0/x17/xbe/x32/xad/x48/x3e0x1f/x35/x8d/x3e/xec/xfe/xd4/x3d/xef/x33/x7b/xbe0xce/x81/x9e/xbe/x02/x7a/x5a/x3e/xa7/xb6/xd5/xbe0xae/x04/x0c/x3e/x95/x40/xd1/x3e/x87/xfe/x1b/x3e0x61/xf3/x82/xbe/x00/xcd/x3c/xbd/x03/x7d/x8b/x3e0x20/xfd/xcb/x3c/x80/x33/x99/xbc/x09/xad/x68/xbe0x7a/xff/x75/x3e/x11/xba/x6d/xbe/x30/xe3/x31/x3d0x00/xfa/xc1/xbd/x9f/x38/xa3/xbe/x97/x88/x28/xbe0x37/x72/x3c/xbe/x58/xc6/x86/xbd/x62/xdf/x70/x3e0xb5/xef/xb8/x3c/x16/x67/x87/x3e/xce/x77/xb4/xbe0x20/x52/x64/x3d/xa8/xcc/xbc/xbd/x17/x62/x49/xbb0x42/x13/x49/x3e/xa6/x0a/x14/x3e/x95/xc9/x37/xbe0x8c/x10/xb2/xbe/xe1/x11/x6f/xbe/x46/x45/x73/x3e0xf0/x26/xbc/xbe/xc0/x4f/x88/xbb/x39/x0a/xbb/x3e0x0f/x32/x88/x3e/xf0/x81/x99/x3e/x3f/xd5/x8c/xbe0x10/x59/x4f/xbd/x63/x85/xb6/x3e/x6a/xe4/x98/xbf0x6b/x1a/xa8/xbe/xc8/x77/xb5/xbe/x80/xea/x3f/xbd0x6d/x91/xac/xbe/xd2/xdd/x63/xbe/x41/xcb/x59/xbe0xb6/x0f/x94/xbe/x45/x12/x89/x3e/x4f/x6f/xb8/x3e0x08/xa5/x02/xbd/x12/x74/xa8/xbe/x56/x2f/xa6/xbe0x9e/xd8/x11/xbd/x3e/xdd/x67/x3e/x3e/xa8/x1a/x3e0x81/x8c/x9c/x3e/x9f/x5b/x96/x3d/x9c/x65/xca/x3d0xb4/x3c/xd2/xbe/x91/x2a/x45/x3e/x63/x53/x9c/x3e0x99/x85/x42/xbe/xfc/xaf/x04/xbe/x12/xd7/x88/xbe0xfa/xc2/xc5/xbe/x71/x2f/x96/x3e/xe5/x0b/x93/x3e0xb8/x85/xf4/x3d/x37/x48/xcd/xbe/xa1/xc8/x63/xbe0x5a/xa0/x3c/x3e/x4d/x84/x1e/x3b/x79/x40/x50/x3e0xcb/xb1/xd0/x3e/xe1/x10/xcd/x3e/x7f/x3c/xcd/x3e0xb8/xda/x37/xbe/x66/xff/xff/xff/x04/x00/x00/x000x40/x00/x00/x00/x9c/x4f/xd6/xbe/x7d/xdb/xf3/x3e0x6b/x2b/xaa/xbe/xc1/x26/x26/xbf/xee/x03/x1f/x3f0x00/x00/x00/x00/x37/xab/xa7/xbe/xd0/x10/x5b/x3e0xe5/xbc/xca/xbe/x00/x00/x00/x00/x94/x4a/x48/x3e0xc3/x7d/x2f/xbd/x27/xef/x9b/xbd/x32/x58/x48/x3f0xfc/x3b/xda/xbb/x18/xfd/x60/xbd/xb2/xff/xff/xff0x04/x00/x00/x00/x40/x00/x00/x00/xa8/xde/x92/x390x4a/x26/x8b/xbf/xb9/x79/x8c/x3e/xc1/xb8/x1e/x400xde/x0b/x9d/xbf/x0e/xe2/x61/xbe/xdc/x1b/xb1/x3e0x47/x15/x60/xbe/x8c/x3c/x06/x3f/xfc/x69/xcf/x3e0xd0/x07/x15/xbf/xa7/x92/x25/x3e/xf6/xf8/xbc/x3e0x31/xe1/xd4/x3f/x3f/xf6/x02/x3f/x78/xf2/x02/x3d0x00/x00/x06/x00/x08/x00/x04/x00/x06/x00/x00/x000x04/x00/x00/x00/x04/x00/x00/x00/xaf/x96/x93/xbe0xb8/xfb/xff/xff/x0f/x00/x00/x00/x54/x4f/x43/x4f0x20/x43/x6f/x6e/x76/x65/x72/x74/x65/x64/x2e/x000x01/x00/x00/x00/x10/x00/x00/x00/x0c/x00/x14/x000x04/x00/x08/x00/x0c/x00/x10/x00/x0c/x00/x00/x000xf0/x00/x00/x00/xe4/x00/x00/x00/xd8/x00/x00/x000x04/x00/x00/x00/x03/x00/x00/x00/x90/x00/x00/x000x48/x00/x00/x00/x04/x00/x00/x00/xce/xff/xff/xff0x00/x00/x00/x08/x18/x00/x00/x00/x0c/x00/x00/x000x04/x00/x00/x00/x1c/xfc/xff/xff/x01/x00/x00/x000x00/x00/x00/x00/x03/x00/x00/x00/x07/x00/x00/x000x08/x00/x00/x00/x09/x00/x00/x00/x00/x00/x0e/x000x14/x00/x00/x00/x08/x00/x0c/x00/x07/x00/x10/x000x0e/x00/x00/x00/x00/x00/x00/x08/x1c/x00/x00/x000x10/x00/x00/x00/x04/x00/x00/x00/xba/xff/xff/xff0x00/x00/x00/x01/x01/x00/x00/x00/x07/x00/x00/x000x03/x00/x00/x00/x04/x00/x00/x00/x05/x00/x00/x000x06/x00/x00/x00/x00/x00/x0e/x00/x16/x00/x00/x000x08/x00/x0c/x00/x07/x00/x10/x00/x0e/x00/x00/x000x00/x00/x00/x08/x24/x00/x00/x00/x18/x00/x00/x000x0c/x00/x00/x00/x00/x00/x06/x00/x08/x00/x07/x000x06/x00/x00/x00/x00/x00/x00/x01/x01/x00/x00/x000x04/x00/x00/x00/x03/x00/x00/x00/x01/x00/x00/x000x02/x00/x00/x00/x03/x00/x00/x00/x01/x00/x00/x000x00/x00/x00/x00/x01/x00/x00/x00/x01/x00/x00/x000x0a/x00/x00/x00/x10/x03/x00/x00/xa4/x02/x00/x000x40/x02/x00/x00/xf4/x01/x00/x00/xac/x01/x00/x000x48/x01/x00/x00/xfc/x00/x00/x00/xb4/x00/x00/x000x50/x00/x00/x00/x04/x00/x00/x00/x26/xfd/xff/xff0x3c/x00/x00/x00/x01/x00/x00/x00/x0c/x00/x00/x000x04/x00/x00/x00/x18/xfd/xff/xff/x20/x00/x00/x000x73/x65/x71/x75/x65/x6e/x74/x69/x61/x6c/x5f/x310x2f/x64/x65/x6e/x73/x65/x5f/x34/x2f/x4d/x61/x740x4d/x75/x6c/x5f/x62/x69/x61/x73/x00/x00/x00/x000x01/x00/x00/x00/x01/x00/x00/x00/x6e/xfd/xff/xff0x50/x00/x00/x00/x02/x00/x00/x00/x0c/x00/x00/x000x04/x00/x00/x00/x60/xfd/xff/xff/x34/x00/x00/x000x73/x65/x71/x75/x65/x6e/x74/x69/x61/x6c/x5f/x310x2f/x64/x65/x6e/x73/x65/x5f/x34/x2f/x4d/x61/x740x4d/x75/x6c/x2f/x52/x65/x61/x64/x56/x61/x72/x690x61/x62/x6c/x65/x4f/x70/x2f/x74/x72/x61/x6e/x730x70/x6f/x73/x65/x00/x00/x00/x00/x02/x00/x00/x000x01/x00/x00/x00/x10/x00/x00/x00/xce/xfd/xff/xff0x34/x00/x00/x00/x08/x00/x00/x00/x0c/x00/x00/x000x04/x00/x00/x00/xc0/xfd/xff/xff/x19/x00/x00/x000x73/x65/x71/x75/x65/x6e/x74/x69/x61/x6c/x5f/x310x2f/x64/x65/x6e/x73/x65/x5f/x33/x2f/x52/x65/x6c0x75/x00/x00/x00/x02/x00/x00/x00/x01/x00/x00/x000x10/x00/x00/x00/x12/xfe/xff/xff/x3c/x00/x00/x000x03/x00/x00/x00/x0c/x00/x00/x00/x04/x00/x00/x000x04/xfe/xff/xff/x20/x00/x00/x00/x73/x65/x71/x750x65/x6e/x74/x69/x61/x6c/x5f/x31/x2f/x64/x65/x6e0x73/x65/x5f/x33/x2f/x4d/x61/x74/x4d/x75/x6c/x5f0x62/x69/x61/x73/x00/x00/x00/x00/x01/x00/x00/x000x10/x00/x00/x00/x5a/xfe/xff/xff/x50/x00/x00/x000x04/x00/x00/x00/x0c/x00/x00/x00/x04/x00/x00/x000x4c/xfe/xff/xff/x34/x00/x00/x00/x73/x65/x71/x750x65/x6e/x74/x69/x61/x6c/x5f/x31/x2f/x64/x65/x6e0x73/x65/x5f/x33/x2f/x4d/x61/x74/x4d/x75/x6c/x2f0x52/x65/x61/x64/x56/x61/x72/x69/x61/x62/x6c/x650x4f/x70/x2f/x74/x72/x61/x6e/x73/x70/x6f/x73/x650x00/x00/x00/x00/x02/x00/x00/x00/x10/x00/x00/x000x10/x00/x00/x00/xba/xfe/xff/xff/x34/x00/x00/x000x0a/x00/x00/x00/x0c/x00/x00/x00/x04/x00/x00/x000xac/xfe/xff/xff/x19/x00/x00/x00/x73/x65/x71/x750x65/x6e/x74/x69/x61/x6c/x5f/x31/x2f/x64/x65/x6e0x73/x65/x5f/x32/x2f/x52/x65/x6c/x75/x00/x00/x000x02/x00/x00/x00/x01/x00/x00/x00/x10/x00/x00/x000xfe/xfe/xff/xff/x3c/x00/x00/x00/x05/x00/x00/x000x0c/x00/x00/x00/x04/x00/x00/x00/xf0/xfe/xff/xff0x20/x00/x00/x00/x73/x65/x71/x75/x65/x6e/x74/x690x61/x6c/x5f/x31/x2f/x64/x65/x6e/x73/x65/x5f/x320x2f/x4d/x61/x74/x4d/x75/x6c/x5f/x62/x69/x61/x730x00/x00/x00/x00/x01/x00/x00/x00/x10/x00/x00/x000x46/xff/xff/xff/x50/x00/x00/x00/x06/x00/x00/x000x0c/x00/x00/x00/x04/x00/x00/x00/x38/xff/xff/xff0x34/x00/x00/x00/x73/x65/x71/x75/x65/x6e/x74/x690x61/x6c/x5f/x31/x2f/x64/x65/x6e/x73/x65/x5f/x320x2f/x4d/x61/x74/x4d/x75/x6c/x2f/x52/x65/x61/x640x56/x61/x72/x69/x61/x62/x6c/x65/x4f/x70/x2f/x740x72/x61/x6e/x73/x70/x6f/x73/x65/x00/x00/x00/x000x02/x00/x00/x00/x10/x00/x00/x00/x01/x00/x00/x000xa6/xff/xff/xff/x48/x00/x00/x00/x09/x00/x00/x000x2c/x00/x00/x00/x0c/x00/x00/x00/x08/x00/x0c/x000x04/x00/x08/x00/x08/x00/x00/x00/x10/x00/x00/x000x04/x00/x00/x00/x01/x00/x00/x00/x00/x00/x7f/x430x01/x00/x00/x00/x00/x00/x00/x00/x0d/x00/x00/x000x64/x65/x6e/x73/x65/x5f/x32/x5f/x69/x6e/x70/x750x74/x00/x00/x00/x02/x00/x00/x00/x01/x00/x00/x000x01/x00/x00/x00/x00/x00/x0e/x00/x14/x00/x04/x000x00/x00/x08/x00/x0c/x00/x10/x00/x0e/x00/x00/x000x28/x00/x00/x00/x07/x00/x00/x00/x10/x00/x00/x000x08/x00/x00/x00/x04/x00/x04/x00/x04/x00/x00/x000x08/x00/x00/x00/x49/x64/x65/x6e/x74/x69/x74/x790x00/x00/x00/x00/x02/x00/x00/x00/x01/x00/x00/x000x01/x00/x00/x00/x01/x00/x00/x00/x10/x00/x00/x000x00/x00/x0a/x00/x0c/x00/x07/x00/x00/x00/x08/x000x0a/x00/x00/x00/x00/x00/x00/x09/x03/x00/x00/x00", model_tflite_len);



  
  pinMode(myLed, OUTPUT);
 // modelSetup(model_tflite);  // name of the model in the tab model.h
  modelSetup(*&sdram_tflite);  // name of the model in the tab model.h
  
}


void loop() {
   myCounter +=1;
    if (myCounter >= 360){
        myCounter = 0;
    }
    
    float x_val = myCounter * 3.14/180;  // degrees to radians
    float y_val = sin(x_val);

    // This uses your model to make a prediction
    float predicted = modelPredict(x_val);

    Serial.println("sin(" +String(x_val)+ ") = " + String(y_val) + "\t predicted: " + String(predicted) );


    // y=1 LED is fully on. The LED's brightness can range from 0-255.
    int brightness = (int)(127.5f * (predicted+1));

     #if  defined (CORE_CM7)  ||  defined (CORE_CM4)

         if (brightness <= 128){
              digitalWrite(myLed, HIGH);  // means off
         } else {
               digitalWrite(myLed, LOW);  // means on             
         }
     #else
         analogWrite(myLed, brightness);  // not on Porttenta
     #endif 



    delay(3); // slows the process down a bit to see the sine wave

  
}


/*================= End Maker Area ======================================*/
