#ifndef SSY_BASIC_DATA_H
#define SSY_BASIC_DATA_H


typedef struct {
	char m_number[16];   // ���ƺ���
	char m_color[8];     // ������ɫ
	int  m_color_code;   // ������ɫ����
	int  m_confidence;   // ���ƿ��Ŷ�
} t_CarPlate;

typedef struct {
	char m_type[16];            // ��������
	char m_color[8];            // ������ɫ
	char m_logo[16];            // Ʒ��
	char m_chexi[32];           // ��ϵ
	char m_chexing[64];         // ����
	int  m_type_code;           // �������ʹ���
	int  m_color_code;          // ������ɫ����
	int  m_logo_code;           // Ʒ�ƴ���
	int  m_chexi_code;          // ��ϵ����
	int  m_chexing_code;        // ���ʹ���
	int  m_logo_confidence;     // Ʒ�ƿ��Ŷ�
	int  m_chexi_confidence;    // ��ϵ���Ŷ�
	int  m_chexing_confidence;  // ���Ϳ��Ŷ�
} t_CarBody;

typedef struct {
	t_CarBody  m_body;
	t_CarPlate m_plate;
} t_Car;

typedef struct {
	char          m_path[256];  // jpg·��
	int           m_size;       // jpg��С
	unsigned char *m_buffer;    // jpg����
} t_Jpg;

typedef struct {
	int   m_status;        // ״̬��0 ��ʼ��1 ����ˣ�2 ���ϴ�
	int   m_time;          // ʱ��
	char  m_location[64];  // �ص�
	int   m_lane_id;       // ������
	int   m_drive_dir;     // ��ʻ�����
	int   m_drive_dirext;  // ��ʻ������չ��
	int   m_jpg_count;     // ͼƬʵ������
	t_Jpg m_jpg[4];        // ͼƬ��0 ͼƬ1��1 ͼƬ2��2 ͼƬ3��3 ����ͼƬ
	t_Car m_car;           // ͼƬ�е��¼�����
	int   m_upload_time;   // �ϴ�ʱ��
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

