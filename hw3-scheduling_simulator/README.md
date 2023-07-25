# Environment
Ubuntu 20.04  
# Get Started
`make clean && make`  
`./scheduler_simulator [algorithm] (algorithm can be: FCFS, RR or PP)`  
# Introduction
# Mechanisms
# Build-in Commands
Based on hw1-shell, but with a few new commands:  
* add:
  *  Create a task named task_name that runs a function named function_name.
  *  Priority is ignored if the scheduling algorithm is not priority-based preemptive scheduling.
  *  This task should be set as **READY** state
  *  Print a message in the format: `Task [task_name] is ready`.  
```
add [task_name] [function_name] [priority]
```  
* del:
  * The task named task_name should be switched to **TERMINATED** state
  * Print a message in the format: `Task [task_name] is killed`.
```
del [task_name]
```
* ps:
  * Show the information of all tasks, including TID, task name, task state, running time, waiting time, 
turnaround time, resources occupied and priority (if any)
```
ps
```
* start:
  * Start or resume the simulation
  * Print a message in the format: `Start simulation`.
```
start
```

# Interrupt Handling
* During a simulation, users can input `Ctrl+Z` to pause simulation and input build-in commands above.  
> Author：成功大學資訊工程學系113級 鄭鈞智  
> Last Edit Time： 2023/07/25 17:36
