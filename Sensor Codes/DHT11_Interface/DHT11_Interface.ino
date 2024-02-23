#include "DHT.h"
#define DHTPIN 4
//our sensor is DHT11 type
#define DHTTYPE DHT11
//create an instance of DHT sensor
DHT dht(DHTPIN, DHTTYPE);
 
void setup()
{
Serial.begin(115200);
//call begin to start sensor
dht.begin();
}
 
void loop() {
//use the functions which are supplied by library.
float h = dht.readHumidity();
// Read temperature as Celsius (the default)
float t = dht.readTemperature();
}
// print the result to Terminal
Serial.print("Humidity: ");
Serial.print(h);
Serial.print(" %\t");
Serial.print("Temperature: ");
Serial.print(t);
Serial.println(" *C ");
//we delay a little bit for next read
delay(2000);
}
