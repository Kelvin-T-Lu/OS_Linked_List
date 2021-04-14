/* Fill in your Name and GNumber in the following two comment fields
 * Name: Kelvin Lu
 * GNumber: G01194210
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "clock.h"
#include "structs.h"
#include "constants.h"
#include "scheduler.h"

/* Initialize the Schedule Struct
 * Follow the specification for this function.
 * Returns a pointer to the new Schedule or NULL on any error.
 */
Schedule *scheduler_init() {
	//Intialize Schedule
	//Intialize the queues
	//Set the variables of the queues
	Schedule *schedule = (Schedule *) malloc(sizeof(Schedule)); 

	Queue *ready = (Queue*) malloc(sizeof(Queue)); 
	Queue *stop = (Queue*) malloc(sizeof(Queue)); 
	Queue *wait = (Queue*) malloc(sizeof(Queue));  
	
	//Check for memory allocations.
	if(!schedule || !ready ||!stop || !wait){
		return NULL; 
	}
	
	//Intilize Queue instance variables.
	ready-> count = 0; 
	ready->head = NULL; 		
	stop-> count = 0; 
	stop->head = NULL; 
	wait-> count = 0; 
	wait->head = NULL; 

	schedule->ready_queue = ready;
	schedule->stopped_queue = stop;
	schedule->waiting_queue = wait;

	return schedule; 

}
/* Finds the Process PID location in a queue before the insertion point.
 * NULL if insertion is before head. 
 */
Process *findPIDAscending(Queue *queue, Process *process){
	Process *current = (queue->head);
	int pid = process->pid; 
	
	//Insert before header
	if(pid<(current->pid)){
		return NULL; 
	}

	//Insert after head with one count
	if(queue->count ==1){
		return current; 
	}

	while((current->next)!=NULL){
		if((current->next)->pid>(process->pid)){
			return current; 
		}
		current = (current->next); 
	}
	return current; 
	
}

void insertAscending(Queue *queue,Process *insert){
	//Check if Queue is empty
	//Find the node before the insertion point
	//Checks if insertion is before the head. 
	//Assign local next variable to next in current.
	//Assign the node before to the insertion node. 
	//Assign the inserted node next. 
	//Increment queue. 

	//CASE:Inserts if queue is empty.
	if( scheduler_count(queue)==0){
		(queue->head) = insert;
		(queue->count) = 1;
		return;
		 
	}
	//Current Process, adding insert after.
	//Next: The process after the insert Node.
	Process *current, *next; 
	current = findPIDAscending(queue, insert);
	
	//Insertion before head. 
	if(!current){
		insert->next = (queue ->head);
		queue->head = insert;
		(queue->count)++;
		return;
	}

	//Insertion after head.
	next = (current->next);
	current->next = insert; 
	(insert->next) = next;
	(queue->count)++;
	
	return;	
	
}

/* Add the process into the appropriate linked list.
 * Follow the specification for this function.
 * Returns a 0 on success or a -1 on any error.
 */
int scheduler_add(Schedule *schedule, Process *process) {
	//Set local variables
	//Check if process is CREATED, READY, or RUNNING
	//	TRUE:
	//		Insert the node in PID order
	//		Set the flags

  	int flag = (process->flags);
	Queue *ready = (schedule->ready_queue); 
	
	//CASE: Created State
	if(((flag&CREATED)>>1)==1){
		insertAscending(ready, process); 
		
		flag = (flag^CREATED); 
		flag = (flag|READY); 
		
		process->flags = flag; 	
		return 0;  
	}
	//CASE: Ready State
	if(((flag&READY)>>2)==1){	
		insertAscending(ready, process);	
	
		flag = (flag|READY); 
			
		process->flags = flag; 	
		return 0; 
	}
	
	//CASE: Running State
	if(((flag&RUNNING)>>3)==1){
		insertAscending(ready, process);	
		flag = (flag^RUNNING); 
		flag = (flag|READY);
		 
		process->flags = flag;
		return 0; 	
	}
	return -1; 
}

/* Receives the process from the CPU and puts it into the Waiting Queue
 * Follow the specification for this function.
 * Returns a 0 on success or -1 on any error.
 */
int scheduler_wait(Schedule *schedule, Process *process, int io_time) {
	//Check for ERROR Cases
	//Set the flag
	//Insert Node at the end of list
	//Increment wait's count
	
	//ERROR CASES: One of the parameters is NULL.
	if(!schedule || !process){
		return -1; 
	}
	//CASE: Process is not running
	if(((process->flags)&RUNNING) ==0){
		printf("Hex %x", process->flags);
		return -1;
	}
	
	//Sets the running flag to 0. `
	(process->flags) = (process->flags)^RUNNING;
	//Sets the wait flag to 1.
	(process->flags) = (process->flags)|WAITING; 
	//Sets the wait remaining field.
	(process->wait_remaining)= io_time;

	//Inserts Process into queue.	
	Queue *wait = (schedule->waiting_queue); 
	Process *current = (wait->head);
	
	//CASE: Empty queue.
	if(!current){
		(wait->head) = process;
		(wait->count)++;
		return 0;		
	}
	
	//CASE: Non-empty queue.
	
	//Finds last process in list. 
	while(current->next != NULL){
		current = current->next; 
	}
	//Inserts into end of queue.
	(current ->next) = process;
	(wait->count)++;	
	return 0;
}

