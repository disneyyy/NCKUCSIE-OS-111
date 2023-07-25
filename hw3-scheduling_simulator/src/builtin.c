#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <dirent.h>
#include <fcntl.h>
#include "../include/builtin.h"
#include "../include/command.h"
#include <ucontext.h>
#include "../include/struct_th.h"
#include "../include/function.h"
#include <signal.h>
#include "../include/shell.h"
#include <sys/time.h>
#include <time.h>

#define STACK_SIZE 10240

#define FCFS 0
#define RR 1
#define PP 2
#define TQ 3

int num_of_T = 0;
bool ctrlz_pause = false;
bool has_ctrlz = false;

struct itimerval signal_timer;
//bool is_idle = false;

int help(char **args)
{
	int i;
    printf("--------------------------------------------------\n");
  	printf("My Little Shell!!\n");
	printf("The following are built in:\n");
	for (i = 0; i < num_builtins(); i++) {
    	printf("%d: %s\n", i, builtin_str[i]);
  	}
	printf("%d: replay\n", i);
    printf("--------------------------------------------------\n");
	return 1;
}

int cd(char **args)
{
	if (args[1] == NULL) {
    	fprintf(stderr, "lsh: expected argument to \"cd\"\n");
  	} else {
    	if (chdir(args[1]) != 0)
      		perror("lsh");
	}
	return 1;
}

int echo(char **args)
{
	bool newline = true;
	for (int i = 1; args[i]; ++i) {
		if (i == 1 && strcmp(args[i], "-n") == 0) {
			newline = false;
			continue;
		}
		printf("%s", args[i]);
		if (args[i + 1])
			printf(" ");
	}
	if (newline)
		printf("\n");

	return 1;
}

int exit_shell(char **args)
{
	//if(has_ctrlz) return 0;
	//printf("exit!\n");
	struct thread* temp = ready_first;
	//printf("ready!\n");
	while(temp != NULL){
		struct thread* dele = temp;
		temp = temp->thread_next;
		//printf("free %s %s\n", dele->name, dele->state);
		free(dele);
	}
	temp = waiting_first;
	//printf("waiting!\n");
	while(temp != NULL){
		struct thread* dele = temp;
		temp = temp->thread_next;
		//printf("free %s %s\n", dele->name, dele->state);
		free(dele);
	}
	temp = terminated_first;
	//printf("terminated!\n");
	while(temp != NULL){
		struct thread* dele = temp;
		temp = temp->thread_next;
		//printf("free %s %s\n", dele->name, dele->state);
		free(dele);
	}
	struct alarm_clock* ar_temp = alarm_first;
	while(ar_temp != NULL){
		struct alarm_clock* dele = ar_temp;
		ar_temp = ar_temp->alarm_next;
		free(dele);
	}
	/*
	ar_temp = re_first;
	while(ar_temp != NULL){
		struct alarm_clock* dele = ar_temp;
		ar_temp = ar_temp->alarm_next;
		free(dele);
	}
	*/
	return 0;
}

int record(char **args)
{
	if (history_count < MAX_RECORD_NUM) {
		for (int i = 0; i < history_count; ++i)
			printf("%2d: %s\n", i + 1, history[i]);
	} else {
		for (int i = history_count % MAX_RECORD_NUM; i < history_count % MAX_RECORD_NUM + MAX_RECORD_NUM; ++i)
			printf("%2d: %s\n", i - history_count % MAX_RECORD_NUM + 1, history[i % MAX_RECORD_NUM]);
	}
	return 1;
}

bool isnum(char *str)
{
	for (int i = 0; i < strlen(str); ++i) {
    	if(str[i] >= 48 && str[i] <= 57)
			continue;
        else
		    return false;
  	}
  	return true;
}

