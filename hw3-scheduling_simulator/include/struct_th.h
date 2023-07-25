#ifndef STRUCT_TH_H
#define STRUCT_TH_H
#include <ucontext.h>
#include <stdbool.h>
#include <sys/time.h>
#include <time.h>
ucontext_t main_context;
//ucontext_t idle_context;
struct thread{
    int TID;
    char name[100];
	char state[20];
	long running;
    long waiting;
    long turnaround;
	char resources[100];
    bool resources_get[8];
    int resources_count;
    int priority;
    ucontext_t thread_context;
    struct thread* thread_next;
    bool start;
    int run_time;
};
struct thread* ready_first;
//struct thread* ready_last;
struct thread* running;
struct thread* waiting_first;
//struct thread* waiting_last;
struct thread* terminated_first;
//struct thread* terminated_last;
struct thread* create_thread(int, char*, int);
void push_ready(struct thread*);
void push_waiting(struct thread*);
void push_terminated(struct thread*);
struct thread* pop(char*, char*);

struct alarm_clock{
    char name[100];
    int ms;
    struct alarm_clock* alarm_next;
    bool is_sleep;
    int count;
    int *resources;
};
struct alarm_clock* create_alarm(char* , int);
struct alarm_clock* create_alarm_re(char* , int, int*);
void push_alarm(struct alarm_clock*);
void pop_alarm(char*);
struct alarm_clock* alarm_first;

bool resource_available[8];
#endif
