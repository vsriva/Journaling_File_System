#include <stdio.h>
#include <sys/types.h>
#include <dirent.h>
#include <string.h>
#include <stdlib.h>

#define storage_size 65535
typedef struct{
    int fileid;
    char data[1024];
}block;

int cell_no=0;

int check_for_block(char *name){
    int len = strlen(name);
    DIR *dirp = opendir("./cells/");
    struct dirent *dp;
    while ((dp = readdir(dirp)) != NULL)
        if (!strcmp(dp->d_name, name)) {
            closedir(dirp);
            return 1;
        }
   closedir(dirp);
   return 0;
}

int is_owner(int file_id, int cells){
    char location[100]="./cells/";
    char type[]=".txt";
    int ret=0;
    char cell_id[20];
    sprintf(cell_id,"%d",cells);
    strcat(cell_id,type);
    strcat(location,cell_id);
    FILE *fp=fopen(location,"r");
    block *new_cell=malloc(sizeof(block));
    if (fp != NULL) {
    fread(new_cell, 1, sizeof(block), fp);
    fclose(fp);
    }
    else{
        //printf("\nCS-OWNER-Error (File_ID:%d):Failure to read!");
        free(new_cell);
        return 2;
    }
    if(new_cell->fileid==file_id){

    }
    else{
        //printf("\nCS-OWNER-Error (File_ID:%d):Access Denied!");
        free(new_cell);
        return 3;
    }
    free(new_cell);
    return 1;
}

int allocate(int file_id)
{
    char location[100]="cells/";
    char type[]=".txt";
    FILE *fp;
//    block new_cell;
    char cell_id[20];
    sprintf(cell_id,"%d",cell_no);
    strcat(cell_id,type);
    strcat(location,cell_id);

    if (check_for_block(cell_id)==1){
        printf("\nCS-ALLOC-Warning (File_ID:%d):Block does exist",file_id);
        return 3;
    }

    block *new_cell=malloc(sizeof(block));
    new_cell->fileid=file_id;
    //new_cell->data=NULL;
	strcpy(new_cell->data,"");
    if (cell_no>storage_size){
        free(new_cell);
        return 1;
    }
    cell_no +=1;
    fp=fopen(location,"w");

    if (fp != NULL) {
    fwrite(new_cell, 1, sizeof(block), fp);
    fclose(fp);
    }
    else{
        printf("\nCS-ALLOC-Error (File_ID:%d):Failure to allocate!",file_id);
        free(new_cell);
        return 2;
    }
    free(new_cell);
    printf("\nCS-ALLOC-STATUS (File_ID:%d):Allocation completed Successfully",file_id);
    return 0;
}


int write(int file_id, int cell, char data[]){
    char location[100]="./cells/";
    char cell_id[20];
    char type[]=".txt";
    sprintf(cell_id,"%d",cell);
    strcat(cell_id,type);
    strcat(location,cell_id);
//    block new_cell;
    if (check_for_block(cell_id)==0){
        printf("\nCS-WRITE-Error (File_ID:%d):Block doesn't exist",file_id);
        return 2;
    }
    if (is_owner(file_id, cell)!=1){
        printf("\nCS-WRITE-Error (File_ID:%d):Access Denied!",file_id);
        return 2;
    }
    block *new_cell=malloc(sizeof(block));
    new_cell->fileid=file_id;
    if (strlen(data)>=1023){
        printf("\nCS-WRITE-Error (File_ID:%d):Data out of bounds",file_id);
        free(new_cell);
        return 1;
    }
    strcpy(new_cell->data,data);
    if (check_for_block(cell_id)==0){
        printf("\nCS-WRITE-Error (File_ID:%d):Block doesn't exist",file_id);
        free(new_cell);
        return 2;
    }
    FILE *fp=fopen(location,"w");
    if (fp != NULL) {
    fwrite(new_cell, 1, sizeof(block), fp);
    fclose(fp);
    }
    else{
        printf("\nCS-WRITE-Error (File_ID:%d):Failure to write!",file_id);
        free(new_cell);
        return 3;
    }
    free(new_cell);
    printf("\nCS-WRITE-STATUS (File_ID:%d):Write completed Successfully",file_id);
    return 0;
}

char ret_data[1024];

char *read(int file_id, int cell){
    char location[100]="./cells/";
    char type[]=".txt";
    char cell_id[20];
    int ret=0;
    sprintf(cell_id,"%d",cell);
    strcat(cell_id,type);
    strcat(location,cell_id);
//    block new_cell;
    if (check_for_block(cell_id)==0){
        printf("\nCS-READ-Error (File_ID:%d):Block doesn't exist",file_id);
        return NULL;
    }
    if (is_owner(file_id, cell)!=1){
        printf("\nCS-READ-Error (File_ID:%d):Access Denied!",file_id);
        return NULL;
    }
    block *new_cell=malloc(sizeof(block));
    FILE *fp=fopen(location,"r");
    if (fp != NULL) {
    fread(new_cell, 1, sizeof(block), fp);
    fclose(fp);
    }
    else{
        printf("\nCS-READ-Error (File_ID:%d):Failure to read!",file_id);
        free(new_cell);
        return NULL;
    }
    free(new_cell);
    printf("\nCS-READ-STATUS (File_ID:%d):Read completed Successfully",file_id);
	strcpy(ret_data,new_cell->data);
    return ret_data;
}
int deallocate(int file_id, int cell){
    char location[100]="./cells/";
    char type[]=".txt";
    int ret=0;
    char cell_id[20];
    sprintf(cell_id,"%d",cell);
    strcat(cell_id,type);
    strcat(location,cell_id);

//    block new_cell;
    if (check_for_block(cell_id)==0){
        printf("\nCS-DEALLOC-Error (File_ID:%d):Block doesn't exist",file_id);
        return 2;
    }
    if (is_owner(file_id, cell)!=1){
        printf("\nCS-DEALLOC-Error (File_ID:%d):Access Denied!",file_id);
        return 3;
    }
    ret=remove(location);
    if(ret == 0){
        printf("\nCS-DEALLOC-Status (File_ID:%d):Block deleted successfully",file_id);
    }
    else{
        printf("\nCS-DEALLOC-Error (File_ID:%d): Unable to delete the block",file_id);
        return 4;
    }
    return 0;
}
/*
int main(){
    char a[1024];
//    printf("\nFile Found:%d",check_for_block("0.txt"));

    allocate(1052);
    allocate(1051);
    printf("\nFile Found:%d",check_for_block("0.txt"));
    write(1052,0,"Hello");    
    write(1050,0,"Hello");
    write(1051,1,"New");
    printf("\nReturn:%s",read(1052,0));    
    printf("\nReturn:%s",read(1050,0));
    printf("\nReturn:%s",read(1051,1));
    deallocate(1052,0);
    deallocate(1051,1);
    write(1052,0,"Hello");
    printf("\nReturn:%s",read(1052,0));

    return 0;
}
*/
