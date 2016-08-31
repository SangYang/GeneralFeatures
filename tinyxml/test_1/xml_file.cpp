#include "xml_file.h"
#include <stdio.h>
#include <string.h>
#include "tinyxml/tinyxml.h"
#include "debug_log.h"

void SaveSystemsetXml(const char *path, const t_SystemSet *systemSet) {
	char max_speed[16];
	char before_second[16];
	char after_second[16];
	char cal_before_second[16];
	char cal_after_second[16];

	TiXmlDocument *doc = new TiXmlDocument();
	TiXmlDeclaration *declar = new TiXmlDeclaration( "1.0", "UTF-8", "" );
	TiXmlElement *root = new TiXmlElement("SystemSet");
	doc->LinkEndChild(declar);
	doc->LinkEndChild(root);
	sprintf(max_speed, "%d", systemSet->m_max_speed);
	sprintf(before_second, "%d", systemSet->m_before_second);
	sprintf(after_second, "%d", systemSet->m_after_second);
	sprintf(cal_before_second, "%d", systemSet->m_cal_before_second);
	sprintf(cal_after_second, "%d", systemSet->m_cal_after_second);
	root->SetAttribute("MaxSpeed", max_speed);
	root->SetAttribute("BeforeSecond", before_second);
	root->SetAttribute("AfterSecond", after_second);
	root->SetAttribute("CalBeforeSecond", cal_before_second);
	root->SetAttribute("CalAfterSecond", cal_after_second);
	doc->SaveFile(path);
	delete doc;
}

bool ParseSystemsetXml(const char *path, t_SystemSet *systemSet) {
	bool ok_load;

	TiXmlDocument doc(path);
	ok_load = doc.LoadFile(TIXML_ENCODING_UTF8);
	if (false == ok_load) {
		LOG("TiXmlDocument.LoadFile(TIXML_ENCODING_UTF8, %s)=false\n", path);
		return false;
	}
	else {
		TiXmlElement *root = doc.RootElement();
		systemSet->m_max_speed = atoi(root->Attribute("MaxSpeed"));
		systemSet->m_before_second = atoi(root->Attribute("BeforeSecond"));
		systemSet->m_after_second = atoi(root->Attribute("AfterSecond"));
		systemSet->m_cal_before_second = atoi(root->Attribute("CalBeforeSecond"));
		systemSet->m_cal_after_second = atoi(root->Attribute("CalAfterSecond"));
		return true;
	}
}

void SaveDevicedataXml(const char *path, const t_DeviceData *deviceData) {
	int lane_count;
	int lane_index;
	int point_index;
	char ach_index[8];
	char ach_x[16];
	char ach_y[16];
	char ach_type[16];

	const t_DeviceLine *deviceLine = &deviceData->m_device_line;
	TiXmlDocument *doc = new TiXmlDocument();
	TiXmlDeclaration *declar = new TiXmlDeclaration( "1.0", "UTF-8", "" );
	TiXmlElement *root = new TiXmlElement("DeviceLine");
	TiXmlElement *first = new TiXmlElement("DetectArea");
	TiXmlElement *second = new TiXmlElement("KakouLine");
	TiXmlElement *third = new TiXmlElement("LaneLine");
	doc->LinkEndChild(declar);
	doc->LinkEndChild(root);
	root->LinkEndChild(first);
	root->LinkEndChild(second);
	root->LinkEndChild(third);
	for (point_index = 0; point_index < deviceLine->m_point_count; point_index++) {
		sprintf(ach_index, "%d", point_index);
		sprintf(ach_x, "%d", deviceLine->m_detect_area[point_index].m_x);
		sprintf(ach_y, "%d", deviceLine->m_detect_area[point_index].m_y);
		TiXmlElement *point = new TiXmlElement("point");
		first->LinkEndChild(point);
		point->SetAttribute("index", ach_index);
		point->SetAttribute("X", ach_x);
		point->SetAttribute("Y", ach_y);
	}
	TiXmlElement *line = new TiXmlElement("Line");
	second->LinkEndChild(line);
	for (point_index = 0; point_index < 2; point_index++) {
		sprintf(ach_index, "%d", point_index);
		if (0 == point_index) {
			sprintf(ach_x, "%d", deviceLine->m_kakou_line.m_head.m_x);
			sprintf(ach_y, "%d", deviceLine->m_kakou_line.m_head.m_y);	
		}
		else {
			sprintf(ach_x, "%d", deviceLine->m_kakou_line.m_tail.m_x);
			sprintf(ach_y, "%d", deviceLine->m_kakou_line.m_tail.m_y);	
		}
		TiXmlElement *point = new TiXmlElement("point");
		line->LinkEndChild(point);
		point->SetAttribute("index", ach_index);
		point->SetAttribute("X", ach_x);
		point->SetAttribute("Y", ach_y);
	}
	lane_count = deviceLine->m_lane_count + 1;
	for (lane_index = 0; lane_index < lane_count; lane_index++) {
		TiXmlElement *lane = new TiXmlElement("Lane");
		third->LinkEndChild(lane);
		for (point_index = 0; point_index < deviceLine->m_lane_line[lane_index].m_count; point_index++) {
			sprintf(ach_index, "%d", point_index);
			if (0 == point_index) {
				sprintf(ach_x, "%d", deviceLine->m_lane_line[lane_index].m_point[point_index].m_x);
				sprintf(ach_y, "%d", deviceLine->m_lane_line[lane_index].m_point[point_index].m_y);	
				sprintf(ach_type, "%d", deviceLine->m_lane_line[lane_index].m_type);	
			}
			else {
				sprintf(ach_x, "%d", deviceLine->m_lane_line[lane_index].m_point[point_index].m_x);
				sprintf(ach_y, "%d", deviceLine->m_lane_line[lane_index].m_point[point_index].m_y);	
				sprintf(ach_type, "%d", deviceLine->m_lane_line[lane_index].m_type);	
			}
			TiXmlElement *point = new TiXmlElement("point");
			lane->SetAttribute("index", lane_index);
			lane->SetAttribute("Type", ach_type);
			lane->LinkEndChild(point);
			point->SetAttribute("index", ach_index);
			point->SetAttribute("X", ach_x);
			point->SetAttribute("Y", ach_y);
		}
	}
	doc->SaveFile(path);
	delete doc;
}

