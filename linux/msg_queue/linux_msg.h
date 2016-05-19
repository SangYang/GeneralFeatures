#ifndef SSY_LINUX_MSG_H
#define SSY_LINUX_MSG_H


#ifndef __plusplus
#define bool   int
#define false  0
#define true   1
#endif // __plusplus

#ifdef __plusplus
extern "C" {
#endif

	bool CreateMsgQueue(int *p_queue_id);
	bool DeleteMsgQueue(const int queue_id);
	int PushMsgQueue(const int queue_id, void *p_buff, const int buff_size);
	int PopMsgQueue(const int queue_id, void *p_buff, const int buff_size);
	bool ShowMsgQueue(const int queue_id);

#ifdef __plusplus
}
#endif


#endif // SSY_LINUX_MSG_H