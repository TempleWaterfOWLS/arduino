#include <SoftwareSerial.h>
SoftwareSerial mySerial(3,2);
void setup() {
  mySerial.begin (9600);
  Serial.begin(9600);
}
 
void loop() {
  mySerial.println ("hello Linksprite!");
  Serial.println ("hello Linksprite!");
  delay(1000);
}
