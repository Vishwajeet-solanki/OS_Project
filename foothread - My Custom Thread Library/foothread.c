#include "foothread.h"

static struct sembuf w, sig;

// Helper function to initialize semaphore
static int initialize_semaphore(int *semaphore, int val) {
    *semaphore = semget(IPC_PRIVATE, 1, 0777 | IPC_CREAT);
    if (*semaphore == -1) {
        perror("Error creating semaphore");
        exit(EXIT_FAILURE);
    }
    if (semctl(*semaphore, 0, SETVAL, val) == -1) {
        perror("Error initializing semaphore");
        exit(EXIT_FAILURE);
    }
    return 0;
}

static void sembuf_init() {
    sig.sem_num = 0;
    sig.sem_flg = 0;
    sig.sem_op = 1;

    w.sem_num = 0;
    w.sem_flg = 0;
    w.sem_op = -1;
}

// Helper function to perform semaphore operations
static void perform_semaphore_operation(int semaphore, struct sembuf *operation) {
    if (semop(semaphore, operation, 1) == -1) {
        perror("Semaphore operation failed");
        exit(EXIT_FAILURE);
    }
}


void foothread_attr_setjointype(foothread_attr_t *attr, int i)
{
    attr->join_type = i;
    // nojthreads++;
}
void foothread_attr_setstacksize(foothread_attr_t *attr, int i)
{
    attr->stack_size = i;
}


static foothread_info_t t_details [FOOTHREAD_THREADS_MAX];
static int cnt = 0;

int foothread_create(foothread_t *thread, foothread_attr_t *attr, int (*start_routine)(void *), void *arg)
{
    if (cnt >= FOOTHREAD_THREADS_MAX)
    {
        perror("Maximum Number of Threads Exceeded");
        return -1;
    }

    // Use default attributes if not provided
    if (attr == NULL)
    {
        attr = (foothread_attr_t *)malloc(sizeof(foothread_attr_t));
        foothread_attr_setjointype(attr, FOOTHREAD_JOINABLE);
        foothread_attr_setstacksize(attr, FOOTHREAD_DEFAULT_STACK_SIZE);
    }

    // flags for clone()
    int flags = CLONE_VM | CLONE_FS | CLONE_FILES | CLONE_SIGHAND | CLONE_THREAD;

    // Allocate stack
    void *st = malloc(attr->stack_size);
    if (st == NULL)
    {
        perror("malloc failed");
        return -1;
    }

    // Call clone() to create the thread
    int tid = clone(start_routine, (char *)st + attr->stack_size, flags, arg);
    if (tid == -1)
    {
        perror("clone failed");
        free(st);
        return -1;
    }

    if (thread != NULL)
        thread->dummy = tid;

    t_details[cnt].tid = tid;
    t_details[cnt].join_type = attr->join_type;
    cnt++;

    return 0;
}

void foothread_mutex_init(foothread_mutex_t *mutex)
{
    sembuf_init();
    mutex->p = 0;
    initialize_semaphore(&(mutex->s),1);
}

void foothread_mutex_lock(foothread_mutex_t *mutex)
{
    perform_semaphore_operation(mutex->s, &w);
    mutex->p = gettid();
}

void foothread_mutex_unlock(foothread_mutex_t *mutex)
{
    if (mutex->p == gettid())
    {
        mutex->p = 0;
        perform_semaphore_operation(mutex->s, &sig);
    }
    else
    {
        perror("Attempt to unlock mutex by another thread");
        exit(EXIT_FAILURE);
    }
}

void foothread_mutex_destroy(foothread_mutex_t *mutex)
{
    semctl(mutex->s, 0, IPC_RMID, 0);
}

void foothread_barrier_init(foothread_barrier_t *barrier, int count)
{
    sembuf_init();
    barrier->count = 0;
    barrier->max = count;
    initialize_semaphore(&(barrier->barrier_sem), 0);
    initialize_semaphore(&(barrier->wait_sem), 1);
}

void foothread_barrier_wait(foothread_barrier_t *barrier)
{
    perform_semaphore_operation(barrier->wait_sem, &w);

    barrier->count++;
    if (barrier->count >= barrier->max)
    {
        for (int i = 0; i < barrier->max - 1; ++i)
        {
            perform_semaphore_operation(barrier->barrier_sem, &sig);
        }
        barrier->count = 0;
        perform_semaphore_operation(barrier->wait_sem, &sig);
    }
    else
    {
        perform_semaphore_operation(barrier->wait_sem, &sig);
        perform_semaphore_operation(barrier->barrier_sem, &w);
    }
}

void foothread_barrier_destroy(foothread_barrier_t *barrier)
{
    semctl(barrier->barrier_sem, 0, IPC_RMID, 0);
    semctl(barrier->wait_sem, 0, IPC_RMID, 0);
}

static int exit_semaphore;
void foothread_exit()
{
    if (getpid() == gettid())
    {
        // leader thread
        initialize_semaphore(&exit_semaphore, 0);
        for (int i = 0; i < cnt; i++)
        {
            if (t_details[i].join_type == FOOTHREAD_JOINABLE)
            perform_semaphore_operation(exit_semaphore, &w);
        }
        semctl(exit_semaphore, 0, IPC_RMID, 0);
    }
    else
    {
        // non-leader thread
        for (int i = 0; i < cnt; i++)
        {
            if (t_details[i].tid == gettid())
            {
                if (t_details[i].join_type == FOOTHREAD_JOINABLE)
                    perform_semaphore_operation(exit_semaphore, &sig);
                break;
            }
        }
    }
}