/* Receives a process from the CPU and moves it to the stopped queue.
 * Follow the specification for this function.
 * Returns a 0 on success or a -1 on any error.
 */
int scheduler_stop(Schedule *schedule, Process *process) {
	//Check for ERROR cases
	//Set the flags
	//Insert the process into Queue
	
	//CASE: One of the parameters are null.
	if(!schedule ||!process){
		return -1;
	}
	//CASE: Process is not ready.
	if(((process->flags)&RUNNING)==0){
		return -1;
	}
  	//Set the flags of process.  	
	(process->flags)=(process->flags)^RUNNING;
	(process->flags)=(process->flags)|STOPPED;
	
	Queue *stop = (schedule->stopped_queue);

	insertAscending(stop, process);
	
	return 0;
}

/* Move the process with matching pid from Stopped to Ready.
 * Follow the specification for this function.
 * Returns a 0 on success or a -1 on any error.
 */
int scheduler_continue(Schedule *schedule, int pid) {
	//Check for ERROR cases
	//	Check if schedule is valid
	//	Check if stop is empty
	//Check head if the process is being searched for.
	//Iterate through process to find the pid Node.
	//	Remove the Node from the Queue
	//	Decrement Queue
	//	Set flags
	//	Add it to Ready Queue
	
  	//ERROR CASE: Null schedule
	if(!schedule){
		return -1;
	}
	Queue *stop = (schedule->stopped_queue);
	
	//ERROR CASE: Empty queue.
	if(stop->count ==0){
		return -1;
	}

	//prev - Node right before maching Process
	//Current - Node that's being returned. 	
	Process *prev= (stop->head), *current = prev;

	//Checks if Header Node is the Process in question
	if((prev->pid)==pid){
		//Sets the head to the 2nd list of queue
		//Decreements stop queue
		//Change the flag value to READY
		//Add it to ready queue. 
		(stop->head)=(prev->next);
		(stop->count)--;
		(prev->flags)=((prev->flags)^STOPPED)|READY;
		scheduler_add(schedule,prev); 
		return 0;
	}
	
	//Iterate through stop list to check for PID. 	
	while(current->next != NULL){
		//Process Found
		if(((current->next)->pid) == pid){
			//Sets the previous Node to the matching PID.
			//Sets the current Node as the return value.
			//Sets the previous Node's next to the node after the found node. 
			//Change process flag value
			//Decrement List
			//Add it to Ready Queue
			prev = current;
			current = (current->next);

			(prev->next) = (current->next); 
			(current->flags)=((current->flags)^STOPPED)|READY; 
			(stop->count)--;

			scheduler_add(schedule, current); 
			return 0;
		}
		
	}
	
	return -1;
}

/* Receives the process from the CPU for removal.
 * Follow the specification for this function.
 * Returns its exit code (from flags) on success or a -1 on any error.
 */
int scheduler_finish(Process *process) {
	//Store the xit_code
	//Free next
	//Free Command
	//Free current process
	//Return exit_code
	

	if(!process){
		return -1; 
	}

	//int is_sudo = (process->flags)& SUDO;
	//Flags with all of the flag values 
	int exit_code =((process->flags)>>7);
 
	//exit_code = (exit_code & (~RUNNING)) | TERMINATED; 	

	free(process->next);
	free(process->command);
	free(process); 
	printf("exit Code %d", exit_code);
	return exit_code; 
}

/* Create a new Process with the given information.
 * - Malloc and copy the command string, don't just assign it!
 * Follow the specification for this function.
 * Returns the Process on success or a NULL on any error.
 */
Process *scheduler_generate(char *command, int pid, int time_remaining, int is_sudo) {
	//Create Process
	//Dynamically allocate memory for char command
	//Copy command paramter into C
	//Copy other int parameters
	//Set sudo bit if applicable 
	//Set flags
	//Return address
  	Process *gen = malloc(sizeof(Process)); 
	if(!gen){
		return NULL; 
	}

	gen->command = (char *) malloc(strlen(command) + 1); 
	strncpy(gen->command, command,strlen(command) +1); 
	gen->pid = pid;
	gen->time_remaining = time_remaining;
	gen->time_last_run = clock_get_time(); 
	gen->wait_remaining = 0; 
	gen -> next = NULL;

	int flag = 0; 
	if(is_sudo==1){
		flag= flag|SUDO; 
	}
	flag = flag | CREATED; 
	//printf("\n%x\n", flag); 
	gen->flags = flag; 

	return gen;   
}

/* Select the next process to run from Ready Queue.
 * Follow the specification for this function.
 * Returns the process selected or NULL if none available or on any errors.
 */
