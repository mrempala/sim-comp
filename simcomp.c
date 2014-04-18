//////////////////////// Header Files ///////////////////////////////////

#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <pthread.h>
#include "cStyleList.h"

//////////////////// Global Declarations ////////////////////////////////

int interrupted; //Global var to handle interrupts

typedef struct simulatorStructure
{
    char *version;
    unsigned int quantum;
    char *processorScheduling;
    char *processFilePath;
    unsigned int processorCycleTime;
    unsigned int monitorDisplayTime;
    unsigned int hardDriveCycleTime;
    unsigned int printerCycleTime;
    unsigned int keyboardCycleTime;
    char *memoryType;
    char *logType;
}simulatorStructure;

typedef struct taskInfoBlock
{
    char operation;
    int totalCycles;
    int cyclesRemaining;
    char *name;
}taskInfoBlock;

typedef struct processControlBlock
{
    int pid;
    time_t arrivalTime;
    time_t timeRemaining;
    bool ioInterrupted;
    bool ioFinished;
    bool threadBeingCreated;
    taskInfoBlock *jobs;
    unsigned int numberOfJobs;
    unsigned int currentJob;
    struct processControlBlock *nextPCB;
    struct processControlBlock *previousPCB;
}processControlBlock;

typedef struct threadInfo
{
    processControlBlock* process;
    unsigned int quantumTime;
	List *log;
}threadInfo;


///////////////////// Function Declarations //////////////////////////////////


//Purpose: Reads in simulator configurations from provided file
bool getSimulatorConfiguration(struct simulatorStructure *simulator, const char *configurationFilePath);


//Purpose: Returns the next character found in a file
char getNextCharacter(FILE *input);


//Purpose: Creates process queue from process file
bool createProcessQueue(struct processControlBlock **process, const char *processFilePath);


//Purpose: Sets the current process based on shceduling type
void setCurrentProcess(struct processControlBlock **currentProcess, struct simulatorStructure simulator);


//Purpose: Deletes the process given and re-links the circularly linked list
processControlBlock * deleteProcess(struct processControlBlock *currentProcess);


//Purpose: Function to pass to I/O thread to make it wait the required time
void* threadWait(void*);


//Purpose: Function to pass to I/O thread to make it wait the required time
void threadCreate(struct processControlBlock **currentProcess,  struct simulatorStructure simulator, List *log);

////////////////////////////////// Main //////////////////////////////////////////

