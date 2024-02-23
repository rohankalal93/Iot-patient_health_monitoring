// Load libraries
#include "BluetoothSerial.h"

// Check if Bluetooth configs are enabled
#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
#error Bluetooth is not enabled! Please run `make menuconfig` to and enable it
#endif

// Bluetooth Serial object
BluetoothSerial SerialBT;

int in = A0;
void setup() {

  Serial.begin(115200);
  pinMode(in,INPUT);
  // Bluetooth device name
  SerialBT.begin("ESP32");
  Serial.println("The device started, now you can pair it with bluetooth!");
}

void loop() {

  byte val = map(analogRead(in),0,4096,0,255);    
  SerialBT.write(val);
  // if above line doesnot work then comment above line and uncomment following line
  //SerialBT.print(val);                                  
  delay(1);
}
