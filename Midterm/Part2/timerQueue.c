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
        tq=13       sum = 25;           in5sert here

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
    printf("entered insert with value=\t%d\n", value);
    int pid = running->pid;
    int sum = 0;

    // before: [5, 5, 8]
    // insert(7)
    // after: [5, 2, 3, 8]
    // 2 = value - sum
    // 3 = t.seconds - value

    for (int i = 0; i < MAXTIMERS; i++) {
        //                  0 30  <   20
        if((tQueue[i].seconds_left + sum) >= value || tQueue[i].status == READY) { // insert here
          printf("secondsleft [%d] + sum [%d] >= value [%d]\n", tQueue[i].seconds_left, sum, value);
          if (tQueue[i].status == READY) {    // at "end" of list
            printf("Ready to insert %d at tQueue value =%d\n", value-sum, i);
            tQueue[i].status = SLEEP;
            tQueue[i].pid = pid;
            tQueue[i].seconds_left = value - sum;
            break;
          }
          else {          // tQueue[i] is already in use, that means we have to manipulate the array
            printf("Detected tQueue[%d] is already busy\n", i);
            int pos = i;
            // go to end of list
            while (tQueue[pos].status == SLEEP) {
              pos++;
            }
            printf("position of first empty area = %d\n", pos);
            // loop until back at i

            // tQueue = 4 --> NULL
            // insert a 10
            // after: tQueue = 4 --> 6

            while (pos > i) {
              tQueue[pos] = tQueue[pos-1];
              printf("new value at tQueue[%d] is %d\n",pos, tQueue[pos].seconds_left);
              if (pos == i+1) {
                tQueue[pos].seconds_left = tQueue[pos].seconds_left - (value-sum);
              }
              pos--;
            }
            // back at position
            tQueue[i].status = SLEEP;
            tQueue[i].pid = pid;    // 7   - 6
            tQueue[i].seconds_left = value - sum;
            break;
          }
        }
        else {
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
        int i=0;

        for (i = 1; i < MAXTIMERS; i++) {
            tQueue[i-1] = tQueue[i];
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
