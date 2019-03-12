/*
 * Copyright (c) 2018, Huawei Technologies Co., Ltd All rights reserved.

 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:

 * 1. Redistributions of source code must retain the above copyright notice,
 * this list of conditions and the following disclaimer.

 * 2. Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution.

 * 3. Neither the name of the copyright holder nor the names of its
 * contributors may be used to endorse or promote products derived from
 * this software without specific prior written permission.

 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING,
 * BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND
 * FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY,
 * OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT
 * OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef FM_API_H
#define FM_API_H

#ifdef	__cplusplus
extern "C" {
#endif

/*! \file fmtkapi.h
\brief API header for intergrating fmtk archieve and library.
*/

/*! \def DBDF_LEN
\brief A macro which defines the length of DBDF string.*/
#define DBDF_LEN                                                       16
/*! \def MAX_NUM_OF_FPGA_PER_HOST
\brief A macro which defines the maximum number of FPGA in a host.*/
#define MAX_NUM_OF_FPGA_PER_HOST                                       20
/*! \def FMTK_MAX_LABEL_LEN
\brief A macro which defines maximum number of char in an FPGA identifiers and data when querying.*/
#define FMTK_MAX_LABEL_LEN                                             128
/*! \def SECURE_DATA_VER
\brief A macro that defines the version of FPGA secure chip data structure.*/
#define SECURE_DATA_VER                                                3
/*! \def POWER_DATA_VER
\brief A macro that defines the version of FPGA power data structure.*/
#define POWER_DATA_VER                                                 3
/*! \def FIRMWARE_ID_DATA_VER
\brief A macro that defines the version of FPGA firmware data structure.*/
#define FIRMWARE_ID_DATA_VER                                           3
/*! \def IDENTIFIERS_DATA_VER
\brief A macro that defines the version of FPGA identifiers data structure.*/
#define IDENTIFIERS_DATA_VER                                           3
/*! \def ALARM_DATA_VER
\brief A macro that defines the version of FPGA alarm data structure.*/
#define ALARM_DATA_VER                                                 3
/*! \def MAC_DATA_BYTES_LEN
\brief A macro that defines the maximum bytes of FPGA mac address data.*/
#define MAC_DATA_BYTES_LEN                                             6
/*! \def MAX_EEPROM_STRING_LEN
\brief A macro that defines the maximum char of FPGA eeprom data string.*/
#define MAX_EEPROM_STRING_LEN                                          1024
/*! \def MAX_MSG_CHAR_LEN
\brief A macro that defines the maximum char of FPGA upgrading/other progress message string.*/
#define MAX_MSG_CHAR_LEN                                               300
/*! \def SECURE_DATA_ZONES
\brief A macro that defines the number of FPGA secure chip data zone.*/
#define SECURE_DATA_ZONE_NUM                                           8
/*! \def SECURE_DATA_MAX_LEN
\brief A macro that defines the maximum char of FPGA secure chip data string.*/
#define SECURE_DATA_MAX_LEN                                            1024
/*! \def SECURE_PUBLICKEY_MAX_LEN
\brief A macro that defines the maximum char of FPGA secure chip public key string.*/
#define SECURE_PUBLICKEY_MAX_LEN                                       256
/*! \def SECURE_CURVEID_MAX_LEN
\brief A macro that defines the maximum char of FPGA secure chip ECC data string.*/
#define SECURE_CURVEID_MAX_LEN                                         32
/*! \def TASK_STATE_OFF
\brief A macro that defines the FPGA upgrade/other multithreads task off state.*/
#define TASK_STATE_OFF                                                 0
/*! \def TASK_STATE_ON
\brief A macro that defines the FPGA upgrade/other multithreads task on state.*/
#define TASK_STATE_ON                                                  1
/*! \def FM_INTERFACE
\brief A macro that defines the alias for extern fmerror enum.*/
#define FM_INTERFACE extern fmerrno

/* User Customized data */
/*! \def SELECTED_API_VERSION
\brief A macro that defines the major version of the matching library.*/
#define SELECTED_API_VERSION   1
/*! \def SELECTED_API_SUB_VER
\brief A macro that defines the minor version of the matching library.*/
#define SELECTED_API_SUB_VER   23
/* End of user customized data */

/*! \def FM_ALARM_SUPPORT_FLAG
\brief A macro whether the shell supports the alarm query function.*/
#define FM_ALARM_SUPPORT_FLAG    unsigned int

/*! \def SENSOR_NUM_LENGTH
\brief A macro that defines the maximum number of sensor.*/
#define SENSOR_NUM_MAX    12

/*! \def SPD_INFO_LENGTH
\brief A macro that defines the maximum char of dimm info string.*/
#define SPD_INFO_LENGTH      512

/*! \def FC_NUM_MAX
\brief A macro that defines the maximum number of fc module */
#define FC_NUM_MAX           2

/*! \def FC_NUM_MAX
\brief A macro that defines the maximum number of dimm */
#define SPD_NUM_MAX          4


/**
  * @brief Data structure of an FMTK API session
  *
  * Data can be fetched from API \ref FmAPI_StartSession
*/
typedef struct
{
    unsigned Allocated ;                                 /*!< The session is allocated or not */
    unsigned PfNum     ;                                 /*!< The FPGA device number in the host */
    unsigned SlotNumMax;                                 /*!< The Maximum FPGA device number of current devices */
    unsigned char TaskState;                             /*!< The multithread task state, use to stop progress checking threads */
    unsigned DeviceIdentifiers_version;                  /*!< The device Identifiers data structure version of current session */
    char Progress[MAX_NUM_OF_FPGA_PER_HOST] ;            /*!< The progress data array, index is FPGA device index */
    unsigned Slot[MAX_NUM_OF_FPGA_PER_HOST];             /*!< The slot number data array , index is FPGA device index*/
    char Msg[MAX_NUM_OF_FPGA_PER_HOST][MAX_MSG_CHAR_LEN];/*!< The message data array , index is FPGA device index*/
    char DBDF[MAX_NUM_OF_FPGA_PER_HOST][DBDF_LEN + 1];   /*!< The dbdf string data array , index is FPGA device index*/
    unsigned DeviceIdx[MAX_NUM_OF_FPGA_PER_HOST];        /*!< The device index data array , index is FPGA device slot number*/
    unsigned ShellID[MAX_NUM_OF_FPGA_PER_HOST];          /*!< The shellID data array , index is FPGA device index*/
    void* pMcuInfoBuff[MAX_NUM_OF_FPGA_PER_HOST];        /*!< The MCU E-Lable array , index is FPGA device index*/

}FmSession;


/**
* \addtogroup SecureModules
*/
/*@{*/

/**
  * @brief Data structure of TLV data */
typedef struct tagSecureTLVBufStru
{
    unsigned char  ucTag;                         /*!< TLV data type  */
    unsigned short  usLength;                     /*!< TLV data length  */
    unsigned char  aucData[SECURE_DATA_MAX_LEN];  /*!< TLV data */
}SECURE_TLV_BUFFER_STRU;

/**
  * @brief Data structure of one certificate key information */
typedef struct tagSecureOneCertKeyInfoStru
{
    unsigned char  ucZone;                        /*!< certificate area */
    unsigned short  usLength;                     /*!< certificate length  */
    unsigned short  usVenderId;                   /*!< certificate vender id */
    unsigned short  usCertId;                     /*!< certificate id */
}SECURE_ONECERT_KEY_INFO_STRU;

/**
  * @brief Data structure of all certificate key information */
typedef struct tagSecureCertKeyInfoStru
{
    unsigned char  ucCertNum;                      /*!< certificate number */
    SECURE_ONECERT_KEY_INFO_STRU astrOneCertKeyInfo[SECURE_DATA_ZONE_NUM]; /*!< one certificate key info */
}SECURE_CERT_KEY_INFO_STRU;

/**
  * @brief Enumerate of secure chip type */
typedef enum enSecureChip
{
    SECURE_STATIC_CHIP        = 0x00,             /*!< Static secure chip  */
    SECURE_DYNAMIC_CHIP       = 0x01,             /*!< Dynamic secure chip */
    SECURE_CHIP_MAX_NUM
}ENUM_SECURE_CHIP;

/**
  * @brief Data structure of public key
  *
  * Data can be fetched from API \ref FmAPI_SecureGetPublicKey
*/
typedef struct tagSecurePublicKeyBufferStru
{
    unsigned char  ucTag;                                   /*!< Pulic key type */
    unsigned short  usPublicKeyLen;                         /*!< Public key data length */
    unsigned char  aucPublicKey[SECURE_PUBLICKEY_MAX_LEN];  /*!< Public key data */
    unsigned short  usCurveIDLen;                           /*!< Elliptic curve cryptography(ECC) Id length */
    unsigned char  aucCurveID[SECURE_CURVEID_MAX_LEN];      /*!< Elliptic curve cryptography(ECC) Id  */
}SECURE_PUBLICKKEY_BUFFER_STRU;

/*@}*/

/**
  * @brief Enumerate of API error code :fmerrno */
