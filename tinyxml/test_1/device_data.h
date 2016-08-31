#ifndef SSY_LANE_DATA_H
#define SSY_LANE_DATA_H


typedef struct {
	int m_x;   // 百分比值
	int m_y;   // 百分比值
} t_Point;

typedef struct {
	t_Point m_head;   // 线段头
	t_Point m_tail;   // 线段尾
} t_Line;

typedef struct {
	int     m_count;     // 车道线Point数量
	t_Point m_point[99]; // 车道线Point
	int     m_type;      // 车道线类型
} t_Lane; 

typedef struct {
	int     m_lane_count;     // 车道数量
	t_Lane  m_lane_line[6];   // 车道线
	int     m_point_count;    // 检测区域Point数量
	t_Point m_detect_area[99];// 检测区域多边形
	t_Line  m_kakou_line;     // 卡口线
	char    m_reserved[32];   // 保留值
} t_DeviceLine;

typedef struct {
	char         m_sign[8];      // 头部，“topsky”
	int          m_type;         // 类型，固定值（3:要xml；4:存xml）
	char         m_ip[20];       // 相机IP
	t_DeviceLine m_device_line;  // 设备线
} t_DeviceData;

typedef struct {
	char    m_sign[8];          // 头部，“topsky”
	int     m_type;	            // 类型，固定值（1:要xml；2:存xml）
	int     m_max_speed;        // 最大时速，km/h
	int     m_before_second;    // 录像事件前的秒数
	int     m_after_second;     // 录像事件后的秒数	
	int     m_cal_before_second;// 算法时间事件前的秒数
	int     m_cal_after_second; // 算法时间事件后的秒数
} t_SystemSet;

typedef struct {
	char    m_sign[8];      // 头部，“topsky”
	int     m_type;         // 类型，固定值（5:要图片）
	char    m_ip[20];       // 相机IP
	int     m_jpg_size;     // JPG图片大小
} t_JpgData;

typedef struct {
	char m_kakou_ip[20];       // 卡口相机IP
	char m_qiuji_ip[20];       // 球机IP
	char m_quanjing_ip[20];    // 全景相机IP
	char m_quanjing_user[20];  // 全景相机用户名
	char m_quanjing_pwd[20];   // 全景相机密码
	int  m_quanjing_port;      // 全景相机端口
} t_IpGroup;

typedef struct {
	char        m_sign[8];           // 头部，“topsky”
	int         m_type;	             // 类型，固定值（6:要xml；7:存xml）
	int         m_ipgroup_count;     // IP组的数量
	t_IpGroup   m_ipgroup_array[64]; // IP组
} t_DeviceGroup;

#ifdef __cplusplus
extern "C" {
#endif

	void GetDeviceDataInfo(int *p_total_size, int *p_header_size, int *p_reserved_size);
	void GetSystemSetInfo(int *p_total_size, int *p_header_size, int *p_reserved_size);
	void GetJpdDataInfo(int *p_total_size, int *p_header_size, int *p_reserved_size);
	void GetDeviceGroupInfo(int *p_total_size, int *p_header_size, int *p_reserved_size);
	void GetHeaderSign(const char *p_header, char *p_sign, const int sign_size);
	void GetHeaderType(const char *p_header, int *p_type);
	void PrintDeviceData(const t_DeviceData *p_device_data);
	void PrintSystemSet(const t_SystemSet *p_system_set);
	void PrintJpgData(const t_JpgData *p_jpg_data);
	void PrintDeviceGroup(const t_DeviceGroup *p_device_group);
	void InitDeviceData(t_DeviceData *p_device_data);
	void InitSystemSet(t_SystemSet *systemSet);
	void InitDeviceGroup(t_DeviceGroup *deviceGroup);

#ifdef __cplusplus
};
#endif


#endif // SSY_LANE_DATA_H

