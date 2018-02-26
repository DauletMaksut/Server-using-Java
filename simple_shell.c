
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>

#define CMDLEN 400

int main(int argc, char *argv[])
{
	if(argc !=2){
		printf(" Program needs name of file (((");
		exit(-1);
	}
	// Next two for fork
	int pid;
	int status;
	// Number o inpudt
	int i = 1;
	// & checker
	int k = 0;
	// Parent fork need next three
	int fd;
	int fd_another;
	char c;
	// read line of comand
	char command[CMDLEN];
	char copy[CMDLEN]; // COPY OF INITIAL COMMAND
	char copy_one[CMDLEN]; // COPY OF INITIAL COMMAND
	// array of words whitch were in command
	char *pass[256];
	// Token reader
	char *token;
	//Open file backup file 
	fd = open(argv[1], O_CREAT);
	close(fd);
	// Next three needs for checking and executing ">>"
	int two_direction = 0;
	int capture;
	char *tname;
	// Next three needs for checking and executing ">"
	int l_direction = 0;
	int l_pos;
	char *lname;
	// Next three needs for checking and executing ">"
	int r_direction = 0;
	int r_pos;
	char *rname;
	// Divide commands into little commands
	char *sub_command[10];
	int  pipe_num = 0;
	int traveller = 0; 
	int controller = 0;
	char *ty;

	for (;;)
	{
		// Reuse array of command words
		for(int j = 0; j < i; j++){
				pass[j] = NULL;
			}
		// BEcause of for loop next 5 variable must be initialized again
		k = 0;
		i = 1;
		two_direction = 0;
		l_direction = 0;
		r_direction = 0;

		if(controller == 0){
		printf( "Please enter a command:   " );
		fgets( command, CMDLEN, stdin);
		command[strlen(command)-1] = '\0';
		//To save initial command in history file
		fd = open(argv[1], O_RDWR);
		lseek(fd, 0 , SEEK_END);
		write(fd, "Please enter a command:   ", strlen("Please enter a command:   "));
		write(fd, command, strlen(command));
		write(fd, "\n", strlen("\n"));
		close(fd);
		strcpy(copy, command);
		strcpy(copy_one, command);


		ty = strtok(copy, "|");
		if(strcmp(ty,copy_one) != 0){
			controller = 1;
		}
		if(controller == 1){
		do{
		sub_command[pipe_num] = malloc(strlen(ty));
		sub_command[pipe_num] = ty;
		pipe_num++;
		ty = strtok(NULL, "|");
		controller = 2;
		}while(ty!=NULL);
		}
		if(controller == 2){
			//controller = 1;
			fd = open("intmp.txt", O_CREAT, 0644);
			close(fd);
			fd = open("outtmp.txt", O_CREAT, 0644);
			close(fd);
			continue;
		}

		//printf("%i\n", pipe_num);
		//printf(" MUST BE HERE NIATCH %s\n", sub_command[0]);
		}else{
		if(traveller == pipe_num-1){
			controller = 0;
			if(pipe_num % 2 == 0) {
				strcpy(command, sub_command[traveller]);
				strcat(command, " < outtmp.txt\0");
			}else{
				strcpy(command, sub_command[traveller]);
				strcat(command," < intmp.txt\0");
			}
			traveller = 0;
			pipe_num = 0;
		}else{
			if(traveller % 2 == 0){
		strcpy(command, sub_command[traveller]);
		strcat(command, " < intmp.txt > outtmp.txt\0");
		traveller++;
		}else{
		strcpy(command, sub_command[traveller]);
		strcat(command, " < outtmp.txt > intmp.txt\0");
		traveller++;
		}
		}
	}

		// Divides to tokens and check for quit command 
		token = strtok(command," ");
		if (strcmp(token, "quit") == 0 ){
			exit(0);
		}
		pass[0] = malloc(strlen(token));
		pass[0] = token;
		token = strtok(NULL, " ");
		while(token!=NULL){
			pass[i] = malloc(strlen(token));
			pass[i] = token;
			if(strcmp(token, ">>")== 0){
				two_direction = 1;
				capture = i + 1;
			}
			if(strcmp(token, ">") == 0){
				r_direction = 1;
				r_pos = i + 1;
			}
			if(strcmp(token, "<") == 0){
				l_direction = 1;
				l_pos = i + 1;
			}
			token = strtok(NULL, " ");
			i++;
		}

		// Check for "<", "<", ">>" scenario
		if(two_direction == 1 && l_direction == 1){ // >>, < combo
			if(capture > l_direction){
				pass[capture-1] = NULL; 
				tname = malloc(strlen(pass[capture]));
				tname = pass[capture];
				if(tname==NULL){
					printf("INCORRECT INPUT CRASHED PROGRAMM" );
					exit(-1);
				}
				fd = open(tname, O_CREAT | O_APPEND | O_RDWR, 0644);
				if(fd<0){
					printf("File mentioned after \">>\" makes problem");
					exit(-1);
				}
				pass[capture] = NULL;
				close(fd);
				for(int g = capture-1; g < i; g++){
					pass[g] = NULL;
					if(pass[g+2]!= NULL){
						pass[g] = malloc(strlen(pass[g+2]));
					}
					pass[g] = pass[g+2];
				}
				i = i - 2;

				pass[l_pos-1] = NULL;
				lname = malloc(strlen(pass[l_pos]));
				lname = pass[l_pos];
				if(lname == NULL){
					printf("INCORRECT INPUT CRASHED PROGRAMM" );
					exit(-1);
				} 
				fd = open(lname, O_RDWR);
				if(fd < 0){
					printf("File mentioned after \"<\" makes problem");
					exit(-1);
				}
				pass[l_pos] = NULL;
				close(fd);
				for(int g = l_pos-1; g< i; g++){
					pass[g] = NULL;
					if(pass[g+2]!= NULL){
						pass[g] = malloc(strlen(pass[g+2]));
					}
					pass[g] = pass[g+2];
				}
				i = i - 2;
			}else{
				pass[l_pos-1] = NULL;
				lname = malloc(strlen(pass[l_pos]));
				lname = pass[l_pos];
				if(lname == NULL){
					printf("INCORRECT INPUT CRASHED PROGRAMM" );
					exit(-1);
				} 
				fd = open(lname, O_RDWR);
				if(fd < 0){
					printf("File mentioned after \"<\" makes problem");
					exit(-1);
				}
				pass[l_pos] = NULL;
				close(fd);
				for(int g = l_pos-1; g< i; g++){
					pass[g] = NULL;
					if(pass[g+2]!= NULL){
						pass[g] = malloc(strlen(pass[g+2]));
					}
					pass[g] = pass[g+2];
				}
				i = i - 2;

				pass[capture-1] = NULL; 
				tname = malloc(strlen(pass[capture]));	
				tname = pass[capture];
				if(tname==NULL){
					printf("INCORRECT INPUT CRASHED PROGRAMM" );
					exit(-1);
				}
				fd = open(tname,O_CREAT| O_APPEND | O_RDWR, 0644);
				if(fd<0){
					printf("File mentioned after \">>\" makes problem");
					exit(-1);
				}
				pass[capture] = NULL;
				close(fd);
				for(int g = capture-1; g < i; g++){
					pass[g] = NULL;
					if(pass[g+2]!= NULL){
						pass[g] = malloc(strlen(pass[g+2]));
					}
					pass[g] = pass[g+2];
				}
				i = i - 2;
			}
		}else if(r_direction == 1 && l_direction == 1){ // <, > combo
			if(r_pos > l_pos){
				pass[r_pos-1] = NULL; 
				rname = malloc(strlen(pass[r_pos]));
				rname = pass[r_pos];
				if(rname==NULL){
					printf("INCORRECT INPUT CRASHED PROGRAMM" );
					exit(-1);
				}
				fd = open(rname, O_CREAT| O_APPEND | O_RDWR, 0644);
				pass[r_pos] = NULL;
				if(fd < 0){
					printf("File mentioned after \">\" makes problem");
					exit(-1);
				}
				close(fd);
				for(int g = r_pos-1; g < i; g++){
					pass[g] = NULL;
					if(pass[g+2]!= NULL){
						pass[g] = malloc(strlen(pass[g+2]));
					}
					pass[g] = pass[g+2];
				}
				i = i - 2;

				pass[l_pos-1] = NULL;
				lname = malloc(strlen(pass[l_pos]));
				lname = pass[l_pos];
				if(lname == NULL){
					printf("INCORRECT INPUT CRASHED PROGRAMM" );
					exit(-1);
				} 
				fd = open(lname, O_RDWR);
				if(fd < 0){
					printf("File mentioned after \"<\" makes problem");
					exit(-1);
				}
				pass[l_pos] = NULL;
				close(fd);
				for(int g = l_pos-1; g< i; g++){
					pass[g] = NULL;
					if(pass[g+2]!= NULL){
						pass[g] = malloc(strlen(pass[g+2]));
					}
					pass[g] = pass[g+2];
				}
				i = i - 2;
			}else{
				pass[l_pos-1] = NULL;
				lname = malloc(strlen(pass[l_pos]));
				lname = pass[l_pos];
				if(lname == NULL){
					printf("INCORRECT INPUT CRASHED PROGRAMM" );
					exit(-1);
				} 
				fd = open(lname, O_RDWR);
				if(fd < 0){
					printf("File mentioned after \"<\" makes problem");
					exit(-1);
				}
				pass[l_pos] = NULL;
				close(fd);
				for(int g = l_pos-1; g< i; g++){
					pass[g] = NULL;
					if(pass[g+2]!= NULL){
						pass[g] = malloc(strlen(pass[g+2]));
					}
					pass[g] = pass[g+2];
				}
				i = i - 2;
				pass[r_pos-1] = NULL; 
				rname = malloc(strlen(pass[r_pos]));
				rname = pass[r_pos];
				if(rname==NULL){
					printf("INCORRECT INPUT CRASHED PROGRAMM" );
					exit(-1);
				}
				fd = open(rname,O_CREAT | O_RDWR, 0644);
				if(fd<0){
					printf("File mentioned after \">>\" makes problem");
					exit(-1);
				}
				pass[r_pos] = NULL;
				close(fd);
				for(int g = r_pos-1; g < i; g++){
					pass[g] = NULL;
					if(pass[g+2]!= NULL){
						pass[g] = malloc(strlen(pass[g+2]));
					}
					pass[g] = pass[g+2];
				}
				i = i - 2;
			}
		}else if(l_direction == 1){// <
				pass[l_pos-1] = NULL;
				lname = malloc(strlen(pass[l_pos]));
				lname = pass[l_pos];
				if(lname == NULL){ 
					printf("INCORRECT INPUT CRASHED PROGRAMM" );
					exit(-1);
				} 
				fd = open(lname, O_RDWR);
				if(fd < 0){
					printf("File mentioned after \"<\" makes problem");
					exit(-1);
				}
				pass[l_pos] = NULL;
				close(fd);
				for(int g = l_pos-1; g< i; g++){
					pass[g] = NULL;
					if(pass[g+2]!=NULL){
						pass[g] = malloc(strlen(pass[g+2]));
					}
					pass[g] = pass[g+2];
				}
				i = i - 2;
		}else if(r_direction == 1){// >
				pass[r_pos-1] = NULL; 
				rname = malloc(strlen(pass[r_pos]));
				rname = pass[r_pos];
				if(rname==NULL){
					printf("INCORRECT INPUT CRASHED PROGRAMM" );
					exit(-1);
				}
				fd = open(rname, O_CREAT | O_RDWR, 0644);
				if(fd<0){
					printf("File mentioned after \">>\" makes problem");
					exit(-1);
				}
				pass[r_pos] = NULL;
				close(fd);
				for(int g = r_pos-1; g < i; g++){
					pass[g] = NULL;
					if(pass[g+2]!= NULL){
						pass[g] = malloc(strlen(pass[g+2]));
					}
					pass[g] = pass[g+2];
				}
				i = i - 2;
		}else if(two_direction == 1){// >>
				pass[capture-1] = NULL; 
				tname = malloc(strlen(pass[capture]));
				tname = pass[capture];
				if(tname==NULL){
					printf("INCORRECT INPUT CRASHED PROGRAMM" );
					exit(-1);
				}
				fd = open(tname,  O_APPEND | O_RDWR |O_CREAT, 0644);
				if( fd < 0 ){
					printf("File mentioned after \">>\" makes problem");
					exit(-1);
				}
				pass[capture] = NULL;
				close(fd);
				for(int g = capture-1; g < i; g++){
					pass[g] = NULL;
					if(pass[g+2] != NULL){
						pass[g] = malloc(strlen(pass[g+2]));
					}
					pass[g] = pass[g+2];
				}
				i = i - 2;
		}
		// Only works for cd
		if(strcmp(pass[0], "cd")==0){
			if(pass[1] != NULL){
				chdir(pass[1]);
			}
		}else{
			// if k==1 then wait(NULL) will not execute
		if(strcmp(pass[i-1],"&") == 0){
			pass[i-1] = NULL;
			k = 1;
		}

		//else is child, pid!=0 is parent
		pid = fork();
		if ( pid < 0 )
		{
			printf( "Error in fork.\n" );
			exit(-1);
		}

		if ( pid != 0 ){
			if(k == 0){
				wait(NULL);
			}else{
				sleep(2);
			}
			if(l_direction == 0 && r_direction == 0 && two_direction == 0){
				fd = open("tmp.txt", O_RDWR);
				fd_another = open(argv[1], O_WRONLY);
				off_t len = lseek(fd, 0, SEEK_END);
				lseek(fd, 0, SEEK_SET);
				lseek(fd_another, 0, SEEK_END);
				for(int p = 0; p < len; p++){
					read(fd, &c, 1);
					printf("%c", c);
					write(fd_another, &c, 1);
				}
				close(fd_another);
				close(fd);
				remove("tmp.txt");
			}
							
		}else{
				
				if(two_direction==1 && l_direction == 1){// < >> or >> <
					fd_another = open(lname, O_RDWR);
					fd = open(tname, O_CREAT | O_APPEND | O_RDWR, 0644);
					lseek(fd, 0, SEEK_END);
					dup2(fd_another, 0);
					dup2(fd, 1);
				}else if(r_direction == 1 && l_direction){ //  < > or > <
					remove(rname);
					fd = open(lname, O_RDWR);
					fd_another = open(rname, O_CREAT | O_RDWR, 0644);
					dup2(fd, 0);
					dup2(fd_another, 1);
				}else if(r_direction == 1){// >
					remove(rname);
					fd = open(rname, O_CREAT |O_RDWR, 0644);
					dup2(fd, 1);
				}else if(l_direction == 1){// <
					fd = open(lname, O_RDWR, 0644);
					dup2(fd, 0);
				}else if(two_direction == 1){// >>
					fd = open(tname, O_CREAT| O_APPEND| O_RDWR);
					lseek(fd, 0, SEEK_END);
					dup2(fd, 1);
				}else{ // NO >, <, >>
					fd = open("tmp.txt", O_CREAT| O_RDWR, 0644);
					dup2(fd, 1);	
				}
				execvp( pass[0], pass);
				close(fd_another);
				close(fd);
				exit(0);
			}
		}
	}
}