typedef enum
{
    FM_API_SUCCESS = 0,                         /*!< \n  Invoking API successfully */
    FM_API_ERR_LIB_NOT_INIT,                    /*!< \n  General API error: API was not initiated */
    FM_API_ERR_DRV_INIT_ERR,                    /*!< \n  General API error: driver was not initiated */
    FM_API_ERR_LOG_INIT_ERR,                    /*!< \n  General API error: FMTK log was not initiated */
    FM_API_ERR_VER_MISMATCH,                    /*!< \n  General API error: API header version mismatched library version */
    FM_API_ERR_NULL_INPUT,                      /*!< \n  General API error: input parameter is null*/
    FM_API_ERR_DUP_SESSION_START,               /*!< \n  General API error: trying to start duplicated sessions */
    FM_API_ERR_SESSION_START_ERR,               /*!< \n  General API error: error when session was starting */
    FM_API_ERR_ILLEGAL_INPUT,                   /*!< \n  General API error: illegal API parameter */
    FM_API_ERR_FILE_NOT_EXISTS,                 /*!< \n  General API error: file does not exist */
    FM_API_ERR_ACQUIRE_DEVICE_MUTEX_ERR,        /*!< \n  General API error: acquring multithreads mutex failed*/
    FM_API_ERR_RELEASE_DEVICE_MUTEX_ERR,        /*!< \n  General API error: releasing multithreads mutex failed */
    FM_API_ERR_GET_IDENTIFIERS_ERR,             /*!< \n  Identifiers API error: failed getting FPGA identifiers */
    FM_API_ERR_GET_FIRMWARE_ERR,                /*!< \n  Identifiers API error: failed getting firmware information  */
    FM_API_ERR_MALLOC_ERR,                      /*!< \n  General API error: memory malloc failed */
    FM_API_ERR_MEMSET_ERR,                      /*!< \n  General API error: memory memset failed */
    FM_API_ERR_NOT_SUPPORT,                     /*!< \n  General API error: parameters were not supported */
    FM_API_ERR_GENERAL_ERR,                     /*!< \n  General API error: general error */
    FM_API_ERR_RESET_FPGA_ERR,                  /*!< \n  Reset API error: failed resetting fpga error */
    FM_API_ERR_EEPROM_ERR,                      /*!< \n  EEPROM API error: eeprom hardware error */
    FM_API_INDEX_ERR,                           /*!< \n  General API error: index was out of range */
    FM_API_BDF_NULL_ERR,                        /*!< \n  General API error: bdf string parameter was null */
    FM_API_SECURE_CHIPIDX_ERR,                  /*!< \n  Secure chip API error: secure chip index out of range */
    FM_API_SECURE_TLVOUT_NULL_ERR,              /*!< \n  Secure chip API error: TLVOUT parameter was null */
    FM_API_SECURE_CERT_NULL_ERR,                /*!< \n  Secure chip API error: certinfo parameter was null */
    FM_API_SECURE_OPERATE_ERR,                  /*!< \n  Secure chip API error: failed to operate secure chip */
    FM_API_SECURE_TLVIN_LEN_ERR,                /*!< \n  Secure chip API error: TLVIN parameter length is invalid */
    FM_API_SECURE_GEN_CMD_ERR,                  /*!< \n  Secure chip API error: secure chip command failed */
    FM_API_SECURE_QUERYTAG_ERR,                 /*!< \n  Secure chip API error: tag was out of range when querying */
    FM_API_SECURE_GETPUB_ERR,                   /*!< \n  Secure chip API error: failed getting public key */
    FM_API_SECURE_READPARA_ERR,                 /*!< \n  Secure chip API error: parameter was invalid when reading data */
    FM_API_SECURE_READ_ERR,                     /*!< \n  Secure chip API error: failed reading data */
    FM_API_SECURE_UPDATEPARA_ERR,               /*!< \n  Secure chip API error: parameter was invalid when writing data */
    FM_API_SECURE_UPRADE_ERR,                   /*!< \n  Secure chip API error: failed writing data */
    FM_API_SECURE_WRITECERT_ERR,                /*!< \n  Secure chip API error: failed writing certificate */
    FM_API_SECURE_CERTHEADER_ERR,               /*!< \n  Secure chip API error: failed when certificate header verified */
    FM_API_SECURE_UNSUPPORT_ERR,                /*!< \n  Secure chip API error: no sucure chip module in the shell */
    FM_API_MODULE_SUPPORT_ERR,                  /*!< \n  General API error: no specific module in the shell */
    FM_API_FPGA_STATUS_ERR,                     /*!< \n  General API error: FPGA device was disabled */
    FM_API_ERR_UPGRADE_FIRMWARE_ERR,            /*!< \n  General API error: failed upgrading firmware */
    FM_API_ERR_GET_OVERHEAT_STATE_ERR,          /*!< \n  Config API error:  failed getting overheat state */
    FM_API_ERR_SET_OVERHEAT_STATE_ERR,          /*!< \n  Config API error:  failed setting overheat state */
    FM_API_ERR_READ_MAC_ADDR_ERR,               /*!< \n  EEPROM API error:  failed reading mac address */
    FM_API_ERR_WRITE_MAC_ADDR_ERR,              /*!< \n  EEPROM API error:  failed writing mac address */
    FM_API_ERR_READ_EEPROM_DATA_ERR,            /*!< \n  EEPROM API error:  failed reading eeprom data */
    FM_API_ERR_WRITE_EEPROM_DATA_ERR,           /*!< \n  EEPROM API error:  failed writing eepromg data */
    FM_API_ERR_COLLECT_MCU_LOG_ERR,             /*!< \n  COLLECT API error:  failed collecting mcu logs */
    FM_API_ERR_CODE_EINVAL = 127                /*!< \n  General API error: default API error for error initiation */
}fmerrno;

/**
* \addtogroup IdModules
*/
/*@{*/


/**
  * @brief Data structure of power information
  *
  * Data can be fetched from API \ref FmAPI_GetDevicePowers
*/
typedef struct {
    unsigned int    version;                  /*!<  Version Number (fmtkDevicePowers_version)  */
    double          curPower;                 /*!<  Current power consumption associated with this device (in W)  */
    unsigned int    shutdownPower;            /*!<  Shutdown power(in W) 72w  */
    double          temperature;
}FM_DEVICE_POWER_DATA;

/**
  * @brief Data structure of optical transceiver information
  *
  * Data can be fetched from API \ref FmAPI_GetOpticalModules
*/
typedef struct
{
    unsigned int version;
    unsigned int fcCount;
    char state[FC_NUM_MAX][FMTK_MAX_LABEL_LEN];
    char manufacturer[FC_NUM_MAX][FMTK_MAX_LABEL_LEN];
    char productionDate[FC_NUM_MAX][FMTK_MAX_LABEL_LEN];
    char partNumber[FC_NUM_MAX][FMTK_MAX_LABEL_LEN];
    
    char serialNumber[FC_NUM_MAX][FMTK_MAX_LABEL_LEN];
    char model[FC_NUM_MAX][FMTK_MAX_LABEL_LEN];
    char fcMode[FC_NUM_MAX][FMTK_MAX_LABEL_LEN];         /*!< "Multi-mode","Single-Mode" */
    char temperature[FC_NUM_MAX][FMTK_MAX_LABEL_LEN];
    char voltage[FC_NUM_MAX][FMTK_MAX_LABEL_LEN];

    char waveLength[FC_NUM_MAX][FMTK_MAX_LABEL_LEN];
    unsigned char channelCount[FC_NUM_MAX];              /*!< The 32-bit Sub System Device ID */
}FM_OPTICAL_MODULE_DATA;

/**
  * @brief Data structure of electronic label information
  *
  * Data can be fetched from API \ref FmAPI_GetDetailedInfos
*/
typedef struct
{
    unsigned int version;
    unsigned int temperatureSensorNum;
    unsigned int voltageSensorNum;
    char VSensorVal[SENSOR_NUM_MAX][FMTK_MAX_LABEL_LEN];
    char TSensorVal[SENSOR_NUM_MAX][FMTK_MAX_LABEL_LEN];
    char sensorName[FMTK_MAX_LABEL_LEN];
    char boardId[FMTK_MAX_LABEL_LEN];
    char pcbId[FMTK_MAX_LABEL_LEN];
    char bomId[FMTK_MAX_LABEL_LEN];
    char healthState[FMTK_MAX_LABEL_LEN];
    char productionDate[FMTK_MAX_LABEL_LEN];
    char boardManufacturer[FMTK_MAX_LABEL_LEN];
    char flashManufacturer[FMTK_MAX_LABEL_LEN];
    char fruFileID[FMTK_MAX_LABEL_LEN];            /*!< fru File ID of the device  */ 
    char productName[FMTK_MAX_LABEL_LEN];          /*!< product Name FX300/FX600   */
    char serial[FMTK_MAX_LABEL_LEN];               /*!< Serial for the device      */
    char manufacturer[FMTK_MAX_LABEL_LEN];         /*!< manufacturer for the device  */
    char mcuFirmwareVer[FMTK_MAX_LABEL_LEN];

}FM_DETAILED_ID_DATA;

/**
  * @brief Data structure of dimm information
  *
  * Data can be fetched from API \ref FmAPI_GetDDRDatas
*/
typedef struct
{
    unsigned int version;
    unsigned int spdCount;                
    char spdMode[SPD_NUM_MAX][FMTK_MAX_LABEL_LEN];     /* !< "Invalid","Valid","Unkown" */
    char sensorName[SPD_NUM_MAX][FMTK_MAX_LABEL_LEN];
    char partNum[SPD_NUM_MAX][FMTK_MAX_LABEL_LEN];
    char serialNum[SPD_NUM_MAX][FMTK_MAX_LABEL_LEN];
    unsigned int size[SPD_NUM_MAX];
    unsigned int frequencyHz[SPD_NUM_MAX];
    unsigned char rank[SPD_NUM_MAX];
    char spdType[SPD_NUM_MAX][FMTK_MAX_LABEL_LEN];  
}FM_DDR_DATA;


/**
  * @brief Data structure of FPGA identifers information
  *
  * Data can be fetched from API \ref FmAPI_GetDeviceIdentifiers
*/
typedef struct
{
    unsigned int version;                          /*!<  Version Number (DeviceIdentifiers_version)  */
    char pciBusId[FMTK_MAX_LABEL_LEN];             /*!<  PCI Bus ID bdf  */
    unsigned int pciVenderId;                      /*!<  The combined 16-bit device id and 16-bit vendor id  */
    unsigned int pciDeviceId;                      /*!<  The 32-bit Device ID    */
    unsigned int pciSubVenderId;                   /*!<  The 32-bit Sub vendor ID  */
    unsigned int pciSubDeviceId;                   /*!<  The 32-bit Sub System Device ID  */
}FM_IDENTIFIERS_DATA;


