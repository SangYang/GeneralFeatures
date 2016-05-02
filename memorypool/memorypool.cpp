#include <stdio.h>
#include <string.h>
#include <windows.h>
#include "debug_log.h"
#include "memorypool.h"

typedef struct {
	char              *m_pool;
	CRITICAL_SECTION  m_lock;
	int               m_data_size;
	int               m_data_count;
	int               m_data_pos;
}t_MemoryPool,*t_LPMemoryPool;

static t_LPMemoryPool g_p_mem_pool = NULL;

static t_LPMemoryPool MallocMemoryPool(const int data_size, const int data_count) {
	t_LPMemoryPool p_mem_pool;

	p_mem_pool = (t_LPMemoryPool)malloc(sizeof(t_MemoryPool));
	if (!p_mem_pool)
		LOG("MallocMemoryPool() error=%d", p_mem_pool);
	else {
		memset(p_mem_pool, 0, sizeof(t_MemoryPool));
		
		if (data_size <= 0 || data_count <= 0) {
			free(p_mem_pool);
			p_mem_pool = NULL;
		}
		else {
			p_mem_pool->m_data_size = data_size;
			p_mem_pool->m_data_count = data_count;
			p_mem_pool->m_data_pos = -1;
			p_mem_pool->m_pool = (char*)malloc(sizeof(char) * data_count * data_size + 512);
			if (!p_mem_pool->m_pool) {
				free(p_mem_pool);
				p_mem_pool = NULL;
			}
			else {
				memset(p_mem_pool->m_pool, 0, sizeof(char) * data_count * data_size);
			}
		}
	}
	return p_mem_pool;
}

static bool FreeMemoryPool(t_LPMemoryPool p_mem_pool) {
	if (!p_mem_pool)
		return false;
	else {
		if (!p_mem_pool->m_pool)
			return false;
		else {
			free(p_mem_pool->m_pool);
			free(p_mem_pool);
			p_mem_pool = NULL;
			return true;
		}		
	}
}

static int NextPosInMemoryPool(void *p_para) {
	t_LPMemoryPool p_mem_pool;

	if (!p_para)
		return -1;
	else {
		p_mem_pool = (t_LPMemoryPool)p_para;
		if (p_mem_pool->m_data_pos >= p_mem_pool->m_data_count-1) 
			p_mem_pool->m_data_pos = 0;
		else
			p_mem_pool->m_data_pos++;
	}
	return p_mem_pool->m_data_pos;
}

static char* NextDataInMemoryPool(void *p_para){
	t_LPMemoryPool p_mem_pool;
	int data_pos;

	if (!p_para)
		return NULL;
	else {
		p_mem_pool = (t_LPMemoryPool)p_para;
		data_pos = NextPosInMemoryPool(p_mem_pool);
		if (data_pos < 0) 
			return NULL;
		else
			return &p_mem_pool->m_pool[data_pos * p_mem_pool->m_data_size];
	}
}

char* WorkMemoryPool() {
	char *p_data;
	EnterCriticalSection(&g_p_mem_pool->m_lock);
	p_data = NextDataInMemoryPool(g_p_mem_pool);
	LeaveCriticalSection(&g_p_mem_pool->m_lock);
	return p_data;
}

bool InitMemoryPool(const int data_size, const int data_count) {
	if (!g_p_mem_pool) {
		g_p_mem_pool = MallocMemoryPool(data_size, data_count);
		if (!g_p_mem_pool)
			return false;
		else {
			InitializeCriticalSection(&g_p_mem_pool->m_lock);
			return true;
		}
	}
	else
		return false;
}

bool DeleteMemoryPool() {
	if (g_p_mem_pool) {
		FreeMemoryPool(g_p_mem_pool);
		DeleteCriticalSection(&g_p_mem_pool->m_lock);
		return true;
	}
	else
		return false;
}

char* RetStrFromMemoryPool(const char *data, const int data_size){
	char *p_ret;
	if (!data) 
		return NULL;
	else {
		InitMemoryPool(data_size, 32);
		p_ret = WorkMemoryPool();
		if (!p_ret)
			return NULL;
		else
			memcpy(p_ret, data, strlen(data)+1);
		return p_ret;
	}
}


#if 0
char* Test_1() {
	char str_1[256] = "11111111";
	return RetStrFromMemoryPool(str_1, 10);
}
char* Test_2() {
	char str_1[256] = "22222222";
	return RetStrFromMemoryPool(str_1, 10);
}
char* Test_3() {
	char str_1[256] = "33333333";
	return RetStrFromMemoryPool(str_1, 10);
}
char* Test_4() {
	char str_1[256] = "44444444";
	return RetStrFromMemoryPool(str_1, 10);
}
char* Test_5() {
	char str_1[256] = "55555555";
	return RetStrFromMemoryPool(str_1, 10);
}
char* Test_6() {
	char str_1[256] = "66666666";
	return RetStrFromMemoryPool(str_1, 10);
}
char* Test_7() {
	char str_1[256] = "77777777";
	return RetStrFromMemoryPool(str_1, 10);
}
char* Test_8() {
	char str_1[256] = "88888888";
	return RetStrFromMemoryPool(str_1, 10);
}

int main() {
	char *str_1;
	char *str_2;
	char *str_3;
	char *str_4;
	char *str_5;
	char *str_6;
	char *str_7;
	char *str_8;
	str_1 = Test_1();
	str_2 = Test_2();
	str_3 = Test_3();
	str_4 = Test_4();
	str_5 = Test_5();
	str_6 = Test_6();
	str_7 = Test_7();
	str_8 = Test_8();
	printf("Test_1: %s\n", str_1);
	printf("Test_2: %s\n", str_2);
	printf("Test_3: %s\n", str_3);
	printf("Test_4: %s\n", str_4);
	printf("Test_5: %s\n", str_5);
	printf("Test_6: %s\n", str_6);
	printf("Test_7: %s\n", str_7);
	printf("Test_8: %s\n", str_8);
	return 0;
}
#endif