int main(int argc, char *argv[])
{
    //Initialize variables to be used later
    simulatorStructure simulator;
    taskInfoBlock test;
    processControlBlock *process = NULL, *currentProcess = NULL, *tempPCB = NULL;
    interrupted = 0;
    int maxTimeProcessing = 0;
    List log;
    char logBuffer [100];
    FILE *output;
    ListNode *tempNode = NULL;
    
    // initializes log to print info to
	// log will print to file or terminal -at the end of the program- depending on the config file
    init(&log);
    
    // Check if configuration file isn't provided
    if(argc != 2) 
    {
        // Display error
        puts("Error: Invalid parameters");
        
        // End program with error
        return 1;
    }
    
    // Read into simulator configuration struct
    // Check if getting simulator configuration failed
    if(!getSimulatorConfiguration(&simulator, argv[1])) 
    {
        // Display error
        puts("Error: Invalid configuration file");
        
        // End program with error
        return 1;
    }
    
    //assign maximum allowed processing time from the simulator configuration
    maxTimeProcessing = (simulator.quantum * simulator.processorCycleTime * 1000);

    // Create process queue
    // Check if creating process queue failed
    if(!createProcessQueue(&process, simulator.processFilePath)) 
    {
        // Display error
        puts("Error: Failed to create job queue");
        
        // End program with error
        return 1;
    }
    
    // Set current process
    currentProcess = &process[0];
    
	// begin writing to log
    insert(&log, "System Start 0 (microsec)\n");
    
    //Begin Simulation Loop and go until no processes remain
    while(currentProcess != NULL)
    {
        // Set process's arrival time if not already set
        if(currentProcess->arrivalTime == 0)
        {
            currentProcess->arrivalTime = time(NULL);
		}

        //if a thread throws an interrupt
        if(interrupted == 1)
        {
            tempPCB = currentProcess;

			//search for the process whose I/O just finished
            while(tempPCB->ioInterrupted == false)
            {
                tempPCB = tempPCB->nextPCB;
            }

			//reset Finished and Interrupted so that the process can continue being processed
            tempPCB->ioFinished = true;
            tempPCB->ioInterrupted = false;

			//reset interrupt flag
            interrupted = 0;
        }

        
        //Process current task by busy waiting
        // if not blocked for I/O wait, continue to process next job
        if( currentProcess->ioFinished && currentProcess->currentJob < currentProcess->numberOfJobs)
        {
            // if a process has a CPU operation
            if(currentProcess->jobs[currentProcess->currentJob].operation == 'P') 
            {
                // Sleep
                //if the quantum time is less than the remaining job time, take the entire quantum
                if(simulator.quantum < currentProcess->jobs[currentProcess->currentJob].cyclesRemaining)
                {
					//write to log
                    sprintf(logBuffer, "PID %d: CPU Process %d (microsec)", currentProcess->pid, maxTimeProcessing);
                    insert(&log, logBuffer);
					
					//sleep for maximum time
                    usleep(maxTimeProcessing);
                    
                    // Subtract quantum time from process
                    currentProcess->timeRemaining -= simulator.quantum;
                    currentProcess->jobs[currentProcess->currentJob].cyclesRemaining -= simulator.quantum;
                }
                //if the quantum is greater than required time, finish the job
                else
                {
					 // write to log
                     sprintf(logBuffer, "PID %d: CPU Process %d (microsec)", currentProcess->pid, currentProcess->jobs[currentProcess->currentJob].cyclesRemaining * simulator.processorCycleTime * 1000);
                insert(&log, logBuffer);

					 // sleep for remaining job time
                     usleep(currentProcess->jobs[currentProcess->currentJob].cyclesRemaining * simulator.processorCycleTime * 1000);
                     
                     // Subtract time run from process
                     currentProcess->timeRemaining -= currentProcess->jobs[currentProcess->currentJob].cyclesRemaining;
                     currentProcess->jobs[currentProcess->currentJob].cyclesRemaining -= currentProcess->jobs[currentProcess->currentJob].cyclesRemaining;
                }
            }

            // if a process has an I/O operation
            else
            {
                
                // output to log
                sprintf(logBuffer, "PID %d: Began %s I/O Process", currentProcess->pid, currentProcess->jobs[currentProcess->currentJob].name);
                insert(&log, logBuffer);
                puts("\n");

                // set ioFinished to false so this process will be skipped in the queue
                currentProcess->ioFinished = false;

                // write reassurance the program is working to the terminal
                printf("Working...\n");

                currentProcess->threadBeingCreated = true;

				// create thread
                threadCreate(&currentProcess, simulator, &log);

                // wait for thread to finish creating itself
                // weird errors happen to process if not made to wait for thread creation
                while(currentProcess->threadBeingCreated);

            }
        }
        
        //If the currentProcess's job is finished, delete it
        if( currentProcess->jobs[currentProcess->currentJob].cyclesRemaining <= 0 )
        {
            if(currentProcess->currentJob < currentProcess->numberOfJobs)
                currentProcess->currentJob++;
        }
        
        //Delete the process if it is done
        if( currentProcess->timeRemaining <= 0 && currentProcess->ioInterrupted == false && currentProcess->ioFinished == true)
		{
			sprintf(logBuffer, "PID %d: finished", currentProcess->pid);
			insert(&log, logBuffer);
            currentProcess = deleteProcess( currentProcess );  
		} 
            
        //Get the next process, context switch
        /*  context switches do not get printed because they happen so many times in larger processes that it takes
        	up the entire log file
        */
        else
		{
            setCurrentProcess(&currentProcess, simulator);
		}         
    }

	//last log output
	insert(&log, "\nSystem End 0 (microsec)");

    //Print output to monitor if specified by the configuration file
    if(strcmp(simulator.logType, "Log to Monitor") == 0 || strcmp(simulator.logType, "Log to Both") == 0)
    {
        print(&log);
    }
    
    //Print output to log file if specified by the configuration file
    if(strcmp(simulator.logType, "Log to File") == 0 || strcmp(simulator.logType, "Log to Both") == 0)
    {
        //Open the output log file
        output = fopen("Log", "w");

        //Check to see that the linked list is not empty
    	if(!empty(&log))
    	{
    	    //Print the first node
    		tempNode = log.head;
    		fprintf(output, "%s\n", tempNode->dataItem);
    
            //Iterate through the linked list
    		while(tempNode->next != 0)
    		{
    		    //Go to the next node
    			tempNode = tempNode->next;
    
                //Print the next node
    			fprintf(output, "%s\n", tempNode->dataItem);
    		}
    	}
    	
    	//Close the output file
        fclose(output);
    }

    // Free memory
    free(simulator.version);
    free(simulator.processorScheduling);
    free(simulator.processFilePath);
    free(simulator.memoryType);
    free(simulator.logType);
    
    //End the program
    return 0;
}

