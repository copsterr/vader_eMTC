#include <Arduino.h>
#include <AltSoftSerial.h>

#define CT_BG96_DEBUG 1 // Debug switch

/* Prototypes */
void printSerialDebug(void);

////////////////////////// HIGH LEVEL FUNCTIONS ////////////////////////////////
int8_t initModule(void);
int8_t openConnection(String APN, String serviceType, String ipAddr, uint16_t port);

/////////////////////////////// AT COMMANDS ////////////////////////////////////
/* General Commands ----------------- */
int8_t checkModule(void);
int8_t configEcho(uint8_t mode=0);
int8_t requestIMEI(void);
int8_t setPhoneFunc(uint8_t mode=1);
int8_t signalQualityReport(void);
int8_t cgatt(uint8_t mode);
int8_t showPdpAddr(void);

/* TCP/IP and UDP Commands ----------------- */
int8_t configTCPcontext(String APN, uint8_t contextID=1, uint8_t contextType=1, String username="", String password="", uint8_t authen=0);
int8_t deactivatePDP(uint8_t contextID=1);
int8_t activatePDP(uint8_t contextID=1, uint8_t contextState=1, uint8_t contextType=1);
int8_t openSocketService(String serviceType, String ipAddr, uint16_t port, uint8_t contextID=1, uint8_t connectID=0);
int8_t closeSocketService(uint8_t connectID=0, uint16_t timeout=10);
int8_t sendData(String data, uint8_t connectID=0);
int8_t pingServer(String host, uint8_t contextID=1, uint8_t timeout=4);