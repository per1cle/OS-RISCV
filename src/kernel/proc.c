#include "proc.h"
#include "csr.h"
#include "kernel_lib.h"
#define PROC_MAX 10

void comutare(struct context *old, struct context *new);

struct proc proc_table[PROC_MAX];
int proc_actual = 0;

void scheduler(void){
    proc_actual = 0;
    proc_table[proc_actual].state = READY;

    static int last_proc = 0;

    while(1){
        //write_csr(mstatus, r_csr(mstatus) | MSTATUS_MIE);

        int next_proc = -1;
        int start = (last_proc + 1) % PROC_MAX;
        for(int i=0;i<PROC_MAX;i++){
            int index = (start + i) % PROC_MAX;

            if(index == 0){
                continue;
            }

            if(proc_table[index].state == READY){
                next_proc = index;
                break;
            }
        }

        if(next_proc == -1){
            continue;
        }

        proc_actual = next_proc;
        last_proc = proc_actual;

        comutare(&proc_table[0].context, &proc_table[proc_actual].context);

        proc_actual = 0;

    }
}

void yield(void){
    proc_table[proc_actual].state = READY;
    comutare(&proc_table[proc_actual].context, &proc_table[0].context);
}

void task1(void){
    kernel_putstring("Task 1: Hey Hey!\n");
    while(1){
        kernel_putstring("1");
        for(volatile int i = 0; i < 1000000; i++);
        yield();
    }
}

void task2(void){
    kernel_putstring("Task 2: Hello!\n");
    while(1){
        kernel_putstring("2");
        for(volatile int i = 0; i < 1000000; i++);
        yield();
    }
}

void init_proc(void){

    for (int i = 0; i < PROC_MAX; i++) {
        proc_table[i].state = UNUSED;
    }
    //proc 1
    proc_table[1].state = READY;
    proc_table[1].context.sp = (long)&proc_table[1].stack[4095];
    proc_table[1].context.ra = (long)task1;

    //proc 2
    proc_table[2].state = READY;
    proc_table[2].context.sp = (long)&proc_table[2].stack[4095];
    proc_table[2].context.ra = (long)task2;
}