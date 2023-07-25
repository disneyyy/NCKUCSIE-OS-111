# Environment
Ubuntu 20.04  
# Get Started
`make clean && make`  
`./my_shell`  
# Prompt Symbol
`>>> $`  
# Build-in Commands
* help: `help`, terminal will show help message to show more usages of other build-in commands.  
* cd: `cd [directory_name]`, you will change the working directory.  
* echo: `echo [-n] [strings]`, terminal will print a line of text to stdout. If “-n” flag is set, “echo” will not output the trailing newline.  
* exit: `exit`, the shell will be terminated.  
* record: `record`, terminal will show the last 16 commands user inputted.  
* replay: `replay [number] (1 <= number <= 16)`, re-execute the command that is listed in record.
* mypid: `mypid [-i|-p|-c] [number] (number indicate a process’ pid)`, show the related pids about the process
  * -i: print process’ pid, which execute “mypid”. (ignore [number])
  * -p: print process’ parent’s pid (i.e., who has child [number])
  * -c: print process’ child’s pid (i.e., whose parent is [number])
# Input Format
* `<`: Input redirection.
* `>`: Output redirection.
* `|`: Pipeline.
* `&`: Background-execution operator.

> Author：成功大學資訊工程學系113級 鄭鈞智  
> Last Edit Time： 2023/07/25 10:30
