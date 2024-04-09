#include <Encoder.h>

#define ENCA 3
#define ENCB 4

Encoder myEnc(ENCA, ENCB); // avec aduiuno nano seul 2,3 sont interupts
int pos = 0;
int count =0;
void setup() {
  Serial.begin(115200);
  Serial.println("Basic Encoder Test:");
  attachInterrupt(digitalPinToInterrupt(ENCA),ReadEnc,RISING);
}



void loop() {
  
  Serial.println(pos);
  
  }


 void ReadEnc(){

  pos += myEnc.read();
  
  }
