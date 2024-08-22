#include <stdio.h>
#include <stdlib.h>
#include "foothread.h"

#define MAX_NODES 1000

foothread_mutex_t mtx;

typedef struct
{
    int id;
    int p;
    int c[MAX_NODES];
    int c_count;
    int s;
    foothread_barrier_t bar;
} Node;

Node **t;


int tmain(void *arg)
{
    Node *node = (Node *)arg;
    int id = node->id;

    if (node->c_count == 0)
    {
        foothread_mutex_lock(&mtx);
        printf("Leaf node %d :: Enter a positive integer: ", id);
        int input;
        scanf("%d", &input);
        node->s += input;
        foothread_mutex_unlock(&mtx);

        Node *p = t[node->p];
        foothread_barrier_wait(&(p->bar));
    }
    else if (node->p == -1)
    {
        foothread_barrier_wait(&(node->bar));

        foothread_mutex_lock(&mtx);
        for (int i = 0; i < node->c_count; i++)
        {
            node->s += t[(node->c)[i]]->s;
        }
        printf("Internal node %d gets the partial sum %d from its children\n", id, node->s);
        printf("Sum at root (node %d) = %d\n", id, node->s);
        foothread_mutex_unlock(&mtx);
    }
    else
    {
        foothread_barrier_wait(&(node->bar));

        foothread_mutex_lock(&mtx);
        for (int i = 0; i < node->c_count; i++)
        {
            node->s += t[(node->c)[i]]->s;
        }
        printf("Internal node %d gets the partial sum %d from its children\n", id, node->s);
        foothread_mutex_unlock(&mtx);

        Node *p = t[node->p];
        foothread_barrier_wait(&(p->bar));
    }

    foothread_exit();
    return 0;
}

int main()
{
    t = (Node **)malloc(MAX_NODES * sizeof(Node *));
    FILE *file = fopen("tree.txt", "r");
    if (file == NULL)
    {
        perror("Error opening file");
        exit(EXIT_FAILURE);
    }

    int n;
    fscanf(file, "%d", &n);

    for (int i = 0; i < n; i++)
    {
        t[i] = (Node *)malloc(sizeof(Node));
        t[i]->c_count = 0;
        t[i]->s = 0;
        t[i]->id = i;
        t[i]->p = -1;
    }

    for (int i = 0; i < n; i++)
    {
        int curr_node, p;
        fscanf(file, "%d %d", &curr_node, &p);

        if (p == curr_node)
            continue;

        t[curr_node]->p = p;
        t[p]->c_count++;
        t[p]->c[t[p]->c_count - 1] = curr_node;
    }

    fclose(file);

    foothread_t threads[n];
    foothread_mutex_init(&mtx);

    for (int i = 0; i < n; i++)
    {
        foothread_barrier_init(&(t[i]->bar), t[i]->c_count + 1);
        foothread_create(&threads[i], NULL, tmain, (void *)t[i]);
    }

    foothread_exit();
    foothread_mutex_destroy(&mtx);

    for (int i = 0; i < n; i++)
    {
        foothread_barrier_destroy(&(t[i]->bar));
        free(t[i]);
    }
    free(t);

    return 0;
}