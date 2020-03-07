#include "ct_AIS_BG96.h"

AltSoftSerial mySerial;

/* User Defined Functions ---------- */
void printSerialDebug(void) {
  delay(1000);
  while (mySerial.available()) {
    Serial.print(mySerial.read(), HEX);
    Serial.print(" ");
  }
}

/* General Commands ----------------- */
int8_t initModule(void) {
  mySerial.begin(9600);
  mySerial.setTimeout(1000);
}

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

int8_t configEcho(uint8_t mode=0) {
  String resp    = "";
  String payload = "ATE";

  if (mode != 0 || mode != 1) {
    Serial.println("Cannot set Echo. Invalid Parameter");
    return -1;
  }
  payload += String(mode) + "\r\n";

  /* write command */
  mySerial.print(payload);

  /* read response */
  mySerial.readStringUntil('\n'); // read echo
  resp = mySerial.readString();

  if (resp.equals("OK\r\n")) {
    return 0;
  }
  
  return -1;

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

int8_t cgatt(uint8_t mode) {
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

  mySerial.readStringUntil('\n'); //read echo
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
int8_t configTCPcontext(uint8_t contextID, uint8_t contextType, String APN, String username, String password, uint8_t authen=0) {
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
  payload += "\"" + username + "\","; // add username
  payload += "\"" + password + "\","; // add password

  // check authentication
  if (authen < 0 || authen > 3)
    return -1;
  else
    payload += String(authen) + "\r\n";


  mySerial.print(payload);
  mySerial.flush();

  mySerial.readStringUntil('\n'); // read echo
  resp = mySerial.readString(); // read response

  if (resp.equals("OK\r\n")) {
    return 0;
  }

  return -1;
}

int8_t deactivatePDP(uint8_t contextID) {
  String payload = "AT+QIDEACT=";
  String resp    = "";

  if (contextID < 1 || contextID > 16)
    return -1;
  else
    payload += String(contextID) + "\r\n";

  mySerial.print(payload);
  mySerial.flush();

  mySerial.readStringUntil('\n'); // read echo
  resp = mySerial.readString();

  if (resp.equals("OK\r\n"))
    return 0;
  
  return -1;
}

int8_t activatePDP(uint8_t contextID, uint8_t contextState, uint8_t contextType) {
  String resp = "";
  String payload = "AT+QIACT=";
  
  /* Params Validation */
  // check context id
  if (contextID < 1 || contextID > 16)
    return -1;
  else
    payload += String(contextID) + ",";
  
  // check context state
  if (contextState < 0 || contextState > 1)
    return -1;
  else
    payload += String(contextState) + ",";


  if (contextType < 1 || contextType > 2)
    return -1;
  else
    payload += String(contextType) + "\r\n";

    
  mySerial.print(payload);
  mySerial.flush();

  mySerial.readStringUntil('\n'); // read echo
  resp = mySerial.readString(); // read response

  if (resp.equals("OK\r\n")) {
    return 0;
  }

  return -1;
}

int8_t openSocketService(uint8_t contextID, uint8_t connectID, String serviceType, String ipAddr, uint16_t port) {
  // AT+QIOPEN=1,0,"UDP","178.128.16.9",41234
  
  String payload  = "AT+QIOPEN=";
  String resp     = "";
  String err_code = "";

  /* Params Validation */
  // check context id
  if (contextID < 1 || contextID > 16 )
    return -1;
  else
    payload += String(contextID) + ",";

  // check connect id
  if (connectID < 0 || connectID > 11)
    return -1;
  else
    payload += String(connectID) + ",";
  
  // check service type
  if (serviceType.equals("TCP") || serviceType.equals("UDP") || \
      serviceType.equals("TCP LISTENER") || serviceType.equals("UDP SERVICE")) {
    payload += "\"" + serviceType + "\",";
  }

  // check ip
  payload += "\"" + ipAddr + "\",";

  // add remote port
  payload += String(port) + "\r\n";

  
  /* write command */
  mySerial.print(payload);
  mySerial.flush();


  /* read response */
  mySerial.readStringUntil('\n'); // read echo
  resp = mySerial.readStringUntil('\n');

  if (resp.equals("ERROR\r")) {
    return -1; // error occurs
  }

  mySerial.readStringUntil(','); // read message header til ','
  err_code = mySerial.readString(); // read error code

  if (!err_code.equals("0\r\n")) {
    Serial.print("Open socket error with error code -> ");
    Serial.print(err_code);
    return -1;
  }
  else {
    Serial.println("Socket opening succeeded.");
    return 0;
  }

  return -1;


}

int8_t closeSocketService(uint8_t connectID, uint16_t timeout=10) {
  String resp    = "";
  String payload = "AT+QICLOSE=";

  /* Params Validation */
  if (connectID < 0 || connectID > 11)
    return -1;
  payload += String(connectID) + ",";
  
  if (timeout < 0 || timeout > 65535)
    return -1;
  payload += String(timeout) + "\r\n";


  /* write command */
  mySerial.print(payload);
  mySerial.flush();

  /* read response */
  mySerial.readStringUntil('\n'); // read echo
  resp = mySerial.readString();

  if (resp.equals("OK\r\n")) {
    Serial.println("Socket closed.");
    return 0;
  }

  return -1;
  
}

int8_t sendData(String data, uint8_t connectID=0) {
  /* Maximum data length is 1460 bytes */
  uint16_t data_length = data.length(); // plus an ETB byte
  String payload     = "AT+QISEND=";
  String resp        = "";

  /* Params Validation */
  if (connectID < 0 || connectID > 11) {
    Serial.println("Cannot send. Invalid Connect ID. (must ranged from 0 to 11)");
    return -1;
  }
  payload += String(connectID) + ",";

  if (data_length > 1460) {
    Serial.println("Cannot send because data length exceeds limit! (maximum 1460 bytes)");
  }
  payload += String(data_length) + "\r\n";

  
  /* write command */
  mySerial.print(payload);
  mySerial.flush();

  /* read response */
  mySerial.readStringUntil('\n'); // read echo
  resp = mySerial.readString();

  if (resp.equals("ERROR\r\n")) {
    Serial.println("Cannot send because socket is not opened.");
    return -1;
  }
  else if (resp.equals("> ")) {
    // Send Data
    mySerial.print(data);
    mySerial.print(0x1A); // ctrl+z (in hex)
    mySerial.flush();
  }
  else {
    Serial.println("Cannot send. Unexpected Error!");
    return -1;
  }

  /* read send response */
  // read echo data from the module
  mySerial.readStringUntil('\n');

  // read response
  resp = mySerial.readString();

  if (resp.equals("SEND OK\r\n")) {
    Serial.println("Send OK.");
    return 0;
  }
  else if (resp.equals("SEND FAIL\r\n")) {
    Serial.println("Send Failed. Buffer is full!");
    return -1;
  }
  else {
    Serial.println("Error! Connection has not established, abnormally closed or parameter is incorrect.");
    return -1;
  }

  return -1;
}





/*
   TODO
1. Buffer overflow problem, may need to disable echo 
*/