#include "event.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>
#include <ctype.h>

#define RED "\x1b[31m"
#define BLUE "\x1b[34m"
#define RST "\x1b[0m"

int d_ready = 0;
pthread_mutex_t doc_mtx;
pthread_cond_t doc_cond;

pthread_barrier_t barrier1;
pthread_barrier_t barrier2;

int v_ready = 0;

pthread_cond_t r_cond;
int r_total = 1;
int r_current = 0;

#define MAX_p 25
pthread_cond_t p_cond;
int p_total = 1;
int p_current = 0;

#define MAX_s 3
pthread_cond_t s_cond;
int s_total = 1;
int s_current = 0;

int t = 0;
int next_t = 0;
int current_t = 0;
int end = 0;

typedef struct
{
    event e;
    int num;
    char *type;
} Visitor;

void initialize()
{
    pthread_mutex_init(&doc_mtx, NULL);
    pthread_cond_init(&doc_cond, NULL);
    pthread_cond_init(&p_cond, NULL);
    pthread_cond_init(&r_cond, NULL);
    pthread_cond_init(&s_cond, NULL);
    pthread_barrier_init(&barrier1, NULL, 2);
    pthread_barrier_init(&barrier2, NULL, 2);
}

#define hour ((((t / 60) + 9) % 12) ? (((t / 60) + 9) % 12) : 12)
#define next_hour ((((next_t / 60) + 9) % 12) ? (((next_t / 60) + 9) % 12) : 12)

#define minute (t % 60)
#define next_minute (next_t % 60)

#define TIME ((t >= 180 && t < 900) ? "pm" : "am")
#define TIMENEXT ((next_t >= 180 && next_t < 900) ? "pm" : "am")

eventQ E;
void *assistant_tmain(void *arg);

void *doctor_tmain(void *arg)
{
    while (1)
    {
        pthread_mutex_lock(&doc_mtx);
        while (!d_ready)
        {
            pthread_cond_wait(&doc_cond, &doc_mtx);
        }
        d_ready = 0;

        if (end)
        {
            pthread_mutex_unlock(&doc_mtx);
            break;
        }

        t = (current_t + 1440) % 1440;
        printf(RED "[%02d:%02d%s] Doctor has next visitor\n", hour, minute, TIME);
        printf(RST);
        pthread_mutex_unlock(&doc_mtx);
        pthread_barrier_wait(&barrier2);
    }

    t = (current_t + 1440) % 1440;
    printf(RED "[%02d:%02d%s] Doctor leaves\n", hour, minute, TIME);
    printf(RST);
    pthread_barrier_wait(&barrier2);
    pthread_exit(NULL);
}

void *visitor_tmain(void *arg)
{
    Visitor *args = (Visitor *)arg;
    event e = args->e;
    int num = args->num;
    char *type = args->type; // Add a type field to Visitor struct

    pthread_mutex_lock(&doc_mtx);
    while (!v_ready)
    {
        pthread_barrier_wait(&barrier1);
        if (strcmp(type, "patient") == 0)
            pthread_cond_wait(&p_cond, &doc_mtx);
        else if (strcmp(type, "reporter") == 0)
            pthread_cond_wait(&r_cond, &doc_mtx);
        else if (strcmp(type, "salesrep") == 0)
            pthread_cond_wait(&s_cond, &doc_mtx);
    }
    v_ready = 0;

    t = (current_t + 1440) % 1440;
    next_t = (current_t + e.duration + 1440) % 1440;

    if (strcmp(type, "patient") == 0)
        printf(BLUE "[%02d:%02d%s - %02d:%02d%s] Patient %d is in doctor's chamber\n", hour, minute, TIME, next_hour, next_minute, TIMENEXT, num);
    else if (strcmp(type, "reporter") == 0)
        printf(BLUE "[%02d:%02d%s - %02d:%02d%s] Reporter %d is in doctor's chamber\n", hour, minute, TIME, next_hour, next_minute, TIMENEXT, num);
    else if (strcmp(type, "salesrep") == 0)
        printf(BLUE "[%02d:%02d%s - %02d:%02d%s] Sales Representative %d is in doctor's chamber\n", hour, minute, TIME, next_hour, next_minute, TIMENEXT, num);

    printf(RST);

    if (strcmp(type, "patient") == 0)
        p_current--;
    else if (strcmp(type, "reporter") == 0)
        r_current--;
    else if (strcmp(type, "salesrep") == 0)
        s_current--;

    event doc_e = {'D', current_t + e.duration, 0};
    E = addevent(E, doc_e);

    pthread_mutex_unlock(&doc_mtx);
    pthread_barrier_wait(&barrier1);
    pthread_exit(NULL);
}

// Function to convert a string to lowercase
void toLowercase(char *str)
{
    for (int i = 0; str[i] != '\0'; i++)
    {
        str[i] = tolower(str[i]);
    }
}

// Function to check if a file exists
int fileExists(const char *filename)
{
    return access(filename, F_OK) != -1;
}

void cleanup()
{
    pthread_mutex_destroy(&doc_mtx);
    pthread_cond_destroy(&doc_cond);
    pthread_cond_destroy(&p_cond);
    pthread_cond_destroy(&r_cond);
    pthread_cond_destroy(&s_cond);
    pthread_barrier_destroy(&barrier1);
    pthread_barrier_destroy(&barrier2);
    free(E.Q); // Free allocated memory for event queue
}

char filename[100] = "ARRIVAL.txt"; // Default filename in uppercase

