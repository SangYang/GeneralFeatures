#ifndef SSY_BASIC_DATA_H
#define SSY_BASIC_DATA_H


typedef struct {
	char m_number[16];   // 车牌号码
	char m_color[8];     // 车牌颜色
	int  m_color_code;   // 车牌颜色代码
	int  m_confidence;   // 车牌可信度
} t_CarPlate;

typedef struct {
	char m_type[16];            // 车辆类型
	char m_color[8];            // 车身颜色
	char m_logo[16];            // 品牌
	char m_chexi[32];           // 车系
	char m_chexing[64];         // 车型
	int  m_type_code;           // 车辆类型代码
	int  m_color_code;          // 车身颜色代码
	int  m_logo_code;           // 品牌代码
	int  m_chexi_code;          // 车系代码
	int  m_chexing_code;        // 车型代码
	int  m_logo_confidence;     // 品牌可信度
	int  m_chexi_confidence;    // 车系可信度
	int  m_chexing_confidence;  // 车型可信度
} t_CarBody;

typedef struct {
	t_CarBody  m_body;
	t_CarPlate m_plate;
} t_Car;

typedef struct {
	char          m_path[256];  // jpg路径
	int           m_size;       // jpg大小
	unsigned char *m_buffer;    // jpg缓存
} t_Jpg;

typedef struct {
	int   m_status;        // 状态：0 初始；1 已审核；2 已上传
	int   m_time;          // 时间
	char  m_location[64];  // 地点
	int   m_lane_id;       // 车道号
	int   m_drive_dir;     // 行驶方向号
	int   m_drive_dirext;  // 行驶方向扩展号
	int   m_jpg_count;     // 图片实际数量
	t_Jpg m_jpg[4];        // 图片：0 图片1；1 图片2；2 图片3；3 车牌图片
	t_Car m_car;           // 图片中的事件车辆
	int   m_upload_time;   // 上传时间
} t_Event;

typedef struct {
	t_Event m_event[100];
	int     m_event_count;
} t_EventGroup;


void InitEvent(t_Event *p_event);
void InitEventGroup(t_EventGroup *p_event_group);
void PrintEvent(const t_Event *p_event);
void PrintEventGroup(const t_EventGroup *p_event_group);


#endif // SSY_BASIC_DATA_H

