#include <io.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <process.h>
#include <Windows.h>
#include "common_log.h"
#include "common_time.h"
#include "curl_package.h"
#include "device_config.h"
#include "directory.h"
#include "jpg.h"
#include "occi_package.h"
#include "record_cache.h"
#include "transcode.h"

// http://10.133.99.106/350404\20160801\48C7B8583B2F453F9378CAF3E2E9CB3F\BC07818103B6AEF7A26366097A210C34\AEC7C6F7F14A4041B7501BE8A1B53820.jpg
static void CheckRomotePath(char *path) {
	char *find = strchr(path, '\\');
	while (NULL != find) {
		*find = '/';
		find = strchr(path, '\\');
	}
}

static void CheckLocationName(char *name) {
	char *find = strpbrk(name, "_- #.()");
	while (NULL != find) {
		*find = '$';
		find = strpbrk(name, "_- #.()");
	}
}

static void GetDownloadAddress(const char *picPath, char *downloadAddress) {
	char head[8];
	strncpy(head, picPath, 6);
	head[6] = 0;
	if (0 == strcmp(head, "350403"))
		sprintf(downloadAddress,  "http://10.133.99.106/%s", picPath);
	else if (0 == strcmp(head, "350404"))
		sprintf(downloadAddress, "http://10.133.99.107/%s", picPath);
}

void DealwithQuery(t_OcciResult *occiResult, const t_CameraConfig *cameraConfig) {
	void *statement;
	char connString[64];
	char downloadGbk[256];
	char downloadUtf8[256];
	char downloadUrl[256];
	char dirName[64];
	char jpgName[256];
	char passTime[32];
	char locationName[128];
	char localPath[256];

	sprintf(connString, "%s:%s/orcl", cameraConfig->m_oracleIP, cameraConfig->m_oraclePort);
	statement = OCCI_CreateConnection("smjj", "smjj", connString);
	OCCI_QueryBaseArea(statement, occiResult->m_areaid, occiResult->m_areaName);
	OCCI_QueryBaseDevice(statement, occiResult->m_deviceid, occiResult->m_deviceName, occiResult->m_deviceIP);
	OCCI_QueryBaseBayonet(statement, occiResult->m_bayid, occiResult->m_bayName, occiResult->m_bayAddress, occiResult->m_bayPicIP);
	OCCI_DestroyConnection(statement);
	//PrintOcciResult(occiResult);

	//sprintf(downloadGbk, "%s/%s", occiResult->m_bayPicIP, occiResult->m_picPath);
	GetDownloadAddress(occiResult->m_picPath, downloadGbk);
	CheckRomotePath(downloadGbk);
	GBKToUTF8(downloadGbk, downloadUtf8, sizeof(downloadUtf8));
	UTF8ToURL(downloadUtf8, downloadUrl, sizeof(downloadUrl));
	strcpy(locationName, occiResult->m_bayAddress);
	CheckLocationName(locationName);
	sprintf(passTime, "%s001", occiResult->m_dateTime);
	strncpy(dirName, occiResult->m_dateTime, 10); // 2016080100
	dirName[10] = 0;
	sprintf(dirName, "%s0000", dirName);
	// ITS-AG205$2278+575-L1-PÃöDCC971-T20160801060103001_II-IO-OV.jpg
	sprintf(jpgName, "ITS-A%s-L%s-P%s-T%s_II-IO-OV.jpg", locationName, occiResult->m_roadNo, occiResult->m_plateNo, passTime);
	sprintf(localPath, "%s\\%s\\%s", cameraConfig->m_cacheDir, dirName, jpgName);
	CheckDirectory(localPath);
	//LOGINFO("downloadUrl=%s, localPath=%s\n", downloadUrl, localPath);
	Curl_Dwonload(downloadUrl, localPath);
}

