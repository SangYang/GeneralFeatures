#ifndef SSY_LANE_DATA_H
#define SSY_LANE_DATA_H


typedef struct {
	int m_x;   // �ٷֱ�ֵ
	int m_y;   // �ٷֱ�ֵ
} t_Point;

typedef struct {
	t_Point m_head;   // �߶�ͷ
	t_Point m_tail;   // �߶�β
} t_Line;

typedef struct {
	int     m_count;     // ������Point����
	t_Point m_point[99]; // ������Point
	int     m_type;      // ����������
} t_Lane; 

typedef struct {
	int     m_lane_count;     // ��������
	t_Lane  m_lane_line[6];   // ������
	int     m_point_count;    // �������Point����
	t_Point m_detect_area[99];// �����������
	t_Line  m_kakou_line;     // ������
	char    m_reserved[32];   // ����ֵ
} t_DeviceLine;

typedef struct {
	char         m_sign[8];      // ͷ������topsky��
	int          m_type;         // ���ͣ��̶�ֵ��3:Ҫxml��4:��xml��
	char         m_ip[20];       // ���IP
	t_DeviceLine m_device_line;  // �豸��
} t_DeviceData;

typedef struct {
	char    m_sign[8];          // ͷ������topsky��
	int     m_type;	            // ���ͣ��̶�ֵ��1:Ҫxml��2:��xml��
	int     m_max_speed;        // ���ʱ�٣�km/h
	int     m_before_second;    // ¼���¼�ǰ������
	int     m_after_second;     // ¼���¼��������	
	int     m_cal_before_second;// �㷨ʱ���¼�ǰ������
	int     m_cal_after_second; // �㷨ʱ���¼��������
} t_SystemSet;

typedef struct {
	char    m_sign[8];      // ͷ������topsky��
	int     m_type;         // ���ͣ��̶�ֵ��5:ҪͼƬ��
	char    m_ip[20];       // ���IP
	int     m_jpg_size;     // JPGͼƬ��С
} t_JpgData;

typedef struct {
	char m_kakou_ip[20];       // �������IP
	char m_qiuji_ip[20];       // ���IP
	char m_quanjing_ip[20];    // ȫ�����IP
	char m_quanjing_user[20];  // ȫ������û���
	char m_quanjing_pwd[20];   // ȫ���������
	int  m_quanjing_port;      // ȫ������˿�
} t_IpGroup;

typedef struct {
	char        m_sign[8];           // ͷ������topsky��
	int         m_type;	             // ���ͣ��̶�ֵ��6:Ҫxml��7:��xml��
	int         m_ipgroup_count;     // IP�������
	t_IpGroup   m_ipgroup_array[64]; // IP��
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

