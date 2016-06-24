#include <stdio.h>
#include "memorycheck.h"
#include "tinyxml.h"


#if defined(TIXML_USE_STL) && defined(_DEBUG)
#pragma comment(lib, "tinyxmld_stl.lib")
#elif defined(TIXML_USE_STL)
#pragma comment(lib, "tinyxml_stl.lib")
#elif defined(_DEBUG)
#pragma comment(lib, "tinyxmld.lib")
#else
#pragma comment(lib, "tinyxml.lib")
#endif
#define ASSERT(f)  NULL
#define LOG        printf

typedef struct {
	int m_x;   // 百分比值
	int m_y;   // 百分比值
} t_Point;

typedef struct {
	t_Point m_head;
	t_Point m_tail;
} t_Line;

typedef struct {
	t_Point m_head;
	t_Point m_tail;
	int     m_type; 
} t_Lane; 

typedef struct {
	int     m_lane_count;     // 车道数量
	t_Lane  m_lane_line[6];   // 车道线
	t_Point m_detect_area[4]; // 检测区域
	t_Line  m_kakou_line;     // 卡口线
	char    m_reserved[32];   // 保留值
} t_DeviceLine;

typedef struct {
	char    m_sign[8];      // 头部，“topsky\0\0”
	int     m_type;	        // 类型，固定值为 2
	int     m_max_speed;    // 最大时速，km/h
	int     m_before_second;// 录像事件前的秒数
	int     m_after_second; // 录像事件后的秒数	
} t_SystemSet;

typedef struct {
	char         m_sign[8];      // 头部，“topsky\0\0”
	int          m_type;         // 类型，固定值为 1
	char         m_ip[20];       // 相机IP
	t_DeviceLine m_device_line;  // 设备线
} t_DeviceData;


void CreateXml(const char *p_file_path, const t_DeviceData *p_device_data) {
	const int c_AreaPoint = 4;
	const int c_LinePoint = 2;
	int lane_count;
	int lane_index;
	int point_index;
	char ach_index[8];
	char ach_x[16];
	char ach_y[16];
	char ach_type[16];

	ASSERT(NULL != p_file_path && NULL != p_device_data);
	const t_DeviceLine *p_device_line = &p_device_data->m_device_line;
	TiXmlDocument *p_doc = new TiXmlDocument();
	TiXmlDeclaration *p_declar = new TiXmlDeclaration( "1.0", "UTF-8", "" );
	TiXmlElement *p_root = new TiXmlElement("DeviceLine");
	TiXmlElement *p_first = new TiXmlElement("DetectArea");
	TiXmlElement *p_second = new TiXmlElement("KakouLine");
	TiXmlElement *p_third = new TiXmlElement("LaneLine");
	p_doc->LinkEndChild(p_declar);
	p_doc->LinkEndChild(p_root);
	p_root->LinkEndChild(p_first);
	p_root->LinkEndChild(p_second);
	p_root->LinkEndChild(p_third);
	for (point_index = 0; point_index < c_AreaPoint; point_index++) {
		sprintf(ach_index, "point%d", point_index + 1);
		sprintf(ach_x, "%d", p_device_line->m_detect_area[point_index].m_x);
		sprintf(ach_y, "%d", p_device_line->m_detect_area[point_index].m_y);
		TiXmlElement *p_point = new TiXmlElement(ach_index);
		p_first->LinkEndChild(p_point);
		p_point->SetAttribute("X", ach_x);
		p_point->SetAttribute("Y", ach_y);
	}
	TiXmlElement *p_line = new TiXmlElement("Line");
	p_second->LinkEndChild(p_line);
	for (point_index = 0; point_index < c_LinePoint; point_index++) {
		sprintf(ach_index, "point%d", point_index + 1);
		if (0 == point_index) {
			sprintf(ach_x, "%d", p_device_line->m_kakou_line.m_head.m_x);
			sprintf(ach_y, "%d", p_device_line->m_kakou_line.m_head.m_y);	
		}
		else {
			sprintf(ach_x, "%d", p_device_line->m_kakou_line.m_tail.m_x);
			sprintf(ach_y, "%d", p_device_line->m_kakou_line.m_tail.m_y);	
		}
		TiXmlElement *p_point = new TiXmlElement(ach_index);
		p_line->LinkEndChild(p_point);
		p_point->SetAttribute("X", ach_x);
		p_point->SetAttribute("Y", ach_y);
	}
	lane_count = p_device_line->m_lane_count;
	for (lane_index = 0; lane_index < lane_count; lane_index++) {
		TiXmlElement *p_lane = new TiXmlElement("Lane");
		p_third->LinkEndChild(p_lane);
		for (point_index = 0; point_index < c_LinePoint; point_index++) {
			sprintf(ach_index, "point%d", point_index + 1);
			if (0 == point_index) {
				sprintf(ach_x, "%d", p_device_line->m_lane_line[lane_index].m_head.m_x);
				sprintf(ach_y, "%d", p_device_line->m_lane_line[lane_index].m_head.m_y);	
				sprintf(ach_type, "%d", p_device_line->m_lane_line[lane_index].m_type);	
			}
			else {
				sprintf(ach_x, "%d", p_device_line->m_lane_line[lane_index].m_head.m_x);
				sprintf(ach_y, "%d", p_device_line->m_lane_line[lane_index].m_head.m_y);	
				sprintf(ach_type, "%d", p_device_line->m_lane_line[lane_index].m_type);	
			}
			TiXmlElement *p_point = new TiXmlElement(ach_index);
			p_lane->SetAttribute("Type", ach_type);
			p_lane->LinkEndChild(p_point);
			p_point->SetAttribute("X", ach_x);
			p_point->SetAttribute("Y", ach_y);
		}
	}
	p_doc->SaveFile(p_file_path);
	delete p_doc;
}

