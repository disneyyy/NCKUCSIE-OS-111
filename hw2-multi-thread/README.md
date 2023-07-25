# Environment
Ubuntu 20.04  
4 cores
# Get Started
`make unload`  
`make clean && make`  
`make load`  
`./MT_matrix [number of worker threads] [file name of input matrix1] [file name of input matrix2]`  

# Implementation
By distributing row number of m1 to each thread to shorten calculation time.  
For the picture below, m1_i stands for row number of m1 and N represents total number of threads.  
![explain1](https://github.com/disneyyy/NCKUCSIE-OS-2022/blob/main/hw2-multi-thread/pic/explain1.jpg)  
If row number of m1 is less than total number of threads, then distribute column number of m2 to each thread.  
For the picture below, m1_i stands for row number of m1 , m2_j means column number of m2, and N represents total number of threads.  
![explain2](https://github.com/disneyyy/NCKUCSIE-OS-2022/blob/main/hw2-multi-thread/pic/explain2.jpg)  
# Comparations
Here are 4 test cases to demonstrate.  
* testcase1: m1:2048×2048, m2:2048×2048  
![test1](https://github.com/disneyyy/NCKUCSIE-OS-2022/blob/main/hw2-multi-thread/pic/t1.jpg)  
* testcase2: m1:4096×4096, m2:4096×4096  
![test2](https://github.com/disneyyy/NCKUCSIE-OS-2022/blob/main/hw2-multi-thread/pic/t2.jpg)
* testcase3: m1:1×4096, m2:4096×4096  
![test3](https://github.com/disneyyy/NCKUCSIE-OS-2022/blob/main/hw2-multi-thread/pic/t3.jpg)
* testcase4: m1:4096×4096, m2:4096×1  
![test4](https://github.com/disneyyy/NCKUCSIE-OS-2022/blob/main/hw2-multi-thread/pic/t4.jpg)  
# Conclusion
* When using less threads, elapsed time will become dramatically large.
* 4 threads has best efficiency among other cases.
* when using more than 4 threads, elapsed time will increase because too much thread can cause redundant context switches, which increase operating time.
> Author：成功大學資訊工程學系113級 鄭鈞智  
> Last Edit Time： 2023/07/25 12:20
