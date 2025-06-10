#include "mcs_lock.h"
#include <stdio.h>
#include <pthread.h>
#include <stdatomic.h>

#define THREAD_COUNT 4

mcs_lock_t lock;
int shared_counter = 0;

void* thread_func(void* arg) {
    mcs_node_t node;
    int id = *(int*)arg;

    for (int i = 0; i < 100000; ++i) {
        mcs_lock(&lock, &node);
        ++shared_counter;
        mcs_unlock(&lock, &node);
    }

    printf("Thread %d done.\n", id);
    return NULL;
}

int main() {
    pthread_t threads[THREAD_COUNT];
    int ids[THREAD_COUNT];

    mcs_lock_init(&lock);

    for (int i = 0; i < THREAD_COUNT; ++i) {
        ids[i] = i;
        pthread_create(&threads[i], NULL, thread_func, &ids[i]);
    }

    for (int i = 0; i < THREAD_COUNT; ++i) {
        pthread_join(threads[i], NULL);
    }

    printf("Final counter value: %d\n", shared_counter);
    return 0;
}