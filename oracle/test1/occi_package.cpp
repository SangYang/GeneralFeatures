#include "occi_package.h"
#include <iostream>
#include <string>
#include <ctime>
#include "occi.h"
#include "common_log.h"
#include "common_time.h"
#include "record_cache.h"

#pragma comment(lib, "oraocci11")

using namespace std;
using namespace oracle::occi;

static Environment *g_environment = NULL;
static Connection *g_connection = NULL;

void* OCCI_CreateConnection(const char *username, const char *password, const char *connectString) {
	Statement *statement = NULL;

	try {
		g_environment = Environment::createEnvironment("ZHS16GBK", "ZHS16GBK");//(Environment::DEFAULT);
		if (NULL != g_environment) {
			g_connection = g_environment->createConnection(username, password, connectString);
			if (NULL != g_connection) {
				statement = g_connection->createStatement();
			}
		}
	}
	catch (SQLException &ex) {
		LOGERROR("[OCCI]username=%s, password=%s, connectString=%s\n", username, password, connectString);
		LOGERROR("[OCCI]%d:%s\n", ex.getErrorCode(), ex.getMessage().c_str());
	}

	return statement;
}

void OCCI_DestroyConnection(const void *statement) {
	Statement *stmt = (Statement *)statement;

	try {
		if (NULL != stmt && NULL != g_connection) 
			g_connection->terminateStatement(stmt);
		if (NULL != g_connection && NULL != g_environment) 
			g_environment->terminateConnection(g_connection);
		if (NULL != g_environment) 
			Environment::terminateEnvironment(g_environment);
	}
	catch (SQLException &ex) {
		LOGERROR("[OCCI]statement=0x%p, connection=0x%p, environment=0x%p\n", stmt, g_connection, g_environment);
		LOGERROR("[OCCI]%d:%s\n", ex.getErrorCode(), ex.getMessage().c_str());
	}
}

// 201608 --- ITS_CAR_PASS18(1~26)
int OCCI_GetCarPassIndex() {
	time_t nowTime;
	struct tm tmTime;
	int nowYear;
	int nowMonth;
	int diff;
	int index;

	time(&nowTime);
	memcpy(&tmTime, localtime(&nowTime), sizeof(struct tm));
	nowYear = tmTime.tm_year+1900;
	nowMonth = tmTime.tm_mon+1;
	diff = (nowYear-2016)*12 + (nowMonth-8) + 18;
	index = diff%27 + (int)(diff/27);
	return index;
}

static void GetQueryEndTime(const char *startTime, const char *intervalTime, char *endTime, int endSize) {
	int startRawTime = GetRawTime(startTime);
	int intervalRawTime = atoi(intervalTime);
	int endRawTime = startRawTime + intervalRawTime;
	GetStrTime(endRawTime, endTime, endSize);
	printf("[query]%s %s %s\n", startTime, intervalTime, endTime);
}

static bool IsRightQueryTime(const char *endTime) {
	int endRawTime = GetRawTime(endTime);
	int nowRawTime = (int)time(NULL);
	int diffTime = nowRawTime - endRawTime;
	if (3600*10 > diffTime) {// 10hour
		LOGINFO("[time]too new date! 10hour(%d) > %d(nowTime %d-endTime %s)\n", 3600*5, diffTime, nowRawTime, endTime);
		return false;
	}
	else 
		return true;
}

