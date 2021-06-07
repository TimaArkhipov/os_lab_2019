#include <errno.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

pthread_mutex_t lock1 = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t lock2 = PTHREAD_MUTEX_INITIALIZER;
int common = 0;

 void function1(int *pnum_times)
    {
       unsigned long k;
       int work;
       pthread_mutex_lock(&lock1);           // шаг 1
       sleep(5);
       pthread_mutex_lock(&lock2);           // шак 3 deadlock!!!
       
                
       printf("doing one thing\n");
       work = *pnum_times;
       printf("counter = %d\n", work);
       work++; /* increment, but not write */
       for (k = 0; k < 500000; k++)
         ;                 /* long cycle */
       *pnum_times = work; /* write back */


       pthread_mutex_unlock(&lock2);
       pthread_mutex_unlock(&lock1);
    } 

    void function2(int *pnum_times)
    {
       unsigned long k;
       int work;
       pthread_mutex_lock(&lock2);           // шаг 2
       pthread_mutex_lock(&lock1);


       printf("doing other thing\n");
       work = *pnum_times;
       printf("counter = %d\n", work);
       work++; /* increment, but not write */
       for (k = 0; k < 500000; k++)
         ;                 /* long cycle */
       *pnum_times = work; /* write back */


       pthread_mutex_unlock(&lock1);
       pthread_mutex_unlock(&lock2);
    } 
  
    int main()
    {
       pthread_t thread1, thread2;

       pthread_create(&thread1, NULL, (void*)function1, (void *)&common);
       pthread_create(&thread2, NULL, (void*)function2, (void *)&common);
       if (pthread_join(thread1, NULL) != 0) 
       {
            perror("pthread_join");
            exit(1);
       }
       if (pthread_join(thread2, NULL) != 0) 
       {
            perror("pthread_join");
            exit(1);
       }
       return 0;
    }