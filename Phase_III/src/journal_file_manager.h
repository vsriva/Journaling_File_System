#include<stdio.h>
//#include "cellstorage.h"
#include "log.h"
#include "storageunits.h"


int NEW_ACTION(int file_id){
	int proc_id=0;
	FILE *fp=NULL;
	fp=fopen(set_file,"r");
	if (fp!=NULL){
		fscanf(fp,"%d",&proc_id);
		fclose(fp);
	}
	int current_id=proc_id;
	proc_id ++;
	allocate(file_id);
	fp=fopen(set_file,"w");
	if (fp!=NULL){
		fprintf(fp,"%d",proc_id);
		fclose(fp);
		logger("NEW_ACTION", current_id, file_id, cell_no,"NULL");
		printf("\nJFM-NEW_ACTION-STATUS (File_ID:%d):Process Creation Completed",file_id);
	}
	else{
		printf("\nJFM-NEW_ACTION-ERROR (File_ID:%d):Process Creation Failed",file_id);
	}

	return current_id;
}
char *READ_CURRENT_VALUE(int file_id,int process_id,int cell_no){
	char *value=NULL;
	value=read(file_id,cell_no);
	logger("READ_CURRENT_VALUE", process_id, file_id, cell_no, value);
	return value;
}
int COMMIT(int file_id,int process_id){
	int value;
	char *prevdata;
	char *commit_data=NULL;
	int i=0;
	int count=0;
	storage *data=(storage *)calloc(100,sizeof(storage));
	FILE *fp;

	
	fp=fopen(buffer_file,"r");
	
	if (fp!=NULL){
	    fread(data, 100, sizeof(storage), fp);
		fclose(fp);
	}

	
	for (i=0;i<100;i++){
		if((data[i].valid==1)&&(data[i].process_id==process_id)&&(data[i].file_id==file_id)){
			commit_data=data[i].cell_data;
			printf("\n%d %d %s",file_id,data[i].cell_no,commit_data);
			prevdata=read(file_id,data[i].cell_no);
			logger("RECOVER", process_id,file_id,data[i].cell_no,prevdata);
			value=write(file_id,data[i].cell_no,commit_data);
			data[i].valid=0;
			logger("COMMIT", process_id, file_id, data[i].cell_no, commit_data);
			count++;
		}
	}
	if(count>=1){
		printf("\nSomething to commit was there");
	}
	fp=fopen(buffer_file,"w");
	
	if (fp!=NULL){
	    fwrite(data, 100, sizeof(storage), fp);
		fclose(fp);
		printf("\nJFM-COMMIT-STATUS (File_ID:%d):Commit Completed",file_id);
		free(data);
		return 0;
	}
	else{
		printf("\nJFM-COMMIT-ERROR (File_ID:%d):Commit Failed",file_id);
		free(data);
		return 1;
	}
	

}
int WRITE_NEW_VALUE(int file_id,int process_id,int cell_no,char *cell_data){
	int i=0;
	storage *data=(storage *)calloc(100,sizeof(storage));
	FILE *fp;
	
	fp=fopen(buffer_file,"r");
	
	if (fp!=NULL){
	    fread(data, 100, sizeof(storage), fp);
		fclose(fp);
	}

	for (i=0;i<100;i++){
		if(data[i].valid != 1){
			data[i].process_id=process_id;
			data[i].file_id=file_id;
			data[i].cell_no=cell_no;
			data[i].cell_data=cell_data;
			data[i].valid=1;
			logger("WRITE", process_id, file_id, cell_no, cell_data);
			break;
		}
	}
	fp=fopen(buffer_file,"w");
	
	if (fp!=NULL){
	    fwrite(data, 100, sizeof(storage), fp);
		fclose(fp);
		printf("\nJFM-WRITE_NEW_VALUE-STATUS (File_ID:%d):Write Completed",file_id);
		free(data);
		return 0;
	}
	else{
		printf("\nJFM-WRITE_NEW_VALUE-ERROR (File_ID:%d):Write Failed",file_id);
		free(data);
		return 1;
	}
}
int ABORT(int file_id,int process_id){
	int value;
	char *commit_data=NULL;
	int i=0;
	int count=0;
	storage *data=(storage *)calloc(100,sizeof(storage));
	FILE *fp;
	
	fp=fopen(buffer_file,"r");
	
	if (fp!=NULL){
	    fread(data, 100, sizeof(storage), fp);
		fclose(fp);
	}

	for (i=0;i<100;i++){
		if((data[i].valid==1)&&(data[i].process_id==process_id)&&(data[i].file_id==file_id)){
			data[i].valid=0;
			count++;
			logger("ABORT", process_id, file_id, data[i].cell_no, commit_data);
		}
	}
	if(count>=1){
		printf("\nAborted");
	}
	else{
		printf("\nValue either commited or unavailable");
	}

	fp=fopen(buffer_file,"w");
	
	if (fp!=NULL){
	    fwrite(data, 100, sizeof(storage), fp);
		fclose(fp);
		printf("\nJFM-ABORT-STATUS (File_ID:%d):Abort Completed",file_id);
		free(data);
		return 0;
	}
	else{
		printf("\nJFM-ABORT-ERROR (File_ID:%d):Abort Failed",file_id);
		free(data);
		return 1;
	}
	
	return 1;
}

int cleanup(){
	int ret=0;
	ret=remove("Temp.txt");
	ret=remove("set.txt");
}

/*
int main(){
	int p_id=0;
	int file_id=0;
	int ret=0;
	int cell_no=0;
	char value_data[100]="Old Int";
	char *read_data=NULL;

	p_id=NEW_ACTION(file_id);
	ret=WRITE_NEW_VALUE(file_id,p_id,cell_no,value_data);
	if(ret!=0){
		printf("\nWrite Error");
		return 1;
	}
	ret=COMMIT(file_id,p_id);
	if(ret==1){
		printf("\nCommit Error");
		return 1;
	}
	read_data=READ_CURRENT_VALUE(file_id,p_id,cell_no);
	if(read_data==NULL){
		printf("\nRead Error");
		return 1;
	}
	printf("\nP_ID:%d F_ID:%d VALUE:%s",p_id,file_id,read_data);

	printf("\n-------------------------------");
	p_id=NEW_ACTION(file_id);
	strcpy(value_data,"New Value");
	ret=WRITE_NEW_VALUE(file_id,p_id,cell_no,value_data);
	if(ret==1){
		printf("\nWrite Error");
		return 1;
	}
	read_data=READ_CURRENT_VALUE(file_id,p_id,cell_no);
	if(read_data==NULL){
		printf("\nRead Error");
		return 1;
	}
	
	printf("\nP_ID:%d F_ID:%d VALUE:%s Tried:%s",p_id,file_id,read_data,value_data);
	cleanup();
	printf("\n");
	return 0;
}
*/
