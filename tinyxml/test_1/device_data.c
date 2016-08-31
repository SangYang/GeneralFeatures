#include "device_data.h"
#include <stdio.h>
#include <string.h>
#include "debug_assert.h"
#include "debug_log.h"

void GetDeviceDataInfo(int *p_total_size, int *p_header_size, int *p_reserved_size) {
	t_DeviceData deviceData;
	int total_size;
	int header_size;
	int reserved_size;

	ASSERT(NULL != p_total_size && NULL != p_header_size && NULL != p_reserved_size);
	total_size = sizeof(deviceData);
	header_size = sizeof(deviceData.m_sign) + sizeof(deviceData.m_type);
	reserved_size = total_size - header_size;

	*p_total_size = total_size;
	*p_header_size = header_size;
	*p_reserved_size = reserved_size;
}

void GetSystemSetInfo(int *p_total_size, int *p_header_size, int *p_reserved_size) {
	t_SystemSet system_set;
	int total_size;
	int header_size;
	int reserved_size;

	ASSERT(NULL != p_total_size && NULL != p_header_size && NULL != p_reserved_size);
	total_size = sizeof(system_set);
	header_size = sizeof(system_set.m_sign) + sizeof(system_set.m_type);
	reserved_size = total_size - header_size;

	*p_total_size = total_size;
	*p_header_size = header_size;
	*p_reserved_size = reserved_size;
}

void GetJpdDataInfo(int *p_total_size, int *p_header_size, int *p_reserved_size) {
	t_JpgData jpg_data;
	int total_size;
	int header_size;
	int reserved_size;

	ASSERT(NULL != p_total_size && NULL != p_header_size && NULL != p_reserved_size);
	total_size = sizeof(jpg_data);
	header_size = sizeof(jpg_data.m_sign) + sizeof(jpg_data.m_type);
	reserved_size = total_size - header_size;

	*p_total_size = total_size;
	*p_header_size = header_size;
	*p_reserved_size = reserved_size;
}

void GetDeviceGroupInfo(int *p_total_size, int *p_header_size, int *p_reserved_size) {
	t_DeviceGroup device_group;
	int total_size;
	int header_size;
	int reserved_size;

	ASSERT(NULL != p_total_size && NULL != p_header_size && NULL != p_reserved_size);
	total_size = sizeof(device_group);
	header_size = sizeof(device_group.m_sign) + sizeof(device_group.m_type);
	reserved_size = total_size - header_size;

	*p_total_size = total_size;
	*p_header_size = header_size;
	*p_reserved_size = reserved_size;
}

void GetHeaderSign(const char *p_header, char *p_sign, const int sign_size) {
	char sign[10];
	
	ASSERT(NULL != p_header && NULL != p_sign);
	ASSERT(8 < sign_size);
	strncpy(sign, p_header, 8);
	sign[8] = '\0';
	strcpy(p_sign, sign);
}

void GetHeaderType(const char *p_header, int *p_type) {
	int type;

	ASSERT(NULL != p_header && NULL != p_type);
	memcpy(&type, p_header + 8, 4);
	*p_type = type;
}

void PrintDeviceData(const t_DeviceData *p_device_data) {
	const t_DeviceLine *p_device_line = &p_device_data->m_device_line;
	int lane_index;
	int lane_count;
	int areaIdx;
	int pointIdx;

	ASSERT(NULL != p_device_data);
	LOG("---------------BEGIN-------------------\n");
	LOG("sign=%s\n", p_device_data->m_sign);
	LOG("type=%d\n", p_device_data->m_type);
	LOG("ip=%s\n", p_device_data->m_ip);
	LOG("detect_area: %d", p_device_line->m_point_count);
	for (areaIdx = 0; areaIdx < p_device_line->m_point_count; areaIdx++) {
		LOG("(%d,%d)", p_device_line->m_detect_area[areaIdx].m_x, p_device_line->m_detect_area[areaIdx].m_y);
	}
	LOG("\r\n");
	LOG("kakou_line:(%d,%d)(%d,%d)\n",
		p_device_line->m_kakou_line.m_head.m_x, p_device_line->m_kakou_line.m_head.m_y,
		p_device_line->m_kakou_line.m_tail.m_x, p_device_line->m_kakou_line.m_tail.m_y);
	lane_count = p_device_line->m_lane_count;
	LOG("lane_count:%d", lane_count);
	for (lane_index = 0; lane_index < lane_count+1; lane_index++) {
		for (pointIdx = 0; pointIdx < p_device_line->m_lane_line[lane_index].m_count; pointIdx++) {
			LOG("(%d,%d)-%d", 
				p_device_line->m_lane_line[lane_index].m_point[pointIdx].m_x, 
				p_device_line->m_lane_line[lane_index].m_point[pointIdx].m_y,
				p_device_line->m_lane_line[lane_index].m_type);
		}
	}
	LOG("\r\n");
	LOG("---------------END-------------------\n");
}

