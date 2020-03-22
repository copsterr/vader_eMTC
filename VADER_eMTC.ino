#include "ct_AIS_BG96.h"
#include "device_config.h"
#include "PMS.h"
#include <ThreeWire.h>  
#include <RtcDS1302.h>

/* Private Defines -------------- */
#define APN               "aistest.emtc"
#define REMOTE_IP         "178.128.16.9"
#define REMOTE_PORT       41234
#define PING_ATTEMPTS_MAX 3

/* Function Prototypes ---------- */
void (* resetFunc)(void) = 0;
void resetArduino(void);

/* PV --------------------------- */
// PMS variables
PMS pms(Serial1);
PMS::DATA pmsData;

// RTC variables
ThreeWire myWire(51,52,53); // IO, SCLK, CE
RtcDS1302<ThreeWire> Rtc(myWire);

// eMTC vars
uint32_t prevMillis = millis();
connect_status_t connect_status = CONNECT_STATUS_UNKNOWN_ERR;
uint8_t pingCount = 0;
#define DUTY_CYCLE 5000


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
  while (pingCount < PING_ATTEMPTS_MAX) {
    if (pingServer("178.128.16.9") != 0) {
      Serial.println("Ping Error! Re-pinging...");
      pingCount++;
      delay(2000);
    }
    else {
      pingCount = 0;
      Serial.println("Ping Success!");
      break;
    }
  }
  if (pingCount == 3) {
    Serial.println("Connection Failed.");
    resetArduino();
  }

  // init PMS Serial port
  Serial1.begin(9600);
  

}


void loop() {
  
  if (pms.read(pmsData))
  {
//    Serial.println(pmsData.PM_AE_UG_1_0);
//    Serial.println(pmsData.PM_AE_UG_2_5);
//    Serial.println(pmsData.PM_AE_UG_10_0);

    // Ping to check connection with the remote server
    while (pingCount < PING_ATTEMPTS_MAX) {
      if (pingServer("178.128.16.9") != 0) {
        Serial.println("Ping Error! Re-pinging...");
        pingCount++;
        delay(2000);
      }
      else {
        pingCount = 0;
        Serial.println("Connected to eMTC.");
        break;
      }
    }
    if (pingCount == 3) {
      Serial.println("Connection Failed.");
      resetArduino();
    }

    // get date and time from RTC
    RtcDateTime now = Rtc.GetDateTime();

    // get time to payload
    char timePayload[80] = {0};
    sprintf(timePayload, "%02u/%02u/%04u %02u:%02u:%02u",
            now.Month(),
            now.Day(),
            now.Year(),
            now.Hour(),
            now.Minute(),
            now.Second());


    String payload = "{\"dev_id\":\"" + String(DEV_ID) + \
                     "\",\"type\":\"PM\",\"timestamp\":\"" + String(timePayload) + \
                     "\",\"PM1_0\":" + String(pmsData.PM_AE_UG_1_0) + \
                     ",\"PM2_5\":" + String(pmsData.PM_AE_UG_2_5) + \
                     ",\"PM10_0\":" + String(pmsData.PM_AE_UG_10_0) + \
                     ",\"Lat\":" + "" + \
                     ",\"Lng\":" + "" + "}";

    // transmit data
    if (millis() - prevMillis > DUTY_CYCLE) {
      sendData(payload);
      prevMillis = millis();
    }
    
  }
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