/**
  * @brief Enumerate of firmware state
  */
typedef enum {
    FMTK_FIRMWARE_STATE_DISABLED    = 1,        /*!< Firmware disabled state */
    FMTK_FIRMWARE_STATE_ENABLED     = 2         /*!< Firmware enabled state */
} FmFirmwareState_t;

/**
  * @brief Enumerate of firmware type
  */
typedef enum
{
    // PCI connectivity states
    FMTK_FIRMWARE_TYPE_GOLDEN       = 0x1,      /*!< Golden type firmware */
    FMTK_FIRMWARE_TYPE_DPDK         = 0x2,      /*!< DPDK type firmware */
    FMTK_FIRMWARE_TYPE_OCL          = 0x3,      /*!< OCL type firmware */
    FMTK_FIRMWARE_TYPE_BASIC        = 0x4       /*!< BASIC type firmware */
} FmFirmwareType_t;

/**
  * @brief Data structure of firmware information
  *
  * Data can be fetched from API \ref FmAPI_GetDeviceFirmwares
*/
typedef struct
{
    unsigned int    version;                   /*!< Version Number (FM_FIRMWARES_DATA version)*/
    unsigned int    curfirmVer;                /*!< Current firmware version */
    unsigned int    releaseDate;               /*!< Firmware release date */
    FmFirmwareType_t  curfirmType;             /*!< Current firmware type */
    FmFirmwareState_t firmStatus;              /*!< Current firmware status */
    char mcuFirmwareVer[FMTK_MAX_LABEL_LEN];   /*!< Microcontroller unit firmware version */
}FM_FIRMWARES_DATA;

/**
  * @brief Enumerate of alarm level
  */
typedef enum
{
    FMTK_ALARM_LVL_NONE     = 0,                /*!< No alarm */
    FMTK_ALARM_LVL_Critical = 1,                /*!< Fatal alarm */
    FMTK_ALARM_LVL_Major    = 2,                /*!< Serious alarm */
    FMTK_ALARM_LVL_Minor    = 3,                /*!< General alarm */
    FMTK_ALARM_LVL_Warning  = 4                 /*!< Prompt alarm */
} FM_ALARM_LEVEL;


/**
  * @brief Data structure of alarm information
  *
  * Data can be fetched from API \ref FmAPI_GetDeviceAlarm
*/
typedef struct
{
    unsigned int    version;               //!< Version Number (fmtkDeviceAlarm_version)
    FM_ALARM_SUPPORT_FLAG    AlarmSupportFlag;                    //!< Function flag (0 indicates not supported, 1 indicates d518, and 2 indicates d516)
    FM_ALARM_LEVEL    systemAlarm;        //!< System integrated alarm
    FM_ALARM_LEVEL    DDR0ECC;            //!< DDR controller 0 ECC error
    FM_ALARM_LEVEL    DDR1ECC;            //!< DDR controller 1 ECC error
    FM_ALARM_LEVEL    mgmtCtrlFirewall;   //!< Management control channel firewall error
    FM_ALARM_LEVEL    userCtrlFirewall;   //!< User control channel firewall error
    FM_ALARM_LEVEL    userDatafirewall;   //!< User data channel firewall error
    unsigned int    systemAlarmReturn;      //!< System integrated alarm return code
    unsigned int    DDR0ECCReturn;          //!< DDR controller 0 ECC error return code
    unsigned int    DDR1ECCReturn;          //!< DDR controller 1 ECC error return code
    unsigned int    mgmtCtrlFirewallReturn; //!< Management control channel firewall error
    unsigned int    userCtrlFirewallReturn; //!< User control channel firewall error return code
    unsigned int    userDatafirewallReturn; //!< User data channel firewall error return code
} FM_DEVICE_ALARM_DATA;

/*@}*/

/**
* \defgroup InitModule General Fmtk APIs
*/
/*@{*/

/**
  * \fn FM_INTERFACE FmAPI_Initialize(unsigned version, unsigned subversion);
  * \brief Initialize host FPGA devices.
  * \remarks This method is used to initialize FMTK within this process.
  * \param[in] version    Major version, the figure before dot of version.
  * \param[in] subversion Sub version, the figure after dot of version.
  * \return \ref FM_INTERFACE
  * \retval - \ref FM_API_SUCCESS FPGA devices were initialed successfully.
  * \retval - \ref FM_API_ERR_LOG_INIT_ERR  The function failed because log initiation failed.
  * \retval - \ref FM_API_ERR_VER_MISMATCH  The function failed because header version mismatches library version.
  * \retval - \ref FM_API_ERR_DRV_INIT_ERR  The function failed because fpga device driver fails during initiation.
  */
FM_INTERFACE FmAPI_Initialize(unsigned version, unsigned subversion);

/**
  * \brief Free the FPGA API system : close fmtk log file.
  * \remarks This must be called after - \ref FmAPI_Initialize
  * \return \ref FM_INTERFACE
  * \retval - \ref FM_API_SUCCESS the FPGA API system was unloaded successfully.
  * \retval - \ref FM_API_ERR_LIB_NOT_INIT The function failed because API system was not initialed.
  */
FM_INTERFACE FmAPI_Unload(void);

/**
  * \brief Start an FPGA API Session : start an FPGA API Session.
  * \remarks This must be called after \ref FmAPI_Initialize, or an \ref FM_API_ERR_LIB_NOT_INIT will be returned.
  * \param[in, out]  FmSessionHdl A pointer to An Fmtk session handle(data pointer).
  * \return \ref FM_INTERFACE
  * \retval - \ref FM_API_SUCCESS the FPGA API Session started successfully.
  * \retval - \ref FM_API_ERR_LIB_NOT_INIT Not initialed before or unloaded.
  * \retval - \ref FM_API_ERR_SESSION_START_ERR The function failed when start session.
  */
FM_INTERFACE FmAPI_StartSession(FmSession** FmSessionHdl );

/**
  * \brief Stop FPGA API Session : stop an FPGA API Session.
  * \remarks This must be called after - \ref FmAPI_StartSession, or an \ref FM_API_ERR_NULL_INPUT will be returned.
  * \param[in] FmSessionHdl A started Fmtk session handle.
  * \return \ref FM_INTERFACE
  * \retval - \ref FM_API_SUCCESS FPGA API Session was stoped successfully.
  * \retval - \ref FM_API_ERR_LIB_NOT_INIT Not initialed before or unloaded, please call  - \ref FmAPI_Initialize.
  * \retval - \ref FM_API_ERR_NULL_INPUT FmSessionHdl is null.
  */
FM_INTERFACE FmAPI_StopSession(FmSession* FmSessionHdl);

/*@}*/

/**
* \defgroup IdModules Identifiers APIs
*/
/*@{*/

/**
  * \brief Scan FPGA Devices number.
  * \remarks This must be called after \ref FmAPI_Initialize, or an \ref FM_API_ERR_LIB_NOT_INIT will be returned.
  * \param[in, out]  Count The amount of FPGA devices currently on the host.
  * \return \ref FM_INTERFACE
  * \retval - \ref FM_API_SUCCESS the FPGA API Session started successfully.
  * \retval - \ref FM_API_ERR_LIB_NOT_INIT The function failed because API system had not been initialed.
  * \retval - \ref FM_API_ERR_NULL_INPUT Count is null.
  */
FM_INTERFACE FmAPI_ScanFpgaDevicesNum(unsigned * Count);

/**
  * \brief Gets device Identifiers corresponding to the FpgaSlot.
  * \remarks This must be called after \ref FmAPI_StartSession, or an \ref FM_API_ERR_ILLEGAL_INPUT will be returned.
  * \param[in] FmSessionHdl  The Fmtk session handle
  * \param[in] FpgaSlot FPGA Index corresponding to which the Identifiers
  *                            would be fetched
  * \param[in, out]  Ids Device \ref FM_IDENTIFIERS_DATA Identifiers corresponding to FpgaSlot.
  * \return \ref FM_INTERFACE
  * \retval - \ref FM_API_SUCCESS Get indentifiers successfully.
  * \retval - \ref FM_API_ERR_LIB_NOT_INIT The API system had not been initialed.
  * \retval - \ref FM_API_ERR_ILLEGAL_INPUT FmSessionHdl is null or Ids is null.
  * \retval - \ref FM_API_ERR_VER_MISMATCH The \ref FM_IDENTIFIERS_DATA version mismatches the one in library.
  * \retval - \ref FM_API_ERR_GET_IDENTIFIERS_ERR Error occured during Getting indentifiers.
  */
FM_INTERFACE FmAPI_GetDeviceIdentifiers(FmSession* FmSessionHdl, unsigned FpgaSlot, FM_IDENTIFIERS_DATA * Ids );

/**
  * \brief Gets device firmwares related information corresponding to the FpgaSlot.
  * \remarks This must be called after \ref FmAPI_StartSession, or an \ref FM_API_ERR_ILLEGAL_INPUT will be returned.
  * \param[in] FmSessionHdl  The Fmtk session handle
  * \param[in] FpgaSlot FPGA Index corresponding to which the Identifiers
  *                            would be fetched
  * \param[in, out]  Ids Device \ref FM_FIRMWARES_DATA Identifiers corresponding to FpgaSlot.
  * \return \ref FM_INTERFACE
  * \retval - \ref FM_API_SUCCESS Get indentifiers successfully.
  * \retval - \ref FM_API_ERR_LIB_NOT_INIT The API system had not been initialed.
  * \retval - \ref FM_API_ERR_ILLEGAL_INPUT FmSessionHdl is null or Ids is null.
  * \retval - \ref FM_API_ERR_VER_MISMATCH The \ref FM_FIRMWARES_DATA version mismatches the one in library.
  * \retval - \ref FM_API_ERR_GET_IDENTIFIERS_ERR Error occured during Getting indentifiers.
  */
