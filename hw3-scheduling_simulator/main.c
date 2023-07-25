#include <stdlib.h>
#include "include/shell.h"
#include "include/command.h"
#include <string.h>
#include <stdio.h>
#include "include/struct_th.h"
#include "include/function.h"

int main(int argc, char *argv[])
{
	if(argc != 2){
		printf("Wrong argument counts!\n");
		return 0;
	}
	ready_first = NULL;
	running = NULL;
	waiting_first = NULL;
	terminated_first = NULL;
	alarm_first = NULL;
	history_count = 0;
	/*
	getcontext(&idle_context);
	idle_context.uc_stack.ss_sp = malloc(10240);
	idle_context.uc_stack.ss_size = 10240;
	idle_context.uc_stack.ss_flags = 0;
	idle_context.uc_link = &main_context;
	makecontext(&idle_context,(void (*)(void))idle,0);
	*/
	for(int i = 0; i < 8; i++){
		resource_available[i] = true;
	}
	for (int i = 0; i < MAX_RECORD_NUM; ++i)
    	history[i] = (char *)malloc(BUF_SIZE * sizeof(char));
	//printf("%s\n", argv[1]);
	//argv[1] is algorithm of schedulng
	if(strcmp(argv[1], "FCFS") == 0) schedule_mode = 0;
	else if(strcmp(argv[1], "RR") == 0) schedule_mode = 1;
	else schedule_mode = 2;
	shell();

	for (int i = 0; i < MAX_RECORD_NUM; ++i)
    	free(history[i]);

	return 0;
}
