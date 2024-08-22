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
#include <limits.h>
#include <time.h>
#include <signal.h>

#define PROB 0.1

#define P(s) semop(s, &pop, 1) // Wait operation
#define V(s) semop(s, &vop, 1) // Signal operation

int pidscheduler;
int pidmmu;

void sighand(int signum)
{
    if (signum == SIGINT)
    {
        // Kill scheduler, MMU, and all the processes
        kill(pidscheduler, SIGINT);
        kill(pidmmu, SIGINT);
        exit(1);
    }
}

typedef struct SM1
{
    int pid;                 // Process id
    int mi;                  // Number of required pages
    int fi;                  // Number of frames allocated
    int pagetable[10000][3]; // Page table
    int totalpagefaults;
    int totalillegalaccess;
} SM1;

int main()
{
    srand(time(0));

    struct sembuf pop = {0, -1, 0};
    struct sembuf vop = {0, 1, 0};

    int k, m, f;
    printf("Enter the number of processes: ");
    scanf("%d", &k);
    printf("Enter the Virtual Address Space size: ");
    scanf("%d", &m);
    printf("Enter the Physical Address Space size: ");
    scanf("%d", &f);

    // Page table for k processes
    key_t key = ftok(".", 'A');
    // printf("%ld\n", sizeof(SM1)); //32
    int shmid1 = shmget(key, k * sizeof(SM1), IPC_CREAT | 0666);
    if (shmid1 == -1)
    {
        perror("shmget failed for SM1");
        exit(EXIT_FAILURE);
    }
    SM1 *sm1 = (SM1 *)shmat(shmid1, NULL, 0);
    if (sm1 == (void *)-1)
    {
        perror("shmat failed for SM1");
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < k; i++)
    {
        // sm1[i].pagetable = (int **)malloc(m * sizeof(int *));
        // for (int j = 0; j < m; j++) {
        //     sm1[i].pagetable[j] = (int *)malloc(3 * sizeof(int));
        // }
        sm1[i].totalpagefaults = 0;
        sm1[i].totalillegalaccess = 0;
    }

    // Free frames list
    key = ftok(".", 'B');
    // printf("%d\n",f);
    int shmid2 = shmget(key, (f + 1) * sizeof(int), IPC_CREAT | 0666);
    if (shmid2 == -1)
    {
        perror("shmget failed for SM2");
        exit(EXIT_FAILURE);
    }
    int *sm2 = (int *)shmat(shmid2, NULL, 0);
    if (sm2 == (void *)-1)
    {
        perror("shmat failed for SM2");
        exit(EXIT_FAILURE);
    }

    // Initialize the frames, 1 means free, 0 means occupied, -1 means end of list
    for (int i = 0; i < f; i++)
    {
        sm2[i] = 1;
    }
    sm2[f] = -1;

    // // Ensuring the initialization is complete
    // printf("Initialization complete.\n");
    // fflush(stdout); // Force the output to be printed immediately

    // process to page mapping
    key = ftok(".", 'C');
    int shmid3 = shmget(key, k * sizeof(int), IPC_CREAT | 0666);
    int *sm3 = (int *)shmat(shmid3, NULL, 0);

    // initially no frames are allocated to any process
    for (int i = 0; i < k; i++)
    {
        sm3[i] = 0;
    }
    // semaphore 1 for Processes
    key = ftok(".", 'D');
    int semid1 = semget(key, 1, IPC_CREAT | 0666);
    semctl(semid1, 0, SETVAL, 0);

    // semaphore 2 for Scheduler
    key = ftok(".", 'E');
    int semid2 = semget(key, 1, IPC_CREAT | 0666);
    semctl(semid2, 0, SETVAL, 0);

    // semaphore 3 for Memory Management Unit
    key = ftok(".", 'F');
    int semid3 = semget(key, 1, IPC_CREAT | 0666);
    semctl(semid3, 0, SETVAL, 0);

    // semaphore 4 for Master
    key = ftok(".", 'G');
    int semid4 = semget(key, 1, IPC_CREAT | 0666);
    semctl(semid4, 0, SETVAL, 0);

    // Message Queue 1 for Ready Queue
    key = ftok(".", 'H');
    int msgid1 = msgget(key, IPC_CREAT | 0666);

    // Message Queue 2 for Scheduler
    key = ftok(".", 'I');
    int msgid2 = msgget(key, IPC_CREAT | 0666);

    // Message Queue 3 for Memory Management Unit
    key = ftok(".", 'J');
    int msgid3 = msgget(key, IPC_CREAT | 0666);

    // Print the IDs
    printf("Message Queue 1 ID: %d\n", msgid1);
    printf("Message Queue 2 ID: %d\n", msgid2);
    printf("Message Queue 3 ID: %d\n", msgid3);

    // convert msgid to string
    char msgid1str[10], msgid2str[10], msgid3str[10];
    sprintf(msgid1str, "%d", msgid1);
    sprintf(msgid2str, "%d", msgid2);
    sprintf(msgid3str, "%d", msgid3);

    // Print shmid1
    printf("Shared Memory ID 1 (Page Table): %d\n", shmid1);
    // Print shmid2
    printf("Shared Memory ID 2 (Free Frames List): %d\n", shmid2);
    // Print shmid3
    printf("Shared Memory ID 3 (Process to Page Mapping): %d\n", shmid3);

    // convert shmids to string
    char shmid1str[10], shmid2str[10], shmid3str[10];
    sprintf(shmid1str, "%d", shmid1);
    sprintf(shmid2str, "%d", shmid2);
    sprintf(shmid3str, "%d", shmid3);

    // pass number of processes for now
    char strk[10];
    sprintf(strk, "%d", k);

    // create Scheduler process, pass msgid1str and msgid2str
    pidscheduler = fork();
    if (pidscheduler == 0)
    {
        execlp("./sched", "./sched", msgid1str, msgid2str, strk, NULL);
    }

    // create Memory Management Unit process, pass msgid2str and msgid3str, shmid1str and shmid2str
    pidmmu = fork();
    if (pidmmu == 0)
    {
        execlp("./mmu", "./mmu", msgid2str, msgid3str, shmid1str, shmid2str, NULL);
        // execlp("xterm", "xterm", "-T", "Memory Management Unit", "-e", "./mmu", msgid2str, msgid3str, shmid1str, shmid2str, NULL);
    }
    printf("Initialization complete.\n");

    int **refi = (int **)malloc((k) * sizeof(int *));
    char **refstr = (char **)malloc((k) * sizeof(char *));

    // int refi[k][100000];
    // char refstr[k][100000];

    // initialize the Processes
    for (int i = 0; i < k; i++)
    {
        // generate random number of pages between 1 to m
        sm1[i].mi = rand() % m + 1;
        sm1[i].fi = 0;
        for (int j = 0; j < m; j++)
        {
            sm1[i].pagetable[j][0] = -1;      // no frame allocated
            sm1[i].pagetable[j][1] = 0;       // invalid
            sm1[i].pagetable[j][2] = INT_MAX; // timestamp
        }

        int y = 0;
        int x = rand() % (8 * sm1[i].mi + 1) + 2 * sm1[i].mi;

        // for (int j = 0; j < x; j++)
        // {
        refi[i] = (int *)malloc(x * sizeof(int));
        // }

        // refi[i] = (int *)malloc((10 * sm1[i].mi + 1) * sizeof(int));
        // // Allocate memory for reference string
        // refstr[i] = (char *)malloc((10 * sm1[i].mi + 1) * sizeof(char));
        // if (refstr[i] == NULL) {
        //     perror("malloc failed for refstr[i]");
        //     exit(EXIT_FAILURE);
        // }

        for (int j = 0; j < x; j++)
        {
            refi[i][j] = rand() % sm1[i].mi;
            int temp = refi[i][j];
            while (temp > 0)
            {
                temp /= 10;
                y++;
            }
            y++;
        }
        y++;

        // with probability PROB, corrupt the reference string, by putting illegal page number
        for (int j = 0; j < x; j++)
        {
            if ((double)rand() / RAND_MAX < PROB)
            {
                refi[i][j] = rand() % m;
            }
        }

        refstr[i] = (char *)malloc((2 * y) * sizeof(char));
        // memset(refstr[i], '\0', y);
        strcpy(refstr[i], "");

        // refstr[i][0] = '\0'; // Initialize as an empty string
        for (int j = 0; j < x; j++)
        {
            char temp[12] = {'\0'};
            sprintf(temp, "%d.", refi[i][j]);
            // puts(temp);
            strcat(refstr[i], temp);
        }
    }

    for (int i = 0; i < k; i++)
    {
        printf("Reference String of Process %d: ", i+1);
        puts(refstr[i]);
    }

    // create Processes
    for (int i = 0; i < k; i++)
    {
        usleep(250000);
        int pid = fork();
        if (pid == -1)
        {
            // Failed to create child process
            perror("fork");
            // Handle the error or exit
            exit(EXIT_FAILURE);
        }
        else if (pid == 0)
        {
            // Child process
            // Pass ref[i], msgid1str, msgid3str to the process program
            execlp("./process", "./process", refstr[i], msgid1str, msgid3str, NULL);
            // If execlp() fails, print error and exit
            perror("execlp");
            exit(EXIT_FAILURE);
        }
        else
        {
            // Parent process
            sm1[i].pid = pid;
        }
    }

    // wait for Scheduler to signal
    P(semid4);

    // Free dynamically allocated memory for refi and refstr
    for (int i = 0; i < k; i++)
    {
        free(refi[i]);
        free(refstr[i]);
    }
    free(refi);
    free(refstr);

    // terminate Scheduler
    kill(pidscheduler, SIGINT);

    // terminate Memory Management Unit
    kill(pidmmu, SIGINT);

    // detach and remove shared memory
    shmdt(sm1);
    shmctl(shmid1, IPC_RMID, NULL);

    shmdt(sm2);
    shmctl(shmid2, IPC_RMID, NULL);

    shmdt(sm3);
    shmctl(shmid3, IPC_RMID, NULL);

    // remove semaphores
    semctl(semid1, 0, IPC_RMID, 0);
    semctl(semid2, 0, IPC_RMID, 0);
    semctl(semid3, 0, IPC_RMID, 0);
    semctl(semid4, 0, IPC_RMID, 0);

    // remove message queues
    msgctl(msgid1, IPC_RMID, NULL);
    msgctl(msgid2, IPC_RMID, NULL);
    msgctl(msgid3, IPC_RMID, NULL);

    exit(EXIT_SUCCESS);
    return 0;
}