int mypid(char **args)
{
	char fname[BUF_SIZE];
	char buffer[BUF_SIZE];
	if(strcmp(args[1], "-i") == 0) {

	    pid_t pid = getpid();
	    printf("%d\n", pid);
	
	} else if (strcmp(args[1], "-p") == 0) {
	
		if (args[2] == NULL) {
      		printf("mypid -p: too few argument\n");
      		return 1;
    	}

    	sprintf(fname, "/proc/%s/stat", args[2]);
    	int fd = open(fname, O_RDONLY);
    	if(fd == -1) {
      		printf("mypid -p: process id not exist\n");
     		return 1;
    	}

    	read(fd, buffer, BUF_SIZE);
	    strtok(buffer, " ");
    	strtok(NULL, " ");
	    strtok(NULL, " ");
    	char *s_ppid = strtok(NULL, " ");
	    int ppid = strtol(s_ppid, NULL, 10);
    	printf("%d\n", ppid);
	    
		close(fd);

  	} else if (strcmp(args[1], "-c") == 0) {

		if (args[2] == NULL) {
      		printf("mypid -c: too few argument\n");
      		return 1;
    	}

    	DIR *dirp;
    	if ((dirp = opendir("/proc/")) == NULL){
      		printf("open directory error!\n");
      		return 1;
    	}

    	struct dirent *direntp;
    	while ((direntp = readdir(dirp)) != NULL) {
      		if (!isnum(direntp->d_name)) {
        		continue;
      		} else {
        		sprintf(fname, "/proc/%s/stat", direntp->d_name);
		        int fd = open(fname, O_RDONLY);
        		if (fd == -1) {
          			printf("mypid -p: process id not exist\n");
          			return 1;
        		}

        		read(fd, buffer, BUF_SIZE);
        		strtok(buffer, " ");
        		strtok(NULL, " ");
        		strtok(NULL, " ");
		        char *s_ppid = strtok(NULL, " ");
		        if(strcmp(s_ppid, args[2]) == 0)
		            printf("%s\n", direntp->d_name);

        		close(fd);
     		}
	   	}
    	
		closedir(dirp);
	
	} else {
    	printf("wrong type! Please type again!\n");
  	}
	
	return 1;
}

int add(char **args)
{
	//args[1] = task_name
	//args[2] = function_name
	//args[3] = priority
	if(args[3] == NULL) {
		args[3] = "0";
	}
	struct thread* temp = create_thread(++num_of_T, args[1], atoi(args[3]));
	getcontext(&(temp->thread_context));
	//printf("TID: %d, name: %s, priority: %d\n", temp->TID, temp->name, temp->priority);
	temp->thread_context.uc_stack.ss_sp = malloc(STACK_SIZE);
	temp->thread_context.uc_stack.ss_size = STACK_SIZE;
	temp->thread_context.uc_stack.ss_flags = 0;
	temp->thread_context.uc_link = &main_context;
	if(strcmp(args[2], "test_exit") == 0){
		makecontext(&(temp->thread_context),(void (*)(void))test_exit,0);
	}
	else if(strcmp(args[2], "test_sleep") == 0){
		makecontext(&(temp->thread_context),(void (*)(void))test_sleep,0);
	}
	else if(strcmp(args[2], "test_resource1") == 0){
		makecontext(&(temp->thread_context),(void (*)(void))test_resource1,0);
	}
	else if(strcmp(args[2], "test_resource2") == 0){
		makecontext(&(temp->thread_context),(void (*)(void))test_resource2,0);
	}
	else if(strcmp(args[2], "idle") == 0){
		makecontext(&(temp->thread_context),(void (*)(void))idle,0);
	}
	else if(strcmp(args[2], "task1") == 0){
		makecontext(&(temp->thread_context),(void (*)(void))task1,0);
	}
	else if(strcmp(args[2], "task2") == 0){
		makecontext(&(temp->thread_context),(void (*)(void))task2,0);
	}
	else if(strcmp(args[2], "task3") == 0){
		makecontext(&(temp->thread_context),(void (*)(void))task3,0);
	}
	else if(strcmp(args[2], "task4") == 0){
		makecontext(&(temp->thread_context),(void (*)(void))task4,0);
	}
	else if(strcmp(args[2], "task5") == 0){
		makecontext(&(temp->thread_context),(void (*)(void))task5,0);
	}
	else if(strcmp(args[2], "task6") == 0){
		makecontext(&(temp->thread_context),(void (*)(void))task6,0);
	}
	else if(strcmp(args[2], "task7") == 0){
		makecontext(&(temp->thread_context),(void (*)(void))task7,0);
	}
	else if(strcmp(args[2], "task8") == 0){
		makecontext(&(temp->thread_context),(void (*)(void))task8,0);
	}
	else if(strcmp(args[2], "task9") == 0){
		makecontext(&(temp->thread_context),(void (*)(void))task9,0);
	}
	else{
		makecontext(&(temp->thread_context),(void (*)(void))test_exit,0);
	}
	//swapcontext(&main_context, &temp->thread_context);
	printf("Task %s is ready.\n", temp->name);
	push_ready(temp);
	return 1;
}

