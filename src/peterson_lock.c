#include "peterson_lock.h"
#include <stdio.h>
#include <pthread.h>

peterson_lock_t lock;
int shared_counter = 0;

void* thread_func(void* arg) {
    int thread_id = *(int*)arg;

    for (int i = 0; i < 100000; ++i) {
        peterson_lock(&lock, thread_id);
        ++shared_counter;
        peterson_unlock(&lock, thread_id);
    }

    printf("Thread %d done.\n", thread_id);
    return NULL;
}

int main() {
    pthread_t t1, t2;
    int id1 = 0, id2 = 1;

    peterson_lock_init(&lock);

    pthread_create(&t1, NULL, thread_func, &id1);
    pthread_create(&t2, NULL, thread_func, &id2);

    pthread_join(t1, NULL);
    pthread_join(t2, NULL);

    printf("Final counter value: %d\n", shared_counter);
    return 0;
}