bool ParseDevicedataXml(const char *path, t_DeviceData *deviceData) {
	int lane_num;
	int lane_index;
	int point_index;
	bool ok_load;
	const char *attrRet;

	TiXmlDocument doc(path);
	ok_load = doc.LoadFile(TIXML_ENCODING_UTF8);
	if (false == ok_load) {
		LOG("TiXmlDocument.LoadFile(TIXML_ENCODING_UTF8, %s)=false\n", path);
		return false;
	}
	else {
		t_DeviceLine *deviceLine = &deviceData->m_device_line;
		TiXmlElement *root = doc.RootElement();
		TiXmlElement *first = root->FirstChildElement();
		TiXmlElement *second = first->NextSiblingElement();
		TiXmlElement *third = second->NextSiblingElement();
		TiXmlElement *line = NULL;
		TiXmlElement *point = NULL;

		memset(deviceLine, 0, sizeof(t_DeviceLine));
		point_index = 0;
		while (0 != first) {
			if (0 == point_index)
				point = first->FirstChildElement();
			else if (NULL != point)
				point = point->NextSiblingElement();
			if (NULL == point)
				break;
			else {
				deviceLine->m_detect_area[point_index].m_x = atoi(point->Attribute("X"));
				deviceLine->m_detect_area[point_index].m_y = atoi(point->Attribute("Y"));
				point_index++;
			}
		}
		deviceLine->m_point_count = point_index;

		if (NULL != second) {
			line = second->FirstChildElement();
			for (point_index = 0; point_index < 2 && NULL != line; point_index++) {
				if (0 == point_index) {
					point = line->FirstChildElement();
					if (NULL != point) {
						deviceLine->m_kakou_line.m_head.m_x = atoi(point->Attribute("X"));
						deviceLine->m_kakou_line.m_head.m_y = atoi(point->Attribute("Y"));						
					}
				}
				else {
					if (NULL != point) {
						point = point->NextSiblingElement();
						if (NULL != point) {
							deviceLine->m_kakou_line.m_tail.m_x = atoi(point->Attribute("X"));
							deviceLine->m_kakou_line.m_tail.m_y = atoi(point->Attribute("Y"));
						}
					}
				}
			}
		}

		lane_num = 0;
		lane_index = 0;
		while (NULL != third) {
			if (0 == lane_index)
				line = third->FirstChildElement();
			else if (NULL != line)
				line = line->NextSiblingElement();
			if (NULL == line)
				break;
			else {
				point_index = 0;
				attrRet = line->Attribute("Type");
				while (NULL != attrRet) {
					deviceLine->m_lane_line[lane_index].m_type = atoi(attrRet);
					if (0 == point_index) {
						point = line->FirstChildElement();
						if (NULL != point) {
							deviceLine->m_lane_line[lane_index].m_point[point_index].m_x = atoi(point->Attribute("X"));
							deviceLine->m_lane_line[lane_index].m_point[point_index].m_y = atoi(point->Attribute("Y"));
						}
						else {
							lane_num++;
							break;
						}
					}
					else {
						if (NULL != point) {
							point = point->NextSiblingElement();
							if (NULL != point) {
								deviceLine->m_lane_line[lane_index].m_point[point_index].m_x = atoi(point->Attribute("X"));
								deviceLine->m_lane_line[lane_index].m_point[point_index].m_y = atoi(point->Attribute("Y"));												
							}
							else {
								lane_num++;
								break;
							}
						}
					}
					point_index++;						
				}
				deviceLine->m_lane_line[lane_index].m_count = point_index;
				lane_index++;
			}
		}
		if (2 > lane_num)
			deviceLine->m_lane_count = 0;
		else
			deviceLine->m_lane_count = lane_num - 1;
		return true;
	}
}

