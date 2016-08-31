#ifndef SSY_XML_FILE_H
#define SSY_XML_FILE_H


#include "device_data.h"

#ifndef __cplusplus
#define bool    int
#define false   0
#define true    1
#endif //__cplusplus

#ifdef __cplusplus
extern "C" {
#endif

	void SaveSystemsetXml(const char *path, const t_SystemSet *systemSet);
	bool ParseSystemsetXml(const char *path, t_SystemSet *systemSet);
	void SaveDevicedataXml(const char *path, const t_DeviceData *deviceData);
	bool ParseDevicedataXml(const char *path, t_DeviceData *deviceData);
	void SaveDevicegroupXml(const char *path, const t_DeviceGroup *deviceGroup);
	bool ParseDevicegroupXml(const char *path, t_DeviceGroup *deviceGroup);

#ifdef __cplusplus
};
#endif


#endif // SSY_XML_FILE_H
