#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>



void foo();
void bar();

pthread_mutex_t mut_a = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mut_b = PTHREAD_MUTEX_INITIALIZER;

int main(){
    pthread_t thread1, thread2;

    if (pthread_create(&thread1, NULL, (void *)foo,
			  NULL) != 0) {
        perror("pthread_create");
        exit(1);
    }
    if (pthread_create(&thread2, NULL, (void *)bar,
			  NULL) != 0) {
        perror("pthread_create");
        exit(1);
    }

    if (pthread_join(thread1, NULL) != 0) {
        perror("pthread_join");
        exit(1);
    }

    if (pthread_join(thread2, NULL) != 0) {
        perror("pthread_join");
        exit(1);
    }

    return 0;
}

void foo(){
    printf("Foo is trying to access resourse A...\n");
    pthread_mutex_lock(&mut_a);
    printf("Foo got resource A!\n");
    sleep(1); //some work
    printf("Foo is trying to access resource B...\n");
    pthread_mutex_lock(&mut_b);
    printf("Foo got resource B!\n");
    sleep(1); // some work
    pthread_mutex_unlock(&mut_b);
    pthread_mutex_unlock(&mut_a);
    printf("Foo made A and B free\n");
}

void bar(){
    printf("Bar is trying to access resourse B...\n");
    pthread_mutex_lock(&mut_b);
    printf("Bar got resource B!\n");
    sleep(1); //some work
    printf("Bar is trying to access resource A...\n");
    pthread_mutex_lock(&mut_a);
    printf("Bar got resource B!\n");
    sleep(1); // some work
    pthread_mutex_unlock(&mut_a);
    pthread_mutex_unlock(&mut_b);
    printf("Bar made A and B free\n");
}