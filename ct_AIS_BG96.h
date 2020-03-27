// Include Guards
#ifndef CT_AIS_BG96_H
#define CT_AIS_BG96_H


#include <Arduino.h>
#include <AltSoftSerial.h>

#define CT_BG96_DEBUG 0 // Debug switch

/* Enums */
typedef enum {
    INIT_STATUS_OK = 0x00,
    INIT_STATUS_CHECK_ERR,
    INIT_STATUS_ECHO_ERR,
    INIT_STATUS_REQIMEI_ERR,
    INIT_STATUS_SETPHONE_ERR,
    INIT_STATUS_SIGREP_ERR,
    INIT_STATUS_CGATT_ERR,
    INIT_STATUS_SHOWADDR_ERR,
    INIT_STATUS_UNKNOWN_ERR = -1
} init_status_t;

typedef enum {
    CONNECT_STATUS_OK = 0x00,
    CONNECT_STATUS_TCP_ERR,
    CONNECT_STATUS_DEACT_ERR,
    CONNECT_STATUS_ACT_ERR,
    CONNECT_STATUS_OPENSOC_ERR,
    CONNECT_STATUS_UNKNOWN_ERR = -1
} connect_status_t;

typedef enum {
  GNSS_OK = 0x00,
  GNSS_INVALID_PARAMS,
  GNSS_ERROR,
  GNSS_UNKNOWN_ERROR
} gnss_t;

typedef struct gnss_data {
  String utc = "";
  String lat = "";
  String lng = "";
  String alt = "";
  String spkm = "";
  String date = "";
  String nsat = "";
} gnss_data_t;

/* Prototypes */
static void printSerialDebug(void);


////////////////////////// HIGH LEVEL FUNCTIONS ////////////////////////////////
init_status_t initModule(void);
connect_status_t openConnection(String APN, String serviceType, String ipAddr, uint16_t port);

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

/* gnss command */
gnss_t GNSS(void);
gnss_t GNSS_end(void);
gnss_t GNSS_getLoc(gnss_data_t* gnss);

#endif
