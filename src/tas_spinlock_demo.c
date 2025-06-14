#include "spinlock.h"
#include <pthread.h>
#include <stdio.h>

spinlock_t lock = 0;
int shared_counter = 0;

void* thread_func(void* arg) {
    int id = *(int*)arg;
    for (int i = 0; i < 100000; ++i) {
        spin_lock(&lock);
        ++shared_counter;
        spin_unlock(&lock);
    }
    printf("Thread %d done.\n", id);
    return NULL;
}

int main() {
    pthread_t t1, t2;
    int id1 = 1, id2 = 2;

    pthread_create(&t1, NULL, thread_func, &id1);
    pthread_create(&t2, NULL, thread_func, &id2);

    pthread_join(t1, NULL);
    pthread_join(t2, NULL);

    printf("Final counter value: %d\n", shared_counter);
    return 0;
}