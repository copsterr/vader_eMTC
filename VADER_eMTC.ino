#include <AltSoftSerial.h>

AltSoftSerial mySerial;

void setup() {
  Serial.begin(9600);
  while(!Serial);
  Serial.println("\r\n########## AIS eMTC Module Started! ##########");

  mySerial.begin(9600);
  mySerial.setTimeout(1000);

  if (checkModule() == 0) Serial.println("Shiled OK.");
  if (requestIMEI() != 0) Serial.println("Cannot Request IMEI.");
  if (setPhoneFunc(1) == 0) Serial.println("Set phone completed.");
  if (signalQualityReport() != 0) Serial.println("ERROR Cannot Read Signal Quality");
}

void loop() {
  
}

/* User Defined Functions ---------- */
void printSerialDebug(void) {
  delay(1000);
  while (mySerial.available()) {
    Serial.print(mySerial.read(), HEX);
    Serial.print(" ");
  }
}

/* General Commands ----------------- */

int8_t checkModule(void) {
  String resp = "";
  
  mySerial.write("AT\r\n");
  mySerial.flush();
  mySerial.readStringUntil('\n'); // read echo
  resp = mySerial.readStringUntil('\n'); // read response
  
  if (resp.equals("OK\r") == true) {
    return 0; // success
  }

  return -1; // error
}

int8_t requestIMEI(void) {
  String imei = "";
  String resp = "";

  mySerial.write("AT+GSN\r\n");
  mySerial.flush();

  mySerial.readStringUntil('\n'); // read echo
  
  imei = mySerial.readStringUntil('\r'); // read IMEI
  resp = mySerial.readString(); // read response
  
  if (resp.equals("\n\r\nOK\r\n") == true) {
      Serial.print("IMEI -> "); Serial.println(imei);
      return 0;
  }
  
  return -1;
}

int8_t setPhoneFunc(uint8_t mode) {
  String resp = "";
  char char_mode = 0;

  if (mode == 0) { // minimum functionality
    char_mode = '0';
  }
  else if (mode == 1) { // full functionality
    char_mode = '1';
  }
  else if (mode == 4) { // disable RF transceive
    char_mode = '4';
  }
  else {
    return -1;
  }
 
  mySerial.write("AT+CFUN=");
  mySerial.write(char_mode);
  mySerial.write("\r\n");
  mySerial.flush();

  mySerial.readStringUntil('\n'); // read echo
  resp = mySerial.readString(); // read response
  
  if (resp.equals("OK\r\n") == true) {
    return 0;
  }

  return -1;
}

int8_t signalQualityReport(void) {
  String resp_head = "";
  String rssi      = "";
  String ber       = "";
  String resp      = "";

  mySerial.write("AT+CSQ\r\n"); mySerial.flush(); // transmit command
  mySerial.readStringUntil('\n'); // read echo

  resp_head = mySerial.readStringUntil(' '); // read response header
  if (resp_head.equals("+CME ERROR:")) {
    return -1;
  }

  rssi = mySerial.readStringUntil(',');
  ber  = mySerial.readStringUntil('\r');
  resp = mySerial.readString();

  if (resp.equals("\n\r\nOK\r\n")) {
    uint8_t rssi_int = rssi.toInt();
    int8_t  signal_strength = -113;

    // calculate dBm
    if (rssi_int >= 0 && rssi_int <= 31) {
      signal_strength = signal_strength + 2*rssi_int;
      rssi = String(signal_strength);
    }
    else if (rssi_int == 99) {
      rssi = "Not known or Undetectable";
    }

    Serial.print("rssi -> "); Serial.print(rssi); Serial.print(" dBm");
    Serial.print(", channel ber(%) -> "); Serial.println(ber); 
    return 0;
  }

  return -1;
}

int8_t cgatt(unit8_t mode) {
  String resp = "";
  char char_mode = '1';
  
  if (mode == 0) {
    char_mode = '0';
  }
  else if (mode == 1) {
    char_mode = '1';
  }
  else {
    return -1;
  }

  mySerial.write("AT+CGATT:");
  mySerial.write(char_mode);
  mySerial.write("\r\n");
  mySerial.flush();

  mySerail.readStringUntil('\n'); //read echo
  resp = mySerial.readString();

  if (resp.equals("OK\r\n")) {
    return 0;
  }

  return -1;
}

int8_t showPdpAddr(void) {
  /* This function is to read only one PDP Addr and only one PDP Service */
  String resp = "";
  String ip   = "";

  mySerial.write("AT+CGPADDR=1\r\n");
  mySerial.flush();
  mySerial.readStringUntil('\n'); // read echo

  resp = mySerial.readStringUntil(' ');
  if (!resp.equals("+CGPADDR:")) // pdp not exist
    return -1;

  // extract ip addr
  mySerial.readStringUntil(',');
  ip = mySerial.readStringUntil('\r');

  // check response
  resp = mySerial.readString();
  if (resp.equals("\n\r\nOK\r\n")) {
    Serial.print("PDP IP Address -> "); Serial.println(ip);
    return 0;
  }

  return -1;
}


/* TCP/IP and UDP Commands ----------------- */
int8_t configTCPcontext(uint8_t contextID, uint8_t contextType, String APN, String username, String password, uint8_t authen) {
  String payload = "AT+QICSGP=";
  String resp    = "";

  /* Params validation */
  // check context id
  if (contextID < 1 || contextID > 16)
    return -1; // invalid contextID return -1
  else
    payload += String(contextID) + ",";

  // check context type
  if (contextType < 1 || contextType > 3)
    return -1;
  else
    payload += String(contextType) + ",";
    
  payload += "\"" + APN + "\","; // add APN name
  payload += "\"" + username + "\"," // add username
  payload += "\"" + password + "\"," // add password

  // check authentication
  if (authen < 0 || authen > 3)
    return -1;
  else
    payload += String(authen) + "\r\n";


  mySerial.write(payload);
  mySerial.flush();

  mySerial.readStringUntil('\n'); // read echo
  resp = mySerial.readString(); // read response

  if (resp.equals("OK\r\n")) {
    return 0;
  }

  return -1;
}

int8_t activatePDP(uint8_t contextID, uint8_t contextState, uint8_t contextType, String IPAddr) {
  String resp = "";
  String payload = "AT+QIACT=";
  
  /* Params Validation */
  // check context id
  if (contextID < 1 || contextID > 16)
    return -1;
  else
    payload += String(context) + ",";
  

  
}
