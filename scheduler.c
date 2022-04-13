/**
 *  scheduler.c
 *
 *  Full Name: Lucas Aguiar
 *  Course section: Section M
 *  Based on the text file (containing processes) and integer provided, will run 1 of 3 scheduling algorithms until all processes have completed running
 *  Either runs First come first server (0), Round Robin (1) or Shortest Job first (2)
 *  Outputs to a file called inputfile-alg.txt ex: if the input file was called test and you ran the RR alg, it will output to test-1.txt
 *  
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <math.h>

#include "queue.h"
#include "scheduler.h"
#include "process.h"

#define SIZE    100
#define FOREACH_STATUS(STATUS) \
	STATUS(not_arrived) \
	STATUS(terminated) \
	STATUS(ready) \
	STATUS(running) \
	STATUS(blocked) \

#define GENERATE_ENUM(ENUM) ENUM,
#define GENERATE_STRING(STRING) #STRING,

//Enum of all the possible statuses that a process can have
enum status {
	FOREACH_STATUS(GENERATE_ENUM)
};

//Creates strings for each status in the enum status (allows writing to a file easier)
static const char *status_string[] = {
	FOREACH_STATUS(GENERATE_STRING)
};

//function is used by all three algorithms and is used to write summary statistics to the output file after all processes have terminated
void summary_stats(Process *list, FILE *output, int num_of_processes, int clock_time, int idle_counter){
	//Writes the Finishing time and CPU util
	fprintf(output, "\n");
	fprintf(output, "Finishing time: %d\n", clock_time-1);
	double cpu_util = (double)(clock_time-idle_counter)/clock_time;
	fprintf(output, "CPU utilization: %.2lf\n", cpu_util);

	//Writes the processes turnaround times in order of PID
	for (int j=0; j<num_of_processes; j++){
		fprintf(output, "Turnaround process %d: %d", list[j].pid, list[j].turnaround_time);
		if(j!=num_of_processes-1) fprintf(output, "\n");
	}
}

//function used by all three algorithms and is used to check if any processes have terminated
void check_terminated(Queue *queue, Process *list, bool *idle, int num_of_processes, int *finished, int *clock_time){
	for(int i=0; i<num_of_processes; i++){
		//Checks processes that haven't yet terminated to see if they terminated
		//Processes can only terminate if they have completed CPU time AND IO time
		//Increments finished counter, changes status to terminated and sets the turnaround time
		//Sets the cpu to idle only if the terminated process is currently running
		if(list[i].status>terminated){
			if((list[i].time_ran==list[i].cpu_time) && list[i].io_time==0) {
				if(list[i].status==running) *idle=true;
				list[i].status = terminated;
				list[i].turnaround_time = (*clock_time) - list[i].arrival_time;
				(*finished)++;
			}
		}
	}
}

//function is used by all three algorithms, and in order of least to greatest PID, decrements io time from blocked processes, increments time ran for running processes
//and writes processes id (as long as they are either running, ready or blocked) to the output file along with their status
void handle_processes(Process *list, int num_of_processes, FILE *output){
	for (int i=0; i<num_of_processes; i++){
		if(list[i].status>terminated){
			if(list[i].status==blocked) list[i].io_time--;
			else if(list[i].status==running) list[i].time_ran++;
			fprintf(output, " %d:%s", list[i].pid, status_string[list[i].status]);			
		}
	}
	fprintf(output, "\n");
}

//function is used by all three algorithms as all algorithms unblock after io time is done
void blocked_transition(Queue *queue, Process *process){
	//If a process status is blocked and it is done with io_time, set it to ready and put it back in the queue
	if(process->io_time==0){
			process->status=ready;
			offer(queue, process);
		}
}

//function is used by all three algorithms as all algorithms block after a burst
void running_transition(Queue *queue, Process *process, bool *idle){
	//If the CPU is not idle (something is running)
	//Checks if process has completed the first burst (and must block for I/O time)
	//If it has, sets the process to blocked and sets the CPU to idle
	if(!*idle){
		double burst = process->cpu_time/2.0;
		if(process->time_ran==ceil(burst) && process->io_time!=0) {
			process->status=blocked;
			*idle=true;
		}		
	}
}

//function is used by all three algorithms and add arriving processes into the queue
void arriving(Queue *queue, Process *process, int clock_time){
	//Adds processes with lowest PIDs first
	if(process->arrival_time==clock_time) {
		process->status = ready;
		offer(queue, process);
	}
}

void first_come_first_serve(Process *list, int num_of_processes, FILE *output){

	//Initialize Variables
	int clock_time = 0, finished = 0, idle_counter = 0;
	bool idle = true;
	Queue *queue = malloc(sizeof(struct queue));
	queue->size = 0;
	Process *currently_running = NULL;


	//While all processes not finished
	while(finished < num_of_processes){
		//Works on processes in the following order: Blocked, Running, Arriving, Ready
		//Loops thru processes sorted by order of PID to ensure that ready processes with lower PID get priority
		for(int i=0; i<num_of_processes; i++){
			switch(list[i].status){
				case blocked: 
					blocked_transition(queue, &list[i]);
					break;
				case running:
					running_transition(queue, &list[i], &idle);
					break;
				case not_arrived:
					arriving(queue, &list[i], clock_time);
					break;
				default:
					break;
			}
		}
		//If the CPU is idle, it checks if the queue is empty
		//If the queue is empty it increments the idle_counter
		//Otherwise it takes the process from the top of the queue and sets it to running
		if(idle) {
			if(!isEmpty(queue)){
				currently_running = poll(queue);
				currently_running->status=running;
				idle=false;
			}
			else idle_counter++;
		}	

		//Handles processes
		fprintf(output, "%d", clock_time);
		handle_processes(list, num_of_processes, output);

		//Increments clock time and then checks if any processes terminated
		clock_time++;
		check_terminated(queue, list, &idle, num_of_processes, &finished, &clock_time);
	}

	//Writes summary stats to the file
	summary_stats(list, output, num_of_processes, clock_time, idle_counter);
	
}

void round_robin(Process *list, int num_of_processes, FILE *output){

	//Initialize Variables
	int clock_time = 0, finished = 0, idle_counter = 0;
	bool idle = true;
	Queue *queue = malloc(sizeof(struct queue));
	queue->size = 0;
	Process *currently_running = NULL;
	int quantum = QUANTUM;

	while(finished < num_of_processes){
		//Works on processes in the following order: Blocked, Running, Arriving, Ready
		//Loops thru processes sorted by order of PID to ensure that ready processes with lower PID get priority
		for(int i=0; i<num_of_processes; i++){
			switch(list[i].status){
				case blocked: 
					blocked_transition(queue, &list[i]);
					break;
				case running:
					running_transition(queue, &list[i], &idle);
					//Handles preemption -- if the cpu is not idle and the quantum is over
					// the currently running process is added to the back of the queue so that a new process can be picked
					if(!idle){
						if(quantum==0){
							quantum=QUANTUM;
							currently_running->status = ready;
							idle=true;
							offer(queue, currently_running);
						}
					}
					break;
				case not_arrived:
					arriving(queue, &list[i], clock_time);
					break;
				default:
					break;
			}

		}
		//If the CPU is idle, it checks if the queue is empty
		//If the queue is empty it increments the idle_counter
		//Otherwise it takes the process from the top of the queue and sets it to running
		//Makes sure it resets the quantum when running a new process
		if(idle) {
			if(!isEmpty(queue)){
				currently_running = poll(queue);
				currently_running->status=running;
				idle=false;
				quantum=QUANTUM;
			}
			else idle_counter++;
		}

		//Handles processes (decrement io time/cpu time)
		fprintf(output, "%d", clock_time);
		handle_processes(list, num_of_processes, output);
		if(quantum!=0 && !idle) quantum--;
		//Increments clock time and checks if processes terminated
		clock_time++;
		check_terminated(queue, list, &idle, num_of_processes, &finished, &clock_time);
	}

	//Writes summary stats to the file
	summary_stats(list, output, num_of_processes, clock_time, idle_counter);
}

void shortest_job_first(Process *list, int num_of_processes, FILE *output){
	//Initialize Variables
	int clock_time = 0, finished = 0, idle_counter = 0;
	bool idle = true;
	Queue *queue = malloc(sizeof(struct queue));
	queue->size = 0;
	Process *currently_running = NULL;
	int lowest_id = -1, lowest_cpu = 0, time_remaining;


	while(finished < num_of_processes){
		//Works on processes in the following order: Blocked, Running, Arriving, Ready
		//Loops thru processes sorted by order of PID to ensure that ready processes with lower PID get priority
		for(int i=0; i<num_of_processes; i++){
			switch(list[i].status){
				case blocked: 
					blocked_transition(queue, &list[i]);
					break;
				case running:
					running_transition(queue, &list[i], &idle);
					break;
				case not_arrived:
					arriving(queue, &list[i], clock_time);
					break;
				default:
					break;
			}
		}
		//If the cpu is idle and the queue is not empty, picks a new process; else increments cpu counter
		//Searches through the ready processes in order to find the one with the smallest REMAINING CPU time to run 
		if(idle) {
			if(!isEmpty(queue)){
				idle=false;
				for(int i=0; i<num_of_processes; i++){
					if(list[i].status==ready){
						time_remaining = list[i].cpu_time - list[i].time_ran;
						if(time_remaining<lowest_cpu || lowest_cpu==0){
							lowest_cpu=time_remaining;
							lowest_id=i;
						}
					}
				}
				currently_running = &list[lowest_id];
				currently_running->status = running;
				poll(queue);
				lowest_id=-1;
				lowest_cpu=0;
			}
			else idle_counter++;
		}

		//Handles processes (decrement io time/cpu time)
		fprintf(output, "%d", clock_time);
		handle_processes(list, num_of_processes, output);

		//Increments clock time and checks if any processes terminated
		clock_time++;
		check_terminated(queue, list, &idle, num_of_processes, &finished, &clock_time);
				
	}

	//Writes summary stats to the file
	summary_stats(list, output, num_of_processes, clock_time, idle_counter);

}

int main(int argc, char *argv[])
{

	//Initialize some variables
	FILE *fp;
	FILE *output;
	char output_name[100];
	int alg_number = atoi(argv[2]);
	int num_of_processes;


	//Read Number of processes
	fp  = fopen(argv[1],"r");
	fscanf(fp, "%d", &num_of_processes);    
	//For each process, read the file and add each process to an array of processes

	Process *list = malloc(num_of_processes * sizeof(Process));      
	for (int i=0; i<num_of_processes; i++){
		fscanf(fp, "%d %d %d %d",
				&list[i].pid, 
				&list[i].cpu_time, 
				&list[i].io_time, 
				&list[i].arrival_time); 
		list[i].status = 0;
		list[i].time_ran = 0;
		list[i].turnaround_time = 0;
	}    


	//In place selection sort algorithm to sort the proccesses by PID                                                                 
	int min_index = 0;
	Process temp;
	for(int i=0; i<num_of_processes-1; i++){
		min_index = i;
		for(int j=i+1; j<num_of_processes; j++){
			if(list[j].pid<list[min_index].pid) min_index = j;
		}
		temp = list[min_index];
		list[min_index] = list[i];
		list[i] = temp;
	}


	//Creates the output file
	//Concatenates some strings to get the correct file name
	strncpy(output_name, argv[1], strlen(argv[1]) - 4);
	output_name[strlen(argv[1])-4] = '\0';
	strcat(output_name, "-");
	strcat(output_name, argv[2]);
	strcat(output_name, ".txt");
	output = fopen(output_name, "w");

	//Reads the value of the second argument, and uses a switch statement to run the correct algorithm
	switch(alg_number) {
		case 0:
			first_come_first_serve(list, num_of_processes, output);
			break;
		case 1:
			round_robin(list, num_of_processes, output);
			break;
		case 2:
			shortest_job_first(list, num_of_processes, output);
			break;
	}


	free(list); 


	fclose(fp);
	fclose(output);


	return 0;
}
