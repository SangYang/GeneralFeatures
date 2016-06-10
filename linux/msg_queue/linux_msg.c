#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/types.h>

#define  MSG_BUFF_MAX    512
#define  MSG_TYPE        1001

typedef struct {
	long m_type;
	char m_text[MSG_BUFF_MAX];
} t_Msg;

static bool SetMsgQueue(const int queue_id, const int msgnmb_max) {
	struct msqid_ds queue_info;
	int ctl_ret;
	
	ctl_ret = msgctl(queue_id, IPC_STAT, &queue_info);
	if (-1 == ctl_ret) {
		printf("msgctl() error! id=%d, errno=%s\n", queue_id, (char *)strerror(errno));
		return false;
	}
	else {
		queue_info.msg_qbytes = sizeof(t_Msg) * msgnmb_max;		
		ctl_ret = msgctl(queue_id, IPC_SET, &queue_info);
		if (-1 == ctl_ret) {
			printf("msgctl() error! id=%d, errno=%s\n", queue_id, (char *)strerror(errno));
			return false;
		}
		else {		
			return true;			
		}
	}
}

bool CreateMsgQueue(const char *p_path, const int msgnmb_max, int *p_queue_id) {
    key_t key;
	int queue_id;
	bool ok_set;
	
	if (NULL == p_path || NULL == p_queue_id) {
		printf("CreateMsg() failure! Input para is error!\n");
		return false;
	}
	else {
        key = ftok(p_path, 0);
        if (-1 == key) {
            printf( "ftok error, %s", (char *)strerror(errno) );
            return false;
        }
		else {
			queue_id = msgget(51002/*key*/, IPC_CREAT | 0666);
			if (-1 == queue_id) {
				printf("msgget() error! errno=%d:%s\n", errno, (char *)strerror(errno));
				return false;
			}
			else {
				ok_set = SetMsgQueue(queue_id, msgnmb_max);
				if (false == ok_set) {
					printf("SetMsgQueue() error!\n");
					return false;
				}
				else {
					*p_queue_id = queue_id;
					return true;					
				}
			}			
		}
	}
}

bool DeleteMsgQueue(const int queue_id) {
	int ctl_ret;
	
	ctl_ret = msgctl(queue_id, IPC_RMID, NULL);
	if (-1 == ctl_ret) {
		printf("msgctl() error! errno=%s\n", (char *)strerror(errno));
		return false;
	}
	else 
		return true;
}


bool GetMsgQueue(const char *p_path, int *p_queue_id) {
    key_t key;
	int queue_id;
	bool ok_set;
	
	if (NULL == p_path || NULL == p_queue_id) {
		printf("CreateMsg() failure! Input para is error!\n");
		return false;
	}
	else {
        key = ftok(p_path, 0);
        if (-1 == key) {
            printf( "ftok error, %s", (char *)strerror(errno) );
            return false;
        }
		else {
			queue_id = msgget(51002/*key*/, IPC_CREAT | 0666);
			if (-1 == queue_id) {
				printf("msgget() error! errno=%d:%s\n", errno, (char *)strerror(errno));
				return false;
			}
			else {
				*p_queue_id = queue_id;
				return true;					
			}			
		}
	}
}


int PushMsgQueue(const int queue_id, void *p_buff, const int buff_size) {
	t_Msg msg;
	int snd_ret;
	
	if (NULL == p_buff || MSG_BUFF_MAX <= buff_size) {
		printf("SendMsg() error! Input para is error!\n");
		return -1;
	}
	else {
		msg.m_type = MSG_TYPE;
		memcpy(msg.m_text, p_buff, (size_t)buff_size);	
		snd_ret = msgsnd(queue_id, &msg, sizeof(msg.m_text), IPC_NOWAIT);
		if (-1 == snd_ret) {
			printf("msgsnd() error! errno=%d,%s\n", errno, (char *)strerror(errno));
			return -1;
		}
		else
			return buff_size;
	}
}