bool ParseXml(const char *p_file_path, t_DeviceData *p_device_data) {
	const int c_AreaPoint = 4;
	const int c_LinePoint = 2;
	int lane_count;
	int lane_index;
	int point_index;
	char ach_index[8];
	char ach_x[16];
	char ach_y[16];
	char ach_type[16];
	bool ok_load;

	TiXmlDocument doc(p_file_path);
	ok_load = doc.LoadFile(TIXML_ENCODING_UTF8);
	if (false == ok_load) {
		printf("TiXmlDocument.LoadFile() error!\n");
		return false;
	}
	else {
		t_DeviceLine *p_device_line = &p_device_data->m_device_line;
		TiXmlElement *p_root = doc.RootElement();
		TiXmlElement *p_first = p_root->FirstChildElement();
		TiXmlElement *p_second = p_first->NextSiblingElement();
		TiXmlElement *p_third = p_second->NextSiblingElement();
		TiXmlElement *p_line = NULL;
		TiXmlElement *p_point = NULL;
		for (point_index = 0; point_index < c_AreaPoint; point_index++) {
			if (0 == point_index)
				p_point = p_first->FirstChildElement();
			else
				p_point = p_point->NextSiblingElement();
			p_device_line->m_detect_area[point_index].m_x = atoi(p_point->Attribute("X"));
			p_device_line->m_detect_area[point_index].m_y = atoi(p_point->Attribute("Y"));
		}
		p_line = p_second->FirstChildElement();
		for (point_index = 0; point_index < c_LinePoint; point_index++) {
			if (0 == point_index) {
				p_point = p_line->FirstChildElement();
				p_device_line->m_kakou_line.m_head.m_x = atoi(p_point->Attribute("X"));
				p_device_line->m_kakou_line.m_head.m_y = atoi(p_point->Attribute("Y"));
			}
			else {
				p_point = p_point->NextSiblingElement();
				p_device_line->m_kakou_line.m_tail.m_x = atoi(p_point->Attribute("X"));
				p_device_line->m_kakou_line.m_tail.m_y = atoi(p_point->Attribute("Y"));
			}
		}
		lane_index = 0;
		while (true) {
			if (0 == lane_index)
				p_line = p_third->FirstChildElement();
			else
				p_line = p_line->NextSiblingElement();
			if (NULL == p_line)
				break;
			else {
				p_device_line->m_lane_line[lane_index].m_type = atoi(p_line->Attribute("Type"));
				for (point_index = 0; point_index < c_LinePoint; point_index++) {
					if (0 == point_index) {
						p_point = p_line->FirstChildElement();
						p_device_line->m_lane_line[lane_index].m_head.m_x = atoi(p_point->Attribute("X"));
						p_device_line->m_lane_line[lane_index].m_head.m_y = atoi(p_point->Attribute("Y"));
					}
					else {
						p_point = p_point->NextSiblingElement();
						p_device_line->m_lane_line[lane_index].m_tail.m_x = atoi(p_point->Attribute("X"));
						p_device_line->m_lane_line[lane_index].m_tail.m_y = atoi(p_point->Attribute("Y"));
					}
				}	
				lane_index++;
			}
		}
		p_device_line->m_lane_count = lane_index;
		return true;
	}
}



