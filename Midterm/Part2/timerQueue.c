#include "timerQueue.h"
#include <stdlib.h>

#define MAXTIMERS 8
#define NULL 0


typedef volatile struct tNode {
  struct tNode* next;     // TQE -> TQE -> TQE
  int seconds_left;     // seconds until expiration
  int pid;              // pid for calling PROC 
} tNode;


volatile tNode *tQueue;

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

int tInsert(int value) {
    tNode *tp;
    tp = tQueue;

    ksleep(running);

    int sum = 0;

    // TODO: malloc a tNode
    tNode* new_node = (tNode*) malloc(sizeof(struct tNode));
    new_node->next = NULL;
    new_node->pid = running->pid;
    new_node->seconds_left = value;
    
    // list EMPTY
    if (!tp) {
        // insert at front
        tQueue = new_node;
    }
    //                                 new  tp 
    // list: 5, insert: 3       :::     3 -> 5      =     3->2
    else if (value < tp->seconds_left) {
        new_node->next = tp;
        sum += value;
        tQueue = new_node;
        while(tp) {
            tp->seconds_left -= sum;
            sum += tp->seconds_left;
            tp = tp->next;
        }

    }
    else {
        sum += tp->seconds_left;
        while(tp->next) {
            if (value > sum + tp->next->seconds_left) {
                break;
            }
            else {
                sum += tp->next->seconds_left;
                tp = tp->next;
            }
        }
       // out of loop means we can insert after  
        new_node->next = tp->next;
        tp->next = new_node;
        tp->seconds_left = value - sum;
        sum += tp->seconds_left;

        // now focus on adjusting the nodes after this
        tp = new_node;
        tp = tp->next;

        while (tp) {
            tp->seconds_left -= sum;
            sum += tp->seconds_left;
            tp = tp->next;
        }
    }
}

void service_handler() {
    tNode* tmp;
    tmp = tQueue;
    tmp->seconds_left--;

    if (tmp->seconds_left <= 0) {
        int pid = tmp->pid;
        tQueue = tQueue->next;
        free(tmp);
        kwakeup(&proc[pid]);
    } 
}

void print_timers() {
    tNode* tmp;
    tmp = tQueue;

    if (!tmp) {
        printf("Nothing in timer queue\n");
        return;
    }

    printf("Remaining\tPID\n---------------\n");

    while (tmp) {
        printf("%d\t\t%d\t\t%d", tmp->seconds_left, tmp->pid);
        tmp = tmp->next;
    }
}