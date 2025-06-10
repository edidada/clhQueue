#include "bakery_lock.h"
#include <stdio.h>
#include <pthread.h>

#define THREAD_COUNT 2

bakery_lock_t lock;
int shared_counter = 0;

void* thread_func(void* arg) {
    int thread_id = *(int*)arg;

    for (int i = 0; i < 100000; ++i) {
        bakery_lock(&lock, thread_id);
        ++shared_counter;
        bakery_unlock(&lock, thread_id);
    }

    printf("Thread %d done.\n", thread_id);
    return NULL;
}

int main() {
    pthread_t threads[THREAD_COUNT];
    int ids[THREAD_COUNT];

    bakery_lock_init(&lock);

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