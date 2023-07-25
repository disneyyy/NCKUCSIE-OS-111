#ifndef _POSIX_C_SOURCE
	#define  _POSIX_C_SOURCE 200809L
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#define SIZE 1000
char* prompt_symbol = ">>> $ ";
typedef struct node{
	char message[SIZE];
	struct node * next;
	int num;
} node;
char command[SIZE];
char temp_file[2][100];//0: write, 1: read
char out_file[100];
int seq = 0;
node *first = NULL, *last = NULL, *temp = NULL;
int replay_fail = 0;
int ispipe = 0;
int isout = 0;
void read_line(char *line){
	//char *line = malloc(sizeof(char)*SIZE);
	char c;
	int pos = 0;
	if(!line){
		printf("error!\n");
		exit(EXIT_FAILURE);
	}
	while((c = getchar()) != '\n' || pos >= SIZE){
		line[pos++] = c;
	}
	line[pos] = '\0';
	//return line;
}
int check_blank(char* command){
	int pos = 0;
	while(command[pos] == ' ' || command[pos] == '\t'){
		pos++;
	}
	if(command[pos] != '\0') return 0;//not blank
	else return 1;//is blank
}
void replace_string(char *a, char *b){
    char * t = a;
    while(1){
        if(strncmp(t, b, strlen(b)) == 0){
            *t = '\0';
            return;
        }
        t++;
    }
}
int check_num(char* str){
    if(str == NULL) return 0;
    while(*str == ' ' || *str == '\t') str++;
    if(str[0]=='\0') return 0;
    else if(str[0] == '1' && (str[1] == '\0' || str[1] == ' ')) return 1;
    else if(str[0] == '2' && (str[1] == '\0' || str[1] == ' ')) return 2;
    else if(str[0] == '3' && (str[1] == '\0' || str[1] == ' ')) return 3;
    else if(str[0] == '4' && (str[1] == '\0' || str[1] == ' ')) return 4;
    else if(str[0] == '5' && (str[1] == '\0' || str[1] == ' ')) return 5;
    else if(str[0] == '6' && (str[1] == '\0' || str[1] == ' ')) return 6;
    else if(str[0] == '7' && (str[1] == '\0' || str[1] == ' ')) return 7;
    else if(str[0] == '8' && (str[1] == '\0' || str[1] == ' ')) return 8;
    else if(str[0] == '9' && (str[1] == '\0' || str[1] == ' ')) return 9;
    else if(str[0] == '1' && str[1] == '0' && (str[2] == '\0' || str[2] == ' ')) return 10;
    else if(str[0] == '1' && str[1] == '1' && (str[2] == '\0' || str[2] == ' ')) return 11;
    else if(str[0] == '1' && str[1] == '2' && (str[2] == '\0' || str[2] == ' ')) return 12;
    else if(str[0] == '1' && str[1] == '3' && (str[2] == '\0' || str[2] == ' ')) return 13;
    else if(str[0] == '1' && str[1] == '4' && (str[2] == '\0' || str[2] == ' ')) return 14;
    else if(str[0] == '1' && str[1] == '5' && (str[2] == '\0' || str[2] == ' ')) return 15;
    else if(str[0] == '1' && str[1] == '6' && (str[2] == '\0' || str[2] == ' ')) return 16;
    else return 0;
}
void forking(char *temp_command){
    pid_t ret, dead;
        int status;
        ret = fork();
        if(ret < 0){//error
            printf("Fork Failed");
            exit(-1);
        }
        else if (ret == 0){//child
            //execlp("/bin/ls", "ls", NULL);
            int i = 1;
            char *temp_token, *temp_save;
            char** temp_char = (char**)malloc(10*sizeof(char*));
            temp_token = strtok_r(temp_command, " ", &temp_save);
            temp_char[0] = (char*)malloc(100);
            strcpy(temp_char[0], temp_token);
            temp_token = strtok_r(NULL, " ", &temp_save);
            for(; i < 10 && temp_token!= NULL; i++){
                temp_char[i] = (char*)malloc(100);
                strcpy(temp_char[i], temp_token);
                temp_token = strtok_r(NULL, " ", &temp_save);
            }
            temp_char[i] = NULL;
            if(seq > 0 && (strcmp(temp_char[0], "cat") == 0 || strcmp(temp_char[0], "head") == 0 || strcmp(temp_char[0], "tail") == 0 || strcmp(temp_char[0], "nl") == 0 || strcmp(temp_char[0], "grep") == 0)){
                temp_char[i] = (char*)malloc(100);
                strcpy(temp_char[i], temp_file[1]);
                temp_char[i+1] = NULL;
                i++;
                int fd = open(temp_file[0], O_WRONLY | O_CREAT | O_TRUNC, S_IRWXU | S_IRWXG | S_IRWXO);// | O_APPEND);
                dup2(fd, 1);
                dup2(fd, 2);
                close(fd);
            }
            //char* const arg[] = {"dfsfffs",NULL};
            //execvp(temp_char[0],temp_char);
            if(execvp(temp_char[0],temp_char)<0){
                printf("error!\n");
                exit(0);
            }
            if(seq > 0 && (strcmp(temp_char[0], "cat") == 0 || strcmp(temp_char[0], "head") == 0 || strcmp(temp_char[0], "tail") == 0 || strcmp(temp_char[0], "nl") == 0 || strcmp(temp_char[0], "grep") == 0)){
                int fd = open(temp_file[0], O_WRONLY | O_CREAT | O_APPEND, S_IRWXU | S_IRWXG | S_IRWXO);// | O_APPEND);
                dup2(fd, 1);//change back to append
                dup2(fd, 2);
                close(fd);
            }
            i--;
            for(;i>=0; i--){
                free(temp_char[i]);
            }
            free(temp_char);
        }
        else{//parent
            wait(NULL);
            //printf("\nChild Complete\n");
            //exit(0);
        }
}
void processing(char *command){
    //replay___:
    if(check_blank(command) == 1){
        return;//skip proccess if command is empty or all blank
    }
    for(int i = 0; i < strlen(command); i++){
        if(command[i] == '<') command[i] = ' ';
    }
    if(command[strlen(command)-1] == '&'){
        command[strlen(command)-1] = '\0';
        int pid = fork();
        int status;
        if(pid < 0){
            exit(1);
        }
        else if(pid == 0){
            int pid2 = fork();
            if(pid2 < 0){
                exit(1);
            }
            else if(pid2 == 0){
                printf("[Pid]: %d\n", getpid());
                usleep(10);
                processing(command);
                exit(0);
            }
            else{
                //int p = waitpid(-1, NULL, WNOHANG);
                //printf("[Pid]: %d\n", p);
                exit(0);
            }
        }
        else{
            //printf("[Pid]: %d\n", getpid());
            //int p = waitpid(-1, &status, WNOHANG);
            int p = wait(NULL);
            //printf("[Pid]: %d\n", p);
            return;
        }
    }
    char *out_tok, *out_save;
    out_tok = strtok_r(command, ">", &out_save);
    out_tok = strtok_r(NULL, ">", &out_save);
    if(out_tok != NULL){
        //printf("out\n");
        while(*out_tok == ' ') out_tok++;
        char cwd[100];
        getcwd(cwd, 100);
        strcat(cwd, "/");
        strcpy(temp_file[0], cwd);
        strcat(temp_file[0], out_tok);
        strcpy(out_file, out_tok);
        int fd = open(temp_file[0], O_WRONLY | O_CREAT | O_TRUNC, S_IRWXU | S_IRWXG | S_IRWXO);
        dup2(fd, 1);
        dup2(fd, 2);
        close(fd);
        isout = 1;
    }
    char pipe_token_temp[SIZE];
    char *token, *pipe_token, *save_ptr, *pipe_ptr;
    pipe_token = strtok_r(command, "|", &pipe_ptr);
    //strcpy(pipe_token_temp, pipe_token);
    while(1){
        if(!pipe_token) break;
        //token = strtok_r(command, " ", &save_ptr);
        strcpy(pipe_token_temp, pipe_token);
        token = strtok_r(pipe_token, " ", &save_ptr);
        if(strcmp(token, "help") == 0){//help
            printf("built-in functions:\n"
                "1. help:\tshow all built-in functions info.\n"
                "2. cd:\t\tchange directory.\n"
                "3. echo:\techo the strings to standard output.\n"
                "4. record:\tshow last-16 cmd you typed in.\n"
                "5. replay:\tre-execute the cmd showed in record.\n"
                "6. mypid:\tfind and print proccess-ids.\n"
                "7. exit:\texit shell.\n\n");
        }
        else if(strcmp(token, "cd") == 0){//cd
            char cwd[100];
            getcwd(cwd, 100);
            strcat(cwd, "/");
            token = strtok_r(NULL, " ", &save_ptr);
            if(token == NULL)
            {
                printf("error\n");
                goto con;
            }
            strcat(cwd, token);
            if(token[0] == '/'){
                strcpy(cwd, token);
            }else if(strcmp(token, "..") == 0){
                int l = strlen(cwd)-1;
                while(cwd[l] != '/') l--;
                l--;
                while(cwd[l] != '/') l--;
                cwd[l] = '\0';
            }
            if(chdir(cwd) < 0){
                printf("error\n");
            }
            else{
                printf("%s\n", cwd);
            }
        }
        else if(strcmp(token, "echo") == 0){//echo
            //printf("echo\n");
            token = strtok_r(NULL, " ", &save_ptr);//get next command string
            if(token == NULL) goto con;
            if(strcmp(token, "-n")==0){
                while(*save_ptr == ' ' || *save_ptr == '\t'){
                    save_ptr++;
                }
                printf("%s", save_ptr);
                fflush(stdout);
            }
            else{
                printf("%s", token);
                if(save_ptr != NULL){
                    printf(" %s\n", save_ptr);
                }
                else{
                    printf("\n");
                }
            }
        }
        else if(strcmp(token, "exit") == 0){//exit
            dup2(3, 1);
            while(first != NULL){
                free(first);
                first = first->next;
            }
            printf("Bye!\n\n");
            exit(0);
        }
        else if(strcmp(token, "record") == 0){
            //printf("record\n");
            node *target = first;
            printf("history cmd:\n");
            if(last->num >= 16) target = target->next;//fix error if queue is full
            for(int i = 1; i <= 16 && target != NULL; i++){
                if(i < 10) printf(" %d: ", i);
                else printf("%d: ", i);
                printf("%s\n", target->message);
                target = target->next;
                if(i == 15 && last->num >= 16){//fix error if queue is full
                    printf("16: %s\n", last->message);
                    break;
                }
            }
            printf("\n");
        }
        else if(strcmp(token, "replay") == 0){
            //printf("%d\n", last->num);
            token = strtok_r(NULL, " ", &save_ptr);
            int num = check_num(token);
            if(last != NULL && last->num > 0 && 0 < num && num <= 16  && num <= last->num){
                node* target = first;
                for(int i = 1; i < num; i++){
                    target = target->next;//get target command
                }
                char command2[SIZE];
                strcpy(command2, target->message);//fix the queue last node
                replace_string(last->message, "replay");
                char command_last[SIZE] = "\0";
                if(pipe_ptr!=NULL){
                    strcpy(command_last, pipe_ptr);
                }
                strcat(last->message, command2);
                char *check = command2; 
                while(*check!='\0' && seq == 0){
                    if(*check == '|'){
                        ispipe = 1;
                        int fd = open(temp_file[0], O_WRONLY | O_CREAT | O_TRUNC, S_IRWXU | S_IRWXG | S_IRWXO);// | O_APPEND);
                        dup2(fd, 1);
                        dup2(fd, 2);
                        close(fd);
                        break;
                    }
                    check++;
                }
                if(command_last[0]!='\0'){
                    strcat(last->message, " |");
                    strcat(last->message, command_last);
                }
                if(isout == 1){
                    strcat(last->message, " > ");
                    strcat(last->message, out_file);
                }
                //strcpy(last->message, command2);
                //goto replay___;//redo the target command
                processing(command2);
            }
            else{//bug occurrs if free memory here
                printf("replay: wrong args\n");
                replay_fail = 1;
                if(last->num < 16) {
                    if(first == last){//this is the first command
                        free(last);
                        first = NULL;
                        last = NULL;//reset queue
                    }else{//1~16 command
                        node *target = first;
                        while(target->next != NULL && target->next != last){
                            target = target->next;//find the node before last
                        }
                        free(last);//delete last node
                        last = target;
                        last->next = NULL;
                    }
                    //pipe_token = strtok_r(NULL, "|", &pipe_ptr);
                    //continue;
                }else{
                    //more than 16 commands
                    first = temp;
                    while(temp->next != last){
                        temp = temp->next;
                    }
                    free(last);
                    last = temp;
                    last->next = NULL;
                    //pipe_token = strtok_r(NULL, "|", &pipe_ptr);
                    //continue;
                }
            }
        }
        else if(strcmp(token, "mypid") == 0){
            //printf("mypid\n");
            token = strtok_r(NULL, " ", &save_ptr);
            if(token == NULL) goto con;
            if(strcmp(token, "-i") == 0)
                printf("%d\n", (int)getpid());//pid of current process
            else if(strcmp(token, "-p") == 0){
                // /proc/(current pid)/stat的第4個
                char proc_addr [100]= "/proc/";
                token = strtok_r(NULL, " ", &save_ptr);
                if(!token) goto con;
                strcat(proc_addr, token);
                strcat(proc_addr, "/stat");
                int proc = open(proc_addr, O_RDONLY);
                if(proc<0){
                    printf("mypid -p: pid %s doesn't exist!\n", token);
                    goto con;
                }
                char buf[SIZE];
                read(proc, buf, sizeof(buf));
                char* save_ptr2;
                char* token2 = strtok_r(buf, " ", &save_ptr2);
                for(int i = 0; i < 3 && token2 != NULL; i++){
                    token2 = strtok_r(NULL, " ", &save_ptr2);
                }
                printf("%s\n", token2);
                close(proc);
            }
            else if(strcmp(token, "-c") == 0){
                char proc_addr [100]= "/proc/";
                token = strtok_r(NULL, " ", &save_ptr);
                if(!token) goto con;
                strcat(proc_addr, token);
                strcat(proc_addr, "/task/");
                strcat(proc_addr, token);
                strcat(proc_addr, "/children");// /proc/(current pid)/task/(current pid)/children
                int proc = open(proc_addr, O_RDONLY);
                if(proc<0){
                    printf("mypid -c: pid %s doesn't exist!\n", token);
                    goto con;
                }
                char buf[SIZE];
                read(proc, buf, sizeof(buf));
                char* save_ptr2;
                char* token2 = strtok_r(buf, " ", &save_ptr2);
                while(token2 != NULL && strlen(token2) <= 5 && atoi(token2) != 0 && atoi(token2) >= atoi(token)){
                    printf("%s\n", token2);
                    token2 = strtok_r(NULL, " ", &save_ptr2);
                }
                close(proc);
                //execlp("cat", "cat", proc_addr, NULL);
                //fflush(stdout);
            }
            else{
                printf("error!\n");
            }
        }
        else{//fork child process
            //printf("error\n");
            forking(pipe_token_temp);
        }
        con:
        //token = strtok_r(NULL, " ", &save_ptr);
        pipe_token = strtok_r(NULL, "|", &pipe_ptr);
        if(ispipe == 1){//clone to infile :p
            int fd = open(temp_file[0], O_RDONLY);// | O_APPEND);
            if(fd<0){
                return;
            }
            int ret;
            char buf[SIZE];
            int fd2 = open(temp_file[1], O_WRONLY | O_CREAT | O_TRUNC, S_IRWXU | S_IRWXG | S_IRWXO);
            if(fd2<0){
                return;
            }
            do { char buf[1024] = {0};
                ret = read(fd, buf, sizeof(buf));
                write(fd2, buf, ret);
            }while(ret > 0);
            close(fd);
            close(fd2);
        }
        seq++;
    }
}
int is_buildin(char * token){
    if ( token == NULL) return 0;
    while(*token == ' ') token++; 
    if(strncmp(token, "echo", 4) == 0 || strncmp(token, "help", 4) == 0 || strncmp(token, "cd", 2) == 0 || strncmp(token, "record", 6) == 0 || strncmp(token, "replay", 6) == 0 || strncmp(token, "mypid", 5) == 0 || strncmp(token, "exit", 4) == 0)
        return 1;
    else return 0;
}
void pipel(char * command){
    char *pipe_token_test, *pipe_token_test_save;
    pipe_token_test = strtok_r(command, "|", &pipe_token_test_save);
    //printf("in pipe\n");
    int fd[2];
    int pid;
    int last_in = 0;
    while (pipe_token_test != NULL) {
        //printf("%s\n",pipe_token_test);
        pipe(fd);
        if ((pid = fork()) == -1) {
            perror("fork");
            exit(1);
        }
        else if (pid == 0) {
            dup2(last_in, 0);
            char* test = pipe_token_test;
            //while(*test != '|' || *test != '\0') test++;
            if (ispipe != 0) {
                dup2(fd[1], 1);
            }
            if(is_buildin(pipe_token_test)  == 1){
                processing(pipe_token_test);
            }
            else{
                int i = 1;
                char *temp_token, *temp_save;
                char** temp_char = (char**)malloc(10*sizeof(char*));
                temp_token = strtok_r(pipe_token_test, " ", &temp_save);
                temp_char[0] = (char*)malloc(100);
                strcpy(temp_char[0], temp_token);
                temp_token = strtok_r(NULL, " ", &temp_save);
                for(; i < 10 && temp_token!= NULL; i++){
                    temp_char[i] = (char*)malloc(100);
                    strcpy(temp_char[i], temp_token);
                    //printf("%s\n",temp_token);
                    temp_token = strtok_r(NULL, " ", &temp_save);
                }
                temp_char[i] = NULL;
                close(fd[0]);
                execvp(temp_char[0], temp_char);
                i--;
                for(;i>=0; i--){
                    free(temp_char[i]);
                }
                free(temp_char);
            }
            //printf("child exit\n");
            exit(1);
        }
        else {
            wait(NULL);
            close(fd[1]);
            last_in = fd[0];
            //printf("parent\n");
            ispipe--;
            pipe_token_test = strtok_r(NULL, "|", &pipe_token_test_save);
        }
    }
}
int main(){
	printf("Welcom to E94091063's simple shell!\n"
			"\nType (help) to see all functions.\n");//hello message
    dup2(1, 3);
    dup2(2, 4);
    char cwd[100];
    getcwd(cwd, 100);
    strcat(cwd, "/");
    strcpy(temp_file[0], cwd);
    strcat(temp_file[0], "tempfile0.txt");
    strcpy(temp_file[1], cwd);
    strcat(temp_file[1], "tempfile1.txt");
    while(1){
        printf("%s", prompt_symbol);
        read_line(command);
        node *node1 = (node *) malloc(sizeof(node));//initiallize a node for queue
        temp = first;// free first node if needed
        node1->next = NULL;
        if(command[0] == '\0'){//fix empty command
            strcpy(node1->message, " ");
        }
        else{
            strcpy(node1->message, command);//copy command to queue node
        }
        if(first == NULL){// if empty queue
            node1->num = 0;
            first = node1;
            last = first;
        }
        else if(last->num < 16){//if queue isn't full
            node1->num = last->num + 1;
            last->next = node1;
            last = node1;
        }
        else{//if queue is full
            node1->num = last->num + 1;
            last->next = node1;
            last = node1;
            //free(temp);
        }
        replay_fail = 0;
        ispipe = 0;
        seq = 0;
        isout = 0;
        char *check = command; 
        strcpy(temp_file[0], cwd);
        strcat(temp_file[0], "tempfile0.txt");
        while(*check!='\0'){
            if(*check == '|'){
                ispipe++;
                /*
                int fd = open(temp_file[0], O_WRONLY | O_CREAT | O_TRUNC, S_IRWXU | S_IRWXG | S_IRWXO);// | O_APPEND);
                dup2(fd, 1);
                dup2(fd, 2);
                close(fd);
                break;
                */
            }
            check++;
        }
        char *check2 = command;
        int have_replay = 0;
        while(strlen(check2) >= 6){
            if(strncmp(check2, "replay", 6) == 0){
                have_replay = 1;
                break;
            }
            check2++;
        }
        if(ispipe>0 && have_replay == 0){
            for(int i = 0; i < strlen(command); i++){
                if(command[i] == '<') command[i] = ' ';
            }
            char *out_tok, *out_save;
            int is_back=0;
            if(command[strlen(command)-1] == '&') {
                is_back = 1;
                command[strlen(command)-2] = '\0';
            }
            out_tok = strtok_r(command, ">", &out_save);
            out_tok = strtok_r(NULL, ">", &out_save);
            if(out_tok != NULL){
                //printf("out\n");
                while(*out_tok == ' ') out_tok++;
                char cwd[100];
                getcwd(cwd, 100);
                strcat(cwd, "/");
                strcpy(temp_file[0], cwd);
                strcat(temp_file[0], out_tok);
                strcpy(out_file, out_tok);
                int fd = open(temp_file[0], O_WRONLY | O_CREAT | O_TRUNC, S_IRWXU | S_IRWXG | S_IRWXO);
                dup2(fd, 1);
                dup2(fd, 2);
                close(fd);
                isout = 1;
            }
            if(is_back==1){
                //command[strlen(command)-1] = '\0';
                int pid = fork();
                int status;
                if(pid < 0){
                    exit(1);
                }
                else if(pid == 0){
                    int pid2 = fork();
                    if(pid2 < 0){
                        exit(1);
                    }
                    else if(pid2 == 0){
                        printf("[Pid]: %d\n", getpid());
                        usleep(10);
                        pipel(command);
                        exit(0);
                    }
                    else{
                        //int p = waitpid(-1, NULL, WNOHANG);
                        //printf("[Pid]: %d\n", p);
                        exit(0);
                    }
                }
                else{
                    //printf("[Pid]: %d\n", getpid());
                    //int p = waitpid(-1, &status, WNOHANG);
                    int p = wait(NULL);
                    ispipe = 0;
                    //printf("[Pid]: %d\n", p);
                    //return;
                }
            }
            else
                pipel(command);
        }
        else{
            if(ispipe > 0){
                int fd = open(temp_file[0], O_WRONLY | O_CREAT | O_TRUNC, S_IRWXU | S_IRWXG | S_IRWXO);// | O_APPEND);
                dup2(fd, 1);
                dup2(fd, 2);
                close(fd);
            }
            processing(command);
        }
        //printf("pipe exit\n");
        if(replay_fail == 0 && last != NULL && last->num >= 16){
            first = first->next;
            free(temp);//free node
        }
        dup2(3,1);
        dup2(4,2);
        if(ispipe == 1 && isout == 0){
            FILE *fp;
            char *ch, *ah;
            ch =(char *) malloc(sizeof(char) * 100);
            fp = fopen(temp_file[0],"r");
            if(fp == NULL)
            {
                printf("Open filefailure!");
                exit(1);
            }
            else
            {
                while(fscanf(fp, "%c", ch) != EOF)
                {
                    printf("%s", ch);
                }
            }
            free(ch);
            fclose(fp);
        }
    }
	return 0;
}