FM_INTERFACE FmAPI_GetDeviceFirmwares(FmSession* FmSessionHdl, unsigned FpgaSlot, FM_FIRMWARES_DATA* Ids );

/**
  * \brief Gets device power related information corresponding to the FpgaSlot.
  * \remarks This must be called after \ref FmAPI_StartSession, or an \ref FM_API_ERR_ILLEGAL_INPUT will be returned.
  * \param[in] FmSessionHdl  The Fmtk session handle
  * \param[in] FpgaSlot FPGA Index corresponding to which the Identifiers
  *                            would be fetched
  * \param[in, out]  Ids Device \ref FM_DEVICE_POWER_DATA Identifiers corresponding to FpgaSlot.
  * \return \ref FM_INTERFACE
  * \retval - \ref FM_API_SUCCESS Get indentifiers successfully.
  * \retval - \ref FM_API_ERR_LIB_NOT_INIT The API system had not been initialed.
  * \retval - \ref FM_API_ERR_ILLEGAL_INPUT FmSessionHdl is null or Ids is null.
  * \retval - \ref FM_API_ERR_VER_MISMATCH The \ref FM_FIRMWARES_DATA version mismatches the one in library.
  * \retval - \ref FM_API_ERR_GET_IDENTIFIERS_ERR Error occured during Getting indentifiers.
  */
FM_INTERFACE FmAPI_GetDevicePowers(FmSession* FmSessionHdl, unsigned FpgaSlot, FM_DEVICE_POWER_DATA * Ids );

/**
  * \brief Gets device optical module information corresponding to the FpgaSlot.
  * \remarks This must be called after \ref FmAPI_StartSession, or an \ref FM_API_ERR_ILLEGAL_INPUT will be returned.
  * \param[in] FmSessionHdl  The Fmtk session handle
  * \param[in] FpgaSlot FPGA Index corresponding to which the Identifiers
  *                            would be fetched
  * \param[in, out]  Ids Device \ref FM_OPTICAL_MODULE_DATA Identifiers corresponding to FpgaSlot.
  * \return \ref FM_INTERFACE
  * \retval - \ref FM_API_SUCCESS Get indentifiers successfully.
  * \retval - \ref FM_API_ERR_LIB_NOT_INIT The API system had not been initialed.
  * \retval - \ref FM_API_ERR_ILLEGAL_INPUT FmSessionHdl is null or Ids is null.
  * \retval - \ref FM_API_ERR_VER_MISMATCH The \ref FM_FIRMWARES_DATA version mismatches the one in library.
  * \retval - \ref FM_API_ERR_GET_IDENTIFIERS_ERR Error occured during Getting indentifiers.
  */
FM_INTERFACE FmAPI_GetOpticalModules(FmSession* FmSessionHdl, unsigned FpgaSlot, FM_OPTICAL_MODULE_DATA* Id);

/**
  * \brief Gets device electronic information corresponding to the FpgaSlot.
  * \remarks This must be called after \ref FmAPI_StartSession, or an \ref FM_API_ERR_ILLEGAL_INPUT will be returned.
  * \param[in] FmSessionHdl  The Fmtk session handle
  * \param[in] FpgaSlot FPGA Index corresponding to which the Identifiers
  *                            would be fetched
  * \param[in, out]  Ids Device \ref FM_DETAILED_ID_DATA Identifiers corresponding to FpgaSlot.
  * \return \ref FM_INTERFACE
  * \retval - \ref FM_API_SUCCESS Get indentifiers successfully.
  * \retval - \ref FM_API_ERR_LIB_NOT_INIT The API system had not been initialed.
  * \retval - \ref FM_API_ERR_ILLEGAL_INPUT FmSessionHdl is null or Ids is null.
  * \retval - \ref FM_API_ERR_VER_MISMATCH The \ref FM_FIRMWARES_DATA version mismatches the one in library.
  * \retval - \ref FM_API_ERR_GET_IDENTIFIERS_ERR Error occured during Getting indentifiers.
  */
FM_INTERFACE FmAPI_GetDetailedInfos(FmSession* FmSessionHdl, unsigned FpgaSlot, FM_DETAILED_ID_DATA* Id );

/**
  * \brief Gets device dimm information corresponding to the FpgaSlot.
  * \remarks This must be called after \ref FmAPI_StartSession, or an \ref FM_API_ERR_ILLEGAL_INPUT will be returned.
  * \param[in] FmSessionHdl  The Fmtk session handle
  * \param[in] FpgaSlot FPGA Index corresponding to which the Identifiers
  *                            would be fetched
  * \param[in, out]  Ids Device \ref FM_DDR_DATA Identifiers corresponding to FpgaSlot.
  * \return \ref FM_INTERFACE
  * \retval - \ref FM_API_SUCCESS Get indentifiers successfully.
  * \retval - \ref FM_API_ERR_LIB_NOT_INIT The API system had not been initialed.
  * \retval - \ref FM_API_ERR_ILLEGAL_INPUT FmSessionHdl is null or Ids is null.
  * \retval - \ref FM_API_ERR_VER_MISMATCH The \ref FM_FIRMWARES_DATA version mismatches the one in library.
  * \retval - \ref FM_API_ERR_GET_IDENTIFIERS_ERR Error occured during Getting indentifiers.
  */
FM_INTERFACE FmAPI_GetDDRDatas(FmSession* FmSessionHdl, unsigned FpgaSlot, FM_DDR_DATA* Id );


/**
  * \brief Gets OCL device Alarm related information corresponding to the FpgaSlot.
  * \remarks This must be called after \ref FmAPI_StartSession, or an \ref FM_API_ERR_ILLEGAL_INPUT will be returned.
  * \param[in] FmSessionHdl  The Fmtk session handle
  * \param[in] FpgaSlot FPGA Index corresponding to which the Identifiers
  *                            would be fetched
  * \param[in, out]  Ids Device \ref FM_DEVICE_ALARM_DATA Identifiers corresponding to FpgaSlot.
  * \return \ref FM_INTERFACE
  * \retval - \ref FM_API_SUCCESS Get indentifiers successfully.
  * \retval - \ref FM_API_ERR_LIB_NOT_INIT The API system had not been initialed.
  * \retval - \ref FM_API_ERR_ILLEGAL_INPUT FmSessionHdl is null or Ids is null.
  * \retval - \ref FM_API_ERR_VER_MISMATCH The \ref FM_FIRMWARES_DATA version mismatches the one in library.
  * \retval - \ref FM_API_ERR_GET_IDENTIFIERS_ERR Error occured during Getting indentifiers.
  */
FM_INTERFACE FmAPI_GetDeviceAlarm(FmSession* FmSessionHdl, unsigned FpgaSlot, FM_DEVICE_ALARM_DATA* Ids );

/*@}*/


/**
* \defgroup FirmwareModules Firmware APIs
*/
/*@{*/

/**
  * \brief Upgrade the golden firmware corresponding to the FpgaSlot.
  * \remarks This must be called after \ref FmAPI_StartSession, or an \ref FM_API_ERR_ILLEGAL_INPUT will be returned.
  * \param[in] FmSessionHdl  The Fmtk session handle
  * \param[in] FpgaSlot FPGA Index corresponding to which the function will work.
  * \param[in] FilePath   .bin/.zip firmware upgrade file path corresponding to FpgaSlot.
  * \return \ref FM_INTERFACE
  * \retval - \ref FM_API_SUCCESS Upgraded the firmware successfully.
  * \retval - \ref FM_API_ERR_LIB_NOT_INIT The API system had not been initialed.
  * \retval - \ref FM_API_ERR_ILLEGAL_INPUT FmSessionHdl is null or Ids is null.
  * \retval - \ref FM_API_FPGA_STATUS_ERR The FpgaSlot status is disabled.
  * \retval - \ref FM_API_MODULE_SUPPORT_ERR The flash module does not supported in the FpgaSlot shell.
  * \retval - \ref FM_API_ERR_ACQUIRE_DEVICE_MUTEX_ERR Failed when acquiring the multithreads mutex of the FpgaSlot.
  * \retval - \ref FM_API_ERR_UPGRADE_FIRMWARE_ERR Error occured during upgrading.
  */
FM_INTERFACE FmAPI_UpgradeGoldenFirmware(FmSession* FmSessionHdl, unsigned FpgaSlot, const char *FilePath);

/**
  * \brief Upgrade the MCU firmware corresponding to the FpgaSlot.
  * \remarks This must be called after \ref FmAPI_StartSession, or an \ref FM_API_ERR_ILLEGAL_INPUT will be returned.
  * \param[in] FmSessionHdl  The Fmtk session handle
  * \param[in] FpgaSlot FPGA Index corresponding to which the function will work.
  * \param[in] FilePath   .bin/.zip firmware upgrade file path corresponding to FpgaSlot.
  * \return \ref FM_INTERFACE
  * \retval - \ref FM_API_SUCCESS Upgraded the firmware successfully.
  * \retval - \ref FM_API_ERR_LIB_NOT_INIT The API system had not been initialed.
  * \retval - \ref FM_API_ERR_ILLEGAL_INPUT FmSessionHdl is null or Ids is null.
  * \retval - \ref FM_API_FPGA_STATUS_ERR The FpgaSlot status is disabled.
  * \retval - \ref FM_API_MODULE_SUPPORT_ERR The mcu module does not supported in the FpgaSlot shell.
  * \retval - \ref FM_API_ERR_ACQUIRE_DEVICE_MUTEX_ERR Failed when acquiring the multithreads mutex of the FpgaSlot.
  * \retval - \ref FM_API_ERR_UPGRADE_FIRMWARE_ERR Error occured during upgrading.
  */