int del(char **args)
{
	struct thread* temp = pop("READY", args[1]);
	if(temp == NULL) temp = pop("WAITING", args[1]);
	if(temp == NULL) return 1;
	//goto terminated Q
	printf("Task %s is killed.\n", temp->name);
	//strcpy(temp->state, "TERMINATED");
	push_terminated(temp);
	strcpy(temp->resources, "none");
	for(int i = 0; i < 8; i++){
		if(temp->resources_get[i]){
			resource_available[i] = true;
			//printf("Task %s releases resource %d\n", temp->name, i);
		}
	}
	pop_alarm(temp->name);
	return 1;
}
void print_info(struct thread* temp){
	printf("%4d|%11s|%11s|%8ld|%8ld|", temp->TID, temp->name, temp->state, temp->running, temp->waiting);
	if(strcmp(temp->state, "TERMINATED") != 0){
		printf("%11s|", "none");
	}else{
		printf("%11ld|", temp->turnaround);
	}
	printf("%10s|%9d\n",temp->resources, temp->priority);
}
int ps(char **args)
{
	bool ps_find = false;
	printf(" TID|       name|      state| running| waiting| turnaround| resources| priority\n"
	"--------------------------------------------------------------------------------\n");
	
	for(int i = 0; i < num_of_T; i++){
		ps_find = false;
		//printf("term\n");
		struct thread* temp = terminated_first;
		while(temp != NULL && ps_find == false){
			if(temp->TID == i+1){
				//printf("%4d|%11s|%11s|%8ld|%8ld|%11ld|%10s|%9d\n", temp->TID, temp->name, temp->state, temp->running, temp->waiting, temp->turnaround, temp->resources, temp->priority);
				print_info(temp);
				ps_find = true;
				break;
			}
			temp = temp->thread_next;
		}
		temp = ready_first;
		while(temp != NULL && ps_find == false){
			if(temp->TID == i+1){
				print_info(temp);
				ps_find = true;
				break;
			}
			temp = temp->thread_next;
		}
		//printf("wait\n");
		temp = waiting_first;
		while(temp != NULL && ps_find == false){
			if(temp->TID == i+1){
				//printf("%4d|%11s|%11s|%8ld|%8ld|%11ld|%10s|%9d\n", temp->TID, temp->name, temp->state, temp->running, temp->waiting, temp->turnaround, temp->resources, temp->priority);
				print_info(temp);
				ps_find = true;
				break;
			}
			temp = temp->thread_next;
		}
		temp = running;
		while(temp != NULL && ps_find == false){
			if(temp->TID == i+1){
				//printf("%4d|%11s|%11s|%8ld|%8ld|%11ld|%10s|%9d\n", temp->TID, temp->name, temp->state, temp->running, temp->waiting, temp->turnaround, temp->resources, temp->priority);
				print_info(temp);
				ps_find = true;
				break;
			}
			temp = temp->thread_next;
		}
	}
	/*
	struct thread* temp = terminated_first;
	while(temp != NULL){
		print_info(temp);
		temp = temp->thread_next;
	}
	temp = ready_first;
	while(temp != NULL){
		print_info(temp);
		temp = temp->thread_next;
	}
	temp = waiting_first;
	while(temp != NULL){
		print_info(temp);
		temp = temp->thread_next;
	}
	*/
	//temp = running;
	//print_info(temp);
	return 1;
}
void ctrlz(){
	printf("\n");
	ctrlz_pause = true;
	has_ctrlz = true;
	if(running != NULL)
		swapcontext(&(running->thread_context), &main_context);
	//if(is_idle)
	//	swapcontext(&idle_context, &main_context);
	//printf("resume\n");
}
void set_timer(){
	signal_timer.it_value.tv_sec = 0;
	signal_timer.it_value.tv_usec = 10000;

	signal_timer.it_interval.tv_sec = 0;
	signal_timer.it_interval.tv_usec = 10000;
	if(setitimer(ITIMER_VIRTUAL, &signal_timer, NULL) < 0)
		printf("ERROR ON SET_TIMER!\n");
}
void signal_handler(){
	if(ctrlz_pause) return;
	struct thread* temp = running;
	if(running != NULL){
		running->running += 1;
		//running->turnaround += 1;
		running->run_time++;
	}
	temp = ready_first;
	while(temp != NULL){
		if(temp != running)
			temp->waiting += 1;
		//if(temp->start){
			temp->turnaround += 1;
		//}
		temp = temp->thread_next;
	}
	temp = waiting_first;
	while(temp != NULL){
		//if(temp->start){
			temp->turnaround += 1;
			//temp->waiting += 1;
		//}
		temp = temp->thread_next;	
	}
	struct alarm_clock* temp_alarm = alarm_first;
	bool re_sche = false;
	while(temp_alarm != NULL){
		//printf("%s %d\n", temp_alarm->name, temp_alarm->ms);
		if(temp_alarm->is_sleep){
			temp_alarm->ms--;
			if(temp_alarm->ms <= 0){
				temp = pop("WAITING", temp_alarm->name);
				push_ready(temp);
				pop_alarm(temp_alarm->name);
				if(schedule_mode == PP && running != NULL && temp->priority < running->priority){
					re_sche = true;
				}
				//if(is_idle)
				//	swapcontext(&idle_context, &main_context);
			}
		}
		else{
			bool re_ready = true;
			for(int i = 0; i < temp_alarm->count; i++){
				//printf("Task %s waits resource %d.\n", temp_alarm->name, temp_alarm->resources[i]);
				if(resource_available[temp_alarm->resources[i]] == false){
					re_ready = false;
					//break;
				}
			}
			if(re_ready){
				temp = pop("WAITING", temp_alarm->name);
				push_ready(temp);
				pop_alarm(temp_alarm->name);
				if(schedule_mode == PP && running != NULL && temp->priority < running->priority){
					re_sche = true;
				}
				//if(is_idle)
				//	swapcontext(&idle_context, &main_context);
			}
		}
		temp_alarm = temp_alarm->alarm_next;
	}
	if(schedule_mode == PP && re_sche && running != NULL) swapcontext(&(running->thread_context), &main_context);
	if(schedule_mode != RR || running == NULL) return;
	if(running->run_time != 0 && running->run_time % TQ == 0){
		//RR
		push_ready(pop("READY", running->name));
		swapcontext(&(running->thread_context), &main_context);
	}
}
int start(char **args)
{
	printf("Start simulation.\n");
	signal(SIGTSTP, ctrlz);
	signal(SIGVTALRM, signal_handler);
	set_timer();
	running = NULL;
	ctrlz_pause = false;
	while(ready_first != NULL  || waiting_first != NULL){
		running = NULL;
		if(ready_first == NULL){
			printf("CPU idle.\n");
			//is_idle = true;
			//swapcontext(&main_context, &idle_context);
			while(ready_first == NULL){
				if(ctrlz_pause)
					return 1;
			};
			//is_idle = false;
			//if(ctrlz_pause)
			//	return 1;
			/*
			
			*/
		}
		running = ready_first;
		strcpy(running->state, "RUNNING");
		running->start = true;
		running->run_time = 0;
		swapcontext(&main_context, &(running->thread_context));//error after ctrlz
		if(ctrlz_pause){
			return 1;
		}
		if(strcmp(running->state, "TERMINATED") == 0){
			push_terminated(pop("READY", running->name));
		}
	}
	printf("Simulation over.\n");
	
	return 1;
}

const char *builtin_str[] = {
 	"help",
 	"cd",
	"echo",
 	"exit",
 	"record",
	"mypid",
	"add",
	"del",
	"ps",
	"start"
};

const int (*builtin_func[]) (char **) = {
	&help,
	&cd,
	&echo,
	&exit_shell,
  	&record,
	&mypid,
	&add,
	&del,
	&ps,
	&start
};

int num_builtins() {
	return sizeof(builtin_str) / sizeof(char *);
}