static bool IsUpload(const char *dirTime) {
	char workDir[256], recordPath[256];
	char recUpTime[32], recUpInterval[16];
	char recDownTime[32], recDownInterval[16];
	int recUpRawTime, recUpRawInterval;
	int recDownRawTime, recDownRawInterval;
	int dirRawTime = GetRawTime(dirTime);
	int diffTime;

	GetCurrentDirectory(sizeof(workDir), workDir);
	sprintf(recordPath, "%s\\record.cache", workDir);
	Record_ReadUpload(recordPath, recUpTime, sizeof(recUpTime), recUpInterval, sizeof(recUpInterval));
	Record_ReadDownload(recordPath, recDownTime, sizeof(recDownTime), recDownInterval, sizeof(recDownInterval));
	recUpRawTime = GetRawTime(recUpTime);
	recUpRawInterval = atoi(recUpInterval);
	recDownRawTime = GetRawTime(recDownTime);
	recDownRawInterval = atoi(recDownInterval);

	diffTime = recDownRawTime - dirRawTime;
	if (3600*5 > diffTime) {// 5hour
		//LOGINFO("[time]too new date! 5hour(%d) > %d(downTime %s-dirTime %s)\n", 3600*5, diffTime, recDownTime, dirTime);
		return false;
	}
	else if (recUpRawTime >= dirRawTime) {
		//LOGINFO("[time]had upload! uploadTime(%s) >= dirTime(%s)\n", recUpTime, dirTime);
		return false;
	}
	else {
		//LOGINFO("[time]start upload! dirTime=%s\n", dirTime);
		Record_WriteUpload(recordPath, (char *)dirTime, "3600");
		return true;
	}
}

static bool UploadFolder(const char *srcDir, const char *srcName, const char *uploadIP) {
	struct _finddata_t fileinfo;
	char srcPath[256];
	char uploadGbk[256];
	char uploadUtf8[256];
	long first_ret;
	int next_ret;
	int file_index;
	bool okJpg;
	bool okTravel;

	strcpy(srcPath, srcDir);
	strcat(srcPath, "\\");
	strcat(srcPath, srcName);
	first_ret = _findfirst(srcPath, &fileinfo);
	if (-1L == first_ret) {
		LOGERROR("_findfirst(%s)=-1\n", srcPath);
		return false;
	}
	else {
		file_index = 0;
		next_ret = _findnext(first_ret, &fileinfo);
		while (-1 != next_ret) {
			sprintf(srcPath, "%s\\%s", srcDir, fileinfo.name);
			if (0 == strcmp(fileinfo.name, ".") || 0 == strcmp(fileinfo.name, "..")) {
			}
			else if (_A_SUBDIR == fileinfo.attrib) {
				okTravel = IsUpload(fileinfo.name);
				if (true == okTravel) {
					LOGINFO("uploadDir=%s, okTravel=%d\n", fileinfo.name, okTravel);
					UploadFolder(srcPath, srcName, uploadIP);
				}
			}
			else {
				//LOGINFO("jpgPath=%s\n", srcPath);
				okJpg = IsRightJPG(srcPath);
				if (true == okJpg) {
					sprintf(uploadGbk, "ftp://topskyftp:topskyftp@%s/%s", uploadIP, fileinfo.name);
					//LOGINFO("uploadGbk=%s\n", uploadGbk);
					GBKToUTF8(uploadGbk, uploadUtf8, sizeof(uploadUtf8));
					Curl_Upload(srcPath, uploadUtf8);
				}
			}
			next_ret = _findnext(first_ret, &fileinfo);
			file_index++;
		}
		_findclose(first_ret);
		return true;
	}
}

static bool IsRemoveDir(const char *dirTime) {
	char workDir[256], recordPath[256];
	char recUpTime[32], recUpInterval[16];
	char recDownTime[32], recDownInterval[16];
	int recUpRawTime;
	int recDownRawTime;
	int dirRawTime = GetRawTime(dirTime);
	int diffDownTime, diffUpTime;

	GetCurrentDirectory(sizeof(workDir), workDir);
	sprintf(recordPath, "%s\\record.cache", workDir);
	Record_ReadUpload(recordPath, recUpTime, sizeof(recUpTime), recUpInterval, sizeof(recUpInterval));
	Record_ReadDownload(recordPath, recDownTime, sizeof(recDownTime), recDownInterval, sizeof(recDownInterval));
	recUpRawTime = GetRawTime(recUpTime);
	recDownRawTime = GetRawTime(recDownTime);

	diffDownTime = recDownRawTime - dirRawTime;
	diffUpTime = recUpRawTime - dirRawTime;
	//LOGINFO("diffDownTime=%d, diffUpTime=%d\n", diffDownTime/3600, diffUpTime/3600);
	if (3600*5 < diffDownTime && 3600*5 < diffUpTime) {
		//LOGINFO("[time]start remove! dirTime=%s\n", dirTime);
		return true;
	}
	else {
		return false;
	}
}

