#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main(int argc, char ** argv, char **envp){

    while(1){

        printf("lab1> ");
        char input[20];
        scanf("%s", input);
        printf("Parent Process %d\n", getpid());


        if (strcmp(input, "exit") == 0){
            exit(0);
        }

        else if (strcmp(input, "printid") == 0){
            printf("The ID of the current process is %d\n", getpid());
        }

        else if (strcmp(input, "greet") == 0){
            printf("Hello\n");
        }
        
        else{
            char bin[11] = "/bin/";
            strcat(bin, input);
            int child_status;
            char *args[] = {bin, NULL};

            int pid = fork();
            if (pid == 0){

                printf("Child process %d will execute the command %s\n", getpid(), input);

                execve(args[0], args, NULL);
                printf("Command Not Found!\n");
                exit(0);
            }
            wait(&child_status);
        }
    }
}
