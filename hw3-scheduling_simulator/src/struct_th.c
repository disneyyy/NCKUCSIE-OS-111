#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <ucontext.h>
#include "../include/struct_th.h"
#include "../include/function.h"
#include "../include/command.h"
#include <sys/time.h>
#include <time.h>

#define FCFS 0
#define RR 1
#define PP 2
struct thread* create_thread(int TID, char* name, int priority){
    struct thread* new_thread = (struct thread*) malloc(sizeof(struct thread)); 
    new_thread->TID = TID;
    strcpy(new_thread->name, name);
	strcpy(new_thread->state, "READY");//ready
	new_thread->running = 0;
    new_thread->waiting = 0;
    new_thread->turnaround = 0;
	strcpy(new_thread->resources, "none");
	for(int i = 0; i < 8; i++)	new_thread->resources_get[i] = false;
	new_thread->resources_count = 0;
    new_thread->priority = priority;
    new_thread->thread_next = NULL;
	new_thread->start = false;
	new_thread->run_time = 0;
	/*
	struct thread* temp = new_thread;
	temp->thread_context.uc_stack.ss_sp = malloc(1024);
	temp->thread_context.uc_stack.ss_size = 1024;
	temp->thread_context.uc_stack.ss_flags = 0;
	temp->thread_context.uc_link = &main_context;
	if(strcmp(func, "test_exit") == 0){
		makecontext(&(temp->thread_context),(void (*)(void))test_exit,0);
	}
	else if(strcmp(func, "test_sleep") == 0){
		makecontext(&(temp->thread_context),(void (*)(void))test_sleep,0);
	}
	else if(strcmp(func, "test_resource1") == 0){
		makecontext(&(temp->thread_context),(void (*)(void))test_resource1,0);
	}
	else if(strcmp(func, "test_resource2") == 0){
		makecontext(&(temp->thread_context),(void (*)(void))test_resource2,0);
	}
	else if(strcmp(func, "idle") == 0){
		makecontext(&(temp->thread_context),(void (*)(void))idle,0);
	}
	else if(strcmp(func, "task1") == 0){
		makecontext(&(temp->thread_context),(void (*)(void))task1,0);
	}
	else if(strcmp(func, "task2") == 0){
		makecontext(&(temp->thread_context),(void (*)(void))task2,0);
	}
	else if(strcmp(func, "task3") == 0){
		makecontext(&(temp->thread_context),(void (*)(void))task3,0);
	}
	else if(strcmp(func, "task4") == 0){
		makecontext(&(temp->thread_context),(void (*)(void))task4,0);
	}
	else if(strcmp(func, "task5") == 0){
		makecontext(&(temp->thread_context),(void (*)(void))task5,0);
	}
	else if(strcmp(func, "task6") == 0){
		makecontext(&(temp->thread_context),(void (*)(void))task6,0);
	}
	else if(strcmp(func, "task7") == 0){
		makecontext(&(temp->thread_context),(void (*)(void))task7,0);
	}
	else if(strcmp(func, "task8") == 0){
		makecontext(&(temp->thread_context),(void (*)(void))task8,0);
	}
	else if(strcmp(func, "task9") == 0){
		makecontext(&(temp->thread_context),(void (*)(void))task9,0);
	}
	else{
		makecontext(&(temp->thread_context),(void (*)(void))test_exit,0);
	}
	getcontext(&(temp->thread_context));
	*/
    return new_thread;
}
void push_ready(struct thread* new_thread){
	strcpy(new_thread->state, "READY");
	if(schedule_mode == PP){
		if(ready_first == NULL){
			ready_first = new_thread;
			new_thread->thread_next = NULL;
		}
		else{
			if(ready_first->priority >= new_thread->priority){
				new_thread->thread_next = ready_first;
				ready_first = new_thread;
				return;
			}
			struct thread* temp_pre = ready_first;
			struct thread* temp = temp_pre->thread_next;
			while(temp != NULL && temp->priority < new_thread->priority ){
				temp_pre = temp;
				temp = temp->thread_next;
			}
			temp_pre->thread_next = new_thread;
			new_thread->thread_next = temp;
		}
	}
	else{
		if(ready_first == NULL){
			ready_first = new_thread;
			new_thread->thread_next = NULL;
		}
		else{
			struct thread* temp = ready_first;
			while(temp->thread_next != NULL) temp = temp->thread_next;
			temp->thread_next = new_thread;
			new_thread->thread_next = NULL;
		}
	}
	
}
void push_waiting(struct thread* new_thread){
	strcpy(new_thread->state, "WAITING");
	if(waiting_first == NULL){
		waiting_first = new_thread;
		new_thread->thread_next = NULL;
	}
	else{
		struct thread* temp = waiting_first;
		while(temp->thread_next != NULL) temp = temp->thread_next;
		temp->thread_next = new_thread;
		new_thread->thread_next = NULL;
	}
}
void push_terminated(struct thread* new_thread){
	strcpy(new_thread->state, "TERMINATED");
	if(terminated_first == NULL){
		terminated_first = new_thread;
		new_thread->thread_next = NULL;
	}
	else{
		struct thread* temp = terminated_first;
		while(temp->thread_next != NULL) temp = temp->thread_next;
		temp->thread_next = new_thread;
		new_thread->thread_next = NULL;
	}
}
struct thread* pop(char* state, char* name){
	struct thread* temp;
	if(strcmp(state, "READY") == 0){
		if(ready_first == NULL) return NULL;
		else{
			temp = ready_first;
			if(strcmp(ready_first->name, name) == 0){
				if(ready_first->thread_next == NULL){
					ready_first = NULL;
				}
				else{
					ready_first = ready_first->thread_next;
				}
			}
			else{
				struct thread* temp_pre = ready_first;
				temp = temp_pre->thread_next;
				while(temp!=NULL){
					if(strcmp(temp->name, name) == 0){
						temp_pre->thread_next = temp->thread_next;
						temp->thread_next = NULL;
						break;
					}
					temp = temp->thread_next;
					temp_pre = temp_pre->thread_next;
				}
			}
		}
	}
	else if(strcmp(state, "WAITING") == 0){
		if(waiting_first == NULL) return NULL;
		else{
			temp = waiting_first;
			if(strcmp(waiting_first->name, name) == 0){
				if(waiting_first->thread_next == NULL){
					waiting_first = NULL;
				}
				else{
					waiting_first = waiting_first->thread_next;
				}
			}
			else{
				struct thread* temp_pre = waiting_first;
				temp = temp_pre->thread_next;
				while(temp!=NULL){
					if(strcmp(temp->name, name) == 0){
						temp_pre->thread_next = temp->thread_next;
						temp->thread_next = NULL;
						break;
					}
					temp = temp->thread_next;
					temp_pre = temp_pre->thread_next;
				}
			}
		}
	}
	return temp;
}

