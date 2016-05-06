HEADERS = journal_file_manager.h
all: program1 program2 

trial_case_multi_writes.o: trial_case_multi_writes.c $(HEADERS)
	gcc -Wall -cw trial_case_multi_writes.c -lpthread

trial_case_race_condition.o: trial_case_race_condition.c $(HEADERS)
	gcc -Wall -cw trial_case_race_condition.c -lpthread

program1: trial_case_race_condition.o
	gcc trial_case_race_condition.o -o trial_case_race_condition -lpthread

program2: trial_case_multi_writes.o
	gcc trial_case_multi_writes.o -o trial_case_multi_writes -lpthread

clean:
	-rm -f trial_case_multi_writes.o
	-rm -f trial_case_multi_writes
	-rm -f trial_case_race_condition.o
	-rm -f trial_case_race_condition