/////////////////////// Function Implementations //////////////////////////////////

bool getSimulatorConfiguration(struct simulatorStructure *simulator, const char *configurationFilePath) {

    // Initialize variables
    FILE *input;
    fpos_t cursor;
    unsigned int length;
    char *line;
    
    // Return failure if the file doesn't exist
    if((input = fopen(configurationFilePath, "r")) == NULL)
        return false;
    
    // Go through file
    while(getNextCharacter(input) != EOF) {
    
        // Get current position in file
        fgetpos(input, &cursor);
        
        // Get length of next part in file until a colon or newline is found
        for(length = 0; getNextCharacter(input) != '\n' && fgetc(input) != ':'; length++);
        
        // Check if the next character isn't a newline
        if(getNextCharacter(input) != '\n') {
        
            // Go back to last position in file
            fsetpos(input, &cursor);
            
            // Allocate memory for line including a space for a null terminator
            line = (char*)malloc(sizeof(char) * (length + 1));
        
            // Read in line
            fread(line, sizeof(char), length, input);
        
            // Set last character of line to null terminator
            line[length] = '\0';
            
            // Skip the colon and space in file
            fgetc(input);
            fgetc(input);
        
            // Get current position in file
            fgetpos(input, &cursor);
        
            // Get length of next part in file until a newline is found
            for(length = 0; fgetc(input) != '\n'; length++);
        
            // Go back to last position in file
            fsetpos(input, &cursor);
            
            // Check if line is version
            if(strcmp(line, "Version") == 0) {
        
                // Allocate memory for version including a space for a null terminator
                simulator->version = (char*)malloc(sizeof(char) * (length + 1));
            
                // Read in version
                fread(simulator->version, sizeof(char), length, input);
            
                // Set last character of version to null terminator
                simulator->version[length] = '\0';
            }
        
            // Otherwise check if line is quantum
            else if(strcmp(line, "Quantum (cycles)") == 0)
            
                // Read in quantum
                fscanf(input, "%u", &simulator->quantum);
            
            // Otherwise check if line is processor scheduling
            else if(strcmp(line, "Processor Scheduling") == 0) {
        
                // Allocate memory for processor scheduling including a space for a null terminator
                simulator->processorScheduling = (char*)malloc(sizeof(char) * (length + 1));
            
                // Read in processor scheduling
                fread(simulator->processorScheduling, sizeof(char), length, input);
            
                // Set last character of processor scheduling to null terminator
                simulator->processorScheduling[length] = '\0';
                
                // Set quantum to big number if FIFO
                if(strcmp( simulator->processorScheduling, "FIFO") == 0)
                    simulator->quantum = 100000;
            }
            
            // Otherwise check if line is process file path
            else if(strcmp(line, "File Path") == 0) {
        
                // Allocate memory for process file path including a space for a null terminator
                simulator->processFilePath = (char*)malloc(sizeof(char) * (length + 1));
            
                // Read in process file path
                fread(simulator->processFilePath, sizeof(char), length, input);
            
                // Set last character of process file path to null terminator
                simulator->processFilePath[length] = '\0';
            }
            
            // Otherwise check if line is processor cycle time
            else if(strcmp(line, "Processor cycle time (msec)") == 0)
        
                // Read in processor cycle time
                fscanf(input, "%u", &simulator->processorCycleTime);
            
            // Otherwise check if line is monitor display time
            else if(strcmp(line, "Monitor display time (msec)") == 0)
        
                // Read in monitor display time
                fscanf(input, "%u", &simulator->monitorDisplayTime);
            
            // Otherwise check if line is hard drive cycle time
            else if(strcmp(line, "Hard drive cycle time (msec)") == 0)
        
                // Read in hard drive cycle time
                fscanf(input, "%u", &simulator->hardDriveCycleTime);
            
            // Otherwise check if line is printer cycle time
            else if(strcmp(line, "Printer cycle time (msec)") == 0)
        
                // Read in printer cycle time
                fscanf(input, "%u", &simulator->printerCycleTime);
            
            // Otherwise check if line is keyboard cycle time
            else if(strcmp(line, "Keyboard cycle time (msec)") == 0)
        
                // Read in keyboard cycle time
                fscanf(input, "%u", &simulator->keyboardCycleTime);
            
            // Otherwise check if line is memory type
            else if(strcmp(line, "Memory type") == 0) {
        
                // Allocate memory for memory type including a space for a null terminator
                simulator->memoryType = (char*)malloc(sizeof(char) * (length + 1));
            
                // Read in memory type
                fread(simulator->memoryType, sizeof(char), length, input);
            
                // Set last character of memory type to null terminator
                simulator->memoryType[length]  = '\0';
            }
            
            // Otherwise check if line is log
            else if(strcmp(line, "Log") == 0) {
        
                // Allocate memory for log type including a space for a null terminator
                simulator->logType = (char*)malloc(sizeof(char) * (length + 1));
            
                // Read in log type
                fread(simulator->logType, sizeof(char), length, input);
            
                // Set last character of log type to null terminator
                simulator->logType[length] = '\0';
            }
            
            // Free memory
            free(line);
        }
        
        // Ignore newline character in file
        fgetc(input);
    }
    
    // Close file
    fclose(input);
    
    // Return success
    return true;
}

