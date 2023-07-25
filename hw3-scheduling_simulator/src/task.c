#include "../include/task.h"
#include <stdio.h>
#include "../include/struct_th.h"
#include <string.h>
#include <unistd.h>

void task_sleep(int ms)
{
    //sleep(3);
    printf("Task %s goes to sleep.\n", running->name);
    push_waiting(pop("READY", running->name));
    struct alarm_clock* temp = create_alarm(running->name, ms);
    push_alarm(temp);
    swapcontext(&(running->thread_context), &main_context);
    //switch to waiting state
    //wait for 10*ms ms
    //switch to ready state
}

void task_exit()
{
    printf("Task %s has terminated.\n", running->name);
    strcpy(running->state, "TERMINATED");
    swapcontext(&(running->thread_context), &main_context);
    //setcontext(&main_context);
    //switch to terminated state
    //goto terminated Q
}