int main()
{
    initialize(); // Initialize mutex, conditions, and barriers

    // Check if the file exists with the uppercase filename
    if (fileExists(filename))
    {
        // Convert the filename to lowercase
        toLowercase(filename);

        // Check if the file already exists with the lowercase filename
        if (fileExists(filename))
        {
            ;
        }
        else
        {
            // Rename the file from uppercase to lowercase
            if (rename("ARRIVAL.txt", filename) != 0)
            {
                perror("Error renaming file");
                return 1; // Exit with error
            }
        }
    }

    strcpy(filename, "arrival.txt");

    // Check if the file exists
    if (!fileExists(filename))
    {
        printf("'%s' not found\n", filename);
    }

    pthread_t assistant_thread;
    pthread_create(&assistant_thread, NULL, assistant_tmain, NULL);

    pthread_join(assistant_thread, NULL);

    cleanup();

    return 0;
}

void *assistant_tmain(void *arg)
{
    E = initEQ(filename);
    if (E.Q == NULL)
    {
        printf("Error initializing event queue\n");
        cleanup();
        return NULL;
    }

    event doc_e = {'D', current_t, 0};
    E = addevent(E, doc_e);

    pthread_t doctor_thread;
    pthread_create(&doctor_thread, NULL, doctor_tmain, NULL);

    event next_e;
    while (!emptyQ(E))
    {
        pthread_mutex_lock(&doc_mtx);
        next_e = nextevent(E);
        E = delevent(E);
        current_t = next_e.time;
        t = (current_t + 1440) % 1440;
        if (next_e.type == 'P')
        {
            printf("\t\t[%02d:%02d%s] Patient %d arrives\n", hour, minute, TIME, p_total);
            if (end)
            {
                printf("\t\t[%02d:%02d%s] Patient %d leaves (session over)\n", hour, minute, TIME, p_total);
                p_total++;
                pthread_mutex_unlock(&doc_mtx);
                continue;
            }
            else if (p_total <= MAX_p)
            {
                pthread_t patient_thread;
                Visitor *args = (Visitor *)malloc(sizeof(Visitor));
                args->e = next_e;
                args->num = p_total;
                args->type = "patient";
                pthread_create(&patient_thread, NULL, visitor_tmain, (void *)args);
                p_current++;
                p_total++;
                pthread_mutex_unlock(&doc_mtx);
            }
            else
            {
                printf("\t\t[%02d:%02d%s] Patient %d leaves (quota full)\n", hour, minute, TIME, p_total);
                p_total++;
                pthread_mutex_unlock(&doc_mtx);
                continue;
            }
        }
        else if (next_e.type == 'R')
        {
            printf("\t\t[%02d:%02d%s] Reporter %d arrives\n", hour, minute, TIME, r_total);
            if (end)
            {
                printf("\t\t[%02d:%02d%s] Reporter %d leaves (session over)\n", hour, minute, TIME, r_total);
                r_total++;
                pthread_mutex_unlock(&doc_mtx);
                continue;
            }
            else
            {
                pthread_t reporter_thread;
                Visitor *args = (Visitor *)malloc(sizeof(Visitor));
                args->e = next_e;
                args->num = r_total;
                args->type = "reporter";
                pthread_create(&reporter_thread, NULL, visitor_tmain, (void *)args);
                r_current++;
                r_total++;
                pthread_mutex_unlock(&doc_mtx);
            }
        }
        else if (next_e.type == 'S')
        {
            printf("\t\t[%02d:%02d%s] Sales representative %d arrives\n", hour, minute, TIME, s_total);
            if (end)
            {
                printf("\t\t[%02d:%02d%s] Sales representative %d leaves (session over)\n", hour, minute, TIME, s_total);
                s_total++;
                pthread_mutex_unlock(&doc_mtx);
                continue;
            }
            else if (s_total <= MAX_s)
            {
                pthread_t salesrep_thread;
                Visitor *args = (Visitor *)malloc(sizeof(Visitor));
                args->e = next_e;
                args->num = s_total;
                args->type = "salesrep";
                pthread_create(&salesrep_thread, NULL, visitor_tmain, (void *)args);
                s_current++;
                s_total++;
                pthread_mutex_unlock(&doc_mtx);
            }
            else
            {
                printf("\t\t[%02d:%02d%s] Sales representative %d leaves (quota full)\n", hour, minute, TIME, s_total);
                s_total++;
                pthread_mutex_unlock(&doc_mtx);
                continue;
            }
        }
        else
        {
            pthread_mutex_unlock(&doc_mtx);
            d_ready = 1;
            if (p_total > MAX_p && s_total > MAX_s && r_current == 0 && p_current == 0 && s_current == 0)
            {
                end = 1;
                pthread_cond_signal(&doc_cond);
                pthread_barrier_wait(&barrier2);
                continue;
            }
            else if (r_current == 0 && p_current == 0 && s_current == 0)
            {
                pthread_mutex_lock(&doc_mtx);
                d_ready = 0;
                event new_doctor = {'D', nextevent(E).time, 0};
                E = addevent(E, new_doctor);
                pthread_mutex_unlock(&doc_mtx);
                continue;
            }
            pthread_cond_signal(&doc_cond);
            pthread_barrier_wait(&barrier2);
            v_ready = 1;
            if (r_current > 0)
            {
                pthread_cond_signal(&r_cond);
            }
            else if (p_current > 0)
            {
                pthread_cond_signal(&p_cond);
            }
            else if (s_current > 0)
            {
                pthread_cond_signal(&s_cond);
            }
        }
        pthread_barrier_wait(&barrier1);
    }

    pthread_join(doctor_thread, NULL);
    pthread_exit(NULL);
}