char getNextCharacter(FILE *input) {

    // Get next character in file
    char returnValue = fgetc(input);
    
    // Go back to last position in file
    ungetc(returnValue, input);
    
    // Return next character in file
    return returnValue;
}

bool createProcessQueue(struct processControlBlock **process, const char *processFilePath) {

    // Initialize variables
    FILE *input;
    fpos_t cursor;
    int tempValue = 0;
    unsigned int numberOfJobs, length, numberOfProcesses = 0;
    processControlBlock *tempProcess = NULL , *previousProcess = NULL;
    
    // Return failure if the file doesn't exist
    if((input = fopen(processFilePath, "r")) == NULL)
        return false;
    
    // Go through file
    while(getNextCharacter(input) != EOF) {
    
        // Check if next character in file is S
        if(getNextCharacter(input) == 'S')
        
            // Ignore character in file until next group
            while(getNextCharacter(input) != EOF && fgetc(input) != ' ');
        
        // Otherwise check if next character in file is A
        else if(getNextCharacter(input) == 'A') {
        
            // Allocate memory for new process
            tempProcess = (processControlBlock*)malloc(sizeof(processControlBlock));
            
            // Check if this is the first process
            if(previousProcess == NULL)
            
                // Set first process
                *process = tempProcess;
            
            // Otherwise
            else
            
                // Set previous process's next process
                previousProcess->nextPCB = tempProcess;
            
            // Increment number of processes
            numberOfProcesses++;
        
            // Ignore character in file until next group
            while(fgetc(input) != ' ');
        
            // Get current position in file
            fgetpos(input, &cursor);
            
            // Skip until end of A process
            for(numberOfJobs = 0; getNextCharacter(input) != 'A'; numberOfJobs++) {
            
                while(fgetc(input) != ';');
                
                fgetc(input);
            }
            
            // Go back to last position in file
            fsetpos(input, &cursor);
        
            // Allocate memory for jobs in the process
            tempProcess->jobs = (taskInfoBlock*)malloc(sizeof(taskInfoBlock) * (numberOfJobs));
            
            // Set details of process
            tempProcess->numberOfJobs = numberOfJobs;
            tempProcess->currentJob = 0;
            tempProcess->pid = tempValue++;
            tempProcess->ioFinished = true;
            tempProcess->ioInterrupted = false;
            tempProcess->arrivalTime = 0;
            tempProcess->threadBeingCreated = false;
            
            // Go through all jobs in the process
            for(unsigned int i = 0; i < numberOfJobs; i++) {
            
                // Get job operation
                tempProcess->jobs[i].operation = fgetc(input);
                
                // Ignore nest character
                fgetc(input);
            
                // Get current position in file
                fgetpos(input, &cursor);
        
                // Get length of next part in file until a semicolon is found
                for(length = 0; fgetc(input) != ')'; length++);
        
                // Go back to last position in file
                fsetpos(input, &cursor);
                
                // Allocate memory for current jobs's name including space for a null terminator
                tempProcess->jobs[i].name = (char*)malloc(sizeof(char) * (length + 1));
            
                // Read in current job
                fread(tempProcess->jobs[i].name, sizeof(char), length, input);
            
                // Set last character of current job to null terminator
                tempProcess->jobs[i].name[length] = '\0';

                // Ignore next character
                fgetc(input);

                // Read in job cycles
                fscanf(input, "%u", &tempProcess->jobs[i].totalCycles);
                tempProcess->jobs[i].cyclesRemaining = tempProcess->jobs[i].totalCycles;
                
                // Ignore semicolon and space characters in file
                fgetc(input);
                fgetc(input);
            }
            
            // Ignore character in file until next group
            while(fgetc(input) != ' ');
            
            // Set process's time remaining
            for(unsigned int i = 0; i < numberOfJobs; i++)
            
                tempProcess->timeRemaining += tempProcess->jobs[i].totalCycles;
            
            // Set previous process to current process
            previousProcess = tempProcess;
            
        }
    }
    
    // Make process queue circular
    previousProcess->nextPCB = *process;
    
    // Set previous PCB
    for(unsigned int i = 0; i < numberOfProcesses; i++) {
        tempProcess = previousProcess;
        previousProcess = previousProcess->nextPCB;
        previousProcess->previousPCB = tempProcess;
    }
    
    // Close file
    fclose(input);

    // Return success
    return true;
}

