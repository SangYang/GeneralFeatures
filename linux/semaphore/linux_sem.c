#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/types.h>

union semun {
	int              val;
	struct semid_ds *buf;
	unsigned short  *array;
	struct seminfo  *__buf;
};

int CreateSem(key_t key, int nums) {
	int semid;

	semid = semget(key, nums, IPC_CREAT | 0666); 
	if (-1 == semid) {
		printf("semget() error=%d,%s\n", errno, strerror(errno));
		return -1;
	}	
	
	return semid;
}

void SetvalueSem(int semid, int index, int value) {
	union semun sem;
	int ctl_ret;
	
	sem.val = value;
	ctl_ret = semctl(semid, index, SETVAL, sem);
	if (-1 == ctl_ret) {
		printf("semctl() error! SETVAL index=%d, value=%d\n", index, value);
	}
}

void GetvalueSem(int semid, int index, int *p_value) {
	union semun sem;
	int ctl_ret;
	
	ctl_ret = semctl(semid, index, GETVAL, sem);
	if (-1 == ctl_ret) {
		printf("semctl() error! GETVAL index=%d, value=%d\n", index, sem.val);
		*p_value = -1;
	}
	else {
		*p_value = sem.val;		
	}
}

int Sem_P(int semid, int index) {
	struct sembuf sops = {index, +1, IPC_NOWAIT};
	return semop(semid, &sops, 1);
}

int Sem_V(int semid, int index) {
	struct sembuf sops = {index, -1, IPC_NOWAIT};
	return semop(semid, &sops, 1);
}

void DeleteSem(int semid) {
	semctl(semid, 0, IPC_RMID);
}

void PrintSeminfo(int semid) {
	struct seminfo seminf;
	union semun sem;
	int ctl_ret;
	
	sem.__buf = &seminf;
	ctl_ret = semctl(semid, 0, SEM_INFO, sem);
	if (-1 == ctl_ret) {
		printf("semctl() error=%d,%s\n", errno, strerror(errno));
	}
	else {
		printf("--------------BEGIN---------------\n"
			"semmap=%d\nsemmni=%d\nsemmns=%d\n"
			"semmnu=%d\nsemmsl=%d\nsemopm=%d\n"
			"semume=%d\nsemusz=%d\nsemvmx=%d\n"
			"semaem=%d\n"
			"-----------------END----------------\n", 
			sem.__buf->semmap, sem.__buf->semmni, sem.__buf->semmns,
			sem.__buf->semmnu, sem.__buf->semmsl, sem.__buf->semopm,
			sem.__buf->semume, sem.__buf->semusz, sem.__buf->semvmx,
			sem.__buf->semaem);
	}
}

#if 1
int main(void) {
	key_t key;
	int semid;
	char i;
	struct semid_ds buf;
	int value;
	
	key = ftok("/home/ssyang", 0);
	semid = CreateSem(key, 5);
	PrintSeminfo(semid);
	GetvalueSem(semid, 0, &value);	
	printf("key=%d, semid=%d, value=%d\n", key, semid, value);
	SetvalueSem(semid, 0, 100);	
	GetvalueSem(semid, 0, &value);		
	printf("value=%d\n", value);
	for (i = 0; i <= 3; i++) {
		Sem_P(semid, 0);
		Sem_V(semid, 0);
		printf("--value=%d\n", value);		
	}
	SetvalueSem(semid, 0, 200);
	GetvalueSem(semid, 0, &value);	
	printf("信号量值为：%d\n", value);
	DeleteSem(semid);
	
	return 0;
	
}
#endif