void PrintSystemSet(const t_SystemSet *p_system_set) {
	ASSERT(NULL != p_system_set);
	LOG("--------------BEGIN--------------------\n");
	LOG("sign=%s\n", p_system_set->m_sign);
	LOG("type=%d\n", p_system_set->m_type);
	LOG("max_speed=%d\n", p_system_set->m_max_speed);
	LOG("before_second=%d\n", p_system_set->m_before_second);
	LOG("after_second=%d\n", p_system_set->m_after_second);
	LOG("cal_before_second=%d\n", p_system_set->m_cal_before_second);
	LOG("cal_after_second=%d\n", p_system_set->m_cal_after_second);
	LOG("----------------END------------------\n");
}

void PrintJpgData(const t_JpgData *p_jpg_data) {
	ASSERT(NULL != p_jpg_data);
	LOG("--------------BEGIN--------------------\n");
	LOG("sign=%s\n", p_jpg_data->m_sign);
	LOG("type=%d\n", p_jpg_data->m_type);
	LOG("ip=%s\n", p_jpg_data->m_ip);
	LOG("jpg_size=%d\n", p_jpg_data->m_jpg_size);
	LOG("----------------END------------------\n");
}

void PrintDeviceGroup(const t_DeviceGroup *p_device_group) {
	int index;
	int count;
	
	ASSERT(NULL != p_device_group);
	LOG("--------------BEGIN--------------------\n");
	LOG("sign=%s\n", p_device_group->m_sign);
	LOG("type=%d\n", p_device_group->m_type);
	LOG("ipgroup_count=%d\n", p_device_group->m_ipgroup_count);
	count = p_device_group->m_ipgroup_count;
	for (index = 0; index < count; index++) {
		const t_IpGroup *p_ip_group = &p_device_group->m_ipgroup_array[index];
		LOG("index=%d\n", index);
		LOG("m_kakou_ip=%s\n", p_ip_group->m_kakou_ip);
		LOG("m_qiuji_ip=%s\n", p_ip_group->m_qiuji_ip);
		LOG("m_quanjing_ip=%s\n", p_ip_group->m_quanjing_ip);
		LOG("m_quanjing_user=%s\n", p_ip_group->m_quanjing_user);
		LOG("m_quanjing_pwd=%s\n", p_ip_group->m_quanjing_pwd);
		LOG("m_quanjing_port=%d\n", p_ip_group->m_quanjing_port);
	}
	LOG("----------------END------------------\n");
}


void InitDeviceData(t_DeviceData *p_device_data) {
	t_DeviceLine *p_device_line;
	int laneIdx;

	memset(p_device_data, 0, sizeof(t_DeviceData));
	strcpy(p_device_data->m_sign, "topsky");
	p_device_data->m_type = 3;
	strcpy(p_device_data->m_ip, "127.0.0.1");
	p_device_line = &p_device_data->m_device_line;
	p_device_line->m_point_count = 2;
	p_device_line->m_lane_count = 2;
	for (laneIdx = 0; laneIdx < p_device_line->m_lane_count+1; laneIdx++)
		p_device_line->m_lane_line[laneIdx].m_count = 2;
}

void InitSystemSet(t_SystemSet *systemSet) {
	memset(systemSet, 0, sizeof(t_SystemSet));
	strcpy(systemSet->m_sign, "topsky");
	systemSet->m_type = 1;
}

void InitDeviceGroup(t_DeviceGroup *deviceGroup) {
	memset(deviceGroup, 0, sizeof(t_DeviceGroup));
	strcpy(deviceGroup->m_sign, "topsky");
	deviceGroup->m_type = 6;
	deviceGroup->m_ipgroup_count = 1;
}


