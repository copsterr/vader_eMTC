#include "ct_AIS_BG96.h"
#include "device_config.h"

/* Private Defines -------------- */
#define APN               "aistest.emtc"
#define REMOTE_IP         "178.128.16.9"
#define REMOTE_PORT       41234
#define PING_ATTEMPTS_MAX 3

/* Function Prototypes ---------- */
void (* resetFunc)(void) = 0;
void resetArduino(void);

/* PV --------------------------- */
connect_status_t connect_status = CONNECT_STATUS_UNKNOWN_ERR;
uint8_t pingCount = 0;

void setup() {
  Serial.begin(VC_BAUD);
  while(!Serial);
  
  Serial.println("\r\n########## AIS eMTC Started! ##########");

  // Init eMTC Shield
  if (initModule() != INIT_STATUS_OK) resetArduino();

  // Keep open connection until it's successful
  Serial.print("Opening Connection");
  do {
    Serial.print(".");
    connect_status = openConnection(APN, "UDP", REMOTE_IP, REMOTE_PORT);
    if (connect_status == CONNECT_STATUS_OK) {
      Serial.println("\r\nSuccessfully open connection.");
    }
    delay(2000);
  } while (connect_status != CONNECT_STATUS_OK);

  // Ping to check connection with the remote server
  while (pingCount != PING_ATTEMPTS_MAX) {
    if (pingServer("178.128.16.9") != 0) {
      Serial.println("Ping Error! Re-pinging...");
      pingCount++;
      delay(2000);
    }
    else {
      Serial.println("Ping Success!");
      break;
    }
  }
  

}


void loop() {
  if (pingServer("178.128.16.9") == 0) {
    Serial.println("Ping success");
  };
  delay(5000);
}


/* User Defined Functions ---------- */
void resetArduino(void)
{
  /*
    Function used for resetting Arduino
  */
  Serial.println("*** ---------- Resetting Arduino ---------- ***");
  resetFunc();
}