#include <stdint.h>
#include <Wire.h>
#include "MAX30100_PulseOximeter.h"  //used arduino builtin MAX30100 lib (https://github.com/oxullo/Arduino-MAX30100)
#include "DHT.h"
#include <OneWire.h>
#include <DallasTemperature.h>
#include <WiFi.h>
#include <HTTPClient.h>

#define I2C_SDA   21
#define I2C_SCL   22
TaskHandle_t poxReadTaskHld = NULL;
TaskHandle_t mqttPubTaskHld = NULL;
// PulseOximeter is the higher-level interface to the sensor
PulseOximeter pox;
uint32_t tsLastReport = 0;
float bpm_dt=0;
float spo2_dt = 0;

#define DHTPIN 4     // Digital pin connected to the DHT sensor
#define DHTTYPE DHT11   //  type of dht DHT11 or DHT22
DHT dht(DHTPIN, DHTTYPE); // Initialize DHT sensor.

// GPIO where the DS18B20 is connected to
const int oneWireBus = 15;     
// Setup a oneWire instance to communicate with any OneWire devices
OneWire oneWire(oneWireBus);
// Pass our oneWire reference to Dallas Temperature sensor 
DallasTemperature sensors(&oneWire);


const char* ssid = "pravin";
const char* password = "abcd1234";

// Domain Name with full URL Path for HTTP POST Request
const char* serverName = "http://api.thingspeak.com/update";
// Service API Key
String apiKey = "KPQ0YS2GFFOG22BR";

const char* serverName2 = "http://maker.ifttt.com/trigger/HighBodyTemp/with/key/tCoXqFi-625LzQ6-jURMIcWveoW5tsnpIAJzng0Kn3";
boolean action = true;


void onBeatDetected()
{
                Serial.println("Beat!");
}
/******************************************* Function for pause MAX30100 Read **************************************************/
void stopReadPOX(){
  pox.shutdown();
}
/******************************************* Function for Start MAX30100 Read **************************************************/
void startReadPOX(){
  pox.resume();
}
/******************************************* MAX30100 Read task **************************************************/
void poxReadTask( void * param )
{
  while(1){
                // Make sure to call update as fast as possible
                pox.update();
                
                vTaskDelay( 1 / portTICK_PERIOD_MS );
  }
  poxReadTaskHld = NULL;
  vTaskDelete(NULL); // kill itself
}


/******************************************* SETUP **************************************************/
void setup()
{
                Serial.begin(115200);
                Wire.begin(I2C_SDA, I2C_SCL);
                Serial.print("Initializing pulse oximeter..");
                // Initialize the PulseOximeter instance
                // Failures are generally due to an improper I2C wiring, missing power supply
                // or wrong target chip
                if (!pox.begin()) {
                Serial.println("FAILED");
                for(;;);
                } else {
                Serial.println("SUCCESS");
                }
                // The default current for the IR LED is 50mA and it could be changed
                //   by uncommenting the following line. Check MAX30100_Registers.h for all the
                //   available options.
                pox.setIRLedCurrent(MAX30100_LED_CURR_7_6MA);
                // Register a callback for the beat detection
                //pox.setOnBeatDetectedCallback(onBeatDetected);
                stopReadPOX();

                // Initialize the DHT11 instance
                dht.begin();
                
                // Initialize the DS18B20 temp sensor instance
                sensors.begin();

                // Initialize the wifi instance
                WiFi.begin(ssid, password);

                
}

/******************************************* LOOP **************************************************/
void loop() {
  
  
                Serial.println(("Starting Sensors... "));
                startReadPOX();
                BaseType_t xReturned;
                if(poxReadTaskHld == NULL){
                xReturned = xTaskCreate(
                                poxReadTask,      /* Function that implements the task. */
                                "pox_read",           /* Text name for the task. */
                                1024*3,                  /* Stack size in words, not bytes. */
                                NULL,    /* Parameter passed into the task. */
                                2,/* Priority at which the task is created. */
                                &poxReadTaskHld );          /* Used to pass out the created task's handle. */
                }
                
                float BPM = pox.getHeartRate();
                float SPO2 = pox.getSpO2();

                // comment following lines if you dont want serial monitor data
                Serial.print("Heart rate:");
                Serial.print(BPM);
                Serial.println("bpm");
                Serial.print("SpO2:");
                Serial.print(SPO2);
                Serial.println("%");

                
                // Reading temperature or humidity takes about 250 milliseconds!
                // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
                float RH = dht.readHumidity();
                // Read temperature as Celsius (the default)
                float RTC = dht.readTemperature();
                // Read temperature as Fahrenheit (isFahrenheit = true)
                float RTF = dht.readTemperature(true);
                // comment following lines if you dont want serial monitor data
                Serial.print("Humidity: ");
                Serial.print(RH);
                Serial.println("%");
                Serial.print("Temperature: ");
                Serial.print(RTC);
                Serial.print("°C ");
                Serial.print(RTF);
                Serial.println("°F");

                // Reading temperature from ds18b20
                sensors.requestTemperatures();
                // Read temperature as Celsius 
                float BTC = sensors.getTempCByIndex(0);
                // Read temperature as Fahrenheit
                float BTF = sensors.getTempFByIndex(0);
                // comment following lines if you dont want serial monitor data
                Serial.print("Body Temperature: ");
                Serial.print(BTC);
                Serial.print(" ºC   ");
                Serial.print(BTF);
                Serial.println(" ºF");

                if(WiFi.status()== WL_CONNECTED){
                    HTTPClient http;
      
                    // connect to thingspeak server
                    http.begin(serverName);
      
                    // Specify content-type header
                    http.addHeader("Content-Type", "application/x-www-form-urlencoded");
                    // Data to send with HTTP POST
                    String httpRequestData = "api_key=" + apiKey + "&field1=" + String(BPM) + "&field2=" + String(SPO2) + "&field3=" + String(RH)+ "&field4=" + String(RTF) + "&field5=" + String(BTF);           
                    // Send HTTP POST request
                    int httpResponseCode = http.POST(httpRequestData);

                    http.end();
                 }

                if (BTF >= 100 && action == true){
                  
                  if(WiFi.status()== WL_CONNECTED){
                    HTTPClient http;
                    http.begin(serverName2);
                    // Specify content-type header
                    http.addHeader("Content-Type", "application/x-www-form-urlencoded");
                    // Data to send with HTTP POST
                    String httpRequestData = "value1=" + String(BTF);           
                    // Send HTTP POST request
                    int httpResponseCode = http.POST(httpRequestData);
                    http.end();
                    action = false;
                }
                if (BTF < 100){
                  action = true;
                }
                }
                else 
                {
                  Serial.println("WiFi Disconnected");
                }
                // this delay can be changed to get results continuously
                delay(1000);
                Serial.println(("Stoping Sensors... "));
                
                // Detele POX read task
                if(poxReadTaskHld != NULL){
                vTaskDelete(poxReadTaskHld);
                poxReadTaskHld = NULL;
                }
                stopReadPOX();
                
  }
                