void SaveSystemsetXml(const char *p_file_path, const t_SystemSet *p_system_set) {
	char max_speed[16];
	char before_second[16];
	char after_second[16];

	ASSERT(NULL != p_file_path && NULL != p_system_set);
	TiXmlDocument *p_doc = new TiXmlDocument();
	TiXmlDeclaration *p_declar = new TiXmlDeclaration( "1.0", "UTF-8", "" );
	TiXmlElement *p_root = new TiXmlElement("SystemSet");
	p_doc->LinkEndChild(p_declar);
	p_doc->LinkEndChild(p_root);
	sprintf(max_speed, "%d", p_system_set->m_max_speed);
	sprintf(before_second, "%d", p_system_set->m_before_second);
	sprintf(after_second, "%d", p_system_set->m_after_second);
	p_root->SetAttribute("MaxSpeed", max_speed);
	p_root->SetAttribute("BeforeSecond", before_second);
	p_root->SetAttribute("AfterSecond", after_second);
	p_doc->SaveFile(p_file_path);
	delete p_doc;
}

bool ParseSystemsetXml(const char *p_file_path, t_SystemSet *p_system_set) {
	bool ok_load;

	ASSERT(NULL != p_file_path && NULL != p_system_set);
	TiXmlDocument doc(p_file_path);
	ok_load = doc.LoadFile(TIXML_ENCODING_UTF8);
	if (false == ok_load) {
		printf("TiXmlDocument.LoadFile() error!\n");
		return false;
	}
	else {
		TiXmlElement *p_root = doc.RootElement();
		p_system_set->m_max_speed = atoi(p_root->Attribute("MaxSpeed"));
		p_system_set->m_before_second = atoi(p_root->Attribute("BeforeSecond"));
		p_system_set->m_after_second = atoi(p_root->Attribute("AfterSecond"));
		return true;
	}
}

#if 1
int main(void) {
	MEMCHECK_BEGIN();	
	t_SystemSet system_set;
	system_set.m_max_speed = 120;
	system_set.m_before_second = 12345;
	system_set.m_after_second = 987654321;

	//SaveSystemsetXml("systemset.xml", &system_set);
	ParseSystemsetXml("systemset.xml", &system_set);
	MEMCHECK_END();
	return 0;
}
#endif


#if 0
int main() {
	MEMCHECK_BEGIN();	
	t_DeviceData device_data;
	t_DeviceLine *p_device_line = NULL;
	memset(&device_data, 0, sizeof(device_data));
	p_device_line = &device_data.m_device_line;
	p_device_line->m_lane_count = 5;
	for (int i = 0; i < 5; i++) {
		p_device_line->m_lane_line[i].m_type = i;
		p_device_line->m_lane_line[i].m_head.m_x = 10 * i;
		p_device_line->m_lane_line[i].m_head.m_y = 11 * i;
		p_device_line->m_lane_line[i].m_tail.m_x = 12 * i;
		p_device_line->m_lane_line[i].m_tail.m_y = 13 * i;
		if (i < 4) {
			p_device_line->m_detect_area[i].m_x = 2 * i;
			p_device_line->m_detect_area[i].m_y = 3 * i;
		}
		if (i < 1) {
			p_device_line->m_kakou_line.m_head.m_x = 23 + i;
			p_device_line->m_kakou_line.m_head.m_y = 24 + i;
			p_device_line->m_kakou_line.m_tail.m_x = 25 + i;
			p_device_line->m_kakou_line.m_tail.m_y = 26 + i;
		}
	}

	char p_file_path[256] = "info.xml";
	CreateXml(p_file_path, &device_data);
	ParseXml(p_file_path, &device_data);
	for (int i = 0; i < 5; i++) {
		printf("%d\n", p_device_line->m_lane_line[i].m_type);
		printf("[%d : %d] : [%d : %d]\n", 
			p_device_line->m_lane_line[i].m_head.m_x,
			p_device_line->m_lane_line[i].m_head.m_y,
			p_device_line->m_lane_line[i].m_tail.m_x,
			p_device_line->m_lane_line[i].m_tail.m_y); 
		if (i < 4) {
			printf("-%d : %d-\n", 
				p_device_line->m_detect_area[i].m_x,
				p_device_line->m_detect_area[i].m_y);
		}
		if (i < 1) {
			printf("head : [%d %d], tail : [%d %d]\n",
				p_device_line->m_kakou_line.m_head.m_x,
				p_device_line->m_kakou_line.m_head.m_y,
				p_device_line->m_kakou_line.m_tail.m_x,
				p_device_line->m_kakou_line.m_tail.m_y);
		}
	}

	MEMCHECK_END();
	return 0;
}
#endif