#ifndef SSY_VIOLATE_DATA_H
#define SSY_VIOLATE_DATA_H


typedef struct {
	char  m_time[16];       // 违章时间 1463482216004（毫秒）
	char  m_location[64];   // 违章地点（上海路）
	int   m_type;           // 违章类型
	int   m_carspeed;       // 车速
	int   m_carcolor;       // 车身颜色
	int   m_cartype;        // 车辆类型
	int   m_chexing;        // 车型代码
	char  m_plate[16];      // 车牌号码
	char  m_plate_area[32]; // 车牌图片坐标
	char  m_pic_path[256];  // 违章图片
} t_VioData;



#endif // SSY_VIOLATE_DATA_H