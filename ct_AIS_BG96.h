#include <Arduino.h>
#include <AltSoftSerial.h>


/* Prototypes */
void printSerialDebug(void);
int8_t initModule(void);


/////////////////
// AT COMMANDS //
/////////////////
/* General Commands ----------------- */
int8_t checkModule(void);
int8_t requestIMEI(void);
int8_t setPhoneFunc(uint8_t mode);
int8_t signalQualityReport(void);
int8_t cgatt(uint8_t mode);
int8_t showPdpAddr(void);

/* TCP/IP and UDP Commands ----------------- */
int8_t configTCPcontext(uint8_t contextID, uint8_t contextType, String APN, String username, String password, uint8_t authen=0);
int8_t deactivatePDP(uint8_t contextID);
int8_t activatePDP(uint8_t contextID, uint8_t contextState, uint8_t contextType);
int8_t openSocketService(uint8_t contextID, uint8_t connectID, String serviceType, String ipAddr, uint16_t port);
int8_t closeSocketService(uint8_t connectID, uint16_t timeout=10);