static bool RemoveFolder(const char *srcDir, const char *srcName) {
	struct _finddata_t fileinfo;
	char srcPath[256];
	char uploadGbk[256];
	long first_ret;
	int next_ret;
	int file_index;
	bool okJpg;
	bool okTravel;

	strcpy(srcPath, srcDir);
	strcat(srcPath, "\\");
	strcat(srcPath, srcName);
	first_ret = _findfirst(srcPath, &fileinfo);
	if (-1L == first_ret) {
		LOGERROR("_findfirst(%s)=-1\n", srcPath);
		return false;
	}
	else {
		file_index = 0;
		next_ret = _findnext(first_ret, &fileinfo);
		while (-1 != next_ret) {
			sprintf(srcPath, "%s\\%s", srcDir, fileinfo.name);
			if (0 == strcmp(fileinfo.name, ".") || 0 == strcmp(fileinfo.name, "..")) {
			}
			else if (_A_SUBDIR == fileinfo.attrib) {
				okTravel = IsRemoveDir(fileinfo.name);
				if (true == okTravel) {
					LOGINFO("removeDir=%s, okTravel=%d\n", fileinfo.name, okTravel);
					RemoveFolder(srcPath, srcName);
					RemoveDirectory(srcPath);
				}
			}
			else {
				//LOGINFO("removeJpg=%s\n", srcPath);
				remove(srcPath);
			}
			next_ret = _findnext(first_ret, &fileinfo);
			file_index++;
		}
		_findclose(first_ret);
		return true;
	}
}

static void UploadThread(void *para) {
	t_CameraConfig *cameraConfig = (t_CameraConfig *)para;
	while (1) {
		printf("===================================================\n");
		UploadFolder(cameraConfig->m_cacheDir, "*", cameraConfig->m_uploadIP);
		LOGINFO("[upload]wait 10min for next...\n");
		printf("[upload]wait 10min for next...\n");
		Sleep(1000*60*9);
		RemoveFolder(cameraConfig->m_cacheDir, "*");
		Sleep(1000*60*1);
	}
}


#if 1
int main(int argc, char *argv[]) {
	t_CameraConfig cameraConfig;
	char workDir[256];
	char confPath[256];
	char recordPath[256];
	char connString[64];
	void *statement;
	int carPassIndex;

	memset(&cameraConfig, 0, sizeof(t_CameraConfig));
	GetCurrentDirectory(sizeof(workDir), workDir);
	sprintf(recordPath, "%s\\record.cache", workDir);
	Record_CheckCache(recordPath);
	sprintf(confPath, "%s\\camera.ini", workDir);
	ParseCameraConfig(confPath, &cameraConfig);
	PrintCameraConfig(&cameraConfig);

	_beginthread(UploadThread, 0, &cameraConfig);
	/*while (1) {
		Sleep(3000);
	}
	*/

	sprintf(connString, "%s:%s/orcl", cameraConfig.m_oracleIP, cameraConfig.m_oraclePort);
	statement = OCCI_CreateConnection("smjj", "smjj", connString);
	while (1) 
	{
		carPassIndex = OCCI_GetCarPassIndex();
		OCCI_QueryCarPass(statement, carPassIndex, recordPath, &cameraConfig, DealwithQuery);
		printf("[query]Over! 10s to next...\n");
		Sleep(10000);
	}
	OCCI_DestroyConnection(statement);

	return 0;
}
#endif

