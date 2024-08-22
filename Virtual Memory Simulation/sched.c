#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/msg.h>
#include <sys/sem.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <string.h>
#include <time.h>

#define P(s) semop(s, &pop, 1)
#define V(s) semop(s, &vop, 1)

typedef struct message1
{
    long type;
    int pid;
} message1;


int main(int argc, char *argv[])
{
    struct sembuf pop = {0, -1, 0};
    struct sembuf vop = {0, 1, 0};

    if (argc != 4)
    {
        printf("Usage: %s <Message Queue 1 ID> <Message Queue 2 ID> <# Processes>\n", argv[0]);
        exit(1);
    }

    int msgid1 = atoi(argv[1]);
    int msgid2 = atoi(argv[2]);
    int k = atoi(argv[3]);

    // Print the values
    // printf("msgid1: %d\n", msgid1);
    // printf("msgid2: %d\n", msgid2);
    // printf("k: %d\n", k);

    key_t key = ftok(".", 'D');
    int semid = semget(key, 1, IPC_CREAT | 0666);

    key = ftok(".", 'G');
    int semid4 = semget(key, 1, IPC_CREAT | 0666);

    int pid = getpid();

    message1 msg1;
    message1 msg2;

    while (k > 0)
    {
        // wait for processes to come
        msgrcv(msgid1, (void *)&msg1, sizeof(message1), 0, 0);

        printf("\t\tScheduling process %d\n", msg1.pid);

        // signal process to start
        V(semid);

        // wait for mmu
        msgrcv(msgid2, (void *)&msg2, sizeof(message1), 0, 0);

        // check the type of message
        if (msg2.type == 2)
        {
            printf("\t\tProcess %d terminated\n", msg2.pid);
            k--;
        }
        else if (msg2.type == 1)
        {
            printf("\t\tProcess %d added to end of queue\n", msg2.pid);
            msg1.pid = msg2.pid;
            msg1.type = 1;
            msgsnd(msgid1, (void *)&msg1, sizeof(message1), 0);
        }
    }

    printf("\t\tScheduler terminated\n");

    // signal master on semid4
    V(semid4);

    return 0;
}