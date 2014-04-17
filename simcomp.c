//Header Files
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <pthread.h>

//Global

//We should probably place structs in files outside this one
//along with any data structures.

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
    //Operation Type/Name
	char operation;
    int totalCycles;
    int cyclesRemaining;
	char *name;
}taskInfoBlock;

typedef struct processControlBlock
{
    pid_t pid;
    pid_t parentPid;
    time_t arrivalTime;
    time_t timeRemaining;
	//new var needs to hold thread info, if != something the thread is not completed
    int priority;
    //List of Actions/Operations (Busy waiting or I/O)
    int busyWaitFlag;
    taskInfoBlock **jobs;
	unsigned int numberOfJobs;
	unsigned int currentJob;
    struct processControlBlock *nextPCB;
}processControlBlock;

//////Function Declarations//////

/*
Name: getSimulatorConfiguration
Purpose: Reads in simulator configurations from provided file
*/
bool getSimulatorConfiguration(struct simulatorStructure *simulator, const char *configurationFilePath);

/*
Name: getNextCharacter
Purpose: Returns the next character found in a file
*/
char getNextCharacter(FILE *input);

/*
Name: createProcessQueue
Purpose: Creates process queue from process file
*/
bool createProcessQueue(struct processControlBlock **process, const char *processFilePath);

/*
Name: threadWait
Purpose: function to pass to I/O thread to make it wait the required time
*/
void* threadWait(void*);


//Global variable interrupt to handle interrupts
int interrupted;

//////Main//////

int main(int argc, char *argv[])
{
    //Initialize variables
    simulatorStructure simulator;
    taskInfoBlock test;
    processControlBlock *process = NULL;
	interrupted = 0;
	int maxTimeAllowed = 0;
    
    // Check if configuration file isn't provided
    if(argc != 2) {
    
    	// Display error
    	puts("Error: Invalid parameters");
    	
    	// Return 0
    	return 0;
    }
    
	// Read into simulator configuration struct
    // Check if getting simulator configuration failed
    if(!getSimulatorConfiguration(&simulator, argv[1])) {
    
    	// Display error
    	puts("Error: Invalid configuration file");
    	
    	// Return 0
    	return 0;
    }
    
	// Create process queue
    // Check if creating process queue failed
    if(!createProcessQueue(&process, simulator.processFilePath)) {
    
    	// Display error
    	puts("Error: Failed to create job queue");
    	
    	// Return 0
    	return 0;
    }
    
	//check scheduling algorithm
	if( !strcmp(simulator.processorScheduling, "RR") == 0)
	{
		printf("Scheduling not implemented\n");
		return 0;
	}
	printf("Implementing RR scheduling\n");

    //Example of dynamic allocation using c (new is c++)
    //test2.nextPCB = malloc(sizeof(processControlBlock));
    //free(test2.nextPmaxTimeAllowedCB);
      
	//assign maxTimeAllowed to processes in microseconds  
	//--maxTimeAllowed = (simulator.processorCycleTime * simulator.quantum);
    //Do we have a pointer to the process queue?

	processControlBlock *currentProcess = &process[0];
	
    //Begin Simulation Loop
    while(currentProcess != NULL) //Jobs remaining
    {        
        //Process current task by busy waiting
		if ( !currentProcess->busyWaitFlag ) //is not blocked for I/O wait, else, skip wait
		{
			while (totalTime < allowedTime)
			{
				switch(currentProcess->jobs[currentProcess->currentJob][0]) {
					case 'P':
					
						// Sleep
						usleep(simulator.quantum);
						currentProcess->timeRemaining- simulator.quantum;
					break;
					
					case 'I':
						// Create thread
					break;
					case 'O':
						// Create a thread
					break;
				}
				/*
				-check if process task is complete
					-if it is move to the next task
					-get task info
					-usleep(maxtime or finish)
					-if next task is I/O start the thread, add process to I/O queue, stop loop
				-else continue */
			}
		}
/*
		****or****
		-check how long next process step is
		-usleep either for maximum allowed time or until process is finished
		-if there is time leftover subtract from max time and continue

		*/
		
		// if current process is finished delete it
        // Perform Context Switch

        //	Log Output Data
    }
    
    //Print output to screen, file, or both
    
    return 0;
}

//Function Implementations
bool getSimulatorConfiguration(struct simulatorStructure *simulator, const char *configurationFilePath) {

	// Initialize variables
	FILE *input;
	fpos_t cursor;
	unsigned int length;
	char *line;
	
	// Check if file doesn't exist
	if((input = fopen(configurationFilePath, "r")) == NULL)
	
		// Return false
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
	
	// Return true
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
	unsigned int numberOfJobs, length;
	processControlBlock *tempProcess = NULL , *previousProcess = NULL;
	
	// Check if file doesn't exist
	if((input = fopen(processFilePath, "r")) == NULL)
	
		// Return false
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
			tempProcess->jobs = (taskInfoBlock**)malloc(sizeof(taskInfoBlock*) * (numberOfJobs));

			tempProcess->numberOfJobs = numberOfJobs;
			tempProcess->currentJob = 0;
			
			// Go through all jobs in the process
			for(unsigned int i = 0; i < numberOfJobs; i++) {

				// Get job operation
				tempProcess->jobs[i]->operation = fgetc(input);
				
				// Ignore nest character
				fgetc(input);
			
				// Get current position in file
				fgetpos(input, &cursor);
		
				// Get length of next part in file until a semicolon is found
				for(length = 0; fgetc(input) != ')'; length++);
		
				// Go back to last position in file
				fsetpos(input, &cursor);
				
				// Allocate memory for current job including a space for a null terminator
				tempProcess->jobs[i] = (taskInfoBlock*)malloc(sizeof(taskInfoBlock) * (length + 1));
			
				// Read in current job
				fread(tempProcess->jobs[i]->name, sizeof(char), length, input);
			
				// Set last character of current job to null terminator
				tempProcess->jobs[i]->name[length] = '\0';

				// Ignore next character
				fgetc(input);

				// Read in job total cycles
				fscanf(input, "%u", &tempProcess->jobs[i]->totalCycles);
				
				// Ignore semicolon and space characters in file
				fgetc(input);
				fgetc(input);
			}
			
			// Ignore character in file until next group
			while(fgetc(input) != ' ');
			
			// Set previous process
			previousProcess = tempProcess;
		}
	}
	
	// Make process queue circular
	previousProcess->nextPCB = *process;
	
	// Close file
	fclose(input);

	// Return true
	return true;
}

void* threadWait(void* wait)
{
	int waitTime = (int) wait;
	usleep(waitTime);

	//check if another interrupt is being serviced
	//if interrupted == 1: wait
	while (interrupted == 1);
	
	//set their own interrupt
	interrupted = 1;	

	//returns
	pthread_exit(0);
}