Process *scheduler_select(Schedule *schedule) {

	//Checks for errors
	//Set the Node for Least time to be head
	//Temporary instance variable of (leastTime)
	//Checks if queue has 1 element.
	//Iterate through Queue.
	//Check for Time Starvation First
	//Assigns leastTime node if applicable. 
	//Assign appropriate variables.
	
	//Schedule is NULL.
	if(!schedule){
		return NULL; 
	}

  	Queue *ready = (schedule ->ready_queue);
	
	//Empty queue. 
	if((ready->count)==0){
		return NULL; 
	}


	//Process with the lead time remaining.
	Process *leastTime = (ready->head);
	//The minimum time of current process. 
	int minTime = (leastTime->time_remaining);

	//If 1 item in ready queue. 
	if(scheduler_count(ready)==1){
		leastTime = ready->head;
		(ready->count)--;
		ready->head = NULL;
		leastTime->flags = ((leastTime->flags)&(~READY))|RUNNING;
		return leastTime; 
	}

	//If Process
	Process *prev = (ready->head); 	
	Process *current = prev;

	//CASE: Head Process >= TIME_STARVATION 
	if((clock_get_time() - (prev->time_last_run))>= TIME_STARVATION){
		ready->head = current->next;
		(ready->count)--; 
		return prev;
	}
	//Iterate through the ready queue. 
	while((current->next) != NULL){
		//Checks for time starvation
		if((clock_get_time() - ((current->next)->time_last_run))>=TIME_STARVATION){
			(current->next) = (current->next)->next;
			(ready->count)--;
			return current;  		
		}
		//Checks for minimum time
		int currentTime = (current->next)->time_remaining;
		
		if(currentTime< minTime){
			prev = current;
			leastTime = (current->next); 
			minTime = currentTime; 
		}
		current = (current->next);
	}
	//Sets the node previous to Least time, to leastTime next.
	(prev->next) = leastTime->next; 
	
	//Sets leastTime next to NULL
	leastTime->next = NULL; 
	//Sets the ready bit to 0;

	(leastTime->flags) = (leastTime->flags)&(~READY); 		
	(leastTime->flags) = (leastTime->flags)|RUNNING; 
	
		
	(ready->count)--;
	return leastTime;
}

/* Updates the first process in the waiting queue by decrementing waiting_time.
 * If that process is done (waiting_time is 0), move it to the ready queue.
 * Follow the specification for this function.
 * Returns the process selected or NULL if none available or on any errors.
 */
int scheduler_io_run(Schedule *schedule) {
	//Check for ERROR cases
	//If wait queue EMPTY, then return 0.
	//Decrement head value of wait_remaining
	//Checks if wait_remaining is 0 afterwards
	//	TRUE
	//	Assigns flags
	//	Assign Wait queue's head to 2nd element in list
	//	Decrement wait queue's count
	//	Add it to Ready Queue. 

	//ERROR CASE: Schedule is NULL
	if(!schedule){
		return -1;
	}
	
	//ERROR CASE: Empty queue.
	Queue *wait = schedule->waiting_queue;
	if(scheduler_count(wait)==0){
		return 0;
	}
	
	//The element being Processed. 
	Process *head = (wait->head);
	(head->wait_remaining)--;
	
	//Wait_remaing time is 0.
	if((head->wait_remaining)==0){
		//Set flags.
		(head->flags) = ((head->flags)^WAITING)|READY;
		
		//Sets the Queue's head to next node and sets the old header's next to null.
		(wait->head)= (head->next);
		head->next = NULL;
		(wait->count)--;
		//ERROR CASE: Error in adding. 
		if(scheduler_add(schedule, head)==-1){
			return -1;
		}
		

	}
  	return 0;
}

/* Returns the number of items in a given Linked List (Queue) (Queue)
 * Follow the specification for this function.
 * Returns the count of the Linked List, or -1 on any errors.
 */
int scheduler_count(Queue *ll) {
	if(!ll) {
		return -1; 
	}
	return (ll->count); 
}
/** Recursively iterates through a Process
 *  and Free the process's command then 
 *  free the process itself.
 */
void deleteProcess(Process *head){
	
	if(head->next != NULL){
		deleteProcess(head->next);
	}
	free(head->command);
	free(head);
}

/* Completely frees all allocated memory in the scheduler
 * Follow the specification for this function.
 */
void scheduler_free(Schedule *scheduler) {
	//Checks the queue if there are any process present.
	//Delete each process.
	//Free each queue
	//Free scheduler. 
	
	//Delete every process of the queue.
	Queue *ready = scheduler->ready_queue;  		
	if((ready->count)!=0){
		deleteProcess(ready->head);
	}	
	Queue *wait = scheduler->waiting_queue;  		
	
	if((wait->count)!=0){
		deleteProcess(wait->head);
	}	
	Queue *stopped = scheduler->stopped_queue;  		
	if((stopped->count)!=0){
		deleteProcess(stopped->head);
	}

	//Free the queue.
	free(ready);
	free(wait);
	free(stopped);

	//Free the scheduler.
	free(scheduler);
	return;
}

