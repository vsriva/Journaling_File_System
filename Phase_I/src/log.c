//NOT IMPLEMENTED YET
#include<stdio.h>
#include<time.h>
#include<string.h>
char path[]="./log/";
char times[100];
char *get_time(){
	time_t t;
	time(&t);
	strcpy(times,ctime(&t));
	strtok(times, "\n");
	return times;
}

int create_log(char log_file[100]){
	FILE *fp;
	char filename[100];
	char log[100];
	char type[10]=".txt";
	strcpy(filename,log_file);
	strcat(filename,type);
	strcpy(log,path);
	strcat(log,filename);
	char log_time[100];
	strcpy(log_time,get_time());
	fp=fopen(log,"w");
	printf("\nTime:%s",get_time());
    if (fp != NULL) {
		fprintf(fp,"\nTime:%s Beginning logging...",log_time);
		fclose(fp);
    }
	else{
		return 1;
	}
	return 0;
}

int append_log(char log_file[100],int p_id,int f_id, int cell_id, int status, char data[]){
	FILE *fp;
	char filename[100];
	char log[100];
	char type[10]=".txt";
	strcpy(filename,log_file);
	strcat(filename,type);
	strcpy(log,path);
	strcat(log,filename);
	char log_time[100];
	strcpy(log_time,get_time());
	fp=fopen(log,"a");
    if (fp != NULL) {
    fprintf(fp,"\nTime:%s P_ID:%d F_ID:%d Cell_ID:%d Status:%d Data:%s",log_time,p_id,f_id,cell_id,status,data);
//    fprintf(fp,"Time:%s Beginning logging...",log_time);
    fclose(fp);
    }
	else{
		return 1;
	}
	return 0;
}
	
int recover_from_log(char log_file[100]){
	//To do
}

int main(){
	int i=0,j=0;
	printf("Creating log");
	create_log("JFS");
	for (i=0;i<100;i++){
		for(j=0;j<100;j++){
			append_log("JFS",i,j, i, 3, "Hello");
			append_log("JFS",i,j, i, 3, "Hello");
			append_log("JFS",i,j, i, 3, "Hello");
		}
	}
	return 0;
}
			
