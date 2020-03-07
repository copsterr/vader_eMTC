#include "ct_AIS_BG96.h"


void setup() {
  Serial.begin(9600);
  while(!Serial);
  
  initModule();
  Serial.println("\r\n########## AIS eMTC Module Started! ##########");

  if (checkModule() == 0) Serial.println("Shiled OK.");
  if (requestIMEI() != 0) Serial.println("Cannot Request IMEI.");
  if (setPhoneFunc(1) == 0) Serial.println("Set phone completed.");
  if (signalQualityReport() != 0) Serial.println("ERROR Cannot Read Signal Quality");
  if (cgatt(1) == 0) Serial.println("cgatt ok.");
  if (showPdpAddr() == 0) Serial.println("PDP ADDR ok.");
  if (configTCPcontext(1, 1, "aistest.emtc", "", "", 0) == 0) Serial.println("config TCP context ok.");
  if (deactivatePDP(1) == 0) Serial.println("PDP deactivated.");
  if (activatePDP(1, 1, 1) == 0) Serial.println("PDP activated.");

  openSocketService(1,0,"UDP","178.128.16.9",41234);
  closeSocketService(0);
}


void loop() {
  
}