FM_INTERFACE FmAPI_UpgradeMCUFirmware(FmSession* FmSessionHdl, unsigned FpgaSlot, const char *FilePath);

/**
  * \brief Upgrade the shell firmware corresponding to the FpgaSlot.
  * \remarks This must be called after \ref FmAPI_StartSession, or an \ref FM_API_ERR_ILLEGAL_INPUT will be returned.
  * \param[in] FmSessionHdl  The Fmtk session handle.
  * \param[in] FpgaSlot FPGA Index corresponding to which the function will work.
  * \param[in] FilePath   .bin/.zip firmware upgrade file path corresponding to FpgaSlot.
  * \return \ref FM_INTERFACE
  * \retval - \ref FM_API_SUCCESS Upgraded the firmware successfully.
  * \retval - \ref FM_API_ERR_LIB_NOT_INIT The API system had not been initialed.
  * \retval - \ref FM_API_ERR_ILLEGAL_INPUT FmSessionHdl is null or Ids is null.
  * \retval - \ref FM_API_FPGA_STATUS_ERR The FpgaSlot status is disabled.
  * \retval - \ref FM_API_MODULE_SUPPORT_ERR The flash module does not supported in the FpgaSlot shell.
  * \retval - \ref FM_API_ERR_ACQUIRE_DEVICE_MUTEX_ERR Failed when acquiring the multithreads mutex of the FpgaSlot.
  * \retval - \ref FM_API_ERR_UPGRADE_FIRMWARE_ERR Error occured during upgrading.
  */
FM_INTERFACE FmAPI_UpgradeShellFirmware(FmSession* FmSessionHdl, unsigned FpgaSlot, const char *FilePath);

/**
  * \brief Erase the FPGA firmware corresponding to the FpgaSlot. FPGA will activate golden firmware after \ref FmAPI_ActivateShell.
  * \remarks This must be called after \ref FmAPI_StartSession, or an \ref FM_API_ERR_ILLEGAL_INPUT will be returned.
  * \param[in] FmSessionHdl  The Fmtk session handle.
  * \param[in] FpgaSlot FPGA Index corresponding to which the function will work.
  * \return \ref FM_INTERFACE
  * \retval - \ref FM_API_SUCCESS Upgraded the firmware successfully.
  * \retval - \ref FM_API_ERR_LIB_NOT_INIT The API system had not been initialed.
  * \retval - \ref FM_API_ERR_ILLEGAL_INPUT FmSessionHdl is null or Ids is null.
  * \retval - \ref FM_API_FPGA_STATUS_ERR The FpgaSlot status is disabled.
  * \retval - \ref FM_API_MODULE_SUPPORT_ERR The flash module does not supported in the FpgaSlot shell.
  * \retval - \ref FM_API_ERR_ACQUIRE_DEVICE_MUTEX_ERR Failed when acquiring the multithreads mutex of the FpgaSlot.
  * \retval - \ref FM_API_ERR_UPGRADE_FIRMWARE_ERR Error occured during upgrading.
  */
FM_INTERFACE FmAPI_EraseHWFirmware(FmSession* FmSessionHdl, unsigned FpgaSlot);

/**
  * \brief Get the FPGA upgrade progress corresponding to the FpgaSlot.
  * \remarks This must be called after one of following functions, or progress will be null string:
  *      -\ref FmAPI_UpgradeGoldenFirmware
  *      -\ref FmAPI_UpgradeMCUFirmware
  *      -\ref FmAPI_UpgradeShellFirmware
  *      -\ref FmAPI_EraseHWFirmware
  * \param[in] FmSessionHdl  The Fmtk session handle
  * \param[in] FpgaSlot FPGA Index corresponding to which the Identifiers
  *                            would be fetched
  * \param[in, out]  progress Upgrading progress messages.
  * \return no return value;
  */
void FmAPI_GetUpgradeProgress(FmSession* FmSessionHdl, unsigned FpgaSlot, char* progress);

/**
  * \brief Activate the FPGA firmware corresponding to the FpgaSlot. Golden firmware will be activated after \ref FmAPI_EraseHWFirmware.
  * \remarks This must be called after \ref FmAPI_StartSession, or an \ref FM_API_ERR_ILLEGAL_INPUT will be returned.
  * \param[in] FmSessionHdl  The Fmtk session handle
  * \param[in] FpgaSlot FPGA Index corresponding to which the function will work.
  * \return \ref FM_INTERFACE
  * \retval - \ref FM_API_SUCCESS Upgraded the firmware successfully.
  * \retval - \ref FM_API_ERR_LIB_NOT_INIT The API system had not been initialed.
  * \retval - \ref FM_API_ERR_ILLEGAL_INPUT FmSessionHdl is null or Ids is null.
  * \retval - \ref FM_API_FPGA_STATUS_ERR The FpgaSlot status is disabled.
  * \retval - \ref FM_API_MODULE_SUPPORT_ERR The flash module does not supported in the FpgaSlot shell.
  * \retval - \ref FM_API_ERR_ACQUIRE_DEVICE_MUTEX_ERR Failed when acquiring the multithreads mutex of the FpgaSlot.
  * \retval - \ref FM_API_ERR_UPGRADE_FIRMWARE_ERR Error occured during activating.
  */
FM_INTERFACE FmAPI_ActivateShell(FmSession* FmSessionHdl, unsigned FpgaSlot);

/**
  * \brief Get the FPGA activating progress corresponding to the FpgaSlot.
  * \remarks This must be called after one of following functions, or progress will be null string:
  *      -\ref FmAPI_ActivateShell
  * \param[in] FmSessionHdl  The Fmtk session handle
  * \param[in] FpgaSlot FPGA Index corresponding to which the function will work
  * \param[in, out]  progress  Activating progress messages.
  * \return no return value.
  */
void FmAPI_GetActivateProgress(FmSession* FmSessionHdl, unsigned FpgaSlot, char* progress);

/*@}*/


/**
* \defgroup ProtectionModules Configuration APIs
*/
/*@{*/

/**
  * \brief Set over power protection feature on/off corresponding to the FpgaSlot.
  * \brief This function supports only Huawei d518 and d520 logic.
  * \remarks This must be called after \ref FmAPI_StartSession, or an \ref FM_API_ERR_ILLEGAL_INPUT will be returned.
  * \param[in] FmSessionHdl  The Fmtk session handle
  * \param[in] FpgaSlot FPGA Index corresponding to which the function will work.
  * \param[in] SwitchStatus 0 for set the feature on; 1 for set the feature off.
  * \return \ref FM_INTERFACE
  * \retval - \ref FM_API_SUCCESS Upgraded the firmware successfully.
  * \retval - \ref FM_API_ERR_LIB_NOT_INIT The API system had not been initialed.
  * \retval - \ref FM_API_ERR_ILLEGAL_INPUT FmSessionHdl is null or Ids is null.
  * \retval - \ref FM_API_FPGA_STATUS_ERR The FpgaSlot status is disabled.
  * \retval - \ref FM_API_MODULE_SUPPORT_ERR The mcu module does not supported in the FpgaSlot shell.
  * \retval - \ref FM_API_ERR_ACQUIRE_DEVICE_MUTEX_ERR Failed when acquiring the multithreads mutex of the FpgaSlot.
  * \retval - \ref FM_API_ERR_RESET_FPGA_ERR Error occured during setting feature status.
  */
FM_INTERFACE FmAPI_SetOverPowerProtection(FmSession* FmSessionHdl, unsigned FpgaSlot, unsigned char SwitchStatus);

/**
  * \brief Get over power protection feature status corresponding to the FpgaSlot.
  * \brief This function supports only Huawei d518 and d520 logic.
  * \remarks This must be called after \ref FmAPI_StartSession, or an \ref FM_API_ERR_ILLEGAL_INPUT will be returned.
  * \param[in] FmSessionHdl  The Fmtk session handle
  * \param[in] FpgaSlot FPGA Index corresponding to which the function will work.
  * \param[in, out]  SwitchStatus 0 for set the feature on; 1 for set the feature off.
  * \return \ref FM_INTERFACE
  * \retval - \ref FM_API_SUCCESS Upgraded the firmware successfully.
  * \retval - \ref FM_API_ERR_LIB_NOT_INIT The API system had not been initialed.
  * \retval - \ref FM_API_ERR_ILLEGAL_INPUT FmSessionHdl is null or Ids is null.
  * \retval - \ref FM_API_FPGA_STATUS_ERR The FpgaSlot status is disabled.
  * \retval - \ref FM_API_MODULE_SUPPORT_ERR The mcu module does not supported in the FpgaSlot shell.
  * \retval - \ref FM_API_ERR_ACQUIRE_DEVICE_MUTEX_ERR Failed when acquiring the multithreads mutex of the FpgaSlot.
  * \retval - \ref FM_API_ERR_RESET_FPGA_ERR Error occured during getting feature status.
  */
FM_INTERFACE FmAPI_GetOverPowerProtection(FmSession* FmSessionHdl, unsigned FpgaSlot, unsigned char *SwitchStatus);
/*@}*/

/**
* \defgroup EepromModules EEPROM APIs
*/
/*@{*/
/**
  * \brief read mac address corresponding to the FpgaSlot.
  * \remarks This must be called after \ref FmAPI_StartSession, or an \ref FM_API_ERR_ILLEGAL_INPUT will be returned.
  * \param[in] FmSessionHdl  The Fmtk session handle
  * \param[in] FpgaSlot FPGA Index corresponding to which the function will work.
  * \param[in] MacIndex Mac address Index.
  * \param[in, out]  MacInfo Array of Mac Address.
  * \return \ref FM_INTERFACE
  * \retval - \ref FM_API_SUCCESS function works successfully.
  * \retval - \ref FM_API_ERR_LIB_NOT_INIT The API system had not been initialed.
  * \retval - \ref FM_API_ERR_ILLEGAL_INPUT FmSessionHdl is null or Ids is null.
  * \retval - \ref FM_API_FPGA_STATUS_ERR The FpgaSlot status is disabled.
  * \retval - \ref FM_API_MODULE_SUPPORT_ERR The eeprom module does not supported in the FpgaSlot shell.
  * \retval - \ref FM_API_ERR_ACQUIRE_DEVICE_MUTEX_ERR Failed when acquiring the multithreads mutex of the FpgaSlot.
  * \retval - \ref FM_API_ERR_READ_MAC_ADDR_ERR Error occured during reading mac.
  */