struct alarm_clock* create_alarm(char* name, int ms){
	struct alarm_clock* new_alarm = (struct alarm_clock*) malloc(sizeof(struct alarm_clock));
	strcpy(new_alarm->name, name);
	new_alarm->ms = ms;
	new_alarm->alarm_next = NULL;
	new_alarm->is_sleep = true;
	new_alarm->resources = NULL;
	return new_alarm;
}
struct alarm_clock* create_alarm_re(char* name, int count, int* resource){
	struct alarm_clock* new_alarm = (struct alarm_clock*) malloc(sizeof(struct alarm_clock));
	strcpy(new_alarm->name, name);
	new_alarm->ms = 2147483600;
	new_alarm->alarm_next = NULL;
	new_alarm->is_sleep = false;
	new_alarm->count = count;
	new_alarm->resources = (int*)malloc(sizeof(int)*count);
	for(int i = 0; i < count; i++){
		new_alarm->resources[i] = resource[i];
	}
	return new_alarm;
}
void push_alarm(struct alarm_clock* new_alarm){
	if(alarm_first == NULL){
		alarm_first = new_alarm;
		new_alarm->alarm_next = NULL;
	}
	else{
		struct alarm_clock* temp = alarm_first;
		while(temp->alarm_next != NULL) temp = temp->alarm_next;
		temp->alarm_next = new_alarm;
		new_alarm->alarm_next = NULL;
	}
}
void pop_alarm(char* name){
	struct alarm_clock* temp;
	if(alarm_first == NULL) return;
	else{
		temp = alarm_first;
		if(strcmp(temp->name, name) == 0){
			if(alarm_first->alarm_next == NULL){
				alarm_first = NULL;
			}
			else{
				alarm_first = alarm_first->alarm_next;
			}
		}
		else{
			struct alarm_clock* temp_pre = alarm_first;
			temp = temp_pre->alarm_next;
			while(temp!=NULL){
				if(strcmp(temp->name, name) == 0){
					temp_pre->alarm_next = temp->alarm_next;
					temp->alarm_next = NULL;
					break;
				}
				temp = temp->alarm_next;
				temp_pre = temp_pre->alarm_next;
			}
		}
	}
	if(temp->resources != NULL){
		free(temp->resources);
	}
	free(temp);
}
