#include<stdio.h>
#include<stdlib.h>
#include<time.h>
#include<string.h>
#include "RecoveryManager.h"
/*
char path[]="./log/";
char times[100];

typedef struct{
	char *time;
	char *type;
	int p_id;
	int f_id;
	int cell_no;
	char *value;
}entry;
	
char *get_time(){
	time_t t;
	time(&t);
	strcpy(times,ctime(&t));
	strtok(times, "\n");
	return times;
}
*/
int checkpoint(){
	FILE *fp;
	char log_time[100];
	strcpy(log_time,get_time());
	char location[100]="./log/";
	char logfile[20]="log.txt";
	char type[50]="CHECK";
	int p_id=-1;
	int f_id=-1;
	int cell_no=-1;	
	char value[512]="NULL";
	strcat(location,logfile);
    
	fp=fopen(location,"w");
    if (fp != NULL) {
		fprintf(fp,"%s %d %d %d %s\n",type,p_id,f_id,cell_no,value);
    	fclose(fp);
		system("cp -rf ./cells ./backup/");
    }
    else{
        printf("\nLog-Checkpoint-Error (Time:%s):Cannot open log file",log_time);
        return 2;
    }
	return 0;
}

int logger(char type[50], int p_id, int f_id, int cell_no,char value[512]){
	FILE *fp;
	char log_time[100];
	strcpy(log_time,get_time());
	char location[100]="./log/";
	char logfile[20]="log.txt";
	strcat(location,logfile);
	fp=fopen(location,"a");
    if (fp != NULL) {
		fprintf(fp,"%s %d %d %d %s\n",type,p_id, f_id,cell_no, value);
    	fclose(fp);
    }
    else{
        printf("\nLog-Logger-Error (P_ID:%d):Cannot append log file",p_id);
        return 2;
    }
	return 0;
}
	

int log_reader(){
	FILE *fp;
	char log_time[100];
	strcpy(log_time,get_time());
	char location[100]="./log/";
	char logfile[20]="log.txt";
	strcat(location,logfile);
	//char type[20];
	char type[50]; 
	int p_id;
	int f_id;
	int cell_no;
	char value[512];
	fp=fopen(location,"r");
    if (fp != NULL) {
		while(!feof(fp)){
		fscanf(fp,"%s %d %d %d %s",type,&(p_id), &(f_id),&(cell_no), value);
		printf("\nType:%s, P_ID:%d, Value:%s",type,p_id,value);
		}
    	fclose(fp);
	}
	else{
        printf("\nLog-LOG_READER-Error :Cannot read log file");
        return 2;
    }

  
	return 0;
}
	
/*	
int recover_from_log(char log_file[100]){
	//To do
}
*/
/*
int main(){
	int i=0,j=0,k=0;
	printf("Creating log\n");
	//checkpoint();
	
	printf("\nCheckpoint Created");
//	logger("CREATE",i,j, k, "");	
	//checkpoint();
	for (i=0;i<1;i++){
		for(j=0;j<1;j++){
			for(k=0;k<1;k++){
				logger("CREATE",i,j, k, "NULL");
				logger("WRITE", i, j, k, "Hello");
				logger("COMMIT", i, j, k, "Hello");
			}
		}
	}
	check_for_error();
	printf("\nRead File:");
	log_reader();
	
	return 0;
}
*/