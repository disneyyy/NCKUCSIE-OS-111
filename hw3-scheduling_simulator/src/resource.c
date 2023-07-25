#include "../include/resource.h"
#include <stdbool.h>
#include <stdio.h>
#include "../include/struct_th.h"
#include <string.h>

//int test_in_resource = 1;
void get_resources(int count, int *resources)
{
    for(int i = 0; i < count; i++){
        if(!resource_available[resources[i]]){
            //printf("resource %d is occupied\n", i);
            //goto waiting Q
            printf("Task %s is waiting resource.\n", running->name);
            struct alarm_clock* temp = create_alarm_re(running->name, count, resources);
            push_alarm(temp);
            push_waiting(pop("READY", running->name));
            swapcontext(&(running->thread_context), &main_context);
            i = -1;
        }
    }
    for(int i = 0; i < count; i++){
        resource_available[resources[i]] = false;
        running->resources_get[resources[i]] = true;
        running->resources_count++;
        printf("Task %s gets resource %d.\n", running->name, resources[i]);
    }
    strcpy(running->resources, "none");
    for(int i = 0; i < 8; i++){
        if(running->resources_get[i]){
			if(strcmp(running->resources, "none")==0){
                char t[100];
                t[1] = '\0';
                t[0] = i +'0';
                strcpy(running->resources, t);
            }
            else{
                char t[100];
                t[2] = '\0';
                t[1] = i +'0';
                t[0] = ' ';
                strcat(running->resources, t);
            }
		}
    }
    //printf("%d\n", test_in_resource);
    //count:length of resource array
    //if sb tooks resource, then the resource becomes unavailable
}

void release_resources(int count, int *resources)
{
    //available resource
    for(int i = 0; i < count; i++){
        printf("Task %s releases resource %d.\n", running->name, resources[i]);
        resource_available[resources[i]] = true;
        running->resources_get[resources[i]] = false;
        running->resources_count--;
    }
    strcpy(running->resources, "none");
    /*
    struct alarm_clock* temp_alarm = re_first;
	while(temp_alarm != NULL){
		bool re_ready = true;
		for(int i = 0; i < temp_alarm->count; i++){
			if(resource_available[temp_alarm->resources[i]] == false){
				re_ready = false;
				break;
			}
		}
		if(re_ready){
			struct thread* temp = pop("WAITING", temp_alarm->name);
			push_ready(temp);
			pop_alarm(temp_alarm->name);
            break;
		}
        temp_alarm = temp_alarm->alarm_next;
	}
    */
}
