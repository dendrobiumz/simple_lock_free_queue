#include "lfq.h"
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <stdatomic.h>
#include <string.h>


#define THREAD_NUM (2)

atomic_int start_insert = 0;
atomic_int start_pop = 0;
atomic_int start_print = 0;
struct lfq *g_queue_str = NULL;

typedef struct {
    char s[10];
    struct list_head head;
} data_t;

const char * string[] = {
    "123", "456", "789", "ABC", "DEF", "GHI", "JKL", "MNO"
};


void *thread_func(void *args)
{
    int thread_num = *(int*) args;

    printf("[INFO] create thread %d successfully\n", thread_num);

    atomic_thread_fence(memory_order_release);
    while(!start_insert)
        ;

    for (int idx = 0; idx < 2; idx++) {
        data_t *d = (data_t*) malloc(sizeof(data_t));
        memcpy(d->s, string[idx * THREAD_NUM + thread_num], strlen(string[idx * THREAD_NUM + thread_num]) + 1);
        if (g_queue_str->push_back(g_queue_str, &d->head))
            printf("[ERR] thread %d: insert failed\n", thread_num);
        else
            printf("[INFO] thread %d: insert successfully\n", thread_num);
    }

    atomic_thread_fence(memory_order_release);
    while(!start_pop)
        ;

    struct list_head *tmp = NULL;
    g_queue_str->pop(g_queue_str, &tmp);
    while (tmp) {
        data_t *d = list_entry(tmp, data_t, head);
        printf("[POP] thread %d : %s\n", thread_num, d->s);
        tmp = NULL;
        g_queue_str->pop(g_queue_str, &tmp);
    }

    return NULL;
}


int main(int argc, char *argv[])
{
    g_queue_str = init_lfq();
    if (!g_queue_str)
        return -1;

    int tidx[4];
    pthread_t thread[THREAD_NUM];
    for (int idx = 0; idx < THREAD_NUM; idx++) {
        tidx[idx] = idx;
        pthread_create(&thread[idx], NULL, thread_func, &tidx[idx]);
        pthread_detach(thread[idx]);
    }

    sleep(1);
    printf("[INFO] START INSERT ELEMENT\n");
    atomic_store_explicit(&start_insert, 1, memory_order_relaxed);
    //atomic_thread_fence(memory_order_release);
    sleep(1);
    


    // for(int idx = 0; idx < 4; idx++) {
    //     data_t *d = (data_t*) malloc(sizeof(data_t));
    //     memcpy(d->s, string[idx], sizeof(string[idx]) + 1);
    //     list_add_tail(&d->head, &g_queue_str->head);
    // }

    puts("[TEST] show element");
    // struct list_head *tmp;
    // g_queue_str->pop(g_queue_str, &tmp);
    // while (tmp) {
    //     data_t *d = list_entry(tmp, data_t, head);
    //     printf("[POP] %s\n", d->s);
    //     g_queue_str->pop(g_queue_str, &tmp);
    // }
    data_t *t;
    list_for_each_entry(t, &g_queue_str->head, head){
        if (t)
            printf("%s\n", t->s);
    }

    atomic_store_explicit(&start_pop, 1, memory_order_relaxed);
    
    printf("[INFO] START POP ELEMENT\n");
    return 0;
}