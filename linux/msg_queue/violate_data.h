#ifndef SSY_VIOLATE_DATA_H
#define SSY_VIOLATE_DATA_H


typedef struct {
	char  m_time[16];       // Υ��ʱ�� 1463482216004�����룩
	char  m_location[64];   // Υ�µص㣨�Ϻ�·��
	int   m_type;           // Υ������
	int   m_carspeed;       // ����
	int   m_carcolor;       // ������ɫ
	int   m_cartype;        // ��������
	int   m_chexing;        // ���ʹ���
	char  m_plate[16];      // ���ƺ���
	char  m_plate_area[32]; // ����ͼƬ����
	char  m_pic_path[256];  // Υ��ͼƬ
} t_VioData;



#endif // SSY_VIOLATE_DATA_H