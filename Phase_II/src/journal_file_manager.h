#include<stdio.h>
#include "cellstorage.h"
#include "storageunits.h"
#include <stdlib.h>
#include <pthread.h>

pthread_mutex_t temp_file_lock;
pthread_mutex_t cell_file_lock;
//Have to change the return type and arguments so that we can return something.
int read_count=0;

storage *NEW_ACTION(storage *a){
	int file_id=a->file_id;
	int proc_id=0;
	int ret=0;
	FILE *fp=NULL;
	pthread_mutex_lock(&temp_file_lock);
	fp=fopen(set_file,"r");
	if (fp!=NULL){
		fscanf(fp,"%d",&proc_id);
		fclose(fp);
	}
	pthread_mutex_unlock(&temp_file_lock);
	
	int current_id=proc_id;
	a->process_id=current_id;
	proc_id ++;
	pthread_mutex_lock(&cell_file_lock);
	if (a->alloc==1){
		ret=allocate(file_id);
	}
	pthread_mutex_unlock(&cell_file_lock);


	if(ret==-1){
		return NULL;
	}
	else{
		a->cell_no=ret;
	}
	pthread_mutex_lock(&temp_file_lock);

	fp=fopen(set_file,"w");
	if (fp!=NULL){
		fprintf(fp,"%d",proc_id);
		fclose(fp);
		pthread_mutex_unlock(&temp_file_lock);

		printf("\nJFM-NEW_ACTION-STATUS (File_ID:%d, Cell No:%d):Process Creation Completed",file_id,ret);
	}
	else{
		pthread_mutex_unlock(&temp_file_lock);
		printf("\nJFM-NEW_ACTION-ERROR (File_ID:%d, Cell No:%d):Process Creation Failed",file_id,ret);
	}
	return a;
	//pthread_exit(a);
//	return current_id;
}

