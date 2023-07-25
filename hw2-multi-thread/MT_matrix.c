#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h> 
#include <sys/stat.h> 
#include <fcntl.h> 
#include <unistd.h>
#include <string.h>
#include <pthread.h>
//#include <time.h>
//#include <sys/time.h>

int ans_i, ans_j;//i, j for ans
int mat_i[2], mat_j[2];//i, j for mat1, mat2
int **mat1, **mat2;
unsigned long long int **ans;
int proc_fd;//opening proc file
pthread_mutex_t mutex1 = PTHREAD_MUTEX_INITIALIZER;
int N;

void write_to_proc(int fd){
	int pid = getpid();
	int tid = gettid();
	char *buf, *buf2;
	int length = snprintf(NULL, 0, "%d", pid);
	int length2 = snprintf(NULL, 0, "%d", tid);
	buf = malloc(length+1);
	buf2 = malloc(length2+1);
	snprintf(buf, length+1, "%d", pid);
	snprintf(buf2, length2+1, "%d", tid);
	strcat(buf, ",");
	strcat(buf, buf2);
	write(fd, buf, strlen(buf));
	free(buf);
	free(buf2);
}
void read_proc(int fd){
	int ret;
	char buf[2048];
	do { char buf[2048] = {0};
		ret = read(fd, buf, sizeof(buf));
		printf("%s", buf);
	}while(ret > 0);
	//free(buf);
}
void convert_to_string(int num, char* str)
{
    int base = 10;
    while(num > base) base *= 10;
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
void *thread_process(void* arg) {//seq: i'th thread, N: number of threads (argc)
   	int seq = *(int*)arg;
	if(N > mat_i[0] && N <= mat_j[1]){
		int limit = ans_j/N;
		if(seq == N-1){
		limit = ans_j-limit*seq;
		}
		for(int j = 0; j < limit; j++){
			for(int i = 0; i < ans_i; i++){
				for(int k = 0; k < mat_j[0]; k++){
					ans[i][j+(ans_j/N)*seq] += (mat1[i][k] * mat2[k][j+(ans_j/N)*seq]);
				}
			}
		}
	}
	else {
		int limit = ans_i/N;
		if(seq == N-1){
		limit = ans_i-limit*seq;
		}
		for(int i = 0; i < limit; i++){
			for(int j = 0; j < ans_j; j++){
				for(int k = 0; k < mat_j[0]; k++){
					ans[i+(ans_i/N)*seq][j] += (mat1[i+(ans_i/N)*seq][k] * mat2[k][j]);
				}
			}
		}
	}
	
	pthread_mutex_lock(&mutex1);//lock
	write_to_proc(proc_fd);
	//read_proc(proc_fd);
	//printf("write success!\n");
	pthread_mutex_unlock(&mutex1);//unlock
	//printf("exit success!\n");
	free(arg);
	pthread_exit(NULL);
}
int main(int argc, char *argv[]) {//argc[1]: number of threads
	//printf("%d\n", sysconf(_SC_CLK_TCK));
	//struct timeval start, end;
	//time_t t;
	if(argc != 4){
		printf("Incorrect argument!\n");
		return 0;
	}
	N = atoi(argv[1]);
	pthread_t *pt = malloc(sizeof(pthread_t) * N);//initiallize multithread
	for(int i = 2; i < 4; i++){
		FILE *fp = fopen(argv[i], "r");
		if(fp == 0){
			printf("%s doesn't exist!\n", argv[i]);
			return 0;
		}
		else{
			fscanf(fp, "%d", &mat_i[i-2]);
			fscanf(fp, "%d", &mat_j[i-2]);
			if(i == 2) mat1 = malloc(sizeof(int*)*mat_i[i-2]);
			else	   mat2 = malloc(sizeof(int*)*mat_i[i-2]);
			for(int j = 0; j < mat_i[i-2]; j++){
				if(i == 2) mat1[j] = malloc(sizeof(int)*mat_j[i-2]);
				else	   mat2[j] = malloc(sizeof(int)*mat_j[i-2]);
			}
			for(int j = 0; j < mat_i[i-2]; j++){
				for(int k = 0; k < mat_j[i-2]; k++){
					if(i == 2) fscanf(fp, "%d", &mat1[j][k]);
					else	   fscanf(fp, "%d", &mat2[j][k]);
				}
			}
		}
		fclose(fp);
	}
	/*test for reading matrix
	for(int i = 0; i < mat_i[0]; i++){
		for(int j = 0; j < mat_j[0]; j++){
			printf("%d ", mat1[i][j]);
		}
		printf("\n");
	}
	for(int i = 0; i < mat_i[1]; i++){
		for(int j = 0; j < mat_j[1]; j++){
			printf("%d ", mat2[i][j]);
		}
		printf("\n");
	}
	return 0;
	*/
	//initiallizing answer mat
	ans_i = mat_i[0];
	ans_j = mat_j[1];
	ans = malloc(sizeof(unsigned long long int*)*ans_i);
	for(int i = 0; i < ans_i; i++){
		ans[i] = malloc(sizeof(unsigned long long int)*ans_j);
	}
	for(int i = 0; i < ans_i; i++){
		for(int j = 0; j < ans_j; j++){
			ans[i][j] = 0;
		}
	}
	//opening proc file
	proc_fd = open("/proc/thread_info", O_RDWR);
	if(proc_fd < 0){
		printf("proc file doesn't exist!\n");
		return 0;
	}
	write(proc_fd, "R", 1);//reset proc file
	//creating threads
	//t = time(NULL);
	//gettimeofday(&start, 0);
	for(int i = 0; i < N; i++){
		int *passed = malloc(sizeof(int));
		*passed = i;
		if(pthread_create(&pt[i], NULL, &thread_process, passed) != 0){
			printf("pthread %d creating failed!\n", i);
			return 1;
		}
	}
	//joining threads
	for(int i = 0; i < N; i++) {
        if(pthread_join(pt[i], NULL) != 0){
			printf("pthread %d joining failed!\n", i);
			return 2;
		}
    }
	//t = time(NULL) - t;
	//gettimeofday(&end, 0);
	//long seconds = end.tv_sec - start.tv_sec;
    //long microseconds = end.tv_usec - start.tv_usec;
    //double elapsed = seconds + microseconds*1e-6;
	//double time_taken = ((double)t)/CLOCKS_PER_SEC; // calculate the elapsed time if t is clock_t
   	//printf("The program took %f seconds to execute\n", t);
	//printf("Time measured: %.3f seconds.\n", elapsed);
	FILE *fp = fopen("result.txt", "w+");
	fprintf(fp, "%d %d\n", ans_i, ans_j);
	for(int i = 0; i < ans_i; i++){
		for(int j = 0; j < ans_j; j++){
			//printf("%u ", ans[i][j]);
			fprintf(fp, "%llu ", ans[i][j]);
		}
		fprintf(fp, "\n");
	}
	fclose(fp);
	free(pt);
	printf("PID:%d\n", getpid());
	read_proc(proc_fd);
	close(proc_fd);
	//printf("free pt success!\n");
	for(int i = 0; i < 2; i++){
		for(int j = 0; j < mat_i[i]; j++){
			if(i == 0) free(mat1[j]);
			else	   free(mat2[j]);
		}
		if(i == 0)	free(mat1);
		else		free(mat2);
	}
	//printf("free mat success!\n");
	for(int i = 0; i < ans_i; i++){
		free(ans[i]);
	}
	free(ans);
	//printf("free ans success!\n");
    return 0;
}