#include "threadpool.h"


struct _threadpool {
    int num_workers;
    pthread_t *workers;
    jobqueue_t *jq;
};


struct _jobqueue {

};

struct _threadtask {
    void *(*func)(void *);
    void *arg;
    struct _threadtask *next;
};