FM_INTERFACE FmAPI_ReadHexMacFromEeprom(FmSession* FmSessionHdl, unsigned FpgaSlot, unsigned MacIndex, unsigned char* MacInfo);

/**
  * \brief write mac address corresponding to the FpgaSlot.
  * \remarks This must be called after \ref FmAPI_StartSession, or an \ref FM_API_ERR_ILLEGAL_INPUT will be returned.
  * \param[in] FmSessionHdl  The Fmtk session handle
  * \param[in] FpgaSlot FPGA Index corresponding to which the function will work.
  * \param[in] MacIndex Mac address Index.
  * \param[in] MacInfo New Mac Address string, delimited with ':'.
  * \return \ref FM_INTERFACE
  * \retval - \ref FM_API_SUCCESS function works successfully.
  * \retval - \ref FM_API_ERR_LIB_NOT_INIT The API system had not been initialed.
  * \retval - \ref FM_API_ERR_ILLEGAL_INPUT FmSessionHdl is null or Ids is null.
  * \retval - \ref FM_API_FPGA_STATUS_ERR The FpgaSlot status is disabled.
  * \retval - \ref FM_API_MODULE_SUPPORT_ERR The eeprom module does not supported in the FpgaSlot shell.
  * \retval - \ref FM_API_ERR_ACQUIRE_DEVICE_MUTEX_ERR Failed when acquiring the multithreads mutex of the FpgaSlot.
  * \retval - \ref FM_API_ERR_WRITE_MAC_ADDR_ERR Error occured during writing mac address.
  */
FM_INTERFACE FmAPI_WriteHexMacFromEeprom(FmSession* FmSessionHdl, unsigned FpgaSlot, unsigned MacIndex, char *MacInfo);

/**
  * \brief collect mcu logs.
  * \remarks This must be called after \ref FmAPI_StartSession, or an \ref FM_API_ERR_ILLEGAL_INPUT will be returned.
  * \param[in] FmSessionHdl  The Fmtk session handle
  * \param[in] FpgaSlot FPGA Index corresponding to which the function will work.
  * \param[in] SavePath Log save path.
  * \return \ref FM_INTERFACE
  * \retval - \ref FM_API_SUCCESS function works successfully.
  * \retval - \ref FM_API_ERR_LIB_NOT_INIT The API system had not been initialed.
  * \retval - \ref FM_API_ERR_ILLEGAL_INPUT FmSessionHdl is null or Ids is null.
  * \retval - \ref FM_API_FPGA_STATUS_ERR The FpgaSlot status is disabled.
  * \retval - \ref FM_API_MODULE_SUPPORT_ERR The eeprom module does not supported in the FpgaSlot shell.
  * \retval - \ref FM_API_ERR_ACQUIRE_DEVICE_MUTEX_ERR Failed when acquiring the multithreads mutex of the FpgaSlot.
  * \retval - \ref FM_API_ERR_COLLECT_MCU_LOG_ERR Failed when collecting mcu logs.

  */

FM_INTERFACE FmAPI_GetMcuLog(FmSession* FmSessionHdl, unsigned FpgaSlot, char *SavePath);


/**
  * \brief read eeprom data corresponding to the FpgaSlot.
  * \remarks This must be called after \ref FmAPI_StartSession, or an \ref FM_API_ERR_ILLEGAL_INPUT will be returned.
  * \param[in] FmSessionHdl  The Fmtk session handle
  * \param[in] FpgaSlot FPGA Index corresponding to which the function will work.
  * \param[in] Addr Hexadecimal Address from which reading start.
  * \param[in] Length Reading data length in number of bytes.
  * \param[in, out]  EepromData Data in string.
  * \return \ref FM_INTERFACE
  * \retval - \ref FM_API_SUCCESS function works successfully.
  * \retval - \ref FM_API_ERR_LIB_NOT_INIT The API system had not been initialed.
  * \retval - \ref FM_API_ERR_ILLEGAL_INPUT FmSessionHdl is null or Ids is null.
  * \retval - \ref FM_API_FPGA_STATUS_ERR The FpgaSlot status is disabled.
  * \retval - \ref FM_API_MODULE_SUPPORT_ERR The eeprom module does not supported in the FpgaSlot shell.
  * \retval - \ref FM_API_ERR_ACQUIRE_DEVICE_MUTEX_ERR Failed when acquiring the multithreads mutex of the FpgaSlot.
  * \retval - \ref FM_API_ERR_RESET_FPGA_ERR Error occured during reading eeprom data.
  */
FM_INTERFACE FmAPI_ReadDataFromEeprom(FmSession* FmSessionHdl, unsigned FpgaSlot, unsigned Addr, unsigned Length, char* EepromData);

/**
  * \brief write eeprom data corresponding to the FpgaSlot.
  * \remarks This must be called after \ref FmAPI_StartSession, or an \ref FM_API_ERR_ILLEGAL_INPUT will be returned.
  * \param[in] FmSessionHdl  The Fmtk session handle
  * \param[in] FpgaSlot FPGA Index corresponding to which the function will work.
  * \param[in] Addr Hexadecimal Address from which writing start.
  * \param[in] EepromData Data in string.
  * \return \ref FM_INTERFACE
  * \retval - \ref FM_API_SUCCESS function works successfully.
  * \retval - \ref FM_API_ERR_LIB_NOT_INIT The API system had not been initialed.
  * \retval - \ref FM_API_ERR_ILLEGAL_INPUT FmSessionHdl is null or Ids is null.
  * \retval - \ref FM_API_FPGA_STATUS_ERR The FpgaSlot status is disabled.
  * \retval - \ref FM_API_MODULE_SUPPORT_ERR The eeprom module does not supported in the FpgaSlot shell.
  * \retval - \ref FM_API_ERR_ACQUIRE_DEVICE_MUTEX_ERR Failed when acquiring the multithreads mutex of the FpgaSlot.
  * \retval - \ref FM_API_ERR_RESET_FPGA_ERR Error occured during writing eeprom data.
  */
FM_INTERFACE FmAPI_WriteDataToEeprom(FmSession* FmSessionHdl, unsigned FpgaSlot, unsigned Addr, char * EepromData);
/*@}*/

/**
* \defgroup FunctionModules Reload APIs
*/
/*@{*/
/**
  * \brief warmboot corresponding to the FpgaSlot.
  * \remarks This must be called after \ref FmAPI_StartSession, or an \ref FM_API_ERR_ILLEGAL_INPUT will be returned.
  * \param[in] FmSessionHdl  The Fmtk session handle
  * \param[in] FpgaSlot FPGA Index corresponding to which the function will work.
  * \param[in] FlashSector Flash sector index in which boot will reload from.
  * \return \ref FM_INTERFACE
  * \retval - \ref FM_API_SUCCESS function works successfully.
  * \retval - \ref FM_API_ERR_LIB_NOT_INIT The API system had not been initialed.
  * \retval - \ref FM_API_ERR_ILLEGAL_INPUT FmSessionHdl is null or Ids is null.
  * \retval - \ref FM_API_FPGA_STATUS_ERR The FpgaSlot status is disabled.
  * \retval - \ref FM_API_MODULE_SUPPORT_ERR The ICAP module does not supported in the FpgaSlot shell.
  * \retval - \ref FM_API_ERR_ACQUIRE_DEVICE_MUTEX_ERR Failed when acquiring the multithreads mutex of the FpgaSlot.
  * \retval - \ref FM_API_ERR_UPGRADE_FIRMWARE_ERR Error occured during reloading.
  */
FM_INTERFACE FmAPI_ReloadFirmware(FmSession* FmSessionHdl, unsigned FpgaSlot, unsigned FlashSector);

/**
  * \brief Get the FPGA reloading progress corresponding to the FpgaSlot.
  * \remarks This must be called after one of following functions, or progress will be null string:
  *      -\ref FmAPI_ReloadFirmware
  * \param[in] FmSessionHdl  The Fmtk session handle
  * \param[in] FpgaSlot FPGA Index corresponding to which the function will work
  * \param[in, out]  progress  reloading progress messages.
  * \return no return value.
  */
void FmAPI_GetReloadProgress(FmSession* FmSessionHdl, unsigned FpgaSlot, char* progress);
/*@}*/

/**
* \defgroup ResetModules Reset APIs
*/
/*@{*/

/**
  * \brief reset FPGA microchip corresponding to the FpgaSlot.
  * \remarks This must be called after \ref FmAPI_StartSession, or an \ref FM_API_ERR_ILLEGAL_INPUT will be returned.
  * \param[in] FmSessionHdl  The Fmtk session handle
  * \param[in] FpgaSlot FPGA Index corresponding to which the function will work.
  * \param[in] ResetMode 0 for both PCIe RST and Global RST pin enable; 1 for only PCIe RST  pin enables; 2 for only Global RST pin enables.
  * \param[in, out]  ResetResult 0 for MCU does't receive the signal;1 for reset successfully; 2 for MCU is busy.
  * \return \ref FM_INTERFACE
  * \retval - \ref FM_API_SUCCESS function works successfully.
  * \retval - \ref FM_API_ERR_LIB_NOT_INIT The API system had not been initialed.
  * \retval - \ref FM_API_ERR_ILLEGAL_INPUT FmSessionHdl is null or Ids is null.
  * \retval - \ref FM_API_FPGA_STATUS_ERR The FpgaSlot status is disabled.
  * \retval - \ref FM_API_MODULE_SUPPORT_ERR The MCU module does not supported in the FpgaSlot shell.
  * \retval - \ref FM_API_ERR_ACQUIRE_DEVICE_MUTEX_ERR Failed when acquiring the multithreads mutex of the FpgaSlot.
  * \retval - \ref FM_API_ERR_RESET_FPGA_ERR Error occured during resetting fpga.
  */
