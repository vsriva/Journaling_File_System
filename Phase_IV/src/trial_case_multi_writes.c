//In this test case we are trying to write 10 different files each having 2 blocks(or cells) simultaneously.
//This test case proves the ability of the program to handle multithreaded environment.
//Note:There could be problem with the values in the first run of the program, so if some incorrect value
//does appear please rerun the program. I believe its due slow I/O on HD and gets fixed due to caching and 
//file brought up in RAM.
//This test case can be run many times without fail.

#include "journal_file_manager.h"

void *process_1(storage *mod){
	
	int *ret=NULL;

	mod=NEW_ACTION(mod);
	ret=WRITE_NEW_VALUE(mod);
	ret=COMMIT(mod);
	if(*ret==1){
		printf("\nCommit Error");
	}
	mod=READ_CURRENT_VALUE(mod);
	if(mod->cell_data==NULL){
		printf("\nRead Error");
	}
	pthread_exit(NULL);
}


#define NUM_THREADS 4
#define BLOCKS 20
int main(){
	int i=0,j=0;
	pthread_t thr[NUM_THREADS];
	int b=NUM_THREADS*BLOCKS;
	char val[512]="abcdefghijklmn";
	char val2[512]="ABCDEFGHIJKLMN";
	storage *a=(storage *)calloc(b,sizeof(storage));

	char **value_set1;
	cleanup();

	//Store the values in array of strings to be passed to different threads
	value_set1 = (char **)malloc(b * sizeof(char*));
	for (i = 0; i < b; i++){
    	value_set1[i] =(char *) malloc(100 * sizeof(char));
		if(i%2){
			strcpy(value_set1[i],val);
		}
		else{
			strcpy(value_set1[i],val2);
		}
			
		printf("\nString:%d Value:%s",i,value_set1[i]);
	}

	//Mutex locks intiallized
	pthread_mutex_init(&temp_file_lock, NULL);
	pthread_mutex_init(&cell_file_lock, NULL);


	for(i=0;i<NUM_THREADS;i++){
		for(j=0;j<BLOCKS;j++){
			a[i*BLOCKS+j].file_id=i;
			a[i*BLOCKS+j].alloc=1;
			
			a[i*BLOCKS+j].cell_data=value_set1[i*BLOCKS+j];
			
		}
	}

	printf("\nThread init");
	printf("\nThread create");
	for(i=0;i<NUM_THREADS;i++){
		for(j=0;j<BLOCKS;j++){
			pthread_create(&thr[i*BLOCKS+j],NULL,(void *)process_1,&a[i*BLOCKS+j]);
		}
	}
	printf("\nThread started");
	for(i=0;i<NUM_THREADS;i++){
		for(j=0;j<BLOCKS;j++){
			pthread_join(thr[i*BLOCKS+j], NULL);
		}
	}
	printf("\nThread finished");

	//Mutex locks destroyed
	pthread_mutex_destroy(&temp_file_lock);
    pthread_mutex_destroy(&cell_file_lock);
	//log_reader();
	check_for_error(0);//Check and repair with 'ALL' in 'All or nothing' atomicity

	//Heap memory freed
	for (i = 0; i < b; i++){
    	free(value_set1[i]);
	}
	free(value_set1);

	free(a);
	cleanup();
	printf("\nMemory freed\n");
	printf("\nProgram complete\n");
	return 0;
}