void SaveDevicegroupXml(const char *path, const t_DeviceGroup *deviceGroup) {
	int count;
	int index;
	char ach_index[8];
	char qjPort[8];

	TiXmlDocument *doc = new TiXmlDocument();
	TiXmlDeclaration *declar = new TiXmlDeclaration( "1.0", "UTF-8", "" );
	TiXmlElement *root = new TiXmlElement("DeviceGroup");
	TiXmlElement *first = new TiXmlElement("IpGroup");
	doc->LinkEndChild(declar);
	doc->LinkEndChild(root);
	root->LinkEndChild(first);
	count = deviceGroup->m_ipgroup_count;
	for (index = 0; index < count; index++) {
		sprintf(ach_index, "group%d", index + 1);
		const t_IpGroup *ipGroup = &deviceGroup->m_ipgroup_array[index];
		TiXmlElement *element = new TiXmlElement(ach_index);
		first->LinkEndChild(element);
		element->SetAttribute("kakou", ipGroup->m_kakou_ip);
		element->SetAttribute("qiuji", ipGroup->m_qiuji_ip);
		element->SetAttribute("quanjing", ipGroup->m_quanjing_ip);
		element->SetAttribute("quanjing_user", ipGroup->m_quanjing_user);
		element->SetAttribute("quanjing_pwd", ipGroup->m_quanjing_pwd);
		sprintf(qjPort, "%d", ipGroup->m_quanjing_port);
		element->SetAttribute("quanjing_port", qjPort);
	}
	doc->SaveFile(path);
	delete doc;
}

bool ParseDevicegroupXml(const char *path, t_DeviceGroup *deviceGroup) {
	int index;
	bool ok_load;

	TiXmlDocument doc(path);
	ok_load = doc.LoadFile(TIXML_ENCODING_UTF8);
	if (false == ok_load) {
		LOG("TiXmlDocument.LoadFile(TIXML_ENCODING_UTF8, %s)=false\n", path);
		return false;
	}
	else {
		TiXmlElement *root = doc.RootElement();
		TiXmlElement *first = root->FirstChildElement();
		TiXmlElement *element = NULL;
		index = 0;
		while (NULL != first) {
			if (0 == index)
				element = first->FirstChildElement();
			else if (NULL != element)
				element = element->NextSiblingElement();
			if (NULL == element)
				break;
			else {
				strcpy(deviceGroup->m_ipgroup_array[index].m_kakou_ip, element->Attribute("kakou"));
				strcpy(deviceGroup->m_ipgroup_array[index].m_qiuji_ip, element->Attribute("qiuji"));
				strcpy(deviceGroup->m_ipgroup_array[index].m_quanjing_ip, element->Attribute("quanjing"));
				strcpy(deviceGroup->m_ipgroup_array[index].m_quanjing_user, element->Attribute("quanjing_user"));
				strcpy(deviceGroup->m_ipgroup_array[index].m_quanjing_pwd, element->Attribute("quanjing_pwd"));
				deviceGroup->m_ipgroup_array[index].m_quanjing_port = atoi(element->Attribute("quanjing_port"));
				index++;
			}
		}
		deviceGroup->m_ipgroup_count = index;
		return true;
	}
}


#if 0
int main() {
	t_DeviceData device_data;
	t_DeviceLine *deviceLine = NULL;
/*
	memset(&device_data, 0, sizeof(device_data));
	deviceLine = &device_data.m_device_line;

	deviceLine->m_point_count = 5;
	for (int i = 0; i < deviceLine->m_point_count; i++) {
		deviceLine->m_detect_area[i].m_x = i+5;
		deviceLine->m_detect_area[i].m_y = i+6;
	}

	deviceLine->m_kakou_line.m_head.m_x = 12;
	deviceLine->m_kakou_line.m_head.m_y = 13;
	deviceLine->m_kakou_line.m_tail.m_x = 14;
	deviceLine->m_kakou_line.m_tail.m_y = 15;

	deviceLine->m_lane_count = 4;
	for (int i = 0; i < deviceLine->m_lane_count+1; i++) {
		deviceLine->m_lane_line[i].m_type = i*11 + 10;
		deviceLine->m_lane_line[i].m_count = i+1;
		for (int j = 0; j < deviceLine->m_lane_line[i].m_count; j++) {
			deviceLine->m_lane_line[i].m_point[j].m_x = 2 * i + 2;
			deviceLine->m_lane_line[i].m_point[j].m_y = 5 * i + 5;
		}
	}
*/
	InitDeviceData(&device_data);

	char path[256] = "xml\\192.168.2.64.xml";
	//SaveDevicedataXml(path, &device_data);
	ParseDevicedataXml(path, &device_data);
	PrintDeviceData(&device_data);

/*
	t_SystemSet systemSet;
	InitSystemSet(&systemSet);
	SaveSystemsetXml("systemset.xml", &systemSet);
*/

/*
	t_DeviceGroup deviceGroup;
	InitDeviceGroup(&deviceGroup);
	SaveDevicegroupXml("devicegroup.xml", &deviceGroup);
*/
	return 0;
}
#endif

