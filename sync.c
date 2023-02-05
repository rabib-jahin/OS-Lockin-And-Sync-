#include <pthread.h>
#include <semaphore.h>
#include <stdbool.h>
#include <stdio.h>
#include<stdlib.h>
#include<time.h>
#include<unistd.h>
#include "zemaphore.h"

int nitrogenCount = 0;
int hydrogenCount = 0;
int currentTemp = 100;
int MIN_TEMP = 100;
int MAX_THREAD = 1000;
pthread_mutex_t mutex;
zem_t hydrogenSem, nitrogenSem, tempSem,lock;

void bond(void) {
printf("An NH3 molecule was formed in  %d degree C\n",currentTemp);
printf("H2 : %d\t N2 : %d\n", hydrogenCount, nitrogenCount);

}

void* updateTemp(void* arg) {
while (true) {
int res = rand() % 10;
int incr = rand() % 50;
int ntemp = currentTemp;
if (currentTemp > 100) {
if (res <= 8) ntemp -= incr;
else ntemp += incr;
}
else {
if (res <= 8) ntemp += incr;
else ntemp -= incr;
}
sleep(1);
zem_down(&lock);
currentTemp = ntemp;
printf("temperature has been updated to %d\n", currentTemp);
if (currentTemp >= MIN_TEMP) {
zem_up(&tempSem);
}
zem_up(&lock);
}
return NULL;
}

void* nitrogen(void* arg) {
//pthread_mutex_lock(&mutex);
 zem_down(&lock);
while (true) {


nitrogenCount++;
printf("a new n2 has come; Total count %d\n ", nitrogenCount);
 while (hydrogenCount < 2) {
    zem_up(&lock);
    zem_down(&hydrogenSem);
    zem_down(&lock);
 }
//sem_post(&nitrogenSem);
  while (currentTemp < MIN_TEMP) {
 zem_up(&lock);
   zem_down(&tempSem);
   zem_down(&lock);
   }

if (nitrogenCount > 0 && hydrogenCount > 2) {
hydrogenCount -= 3;
nitrogenCount--;
bond();

}
zem_broadcast(&nitrogenSem);
break;
}
zem_up(&lock);
return NULL;
}

void* hydrogen(void* arg) {
  zem_down(&lock);
    while (true) {
        hydrogenCount++;
        printf("a new h2 has come: total count %d\n", hydrogenCount);
        while (nitrogenCount < 1 || hydrogenCount < 3) {
              zem_up(&lock);
            zem_down(&nitrogenSem);
             zem_down(&lock);
        }
        while (currentTemp < MIN_TEMP) {
             zem_up(&lock);
        
              zem_down(&tempSem);
               zem_down(&lock);

        }
        if (nitrogenCount > 0 && hydrogenCount > 2) {
            nitrogenCount--;
            hydrogenCount -= 3;
            bond();
        }
        zem_broadcast(&hydrogenSem);
        break;
    }
zem_up(&lock);
    return NULL;
}


int main(int argc, char* argv[]) {
    pthread_t h1, h2, h3, n1, n2;
    pthread_t h[MAX_THREAD];
    pthread_t n[MAX_THREAD];
    pthread_t update;
    pthread_t initializer;
zem_init(&hydrogenSem, 1);
zem_init(&nitrogenSem,  1);
zem_init(&tempSem,  0);
zem_init(&lock,  1);
 
    

 
 
    pthread_create(&update, NULL, updateTemp, NULL);
    int h_index = 0, n_index = 0;
    while (h_index < MAX_THREAD || n_index < MAX_THREAD) {
        if (h_index >= MAX_THREAD) {
            while (n_index < MAX_THREAD) {
                sleep(1);
                pthread_create(&n[n_index], NULL, nitrogen, NULL);
                ++n_index;
            }
        }
 
        else if (n_index >= MAX_THREAD) {
            while (h_index < MAX_THREAD) {
                sleep(1);
                pthread_create(&h[h_index], NULL, hydrogen, NULL);
                ++h_index;
            }
        }
        else {
            int res = rand() % 2;
            sleep(1);
            if (res == 0) {
                pthread_create(&h[h_index], NULL, hydrogen, NULL);
                ++h_index;
            }
            else {
                pthread_create(&n[n_index], NULL, nitrogen, NULL);
                ++n_index;
 
            }
        }
    }
    // pthread_create(&h1, NULL, hydrogen, NULL);
    // pthread_create(&h2, NULL, hydrogen, NULL);
    // pthread_create(&n1, NULL, nitrogen, NULL);
    // pthread_create(&h3, NULL, hydrogen, NULL);
    // pthread_create(&n2, NULL, nitrogen, NULL);
 
    // pthread_join(h1, NULL);
    // pthread_join(h2, NULL);
    // pthread_join(h3, NULL);
    // pthread_join(n1, NULL);
    // pthread_join(n2, NULL);
 
    for (int i = 0; i < MAX_THREAD; ++i) {
        pthread_join(h[i], NULL);
    }
 
    for (int i = 0; i < MAX_THREAD; ++i) {
        pthread_join(n[i], NULL);
    }
 
    pthread_join(update, NULL);
   
    
 
    return 0;
}

