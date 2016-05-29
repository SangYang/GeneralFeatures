#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/sem.h>
#include <sys/ipc.h>

typedef int sem_t;

union semun {
	int val;
	struct semid_ds *buf;
	unsigned short *array;
	struct seminfo *__buf;
} arg;

sem_t CreateSem(key_t key, int value) {
	union semun sem;
	sem_t semid;

	sem.val = value;
	semid = semget(key, 1, IPC_CREAT | 0666);
	if (-1 == semid) {
		printf("semget() error=%d,%s\n", errno, strerror(2));
		return -1;
	}
	
	semctl(semid, 0, SETVAL, sem);

	return semid;
}

/*
struct sembuf {
	ushort sem_num;
	short sem_op;
	short sem_flg;
};
*/

int Sem_P(sem_t semid) {
	struct sembuf sops = {0, +1, IPC_NOWAIT};
	return (semop(semid, &sops, 1));
}

int Sem_V(sem_t semid) {
	struct sembuf sops = {0, -1, IPC_NOWAIT};
	return (semop(semid, &sops, 1));
}

void SetvalueSem(sem_t semid, int value) {
	union semun sem;

	sem.val = value;
	semctl(semid, 0, SETVAL, sem);
}

int GetvalueSem(sem_t semid) {
	union semun sem;

	return semctl(semid, 0, GETVAL, sem);
}

void DestroySem(sem_t semid) {
	union semun sem;
	
	sem.val = 0;
	semctl(semid, 0, IPC_RMID, sem);
}


#if 1
int main(void) {
	key_t key;
	int semid;
	char i;
	struct semid_ds buf;
	int value = 0;
	
	key = ftok("/home/sang", 0);
	printf("key=%d\n", key);
	semid = CreateSem(key, 100);
	for (i = 0; i <= 3; i++) {
		Sem_P(semid);
		Sem_V(semid);
	}
	value = GetvalueSem(semid);
	printf("信号量值为：%d\n", value);
	DestroySem(semid);
	
	return 0;
	
}
#endif
