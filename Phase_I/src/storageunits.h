#include<stdio.h>
typedef struct{
	int process_id;
	int file_id;
	int cell_no;
	char *cell_data;
	int valid;
}storage;
//storage data[100];

char buffer_file[20]="Temp.txt";
char set_file[20]="set.txt";