void setCurrentProcess(struct processControlBlock **currentProcess, struct simulatorStructure simulator) {

    // Check if process scheduling is FIFO
    if(strcmp(simulator.processorScheduling, "FIFO") == 0)
    {
        // Set current process to next process
        (*currentProcess) = (*currentProcess)->nextPCB;
    }
    
    // Otherwise check if process scheduling is RR
    else if(strcmp(simulator.processorScheduling, "RR") == 0)
    {
        // Set current process to next process
        (*currentProcess) = (*currentProcess)->nextPCB;
    }
    // Otherwise check if process scheduling is SJF
    else if(strcmp(simulator.processorScheduling, "SJF") == 0) 
    {
        // Implement later
    }
    
    // Otherwise check if process scheduling is SRTF
    else if(strcmp(simulator.processorScheduling, "SRTF") == 0) 
    {
        // Implement later
    }
}

processControlBlock* deleteProcess(struct processControlBlock *currentProcess)
{
    //Initalize variables
    processControlBlock *temp = NULL;
    
    //Make sure currentProcess is not NULL
    if( currentProcess == NULL )
    {
        return NULL;
    }
    
    //Check to see if node is the last node remaining
    if( currentProcess->nextPCB == currentProcess)
    {
    	for(unsigned int i = 0; i < currentProcess->numberOfJobs; i++)
    		free(currentProcess->jobs[i].name);
        //If so, delete it
        free(currentProcess->jobs);
        free( currentProcess );
        currentProcess = NULL;
        return NULL;
    }
    
    //Otherwise re-link the list and then delete it
    else
    {
        currentProcess->previousPCB->nextPCB = currentProcess->nextPCB;
        currentProcess->nextPCB->previousPCB = currentProcess->previousPCB;
        temp = currentProcess->nextPCB;
        for(unsigned int i = 0; i < currentProcess->numberOfJobs; i++)
    		free(currentProcess->jobs[i].name);
    	free(currentProcess->jobs);
        free( currentProcess );
    }
    
    return temp;
}