storage *READ_CURRENT_VALUE(storage *a){
	int file_id=a->file_id;
	int process_id=a->process_id;
	int cell_no=a->cell_no;
	char *value=NULL;
	pthread_mutex_lock(&cell_file_lock);
	a->cell_data=read(file_id,cell_no);
	read_count++;
	printf("%d",read_count);
	pthread_mutex_unlock(&cell_file_lock);
	printf("\n-----------------READCURR:%s",a->cell_data);
	return a;
	//pthread_exit(a);
	//return value;
}
void *COMMIT(storage *a){
	int file_id=a->file_id;
	int process_id=a->process_id;
	int value=0;
	char *commit_data=NULL;
	int ret=0; 
	int i=0;
	int count=0;
	storage *data=(storage *)calloc(100,sizeof(storage));
	pthread_mutex_lock(&cell_file_lock);

	
	if (data==NULL){
		printf("\nNothing allocated");
	}
	else{
		//printf("\nAllocation fine");
	}
	FILE *fp;
	//printf("\nInside Commit");	
	pthread_mutex_lock(&temp_file_lock);
	fp=fopen(buffer_file,"r");
	
	if (fp!=NULL){
	    fread(data, 100, sizeof(storage), fp);
		fclose(fp);
		//printf("\nFile Read done");
	}
	else{
		printf("\nTemp file doesn't exist");
	}
	//pthread_mutex_unlock(&temp_file_lock);

	//printf("\nCheckpoint");
	for (i=0;i<100;i++){
		if((data[i].valid==1)&&(data[i].process_id==process_id)&&(data[i].file_id==file_id)){
			commit_data=data[i].cell_data;
			printf("\n%d %d %s",file_id,data[i].cell_no,commit_data);
	//printf("\nCheckpoint");

			value=write(file_id,data[i].cell_no,commit_data);
	
			data[i].valid=0;
				
			//printf("\nJFM-COMMIT-STATUS (File_ID:%d):Commit Completed",file_id);
			count++;
		}
	}
	
	if(count>=1){
		printf("\nSomething to commit was there");
	}
	//pthread_mutex_lock(&temp_file_lock);

	fp=fopen(buffer_file,"w");
	
	if (fp!=NULL){
	    fwrite(data, 100, sizeof(storage), fp);
		fclose(fp);

		printf("\nJFM-COMMIT-STATUS (File_ID:%d):Commit Completed",file_id);
//		free(data);
		pthread_mutex_unlock(&temp_file_lock);
		pthread_mutex_unlock(&cell_file_lock);
		ret=0;
		//printf("\nLeaving Commit");
//		pthread_exit(&ret);
		return &ret;
	}
	else{
		printf("\nJFM-COMMIT-ERROR (File_ID:%d):Commit Failed",file_id);
		free(data);
		pthread_mutex_unlock(&temp_file_lock);
		pthread_mutex_unlock(&cell_file_lock);
		ret=1;
		//pthread_exit(&ret);
		return &ret;
	}
	

}
void *WRITE_NEW_VALUE(storage *a){
	int file_id=a->file_id;
	int process_id=a->process_id;
	int cell_no=a->cell_no;
	char *cell_data=a->cell_data;
	int i=0;
	int ret=0;
	storage *data=(storage *)calloc(100,sizeof(storage));
	FILE *fp;
	pthread_mutex_lock(&temp_file_lock);
	
	fp=fopen(buffer_file,"r");
	
	if (fp!=NULL){
	    fread(data, 100, sizeof(storage), fp);
		fclose(fp);
	}
	//pthread_mutex_unlock(&temp_file_lock);

	for (i=0;i<100;i++){
		if(data[i].valid != 1){
			data[i].process_id=process_id;
			data[i].file_id=file_id;
			data[i].cell_no=cell_no;
			data[i].cell_data=cell_data;
			data[i].valid=1;
			break;
		}
	}
	//pthread_mutex_lock(&temp_file_lock);

	fp=fopen(buffer_file,"w");
	
	if (fp!=NULL){
	    fwrite(data, 100, sizeof(storage), fp);
		fclose(fp);
		//printf("\nTry");
		printf("\nJFM-WRITE_NEW_VALUE-STATUS (File_ID:%d):Write Completed",file_id);
		free(data);
		pthread_mutex_unlock(&temp_file_lock);
		ret=0;
		//printf("\nLeaving write");
		//pthread_exit(&ret);
		return &ret;
	}
	else{
		printf("\nJFM-WRITE_NEW_VALUE-ERROR (File_ID:%d):Write Failed",file_id);
		free(data);
		pthread_mutex_unlock(&temp_file_lock);
		ret=1;
		//pthread_exit(&ret);
		return &ret;
	}
}
int *ABORT(int process_id,int file_id){
	int value;
	char *commit_data=NULL;
	int i=0,ret=0;
	int count=0;
	storage *data=(storage *)calloc(100,sizeof(storage));
	FILE *fp;
	pthread_mutex_lock(&temp_file_lock);

	fp=fopen(buffer_file,"r");
	
	if (fp!=NULL){
	    fread(data, 100, sizeof(storage), fp);
		fclose(fp);
	}
	//pthread_mutex_unlock(&temp_file_lock);

	for (i=0;i<100;i++){
		if((data[i].valid==1)&&(data[i].process_id==process_id)&&(data[i].file_id==file_id)){
			data[i].valid=0;
			count++;
		}
	}
	if(count>=1){
		printf("\nAborted");
	}
	else{
		printf("\nValue either commited or unavailable");
	}
	//pthread_mutex_lock(&temp_file_lock);

	fp=fopen(buffer_file,"w");
	
	if (fp!=NULL){
	    fwrite(data, 100, sizeof(storage), fp);
		fclose(fp);
		printf("\nJFM-ABORT-STATUS (File_ID:%d):Abort Completed",file_id);
		free(data);
		pthread_mutex_unlock(&temp_file_lock);
		ret=0;
//		pthread_exit(&ret);
		return &ret;
	}
	else{
		printf("\nJFM-ABORT-ERROR (File_ID:%d):Abort Failed",file_id);
		free(data);
		pthread_mutex_unlock(&temp_file_lock);
		ret=1;
//		pthread_exit(&ret);
		return &ret;
	}
	
}

int cleanup(){
	int ret=0;
	ret=remove("Temp.txt");
	ret=remove("set.txt");
	system("rm -r ./cells/*");
	return 0;
}
