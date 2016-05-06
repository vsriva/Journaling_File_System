//In this program we are trying to introduce race condition while trying to write a single cell
//by two different process. The final result is totally random and difficult to say which value will be 
//written at the end of program.

//First Process trying to write "abcdefghijklmn" to Cell No:0 with File_ID=0

#include "journal_file_manager.h"

void *process_1(storage *mod){
	
	int *ret=NULL;
	printf("\n---------------------PROCESS 1 STARTED------------------");
	if(mod->cell_data==NULL){
		printf("\nRead Error");
	}
	ret=WRITE_NEW_VALUE(mod);
	ret=COMMIT(mod);
	if(*ret==1){
		printf("\nCommit Error");
	}
	mod=READ_CURRENT_VALUE(mod);
	if(mod->cell_data==NULL){
		printf("\nRead Error");
	}
	printf("\n---------------------PROCESS 1 FINISHED------------------");
	pthread_exit(NULL);
}

//Second Process trying to write "ABCDEFGHIJKLMN" to Cell No:0 with File_ID=0
void *process_2(storage *mod){
	
	int *ret=NULL;
	printf("\n---------------------PROCESS 2 STARTED------------------");
	if(mod->cell_data==NULL){
		printf("\nRead Error");
	}
	ret=WRITE_NEW_VALUE(mod);
	ret=COMMIT(mod);
	if(*ret==1){
		printf("\nCommit Error");
	}
	mod=READ_CURRENT_VALUE(mod);
	if(mod->cell_data==NULL){
		printf("\nRead Error");
	}
	printf("\n---------------------PROCESS 2 FINISHED------------------");
	pthread_exit(NULL);
}


#define NUM_THREADS 2
#define BLOCKS 1
int main(){
	pthread_t thr[NUM_THREADS];
	char val[512]="abcdefghijklmn";
	char val2[512]="ABCDEFGHIJKLMN";
	storage *a=(storage *)malloc(sizeof(storage));
	storage *c=(storage *)malloc(sizeof(storage));

	cleanup();
	//Allocating Cell_No:0 for File ID:0
	allocate(0);
	
	//Allocating Storage parameters to write
	a->process_id=0;
	a->file_id=0;
	a->cell_no=0;
	a->cell_data=val;
	a->valid=0;
	a->alloc=1;

	//Allocating Storage parameters to write
	c->process_id=0;
	c->file_id=0;
	c->cell_no=0;
	c->cell_data=val2;
	c->valid=0;
	c->alloc=1;
	

	pthread_mutex_init(&temp_file_lock, NULL);
	pthread_mutex_init(&cell_file_lock, NULL);

	printf("\nThread init");
	printf("\nThread create");
	pthread_create(&thr[0],NULL,(void *)process_1,a);
	pthread_create(&thr[1],NULL,(void *)process_2,c);
	

	printf("\nThread started");
	pthread_join(thr[0], NULL);
	pthread_join(thr[1], NULL);

	printf("\nThread finished");

	
	pthread_mutex_destroy(&temp_file_lock);
    pthread_mutex_destroy(&cell_file_lock);
	printf("\nAddress = %p Address = %p\n", a,c);

	free(a);
	free(c);
	//log_reader();
	check_for_error(0);//Check and repair with 'ALL' in 'All or nothing' atomicity

	printf("\nMemory freed\n");
	printf("\nProgram complete\n");
	return 0;
}

