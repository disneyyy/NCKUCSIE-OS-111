#include <linux/kernel.h>
#include <linux/module.h> 
#include <linux/proc_fs.h>  
#include <linux/uaccess.h>//Input or output between user
#include <linux/version.h> 
#include <linux/sched.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/seq_file.h>
#include <linux/slab.h>
 
#if LINUX_VERSION_CODE >= KERNEL_VERSION(5, 6, 0) 
#define HAVE_PROC_OPS 
#endif 
 
#define MAX 1024 
#define FILE_NAME "thread_info" 

static struct proc_dir_entry *proc_ptr;//hold information of /proc file
static char write_buffer[MAX]; 
static unsigned long write_buffer_size = 0; 
static char *read_buffer;
//static int pid; 
//static int utime_;
//static int switch_time;
static int i = 0;

static int convert_to_int(char* write_buffer)
{
    int num = 0;
    char* temp = write_buffer;
    while(*temp >= '0' && *temp <= '9'){
        num *= 10;
        num += ((*temp)-'0');
        temp++;
    }
    return num;
}
static void convert_to_string(int num, char* str)
{
    int base = 10;
    while(num >= base) base *= 10;
    base /= 10;
    while(base > 1){
        *str = (num/base)+'0';
        str++;
        num %= base;
        base /= 10;
    }
    *str = (num) + '0';
    *(str+1) = '\0';
    return;
}
static void print_task_info(char* tgid)
{  
    int pid = convert_to_int(tgid);
    while(*tgid != ',') tgid++;//get tid
    tgid++;
    int tid = convert_to_int(tgid);
    struct task_struct *the_process;
    struct task_struct *the_thread;
    char* write_buffer_temp;
    char* write_buffer_temp1 = "\tThreadID:";
    char* write_buffer_temp2 = " Time:";
    char* write_buffer_temp3 = "(ms) context switch times:";
    int switch_time;
    long long int utime_;
    char temp_str1[100];
    char temp_str2[100];
    pr_info("pid: %d\n", pid);
    for_each_process(the_process) {
        if (task_tgid_nr(the_process) == pid) {
            for_each_thread(the_process, the_thread) {
                if(task_pid_nr(the_thread) == tid){
                    pr_info("\tThreadID:%d\tuTime:%lld sTime:%lld\tcontext switch times:%lu\n",
                        tid, the_thread->utime, the_thread->stime, the_thread->nvcsw + the_thread->nivcsw);
                    utime_ = the_thread->utime/1000000;
                    switch_time = the_thread->nvcsw + the_thread->nivcsw;
                }
            }
            //char* write_buffer_temp = ("\tThreadID:%d\tTime:%d(ms)\tcontext switch times:%lu",
            //                            pid, utime_, switch_time);
            write_buffer_temp = write_buffer_temp1;
            //i = 0;
            while(*write_buffer_temp != '\0'){
                read_buffer[i++] = *write_buffer_temp;
                write_buffer_temp++;
            }
            //read_buffer[i] = '\0';
            //copy pid
            write_buffer_temp = tgid;
            while(*write_buffer_temp != '\0'){
                read_buffer[i++] = *write_buffer_temp;
                write_buffer_temp++;
            }
            write_buffer_temp = write_buffer_temp2;
            while(*write_buffer_temp != '\0'){
                read_buffer[i++] = *write_buffer_temp;
                write_buffer_temp++;
            }
            //copy utime
            convert_to_string(utime_, temp_str1);
            write_buffer_temp = temp_str1;
            while(*write_buffer_temp != '\0'){
                read_buffer[i++] = *write_buffer_temp;
                write_buffer_temp++;
            }
            write_buffer_temp = write_buffer_temp3;
            while(*write_buffer_temp != '\0'){
                read_buffer[i++] = *write_buffer_temp;
                write_buffer_temp++;
            }
            //copy switch_time
            convert_to_string(switch_time, temp_str2);
            write_buffer_temp = temp_str2;
            while(*write_buffer_temp != '\0'){
                read_buffer[i++] = *write_buffer_temp;
                write_buffer_temp++;
            }
            read_buffer[i++] = '\n';
            read_buffer[i] = '\0';
            break;
        }
    }
        
    return;
}
/* This function is called then the /proc file is read */ 
static ssize_t procfile_read(struct file *fp_read, char __user *buffer, size_t buffer_length, loff_t *offset) 
{ 
    //buffer will returned to user application e.g. cat
    //offset is the current position in the file
    /*
    char* write_buffer_temp = ("hello!\n");
    int i = 0;
    while(*write_buffer_temp != '\0'){
        read_buffer[i++] = *write_buffer_temp;
        write_buffer_temp++;
    }
    read_buffer[i] = '\0';
    */
    int len = i; 
    ssize_t ret = len; 
    if (*offset >= len || copy_to_user(buffer, read_buffer, len)) { 
        pr_info("end of read file\n");
        *offset = 0;//clear to zero for next proc read 
        ret = 0;//return 0 to terminate file read
    } else { 
        pr_info("procfile read %s\n", FILE_NAME); 
        *offset += len; 
    } 
    return ret; 
}
/* This function is called with the /proc file is written. */ 
static ssize_t procfile_write(struct file *fp_write, const char __user *buff, size_t len, loff_t *off) 
{ 
    write_buffer_size = len; 
    if (write_buffer_size > MAX) 
        write_buffer_size = MAX; 
 
    if (copy_from_user(write_buffer, buff, write_buffer_size)) //user writes to proc
        return -EFAULT; 
 
    write_buffer[write_buffer_size & (MAX - 1)] = '\0'; 
    *off += write_buffer_size; 
    pr_info("procfile write %s\n", write_buffer);
    if(*write_buffer == 'R'){
        //reset
        kfree(read_buffer);
        read_buffer = kzalloc((4096),GFP_KERNEL);
        if(!read_buffer)return -ENOMEM;
        i = 0;
        return 0;   
    } 
    print_task_info(write_buffer);//information that user need
    return write_buffer_size; 
} 
#ifdef HAVE_PROC_OPS //version control :P
static const struct proc_ops proc_file_fops = { 
    .proc_read = procfile_read, 
    .proc_write = procfile_write, 
}; 
#else 
static const struct file_operations proc_file_fops = { 
    .read = procfile_read, 
    .write = procfile_write, 
}; 
#endif 
 
static int __init proc_initialize(void) 
{ 
    proc_ptr = proc_create(FILE_NAME, 0666, NULL, &proc_file_fops); 
    if (NULL == proc_ptr) { 
        proc_remove(proc_ptr); 
        pr_alert("Error:Could not initialize /proc/%s\n", FILE_NAME); 
        return -ENOMEM; 
    } 
    read_buffer = kzalloc((3),GFP_KERNEL);
    if(!read_buffer)return -ENOMEM;
    i = 3;
    read_buffer[0] = 'X';
    read_buffer[1] = '\n';
    read_buffer[2] = '\0';
    pr_info("/proc/%s created\n", FILE_NAME); 
    return 0; 
} 
 
static void __exit proc_exit(void) 
{ 
    proc_remove(proc_ptr); 
    pr_info("/proc/%s removed\n", FILE_NAME); 
} 
 
module_init(proc_initialize); 
module_exit(proc_exit); 
 
MODULE_LICENSE("GPL");