#ifndef CAR_MEMORYPOOL_H
#define CAR_MEMORYPOOL_H


#ifndef __cplusplus
#define bool    int
#define true    1
#define false   0
#endif // __cplusplus


#ifdef __cplusplus
extern "C" {
#endif

bool  InitMemoryPool(const int data_size, const int data_count);
bool  DeleteMemoryPool();
char* WorkMemoryPool();
char* RetStrFromMemoryPool(const char *data, const int data_size);

#ifdef __cplusplus
};
#endif



#endif // CAR_MEMORYPOOL_H