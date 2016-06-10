#ifndef SSY_SEMAPHORE_H
#define SSY_SEMAPHORE_H


int CreateSem(key_t key, int nums);
void SetvalueSem(int semid, int index, int value);
void GetvalueSem(int semid, int index, int *p_value);
int Sem_P(int semid, int index);
int Sem_V(int semid, int index);
void DeleteSem(int semid);
void PrintSeminfo(int semid);


#endif // SSY_SEMAPHORE_H
