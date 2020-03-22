#include "ct_AIS_BG96.h"


void setup() {
  Serial.begin(9600);
  while(!Serial);
  
  
  Serial.println("\r\n########## AIS eMTC Module Started! ##########");

  initModule();
  openConnection("aistest.emtc", "UDP", "178.128.16.9", 41234);
  pingServer("178.128.16.9");


}


void loop() {
  if (pingServer("178.128.16.9") == 0) {
    Serial.println("Ping success");
  };
  delay(5000);
}