void OCCI_QueryCarPass(const void *statement, 
					   const int carPassIndex, 
					   const char *recordPath,
					   const t_CameraConfig *cameraConfig, 
					   void (*DealwithQuery)(t_OcciResult *occiResult, const t_CameraConfig *cameraConfig)) {
	Statement *stmt = (Statement *)statement;
	ResultSet *rs;
	int result;
	t_OcciResult occiResult;
	int areaIdx, devIdx;
	char table[32];
	char startTime[32];
	char endTime[32];
	char intervalTime[8];
	char *area = NULL;
	char *device = NULL;
	char *sql = NULL;
	char tmpsql[] = "SELECT "
		"CITS.%s.UNID, "
		"CITS.%s.PASS_AREA_UNID, "
		"CITS.%s.PASS_DEVICE_UNID, "
		"CITS.%s.PASS_BAY_UNID, "
		"CITS.%s.PASS_DATETIME, " // 
		"CITS.%s.PASS_PLATE_TYPE, "
		"CITS.%s.PASS_PLATE_COLOR, "
		"CITS.%s.PASS_PLATE_NO, "
		"CITS.%s.PASS_CAR_SPEED, "
		"CITS.%s.PASS_CAR_TYPE, " //
		"CITS.%s.PASS_CAR_COLOR, "
		"CITS.%s.PASS_PIC_FULL_PATH, "
		"CITS.%s.PASS_ROAD_NO "
		"FROM "
		"CITS.%s "
		"WHERE "
		"ROWNUM <= 300 "
		"AND "
		"CITS.%s.PASS_DATETIME >= TO_DATE('%s', 'YYYYMMDDHH24MISS') " //
		"AND "
		"CITS.%s.PASS_DATETIME < TO_DATE('%s', 'YYYYMMDDHH24MISS') "
		"AND "
		"CITS.%s.PASS_AREA_UNID IN (%s) "
		"AND "
		"CITS.%s.PASS_DEVICE_UNID IN (%s)";

	Record_ReadDownload(recordPath, startTime, sizeof(startTime), intervalTime, sizeof(intervalTime));
	GetQueryEndTime(startTime, intervalTime, endTime, sizeof(endTime));
	if (NULL == statement) {
		LOGERROR("NULL == statement\n");
		return;
	}
	if (false == IsRightQueryTime(endTime)) {
		LOGINFO("false == IsRightQueryTime(%s)", endTime);
		return;
	}
	try {
		area = (char *)malloc(1024);
		device = (char *)malloc(2048);
		sql = (char *)malloc(4096);
		sprintf(table, "ITS_CAR_PASS%d", carPassIndex);
		strcpy(area, "'test'");
		strcpy(device, "'test'");
		for (areaIdx = 0; areaIdx < cameraConfig->m_areaNum; areaIdx++) {
			sprintf(area, "%s, '%s'", area, cameraConfig->m_area[areaIdx].m_id);		
			for (devIdx = 0; devIdx < cameraConfig->m_area[areaIdx].m_deviceNum; devIdx++) {
				sprintf(device, "%s, '%s'", device, cameraConfig->m_area[areaIdx].m_device[devIdx].m_id);			
			}
		}
		sprintf(sql, tmpsql, table, table, table, table, table, 
			table, table, table, table, table, 
			table, table, table, table, table, 
			startTime, table, endTime, table, area, 
			table, device);
		LOGINFO("\r\n%s\n", sql);
		stmt->setSQL(sql);
		free(sql);
		free(device);
		free(area);
		rs = stmt->executeQuery();
		while (rs->next()) {
			memset(&occiResult, 0, sizeof(t_OcciResult));           
			strcpy(occiResult.m_unid, rs->getString(1).c_str());
			strcpy(occiResult.m_areaid, rs->getString(2).c_str());
			strcpy(occiResult.m_deviceid, rs->getString(3).c_str());
			strcpy(occiResult.m_bayid, rs->getString(4).c_str());
			strcpy(occiResult.m_dateTime, rs->getDate(5).toText("yyyymmddhh24miss").c_str());
			strcpy(occiResult.m_plateType, rs->getString(6).c_str());
			strcpy(occiResult.m_plateColor, rs->getString(7).c_str());
			strcpy(occiResult.m_plateNo, rs->getString(8).c_str());
			strcpy(occiResult.m_carSpeed, rs->getString(9).c_str());
			strcpy(occiResult.m_carType, rs->getString(10).c_str());
			strcpy(occiResult.m_carColor, rs->getString(11).c_str());
			strcpy(occiResult.m_picPath, rs->getString(12).c_str());
			strcpy(occiResult.m_roadNo, rs->getString(13).c_str());
			DealwithQuery(&occiResult, cameraConfig);
		}
		stmt->closeResultSet(rs);
		Record_WriteDownload(recordPath, endTime, intervalTime);
	}
	catch (SQLException &ex) {
		LOGERROR("[OCCI]statement=0x%p, sql=%s\n", statement, sql);
		LOGERROR("[OCCI]%d:%s\n", ex.getErrorCode(), ex.getMessage().c_str());
	}
}

void OCCI_QueryBaseArea(const void *statement, const char *areaid, char *areaName) {
	Statement *stmt = (Statement *)statement;
	ResultSet *rs;
	int result;
	char sql[512];
	char tmpsql[] = "SELECT "
		"CITS.BASE_AREA.AREANAME "
		"FROM "
		"CITS.BASE_AREA "
		"WHERE "
		"CITS.BASE_AREA.AREAID = '%s'";

	if (NULL == statement) return;
	try {
		sprintf(sql, tmpsql, areaid);
		//LOGINFO("%s\n", sql);
		stmt->setSQL(sql);
		rs = stmt->executeQuery();
		while (rs->next()) {
			//printf("%s\n", rs->getString(1).c_str());    
			strcpy(areaName, rs->getString(1).c_str());           
		}
		stmt->closeResultSet(rs);
	}
	catch (SQLException &ex) {
		LOGERROR("[OCCI]statement=0x%p, sql=%s\n", statement, sql);
		LOGERROR("[OCCI]%d:%s\n", ex.getErrorCode(), ex.getMessage().c_str());
	}
}

