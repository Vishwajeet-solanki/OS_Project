#ifndef FOO_THREAD_H
#define FOO_THREAD_H
#define _GNU_SOURCE

#include <unistd.h>
#include <sys/sem.h>
#include <sched.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>
#include <sys/syscall.h>

#define FOOTHREAD_THREADS_MAX 128
#define FOOTHREAD_DEFAULT_STACK_SIZE 2097152
#define FOOTHREAD_JOINABLE 0
#define FOOTHREAD_DETACHED 1


// static int semmade=0;
// static pid_t ltid;

typedef struct {
    int dummy; // Placeholder for thread data
    // int joinablechld=0;
} foothread_t;

typedef struct {
    int join_type;
    size_t stack_size;
} foothread_attr_t;

void foothread_attr_setjointype ( foothread_attr_t * , int ) ;
void foothread_attr_setstacksize ( foothread_attr_t * , int ) ;

#define FOOTHREAD_ATTR_INITIALIZER {FOOTHREAD_JOINABLE, FOOTHREAD_DEFAULT_STACK_SIZE}


typedef struct {
    int barrier_sem;
    int wait_sem;
    int count;
    int max;
} foothread_barrier_t;

typedef struct
{
    pid_t tid;
    int join_type;
} foothread_info_t;

typedef struct
{
    int s;
    pid_t p;
} foothread_mutex_t;



int foothread_create(foothread_t *, foothread_attr_t *, int (*)(void *), void *);

void foothread_exit();

void foothread_mutex_init(foothread_mutex_t *mutex);

void foothread_mutex_lock(foothread_mutex_t *mutex);

void foothread_mutex_unlock(foothread_mutex_t *mutex);

void foothread_mutex_destroy(foothread_mutex_t *mutex);

void foothread_barrier_init(foothread_barrier_t *barrier, int count);

void foothread_barrier_wait(foothread_barrier_t *barrier);

void foothread_barrier_destroy(foothread_barrier_t *barrier);

#endif