void* threadWait(void* threadInfo)
{
    //Initalize varables, and thread info struct recast as usable pointer
    struct threadInfo *info = (struct threadInfo*)threadInfo;
    unsigned int temp = info->process->jobs[info->process->currentJob].cyclesRemaining;
	char logBuffer [100];
    
    // Clear cycles remaining for current job
    info->process->timeRemaining -= info->process->jobs[info->process->currentJob].cyclesRemaining;
    info->process->jobs[info->process->currentJob].cyclesRemaining = 0;
    
    // Clear thread being created
    info->process->threadBeingCreated = false;
   
	//write to log 
    sprintf(logBuffer, "PID %d: I/O thread created", info->process->pid);
	insert(info->log, logBuffer);
    
    //calculate waitTime
    int waitTime = (temp * info->quantumTime * 1000);
    usleep(waitTime);

    //If there is another interrupt,
    //wait for it to be serviced    
    while (interrupted == 1);
    
    //Set interrupt for itself
    interrupted = 1;   
    
    //write data to log    
    sprintf(logBuffer, "PID %d: I/O thread finished %d (microsec)", info->process->pid, waitTime);
	insert(info->log, logBuffer);

    //alert process to thread completion
    info->process->ioInterrupted = true;
    free(threadInfo);

    //Returns
    pthread_exit(0);
}

void threadCreate(struct processControlBlock **process,  struct simulatorStructure simulator, List *log)
{
    pthread_t thread;
    threadInfo *info = (threadInfo*)malloc(sizeof(threadInfo));
    
    // put correct information into threadInfo struct
    info->process = *process;
	info->log = log;
    
	//check for what type of job for I/O so that the right cycle time is assigned to the thread
    if( strcmp("monitor", (*process)->jobs[(*process)->currentJob].name) == 0)
    {
        info->quantumTime = simulator.monitorDisplayTime;
    }
    else if( strcmp("hard drive", (*process)->jobs[(*process)->currentJob].name) == 0)
    {
        info->quantumTime = simulator.hardDriveCycleTime;
    }
    else if( strcmp("keyboard", (*process)->jobs[(*process)->currentJob].name) == 0)
    {
        info->quantumTime = simulator.keyboardCycleTime;
    }
    else
    {
        info->quantumTime = simulator.printerCycleTime;
    }

	//create the thread
    pthread_create(&thread, NULL, threadWait, (void*)info);
}
