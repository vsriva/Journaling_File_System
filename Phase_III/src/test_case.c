#include<stdio.h>
#include "journal_file_manager.h"
//There are three test cases in the program.
int main(){
	int p_id=0;//Process_ID/P_ID. Will be generated by NEW_ACTION
	int file_id=0;//Which file to edit. In this design a file can be allocated multiple blocks/cells
	int ret=0;//To check return flags
	int cell_no=0;//The cell/block to write. Possible only if the file owns that cell/block.
	char value_data[512]="First_Value";//A data to write in cell/block
	char *read_data=NULL;//To store the return of READ_CURRENT_VALUE. Possible only if the file owns that cell/block.

//Test Case #1	
//To check functionality of Recovery Manager and Logs with checkpoints and provide fault tolerant model
//Here we are trying to first recover the cellstorage system from soft errors(data corruption,i/o errors). 
//The policy for recovery is 'All' i.e. the cell storage data is restored till last commits in the log.
//Data inconsistency fund b/w logs and cell storage data is rectified by using log data during commits.
//Then a checkpoint is created and backup image is stored for recovery from hard errors. 
//Then we procced as below
//Allocate a block to a file in cell storage system. 
//Write some string to block. Block Size limited to 1024 bytes. Can only hold 100 blocks in temp 
//Commit the changes for a particular file(All the blocks of the file will be commited in a go)
//Read the value from the block, and display the changed value.
	
	system("rm -rf ./cells/0.txt");//Forcefully removing or corrupting the cell 0. Making it inaccessible
	
	printf("\n\n-------------------------------TEST CASE 1:Recovery soft error with 'ALL' policy------------------------");
	printf("\n\nChecking for Errors and Recovering");
	check_for_error(0);//Check and repair with 'ALL' in 'All or nothing' atomicity
	cell_no=0;
	strcpy(value_data,"First_Value");
	
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

	//Second data
	cell_no=1;
	strcpy(value_data,"Second_Value");
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
	
//Test Case #2	
//To check functionality of Recovery Manager and Logs with checkpoints  and provide fault tolerant model
//Here we are trying to first recover the cellstorage system from soft errors(data corruption,i/o errors). 
//The policy for recovery is 'Nothing' i.e. the cell storage data is restored before the last commits in the log.
//Data inconsistency found b/w logs and cell storage data is rectified by using log data.
//Then a checkpoint is created and backup image is stored for recovery from hard errors. 
//Then we procced as below
//Allocate a block to a file in cell storage system. 
//Write some string to block. Block Size limited to 1024 bytes. Can only hold 100 blocks in temp 
//Commit the changes for a particular file(All the blocks of the file will be commited in a go)
//Read the value from the block, and display the changed value.
	
	system("rm -rf ./cells/0.txt");//Forcefully removing or corrupting the cell 0. Making it inaccessible
	
	printf("\n\n-------------------------------TEST CASE 2:Recovery soft error with 'Nothing' policy------------------------");
	printf("\n\nChecking for Errors and Recovering");
	check_for_error(1);//Check and repair with 'Nothing' in 'All or nothing' atomicity
	cell_no=0;
	strcpy(value_data,"First_Value");
	
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

	//Second data
	cell_no=1;
	strcpy(value_data,"Second_Value");
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

//Test Case #3	
//To check functionality of Recovery Manager and Logs with checkpoints and provide fault tolerant model
//Here we are trying to first recover the cellstorage system from hard errors(disk failure, so all local data lost). 
//The policy for recovery is 'All' i.e. the cell storage data is restored till last commits in the log.
//When directory 'Cells' is missing detects a hard failure and try to recover the cell storage from a remote copy in 'backup' directory.	
//Data inconsistency found b/w logs and recovered cell storage data (from checkpoint) is rectified by using log data during commits, we are considering that logs are remotly stored.
//Then a checkpoint is created and backup image is stored for recovery from hard errors in remote location. 
//Then we procced as below
//Allocate a block to a file in cell storage system. 
//Write some string to block. Block Size limited to 1024 bytes. Can only hold 100 blocks in temp 
//Commit the changes for a particular file(All the blocks of the file will be commited in a go)
//Read the value from the block, and display the changed value.
	
	system("rm -rf ./cells");//Forcefully removing cell directory. Depicts that after failure, storage disk has been replaced and there is no cell storage data on new disk
	printf("\n\n-------------------------------TEST CASE 3:Recovery hard error with 'ALL' policy------------------------");

	printf("\n\nChecking for Errors and Recovering");
	check_for_error(0);//Check and repair with 'ALL' in 'All or nothing' atomicity
	cell_no=0;
	strcpy(value_data,"First_Value");
	
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

	//Second data
	cell_no=1;
	strcpy(value_data,"Second_Value");
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
	
	cleanup();
	printf("\n");
	return 0;
}
