#include "timerQueue.h"

#define MAXTIMERS 8
#define NULL 0


typedef volatile struct tNode {
  int seconds_left;     // seconds until expiration
  int pid;              // pid for calling PROC 
  int status;
} tNode;


tNode tQueue[MAXTIMERS];

/* 
    Use case:
        tInsert called from body as tInsert(seconds)
        tInsert iterates through tQueue and compares its value with the value at each spot 
        ex: tInserT(20)

                  .   next
        0    1    2    3     4
        4 -> 6 -> 2 -> 13 -> 7
        
        start       sum = 0;
                    value <= next sum? : insert ; iterate to next
        tq=4        sum = 4;            pass
        tq=6        sum = 10;           pass
        tq=2        sum = 12;           pass
        tq=13       sum = 25;           insert here 

        after:         
        0    4    .   new   next       
        0    1    2    3     4     5  
        4 -> 6 -> 2 -> 20 -> 13 -> 7
*/ 

int tInit() {
    for (int i = 0; i < MAXTIMERS; i++) {
        tQueue[i].pid = 0;
        tQueue[i].seconds_left = 0;
        tQueue[i].status = READY;
    }

    printf("initialized timers\n");
}

int tInsert(int value) {
    //tNode temp;
    //temp.pid = running->pid;

    printf("entered insert with value=\t%d\n", value);
    int pid = running->pid;
    int sum = 0;

    // insert


    // 5 -> 3 -> 3 
    //insert(9)


    // 5 < 9 --> skip
    // 8 < 9 --> skip
    // 12 !< 9 --> go into 
    // sum + seconds_left > value

    for (int i = 0; i < MAXTIMERS; i++) {



        
        if (tQueue[i].status == READY && (tQueue[i].seconds_left + sum) < value) {
            tQueue[i].status = SLEEP;
            tQueue[i].pid = pid;
            tQueue[i].seconds_left = value - sum;
            printf("inserted pid=%d at i=%d\n", pid, i);
            break;
        } 
        else if (tQueue[i].status == SLEEP) {

        }
        else {
            printf("skip..\n");
            sum += tQueue[i].seconds_left;
        }
    }
    ksleep(running);
}

void service_handler() {

    if (tQueue[0].status == SLEEP) {
        tQueue[0].seconds_left--;
    }

    //    0 1 2 3 4 5 6 7 8 
    //    * 
    //    5 4 3 2 FREE FREE FREE  
    //    4 3 2 FREE FREE FREE FREE     

    if (tQueue[0].status == SLEEP && tQueue[0].seconds_left <= 0) {
        // move everything back 
        int pid = tQueue[0].pid;
        int i;
        for (i = 1; i < MAXTIMERS; i++) {
            if (tQueue[i].status == SLEEP) {
                tQueue[i-1] = tQueue[i];
            }
            tQueue[i-1].pid = 0;
            tQueue[i-1].status = READY;
            tQueue[i-1].seconds_left = 0;
        }

        kwakeup(&proc[pid]);
    } 
}

void print_timers() {
    /*
    if (tQueue[0].status==READY) {
        printf("Nothing in timer queue\n");
        return;
    }
    */

    printf("Remaining\tPID\t\tStatus\n---------------\n");
    int i = 0;

    for (i = 0; i < MAXTIMERS; i++) {
        printf("%d\t\t%d\t\t%d\n", tQueue[i].seconds_left, tQueue[i].pid, tQueue[i].status);
    }
}