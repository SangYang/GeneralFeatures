#include "basic_data.h"
#include <stdio.h>
#include <string.h>
#include <time.h>

void InitEvent(t_Event *p_event) {
	t_CarPlate *p_carplate = NULL;
	t_CarBody *p_carbody = NULL;
	t_Jpg *p_jpg = NULL;
	int jpg_index;
	int t_index;
	int t_index2;

	p_event->m_status = 1;
	p_event->m_time = (int)time(NULL);
	strcpy(p_event->m_location, "新模范马路5号");
	p_event->m_lane_id = 2;
	p_event->m_drive_dir = 3;
	p_event->m_drive_dirext = 4;
	p_event->m_jpg_count = 2;
	for (jpg_index = 0; jpg_index < p_event->m_jpg_count; jpg_index++) {
		p_jpg = &p_event->m_jpg[jpg_index];
		strcpy(p_jpg->m_path, "D:\\photo\\test\\1.jpg");
		p_jpg->m_size = 234567;
		p_jpg->m_buffer = (unsigned char *)&p_jpg->m_size;
	}

	p_carplate = &p_event->m_car.m_plate;
	strcpy(p_carplate->m_number, "苏AB12345");
	strcpy(p_carplate->m_color, "蓝");
	p_carplate->m_color_code = 1;
	p_carplate->m_confidence = 95;
	p_carbody = &p_event->m_car.m_body;
	strcpy(p_carbody->m_type, "小型车");
	strcpy(p_carbody->m_color, "红");
	strcpy(p_carbody->m_logo, "大众");
	strcpy(p_carbody->m_chexi, "大众系");
	strcpy(p_carbody->m_chexing, "大众 2003");
	p_carbody->m_type_code = 3;
	p_carbody->m_color_code = 4;
	p_carbody->m_logo_code = 5;
	p_carbody->m_chexi_code = 6;
	p_carbody->m_chexing_code = 7;
	p_carbody->m_logo_confidence = 95;
	p_carbody->m_chexi_confidence = 96;
	p_carbody->m_chexing_confidence = 97;

	p_event->m_upload_time = (int)time(NULL);
}

void InitEventGroup(t_EventGroup *p_event_group) {
	t_Event *p_event = NULL;
	int event_index;

	p_event_group->m_event_count = 3;
	for (event_index = 0; event_index < p_event_group->m_event_count; event_index++) {
		p_event = &p_event_group->m_event[event_index];
		InitEvent(p_event);
	}
}

void PrintEvent(const t_Event *p_event) {
	const t_Jpg *p_jpg = NULL;
	const t_CarPlate *p_carplate = NULL;
	const t_CarBody *p_carbody = NULL;
	int index;

	printf("%d ", p_event->m_status);
	printf("%d ", p_event->m_time);
	printf("%s ", p_event->m_location);
	printf("%d ", p_event->m_lane_id);
	printf("%d ", p_event->m_drive_dir);
	printf("%d \n", p_event->m_drive_dirext);
	printf(">>jpg_count=%d\n", p_event->m_jpg_count);
	for (index = 0; index < p_event->m_jpg_count; index++) {
		p_jpg = &p_event->m_jpg[index];
		printf("path=%s, size=%d, buffer=%p\n", 
			p_jpg->m_path, p_jpg->m_size, p_jpg->m_buffer
			);
	}
	p_carplate = &p_event->m_car.m_plate;
	p_carbody = &p_event->m_car.m_body;
	printf(">>carplate\n");
	printf("%s, color=%s, %d, %d\n",
		p_carplate->m_number, p_carplate->m_color, 
		p_carplate->m_color_code, p_carplate->m_confidence
		);
	printf(">>body\n");
	printf("type=%s, %d\n"
		"color=%s, %d\n"
		"logo=%s, %d, %d\n"
		"chexi=%s, %d, %d\n"
		"chexing=%s, %d, %d\n",
		p_carbody->m_type, p_carbody->m_type_code, 
		p_carbody->m_color, p_carbody->m_color_code, 
		p_carbody->m_logo, p_carbody->m_logo_code, p_carbody->m_logo_confidence, 
		p_carbody->m_chexi, p_carbody->m_chexi_code, p_carbody->m_chexi_confidence, 
		p_carbody->m_chexing, p_carbody->m_chexing_code, p_carbody->m_chexing_confidence
		);	
}

void PrintEventGroup(const t_EventGroup *p_event_group) {
	const t_Event *p_event = NULL;
	int event_index;

	printf("event_count:%d\n", p_event_group->m_event_count);
	for (event_index = 0; event_index < p_event_group->m_event_count; event_index++) {
		p_event = &p_event_group->m_event[event_index];
		printf("-------------%d----------------\n", event_index);
		PrintEvent(p_event);
	}
}


