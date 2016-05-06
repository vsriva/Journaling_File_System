//README

This is a README file for this program
As the functions in the program and interdependant on some shared variables they are all present in 'journal_file_manager.h', 'RecoveryManager.h' and 'Log.h' and 'cellstorage.h'. 
But the functions are completely modular and thus can be interrupted and will resume from that state of cell storage system. 
As the cell storage and temperory data are managed in files(non-volatile storage) rather than using RAM. 
But as for reseting the state we have called a cleanup function, that removes all the state files. 

I am providing here the program with two test cases already hard coded with certain parameters, in the 'test_case_race_condition.c' and 'test_case_multi_writes.c' file.
Test case 'test_case_race_condition.c' provides a simulation when two processes are trying to write to the same cell.
Test case 'test_case_multi_writes.c' provides a simulation when 10 different files with each having 2 cells are trying to write simultaneously.

The complete code for journal file manager, storage units and cell storage and provided in seperate header files.
 
So following are the steps to compile and run the test cases:
1. Navigate to the directory 'src' and type 'make' in terminal.
2. Now run the file by typing './test_case_multi_write' in terminal to run Multiple file write simulation. It can give garbage values at first run (low I/O bandwidth of HD) but will recover to correct value.
2. Now run the file by typing './test_case_race_condition' in terminal to run Race condition simulation.
3. Each test_case can be run multiple times as there is cleanup of status files done before each run.

Note: Don't try to run both simulations together as they access the same directory 'cells' and try to create cell storage there. 
  