void OCCI_QueryBaseDevice(const void *statement, const char *deviceid, char *deviceName, char *deviceIP) {
	Statement *stmt = (Statement *)statement;
	ResultSet *rs;
	int result;
	char sql[512];
	char tmpsql[] = "SELECT "
		"CITS.BASE_DEVICE.DEVICENAME, "
		"CITS.BASE_DEVICE.DEVICEIP "
		"FROM "
		"CITS.BASE_DEVICE "
		"WHERE "
		"CITS.BASE_DEVICE.DEVICEID = '%s'";

	if (NULL == statement) return;
	try {
		sprintf(sql, tmpsql, deviceid);
		stmt->setSQL(sql);
		rs = stmt->executeQuery();
		while (rs->next()) {
			strcpy(deviceName, rs->getString(1).c_str()); 
			strcpy(deviceIP, rs->getString(2).c_str());
		}
		stmt->closeResultSet(rs);
	}
	catch (SQLException &ex) {
		LOGERROR("[OCCI]statement=0x%p, sql=%s\n", statement, sql);
		LOGERROR("[OCCI]%d:%s\n", ex.getErrorCode(), ex.getMessage().c_str());
	}
}

void OCCI_QueryBaseBayonet(const void *statement, const char *bayid, char *bayName, char *bayAddress, char *bayPicIP) {
	Statement *stmt = (Statement *)statement;
	ResultSet *rs;
	int result;
	char sql[512];
	char tmpsql[] = "SELECT "
		"CITS.BASE_BAYONET.BAYONETNAME, "
		"CITS.BASE_BAYONET.ADDRESS, "
		"CITS.BASE_BAYONET.PIC_IP "
		"FROM "
		"CITS.BASE_BAYONET "
		"WHERE "
		"CITS.BASE_BAYONET.BAYONETID = '%s'";

	if (NULL == statement) return;
	try {
		sprintf(sql, tmpsql, bayid);
		stmt->setSQL(sql);
		rs = stmt->executeQuery();
		while (rs->next()) {
			strcpy(bayName, rs->getString(1).c_str()); 
			strcpy(bayAddress, rs->getString(2).c_str());
			strcpy(bayPicIP, rs->getString(3).c_str());
		}
		stmt->closeResultSet(rs);
	}
	catch (SQLException &ex) {
		LOGERROR("[OCCI]statement=0x%p, sql=%s\n", statement, sql);
		LOGERROR("[OCCI]%d:%s\n", ex.getErrorCode(), ex.getMessage().c_str());
	}
}

void PrintOcciResult(const t_OcciResult *occiResult) {
	LOGINFO("\r\n[occiResult]%s %s %s %s %s "
		"%s %s %s %s %s "
		"%s %s %s %s %s "
		"%s %s %s %s\n",
		occiResult->m_unid, occiResult->m_areaid, occiResult->m_areaName, occiResult->m_deviceid, occiResult->m_deviceName,
		occiResult->m_deviceIP, occiResult->m_bayid, occiResult->m_bayName, occiResult->m_bayAddress, occiResult->m_bayPicIP,
		occiResult->m_dateTime, occiResult->m_plateType, occiResult->m_plateColor, occiResult->m_plateNo, occiResult->m_carSpeed, 
		occiResult->m_carType, occiResult->m_carColor, occiResult->m_picPath, occiResult->m_roadNo);
}

#if 0
int main(void) {
	t_OcciResult occiResult;
	void *statement = NULL;
	statement = OCCI_CreateConnection("smjj", "smjj", "10.133.99.99:1521/orcl");
	//while (1) 
	{
		OCCI_QueryBaseArea(statement, "46EBCFD3B669CC237598767A5D32979C", occiResult.m_areaName);
		OCCI_QueryBaseDevice(statement, "79553E24D1A1BF42E22BD8B933FA1A4A", occiResult.m_deviceName, occiResult.m_deviceIP);
		//OCCI_QueryCarPass(statement, sql);
	}
	OCCI_DestroyConnection(statement);

	return 0;
}
#endif 