FM_INTERFACE FmAPI_ResetFpga(FmSession* FmSessionHdl, unsigned FpgaSlot, unsigned char ResetMode, unsigned char * ResetResult);
/*@}*/

/**
* \defgroup SecureModules Secure Chip APIs
*/
/*@{*/
/**
  * \brief reset secure chip corresponding to the FpgaSlot.
  * \remarks This must be called after \ref FmAPI_StartSession, or an \ref FM_API_ERR_ILLEGAL_INPUT will be returned.
  * \param[in] FmSessionHdl  The Fmtk session handle
  * \param[in] FpgaSlot FPGA Index corresponding to which the function will be called.
  * \param[in] ChipIndex Security chip index corresponding to which the function will work.
  * \return \ref FM_INTERFACE
  * \retval - \ref FM_API_SUCCESS function works successfully.
  * \retval - \ref FM_API_ERR_LIB_NOT_INIT The API system had not been initialed.
  * \retval - \ref FM_API_ERR_ILLEGAL_INPUT FmSessionHdl is null or Ids is null.
  * \retval - \ref FM_API_FPGA_STATUS_ERR The FpgaSlot status is disabled.
  * \retval - \ref FM_API_MODULE_SUPPORT_ERR The secure module does not supported in the FpgaSlot shell.
  * \retval - \ref FM_API_ERR_ACQUIRE_DEVICE_MUTEX_ERR Failed when acquiring the multithreads mutex of the FpgaSlot.
  * \retval - \ref FM_API_SECURE_OPERATE_ERR Error occured during resetting secure chip.
  */
FM_INTERFACE FmAPI_SecureReset(FmSession* FmSessionHdl, unsigned FpgaSlot, ENUM_SECURE_CHIP ChipIndex);

/**
  * \brief secure chip echo function corresponding to the FpgaSlot.
  * \remarks This must be called after \ref FmAPI_StartSession, or an \ref FM_API_ERR_ILLEGAL_INPUT will be returned.
  * \param[in] FmSessionHdl  The Fmtk session handle
  * \param[in] FpgaSlot FPGA Index corresponding to which the function will be called.
  * \param[in] ChipIndex Security chip index corresponding to which the function will work.
  * \param[in] TLVIn Input data in TLV format.
  * \param[in, out] TLVOut Output data in TLV format.
  * \return \ref FM_INTERFACE
  * \retval - \ref FM_API_SUCCESS function works successfully.
  * \retval - \ref FM_API_ERR_LIB_NOT_INIT The API system had not been initialed.
  * \retval - \ref FM_API_ERR_ILLEGAL_INPUT FmSessionHdl is null or Ids is null.
  * \retval - \ref FM_API_FPGA_STATUS_ERR The FpgaSlot status is disabled.
  * \retval - \ref FM_API_MODULE_SUPPORT_ERR The secure module does not supported in the FpgaSlot shell.
  * \retval - \ref FM_API_ERR_ACQUIRE_DEVICE_MUTEX_ERR Failed when acquiring the multithreads mutex of the FpgaSlot.
  * \retval - \ref FM_API_SECURE_OPERATE_ERR Error occured during resetting secure chip.
  */
FM_INTERFACE FmAPI_SecureEcho(FmSession* FmSessionHdl, unsigned FpgaSlot, ENUM_SECURE_CHIP ChipIndex,
                                                SECURE_TLV_BUFFER_STRU TLVIn, SECURE_TLV_BUFFER_STRU *TLVOut);
/**
  * \brief query any information by tags of the secure chip corresponding to the FpgaSlot.
  * \remarks This must be called after \ref FmAPI_StartSession, or an \ref FM_API_ERR_ILLEGAL_INPUT will be returned.
  * \param[in] FmSessionHdl  The Fmtk session handle
  * \param[in] FpgaSlot FPGA Index corresponding to which the function will be called.
  * \param[in] ChipIndex Secure chip index corresponding to which the function will work.
  * \param[in] attributeTag Secure chip attribute.(0x8 for , 0xa for 0xb for 0x11 for 0x12 for )
  * \param[in, out] TLVOut Output data in TLV format.
  * \return \ref FM_INTERFACE
  * \retval - \ref FM_API_SUCCESS function works successfully.
  * \retval - \ref FM_API_ERR_LIB_NOT_INIT The API system had not been initialed.
  * \retval - \ref FM_API_ERR_ILLEGAL_INPUT FmSessionHdl is null or Ids is null.
  * \retval - \ref FM_API_FPGA_STATUS_ERR The FpgaSlot status is disabled.
  * \retval - \ref FM_API_MODULE_SUPPORT_ERR The secure module does not supported in the FpgaSlot shell.
  * \retval - \ref FM_API_ERR_ACQUIRE_DEVICE_MUTEX_ERR Failed when acquiring the multithreads mutex of the FpgaSlot.
  * \retval - \ref FM_API_SECURE_OPERATE_ERR Error occured during resetting secure chip.
  */
FM_INTERFACE FmAPI_SecureQuery(FmSession* FmSessionHdl, unsigned FpgaSlot, ENUM_SECURE_CHIP  ChipIndex,
           unsigned attributeTag, SECURE_TLV_BUFFER_STRU *TLVOut);
/**
  * \brief query information in TLV format of the secure chip corresponding to the FpgaSlot.
  * \remarks This must be called after \ref FmAPI_StartSession, or an \ref FM_API_ERR_ILLEGAL_INPUT will be returned.
  * \param[in] FmSessionHdl  The Fmtk session handle
  * \param[in] FpgaSlot FPGA Index corresponding to which the function will be called.
  * \param[in] ChipIndex Security chip index corresponding to which the function will work.
  * \param[in, out] TLVOut Output data in TLV format.
  * \return \ref FM_INTERFACE
  * \retval - \ref FM_API_SUCCESS function works successfully.
  * \retval - \ref FM_API_ERR_LIB_NOT_INIT The API system had not been initialed.
  * \retval - \ref FM_API_ERR_ILLEGAL_INPUT FmSessionHdl is null or Ids is null.
  * \retval - \ref FM_API_FPGA_STATUS_ERR The FpgaSlot status is disabled.
  * \retval - \ref FM_API_MODULE_SUPPORT_ERR The secure module does not supported in the FpgaSlot shell.
  * \retval - \ref FM_API_ERR_ACQUIRE_DEVICE_MUTEX_ERR Failed when acquiring the multithreads mutex of the FpgaSlot.
  * \retval - \ref FM_API_SECURE_OPERATE_ERR Error occured during resetting secure chip.
  */
FM_INTERFACE FmAPI_SecureGetChipInfo(FmSession* FmSessionHdl, unsigned FpgaSlot, ENUM_SECURE_CHIP  ChipIndex,
                                                      SECURE_TLV_BUFFER_STRU *TLVOut);
/**
  * \brief get public key stored in the secure chip corresponding to the FpgaSlot.
  * \remarks This must be called after \ref FmAPI_StartSession, or an \ref FM_API_ERR_ILLEGAL_INPUT will be returned.
  * \param[in] FmSessionHdl  The Fmtk session handle
  * \param[in] FpgaSlot FPGA Index corresponding to which the function will be called.
  * \param[in] ChipIndex Security chip index corresponding to which the function will work.
  * \param[in, out]  PublicKey public key data \ref SECURE_PUBLICKKEY_BUFFER_STRU
  * \return \ref FM_INTERFACE
  * \retval - \ref FM_API_SUCCESS function works successfully.
  * \retval - \ref FM_API_ERR_LIB_NOT_INIT The API system had not been initialed.
  * \retval - \ref FM_API_ERR_ILLEGAL_INPUT FmSessionHdl is null or Ids is null.
  * \retval - \ref FM_API_FPGA_STATUS_ERR The FpgaSlot status is disabled.
  * \retval - \ref FM_API_MODULE_SUPPORT_ERR The secure module does not supported in the FpgaSlot shell.
  * \retval - \ref FM_API_ERR_ACQUIRE_DEVICE_MUTEX_ERR Failed when acquiring the multithreads mutex of the FpgaSlot.
  * \retval - \ref FM_API_SECURE_OPERATE_ERR Error occured during resetting secure chip.
  */
FM_INTERFACE FmAPI_SecureGetPublicKey(FmSession* FmSessionHdl, unsigned FpgaSlot, ENUM_SECURE_CHIP  ChipIndex,
                                                     SECURE_PUBLICKKEY_BUFFER_STRU *PublicKey);
/**
  * \brief Perform digital signature in the secure chip after hash256 algrithom corresponding to the FpgaSlot.
  * \remarks This must be called after \ref FmAPI_StartSession, or an \ref FM_API_ERR_ILLEGAL_INPUT will be returned.
  * \param[in] FmSessionHdl  The Fmtk session handle
  * \param[in] FpgaSlot FPGA Index corresponding to which the function will be called.
  * \param[in] ChipIndex Security chip index corresponding to which the function will work.
  * \param[in] TLVIn
  * \param[in, out] TLVOut Output data in TLV format.
  * \return \ref FM_INTERFACE
  * \retval - \ref FM_API_SUCCESS function works successfully.
  * \retval - \ref FM_API_ERR_LIB_NOT_INIT The API system had not been initialed.
  * \retval - \ref FM_API_ERR_ILLEGAL_INPUT FmSessionHdl is null or Ids is null.
  * \retval - \ref FM_API_FPGA_STATUS_ERR The FpgaSlot status is disabled.
  * \retval - \ref FM_API_MODULE_SUPPORT_ERR The secure module does not supported in the FpgaSlot shell.
  * \retval - \ref FM_API_ERR_ACQUIRE_DEVICE_MUTEX_ERR Failed when acquiring the multithreads mutex of the FpgaSlot.
  * \retval - \ref FM_API_SECURE_OPERATE_ERR Error occured during resetting secure chip.
  */
