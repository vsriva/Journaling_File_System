//RecoveryManager
#include<stdio.h>
#include<stdlib.h>
#include<time.h>
#include<string.h>
#include <errno.h>
#include "cellstorage.h"

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

int search_directory(char a[30]){
	DIR* dir = opendir(a);
	if (dir)
	{
    /* Directory exists. */
    	closedir(dir);
		return 0;
	}
	else if (ENOENT == errno)
	{
    /* Directory does not exist. */
		return 1;
	}
	else
	{
    /* opendir() failed for some other reason. */
		return 2;
	}
}

int hard_recover(){
	return system("cp -r ./backup/cells ./cells");
	 
}

int check_and_fix(entry *c){
	char *value;
	int ret=0;
	char cell[50];
	char type[]=".txt";
	sprintf(cell,"%d",c->cell_no);
	strcat(cell,type);
	value=read(c->f_id,c->cell_no);
	printf("\nOld Value:%s Trying:%s",value,c->value);
	
	int counter=1;
	do{
		if (value!=NULL){
			if (strcmp(value,c->value)==0){
			printf("\nValue Matched");
				return 0;
			}
		}
		printf("\nFix Attempt:%d",counter);
		ret=check_for_block(cell);//Returns 1 if found
		if(ret ==1){
			ret=write(c->f_id,c->cell_no,c->value);
		}//Considering the situation when block is not allocated.
		else{
			printf("\nBuilding cell");
			alloc_overide(c->f_id,c->cell_no);
			ret=write(c->f_id,c->cell_no,c->value);
		}
		value=read(c->f_id,c->cell_no);
		printf("\nNew Value:%s",value);
		counter++;
	}while((strcmp(value,c->value)!=0)&&(counter<2));

	
	return 0;
}
	

int check_for_error(int rec){
//Acquire locks(to be done in multithreading)
	int recovery_policy=rec;// Recovery policy 0:To complete the Commit or 1:To Restore the previous state before commit
	FILE *fp;
	char log_time[100];
	int ret=0,counter=0;
	strcpy(log_time,get_time());
	char location[100]="./log/";
	char logfile[20]="log.txt";
	char recover_type[30];
	char commit[30]="COMMIT";
	char recover[30]="RECOVER";
	strcat(location,logfile);
    entry *log=malloc(sizeof(entry));
	char type[50]; 
	char value[512];
	
	if(search_directory("cells")==1){
		printf("\nRecovery_Manager-CHECK_FOR_ERROR-Status:Hard Error Detected");
		if(search_directory("backup")==0){
			printf("\nRecovery_Manager-CHECK_FOR_ERROR-Status:Trying to recover Hard Error");
			if (hard_recover()==0){
				printf("\nRecovery_Manager-CHECK_FOR_ERROR-Status:Hard Error Recovery Successful");
			}
			else{
				printf("\nRecovery_Manager-CHECK_FOR_ERROR-Status:Unable to recover from Hard Error. Continuing from logs");
			}

		}
		else{
			printf("\nRecovery_Manager-CHECK_FOR_ERROR-Status:Unable to recover from Hard Error. Continuing from logs");
		}
	}
			
	
	
	if(recovery_policy==0){
		strcpy(recover_type,commit);
	}
	else{
		strcpy(recover_type,recover);
	}
	
	
	log->type=type;
	log->value=value;
	fp=fopen(location,"r");
    if (fp != NULL) {
		while(!feof(fp)){
//    		fread(log, 1, sizeof(entry), fp);
			fscanf(fp,"%s %d %d %d %s",	log->type,&(log->p_id), &(log->f_id),&(log->cell_no), log->value);
//		fscanf(fp,"%s %d %d %d %s",type,&(p_id), &(f_id),&(cell_no), value);
			if(strcmp(log->type,recover_type)==0){
				printf("\nRecovery_Manager-CHECK_FOR_ERROR-Status:Checking-> Type:%s, P_ID:%d, F_ID:%d, Cell_no:%d, Value:%s",log->type,log->p_id,log->f_id,log->cell_no,log->value);
				ret=check_and_fix(log);
				if(ret != 0){
					printf("\nRecovery_Manager-CHECK_FOR_ERROR-Error:Unable to Recover: P_ID:%d, F_ID:%d, Cell_no:%d",log->p_id,log->f_id,log->cell_no);
				}
			}
			else{
				//printf("\n%s",log->type);
			}
			
		}
    	fclose(fp);
	}
	else{
        printf("\nRecovery_Manager-CHECK_FOR_ERROR-Error :Cannot read log file");
		free(log);
        return 2;
    }
	//printf("\nValues Consistent");
	printf("\nRecovery_Manager-CHECK_FOR_ERROR-Status:Saving Checkpoint");
	checkpoint();
	free(log);
//Release locks  
	return 0;
}
