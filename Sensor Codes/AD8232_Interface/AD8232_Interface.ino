#define ecgpin A0

void setup() {
  // initialize the serial communication:
  Serial.begin(115200);
  //pinMode(34, INPUT); // Setup for leads off detection LO +
  //pinMode(35, INPUT); // Setup for leads off detection LO -
  pinMode(ecgpin, INPUT);
 
}
 
void loop() {
  
  //if((digitalRead(34) == 1)||(digitalRead(35) == 1)){
  //  Serial.println("No Signal");
  //}
  //else{
    // send the value of analog input 0:
      Serial.println(analogRead(SENSORPIN));
  //}
  //Wait for a bit to keep serial data from saturating
  delay(1);
}
