#include <OneWire.h>
#include <DallasTemperature.h>

// GPIO where the DS18B20 is connected to
const int oneWireBus = 15;     

// Setup a oneWire instance to communicate with any OneWire devices
OneWire oneWire(oneWireBus);

// Pass our oneWire reference to Dallas Temperature sensor 
DallasTemperature sensors(&oneWire);

void setup() {
  // Start the Serial Monitor
  Serial.begin(115200);
  // Start the DS18B20 sensor
  sensors.begin();
}

void loop() {
  sensors.requestTemperatures(); 
  float BTC = sensors.getTempCByIndex(0);
  float BTF = sensors.getTempFByIndex(0);
  Serial.print("Temp in Cent: ");
  Serial.print(BTC);
  Serial.println("ºC");
  Serial.print("Temp in Ferh: ");
  Serial.print(BTF);
  Serial.println("ºF");
  delay(2000);
}
