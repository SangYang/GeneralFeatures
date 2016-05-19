#include <stdio.h>
#include <string.h>
#include "violate_data.h"
#include "linux_msg.h"

int main(void) {
	int queue_id;
	int push_ret;
	int pop_ret;
	t_VioData vio_data;
	bool ok_create;	
	
	ok_create = CreateMsgQueue(&queue_id);
	if (false == ok_create) {
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

		/*memset(&vio_data, 0, sizeof(vio_data));		
		pop_ret = PopMsgQueue(queue_id, &vio_data, sizeof(vio_data));
		printf("pop_ret=%d\n", pop_ret);	
		printf("vio_data.m_pic_path=%s\n", vio_data.m_pic_path);
		ShowMsgQueue(queue_id);	*/
		
		DeleteMsgQueue(queue_id);
	}

	return 0;
}