FM_INTERFACE FmAPI_SecureSignature(FmSession* FmSessionHdl, unsigned FpgaSlot, ENUM_SECURE_CHIP  ChipIndex,
                                                      SECURE_TLV_BUFFER_STRU TLVIn, SECURE_TLV_BUFFER_STRU *TLVOut);
/**
  * \brief read data from the secure chip corresponding to the FpgaSlot.
  * \remarks This must be called after \ref FmAPI_StartSession, or an \ref FM_API_ERR_ILLEGAL_INPUT will be returned.
  * \param[in] FmSessionHdl  The Fmtk session handle
  * \param[in] FpgaSlot FPGA Index corresponding to which the function will be called.
  * \param[in] ChipIndex Security chip index corresponding to which the function will work.
  * \param[in] ZoneIdx Index of the secure chip memory zone.
  * \param[in] Addr  memory address from which the data is read.
  * \param[in] Length Bytes number
  * \param[in, out] TLVOut Output data in TLV format.
  * \return \ref FM_INTERFACE
  * \retval - \ref FM_API_SUCCESS function works successfully.
  * \retval - \ref FM_API_ERR_LIB_NOT_INIT The API system had not been initialed.
  * \retval - \ref FM_API_ERR_ILLEGAL_INPUT FmSessionHdl is null or Ids is null.
  * \retval - \ref FM_API_FPGA_STATUS_ERR The FpgaSlot status is disabled.
  * \retval - \ref FM_API_MODULE_SUPPORT_ERR The secure module does not supported in the FpgaSlot shell.
  * \retval - \ref FM_API_ERR_ACQUIRE_DEVICE_MUTEX_ERR Failed when acquiring the multithreads mutex of the FpgaSlot.
  * \retval - \ref FM_API_SECURE_OPERATE_ERR Error occured during resetting secure chip.
  */
FM_INTERFACE FmAPI_SecureRead(FmSession* FmSessionHdl, unsigned FpgaSlot, ENUM_SECURE_CHIP  ChipIndex,
             unsigned ZoneIdx, unsigned Addr, unsigned Length, SECURE_TLV_BUFFER_STRU *TLVOut);
/**
  * \brief write a certificate into the secure chip corresponding to the FpgaSlot.
  * \remarks This must be called after \ref FmAPI_StartSession, or an \ref FM_API_ERR_ILLEGAL_INPUT will be returned.
  * \param[in] FmSessionHdl  The Fmtk session handle
  * \param[in] FpgaSlot FPGA Index corresponding to which the function will be called.
  * \param[in] ChipIndex Security chip index corresponding to which the function will work.
  * \param[in] ZoneIdx Index of the secure chip memory zone.
  * \param[in] VendorID  Identification of the certificate vendor.
  * \param[in] CertId    Identification of the certificate.
  * \param[in] CertTLVIn Certificate data in TLV format \ref SECURE_TLV_BUFFER_STRU.
  * \return \ref FM_INTERFACE
  * \retval - \ref FM_API_SUCCESS function works successfully.
  * \retval - \ref FM_API_ERR_LIB_NOT_INIT The API system had not been initialed.
  * \retval - \ref FM_API_ERR_ILLEGAL_INPUT FmSessionHdl is null or Ids is null.
  * \retval - \ref FM_API_FPGA_STATUS_ERR The FpgaSlot status is disabled.
  * \retval - \ref FM_API_MODULE_SUPPORT_ERR The secure module does not supported in the FpgaSlot shell.
  * \retval - \ref FM_API_ERR_ACQUIRE_DEVICE_MUTEX_ERR Failed when acquiring the multithreads mutex of the FpgaSlot.
  * \retval - \ref FM_API_SECURE_OPERATE_ERR Error occured during resetting secure chip.
  */
FM_INTERFACE FmAPI_SecureWriteCert(FmSession* FmSessionHdl, unsigned FpgaSlot, ENUM_SECURE_CHIP  ChipIndex,
                                             unsigned ZoneIdx, unsigned short VendorID, unsigned short CertId, SECURE_TLV_BUFFER_STRU CertTLVIn);

/**
  * \brief secure chip echo corresponding to the FpgaSlot.
  * \remarks This must be called after \ref FmAPI_StartSession, or an \ref FM_API_ERR_ILLEGAL_INPUT will be returned.
  * \param[in] FmSessionHdl  The Fmtk session handle
  * \param[in] FpgaSlot FPGA Index corresponding to which the function will be called.
  * \param[in] ChipIndex Security chip index corresponding to which the function will work.
  * \param[in] ZoneIdx Index of the secure chip memory zone.
  * \param[in, out] CertTLVOut Certificate data in TLV format \ref SECURE_TLV_BUFFER_STRU.
  * \return \ref FM_INTERFACE
  * \retval - \ref FM_API_SUCCESS function works successfully.
  * \retval - \ref FM_API_ERR_LIB_NOT_INIT The API system had not been initialed.
  * \retval - \ref FM_API_ERR_ILLEGAL_INPUT FmSessionHdl is null or Ids is null.
  * \retval - \ref FM_API_FPGA_STATUS_ERR The FpgaSlot status is disabled.
  * \retval - \ref FM_API_MODULE_SUPPORT_ERR The secure module does not supported in the FpgaSlot shell.
  * \retval - \ref FM_API_ERR_ACQUIRE_DEVICE_MUTEX_ERR Failed when acquiring the multithreads mutex of the FpgaSlot.
  * \retval - \ref FM_API_SECURE_OPERATE_ERR Error occured during resetting secure chip.
  */
FM_INTERFACE FmAPI_SecureReadCert(FmSession* FmSessionHdl, unsigned FpgaSlot, ENUM_SECURE_CHIP  ChipIndex,
                                                      unsigned ZoneIdx, SECURE_TLV_BUFFER_STRU *CertTLVOut);
/**
  * \brief search the certificate by vender ID in the secure chip echo corresponding to the FpgaSlot.
  * \remarks This must be called after \ref FmAPI_StartSession, or an \ref FM_API_ERR_ILLEGAL_INPUT will be returned.
  * \param[in] FmSessionHdl  The Fmtk session handle
  * \param[in] FpgaSlot FPGA Index corresponding to which the function will be called.
  * \param[in] ChipIndex Security chip index corresponding to which the function will work.
  * \param[in] VendorID  Identification of the certificate vendor.
  * \param[in, out] CertInfo Output data of the certificate info.
  * \return \ref FM_INTERFACE
  * \retval - \ref FM_API_SUCCESS function works successfully.
  * \retval - \ref FM_API_ERR_LIB_NOT_INIT The API system had not been initialed.
  * \retval - \ref FM_API_ERR_ILLEGAL_INPUT FmSessionHdl is null or Ids is null.
  * \retval - \ref FM_API_FPGA_STATUS_ERR The FpgaSlot status is disabled.
  * \retval - \ref FM_API_MODULE_SUPPORT_ERR The secure module does not supported in the FpgaSlot shell.
  * \retval - \ref FM_API_ERR_ACQUIRE_DEVICE_MUTEX_ERR Failed when acquiring the multithreads mutex of the FpgaSlot.
  * \retval - \ref FM_API_SECURE_OPERATE_ERR Error occured during resetting secure chip.
  */
FM_INTERFACE FmAPI_SecureSearchCertByVID(FmSession* FmSessionHdl, unsigned FpgaSlot, ENUM_SECURE_CHIP  ChipIndex,
                                                      unsigned short VendorID, SECURE_CERT_KEY_INFO_STRU *CertInfo);

/**
  * \brief search the certificate by vender ID in the secure chip echo corresponding to the FpgaSlot.
  * \remarks This must be called after \ref FmAPI_StartSession, or an \ref FM_API_ERR_ILLEGAL_INPUT will be returned.
  * \param[in] FmSessionHdl  The Fmtk session handle
  * \param[in] FpgaSlot FPGA Index corresponding to which the function will be called.
  * \param[in] ChipIndex Security chip index corresponding to which the function will work.
  * \param[in] VendorID  Identification of the certificate vendor.
  * \param[in] CertID    Identification of the certificate.
  * \param[in, out] CertInfo Output data of the certificate info.
  * \return \ref FM_INTERFACE
  * \retval - \ref FM_API_SUCCESS function works successfully.
  * \retval - \ref FM_API_ERR_LIB_NOT_INIT The API system had not been initialed.
  * \retval - \ref FM_API_ERR_ILLEGAL_INPUT FmSessionHdl is null or Ids is null.
  * \retval - \ref FM_API_FPGA_STATUS_ERR The FpgaSlot status is disabled.
  * \retval - \ref FM_API_MODULE_SUPPORT_ERR The secure module does not supported in the FpgaSlot shell.
  * \retval - \ref FM_API_ERR_ACQUIRE_DEVICE_MUTEX_ERR Failed when acquiring the multithreads mutex of the FpgaSlot.
  * \retval - \ref FM_API_SECURE_OPERATE_ERR Error occured during resetting secure chip.
  */
FM_INTERFACE FmAPI_SecureSearchCertByVIDandCID(FmSession* FmSessionHdl, unsigned FpgaSlot, ENUM_SECURE_CHIP  ChipIndex,
                                                    unsigned short VendorID, unsigned short CertID, SECURE_CERT_KEY_INFO_STRU *CertInfo);

/*@}*/


#ifdef	__cplusplus
}
#endif

#endif