int PopMsgQueue(const int queue_id, void *p_buff, const int buff_size) {
	t_Msg msg;
	int rcv_ret;
	
	if (NULL == p_buff) {
		printf("RecvMsg() error! Input para is error!\n");
		return -1;
	}
	else {
		msg.m_type = MSG_TYPE;
		rcv_ret = msgrcv(queue_id, &msg, sizeof(msg.m_text), 0, IPC_NOWAIT | MSG_NOERROR); // 先进先出
		if (-1 == rcv_ret) {
			printf("msgrcv() error! errno=%d,%s\n", errno, (char *)strerror(errno));
			return -1;
		}
		else {
			if (sizeof(msg.m_text) < rcv_ret || buff_size > rcv_ret) {
				printf("RecvMsg() error! Input para buff_size is small!\n");
				return -1;
			}
			else {
				memcpy(p_buff, msg.m_text, buff_size);
				return buff_size;
			}
		}
	}
}

bool ShowMsgQueue(const int queue_id) {
	struct msqid_ds queue_info;
	int ctl_ret;
	
	ctl_ret = msgctl(queue_id, IPC_STAT, &queue_info);
	if (-1 == ctl_ret) {
		printf("msgctl() error! errno=%s\n", (char *)strerror(errno));
		return false;
	}
	else {
		printf("--------------MsgQueue:%d--------------\n", queue_id);
		printf("msg_cbytes=%d\n", (int)queue_info.msg_cbytes);
		printf("msg_qnum=%d\n", (int)queue_info.msg_qnum);
		printf("msg_qbytes=%d\n", (int)queue_info.msg_qbytes);
		printf("msg_lspid=%d\n", (int)queue_info.msg_lspid);
		printf("msg_lrpid=%d\n", (int)queue_info.msg_lrpid);
		printf("msg_stime=%d\n", (int)queue_info.msg_stime);
		printf("msg_rtime=%d\n", (int)queue_info.msg_rtime);
		printf("msg_ctime=%d\n", (int)queue_info.msg_ctime);
		printf("msg_perm.uid=%d\n", (int)queue_info.msg_perm.uid);
		printf("msg_perm.gid=%d\n", (int)queue_info.msg_perm.gid);
		return true;
	}
}

int GetMsgNumInQueue(const int queue_id) {
	struct msqid_ds queue_info;
	int ctl_ret;
	
	ctl_ret = msgctl(queue_id, IPC_STAT, &queue_info);
	if (-1 == ctl_ret) {
		printf("msgctl() error! errno=%s\n", (char *)strerror(errno));
		return -1;
	}
	else {
		return (int)queue_info.msg_qnum;
	}
}

#if 0
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

int main(void) {
	int queue_id;
	int push_ret;
	int pop_ret;
	t_VioData vio_data;
	bool ok_open;	
	
	ok_open = GetMsgQueue("/TOPSKY/LOCAL/car_phone_server_queue", &queue_id);
	//ok_open = CreateMsgQueue(".", 6, &queue_id);
	if (false == ok_open) {
		printf("CreateMsgQueue() error!\n");
	}
	else {	
		printf("CreateMsgQueue() success! queue_id=%d\n", queue_id);	
		ShowMsgQueue(queue_id);
		
		memset(&vio_data, 0, sizeof(vio_data));			
		strcpy(vio_data.m_pic_path, "abcdefg");		
		push_ret = PushMsgQueue(queue_id, &vio_data, sizeof(vio_data));
		printf("push_ret=%d\n", push_ret);		
		ShowMsgQueue(queue_id);
	
		strcpy(vio_data.m_pic_path, "hijklmn");	
		push_ret = PushMsgQueue(queue_id, &vio_data, sizeof(vio_data));
		printf("push_ret=%d\n", push_ret);		
		ShowMsgQueue(queue_id);			

		memset(&vio_data, 0, sizeof(vio_data));		
		pop_ret = PopMsgQueue(queue_id, &vio_data, sizeof(vio_data));
		printf("pop_ret=%d\n", pop_ret);	
		printf("vio_data.m_pic_path=%s\n", vio_data.m_pic_path);
		ShowMsgQueue(queue_id);	

		memset(&vio_data, 0, sizeof(vio_data));		
		pop_ret = PopMsgQueue(queue_id, &vio_data, sizeof(vio_data));
		printf("pop_ret=%d\n", pop_ret);	
		printf("vio_data.m_pic_path=%s\n", vio_data.m_pic_path);
		ShowMsgQueue(queue_id);	
		
		//DeleteMsgQueue(queue_id);
	}

	return 0;
}

#endif
