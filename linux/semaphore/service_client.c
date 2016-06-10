#include <errno.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <sys/types.h>

static int g_semid = 0;

union semun {
	int val;
	struct semid_ds *buf;
	unsigned short *array;
	struct seminfo *__buf;
};
 
static void sigintfunc(int signum) {
    exit(0);
}
 
static void atexitfunc() {
    printf("DelSem:%d\n", g_semid);
    semctl(g_semid, 0, IPC_RMID, 0);
}
 
void server(void) {
	struct sembuf semb;
    union semun sem; 
    int semid, shmid;
    char *shmdata;
	int semkey, shmkey;
	int i;
	
    atexit(&atexitfunc); // 在 exit() 函数执行前删除信号量	
    signal(SIGINT, &sigintfunc); // Ctrl+C 终止程序信号

	semkey = ftok("/etc", 0);
	printf("semkey=%d\n", semkey);
    semid = semget(semkey, 2, IPC_CREAT | SHM_R | SHM_W);
	g_semid = semid;  
    sem.val = 1;
    semctl(semid, 0, SETVAL, sem);
    sem.val = 0; 
    semctl(semid, 1, SETVAL, sem);
	
	shmkey = ftok("/etc", 0);
	printf("shmkey=%d\n", shmkey);
    shmid = shmget(shmkey, 256, IPC_CREAT | SHM_R | SHM_W); 
    shmdata = shmat(shmid, 0, 0);
    shmctl(shmid, IPC_RMID, NULL); // 将该共享内存标志为已销毁的，使用完毕后将被自动销毁
	
    printf("[server]semid=%d, shmid=%d, shmdata:%s\n" , semid, shmid, shmdata);
	
    while (1) {
		printf("[server]waiting...\n" );
		
		semb.sem_num = 1;
		semb.sem_op = -1;
		semb.sem_flg = SEM_UNDO;
		semop(semid, &semb, 1);
		
		printf("[server]shmdata:%s\n", shmdata);

		semb.sem_num = 0;
		semb.sem_op = 1;
		semb.sem_flg = SEM_UNDO;
		semop(semid, &semb, 1);
			
    }
}
 
void client(int semid, int shmid) {
	struct sembuf semb;
    char *shmdata;
    char input[8];	
	
    shmdata = shmat(shmid, 0, 0); 
	
    printf("[client]semid=%d, shmid=%d, shmdata:%s\n" , semid, shmid, shmdata);
	
    while (1) {
       printf("\n -menu \n -1.Send a message \n -2.Exit \n -Input:" );
       fgets(input, sizeof(input), stdin);
       switch (input[0]) {
		   case '1' :
				semb.sem_num = 0;
				semb.sem_op = -1;
				semb.sem_flg = SEM_UNDO;
				semop(semid, &semb, 1);			
				
				printf("[client]enter message:");
				gets(shmdata);
				
				semb.sem_num = 1;
				semb.sem_op = 1;
				semb.sem_flg = SEM_UNDO;
				semop(semid, &semb, 1);	
				
				break ;
		   case '2' :
				exit(0);
				break ;
       }
    }
}

int main(int argc, char *argv[]) 
{
    if (1 == argc) {
       server();
    }
    else if (3 == argc) {
       client(atoi(argv[1]), atoi(argv[2])); 
    }
	else {
		printf("main para is error!\n");
	}
	
    return 0;
}
 
