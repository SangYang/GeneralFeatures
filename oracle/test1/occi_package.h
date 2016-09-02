#ifndef SSY_OCCI_PACKAGE_H
#define SSY_OCCI_PACKAGE_H


#include "device_config.h"

typedef struct {
	char m_unid[64];
	char m_areaid[64];
	char m_areaName[64];
	char m_deviceid[64];
	char m_deviceName[64];
	char m_deviceIP[32];
	char m_bayid[64];
	char m_bayName[64];
	char m_bayAddress[64];
	char m_bayPicIP[32];
	char m_dateTime[32];
	char m_plateType[8];
	char m_plateColor[8];
	char m_plateNo[16];
	char m_carSpeed[8];
	char m_carType[8];
	char m_carColor[8];
	char m_picPath[256];
	char m_roadNo[8];
} t_OcciResult;

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

	void* OCCI_CreateConnection(const char *username, const char *password, const char *connectString);
	void OCCI_DestroyConnection(const void *statement);
	int OCCI_GetCarPassIndex();
	void OCCI_QueryCarPass(const void *statement, 
		const int carPassIndex, 
		const char *recordPath,
		const t_CameraConfig *cameraConfig, 
		void (*DealwithQuery)(t_OcciResult *occiResult, const t_CameraConfig *cameraConfig));
	void OCCI_QueryBaseArea(const void *statement, const char *areaid, char *areaName);
	void OCCI_QueryBaseDevice(const void *statement, const char *deviceid, char *deviceName, char *deviceIP);
	void OCCI_QueryBaseBayonet(const void *statement, const char *bayid, char *bayName, char *bayAddress, char *bayPicIP);
	void PrintOcciResult(const t_OcciResult *occiResult);

#ifdef __cplusplus
};
#endif // __cplusplus


#endif // SSY_OCCI_PACKAGE_H

