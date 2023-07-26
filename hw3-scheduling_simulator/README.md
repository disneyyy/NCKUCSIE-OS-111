# Environment
Ubuntu 20.04  
# Get Started
`make clean && make`  
`./scheduler_simulator [algorithm] (algorithm can be: FCFS, RR or PP)`  
# Introduction
This is a task scheduling simulator. There are 3 algorithms to simulate scheduling:
* FCFS (**F**irst **C**ome **F**irst **S**erve)
* RR (**R**ound **R**obin) with time quantum = 30 (ms)
* PP (**P**riority-based **P**reemptive scheduling). The smaller interger in priority field has higher priorty.
# Mechanisms
* When scheduler dispatches CPU to a task, then print the message below:
```
Task [task_name] is running.
```
* If there are no tasks to be scheduled, but there are still tasks waiting, print a message in the format:
```
CPU idle.
```
* When the simulation is over, switch back to shell mode after printing a message in the format:
```
Simulation over.
```
There are 4 states for a task:
* **ready**:
  * When a task is added by command `add`.
  * When a task in running state is preempted by other tasks or running out of time quantum.
  * When a task in waiting queue founds that resource is available or sleep time is over.
* **waiting**:
  * When a task in running state founds that resource is inadequate.
  * When a task in running state is forced to sleep.
* **running**:
  * Scheduler dispatched a task in ready queue to running state.
* **terminated**:
  * When a task finishes all its work.
  * When a task is deleted by command `del`.
Simulator will print some messages to inform changes to users:
* When a task is forced sleeping:
```
Task [task_name] goes to sleep.
```
* Whan a task gains all resources:
```
Task [task_name] gets resource [resource_id].
```
* When a task releases its resources:
```
Task [task_name] releases resource [resource_id].
```
* When a task is lack of resource:
```
Task [task_name] is waiting resource.
```
* When a task is terminated:
```
Task [task_name] has terminated.
```
# Build-in Commands
Based on hw1-shell, but with a few new commands:  
* add:
  *  Create a task named task_name that runs a function named function_name.
  *  Priority is ignored if the scheduling algorithm is not priority-based preemptive scheduling.
  *  This task should be set as **READY** state
  *  Print a message in the format: `Task [task_name] is ready`.  
```
>>> $ add [task_name] [function_name] [priority]
Task [task_name] is ready.
```  
* del:
  * The task named task_name should be switched to **TERMINATED** state
  * Print a message in the format: `Task [task_name] is killed`.
```
>>> $ del [task_name]
Task [task_name] is killed.
```
* ps:
  * Show the information of all tasks, including TID, task name, task state, running time, waiting time, 
turnaround time, resources occupied and priority (if any)
  * The TID of each task is unique, and TID starts from 1. 
  * There is no turnaround time for unterminated tasks.
  * Time unit: 10ms
```
>>> $ ps
 TID|       name|      state| running| waiting| turnaround| resources| priority
--------------------------------------------------------------------------------
   1|         T1| TERMINATED|       1|       0|          1|      none|        1
   2|         T2|    WAITING|       1|       1|       none|     1 3 7|        2
   3|         T3|      READY|       0|       2|       none|      none|        3
   4|         T4|    RUNNING|       3|       2|       none|      none|        4
```
* start:
  * Start or resume the simulation
  * Print a message in the format: `Start simulation`.
```
>>> $ start
Start simulation.
```

# Interrupt Handling
* During a simulation, users can input `Ctrl+Z` to pause simulation and input build-in commands above.
```
>>> $ start
Start simulation.
Task T1 is running.
Task T1 goes to sleep.
Task T2 is running.
^Z
>>> $
```
> Author：成功大學資訊工程學系113級 鄭鈞智  
> Last Edit Time： 2023/07/26 10:24
