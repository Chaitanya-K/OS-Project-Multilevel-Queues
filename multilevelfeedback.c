/*
*	AUTHOR : Chaitanya Kumar
*	A simple C program to simulate a multilevel feedback queue
*	for a set of processes, given their details
*/

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#define min(a,b) (a<b?a:b)
/*
*	Structure to hold all relevant process details
*	Arrival, burst and waiting time are all in milliseconds
*	stage ranges from 1 to 3
*/
typedef struct Process
{
	int pid, ar_time, bt_time, wt_time, turn_time, stage;
}Process;

/*
* Queue structure to encapsulate a single process
* and all relevant queue operations under 1 entity
*/
typedef struct Queue
{
	Process process;
	struct Queue *next;
}Queue;

//Global process queue
Queue *processQueue; 
//Global task queue (ready for execution)
Queue *taskQueue;
//Global queue for finished processes
Queue *outQueue;
//Pointers to parts of the queue for O(1) operation
Queue *stage1,*stage2,*stage3;


//Adds the process to the first stage of the queue
void addToStage1(Queue * temp)
{

	//If no queue exists, add to front
	if (taskQueue == NULL)
		taskQueue = temp;

	//If no stage 1 process exists
	if (taskQueue->process.stage > 1)
	{
		temp->next=taskQueue;
		taskQueue=temp;
		stage1=temp;
		return;
	}	
	if (stage1==NULL)
		stage1 = temp;
	else 
	{
		Queue *beg = stage1->next;
		stage1->next = temp;
		temp->next = beg;
		stage1=temp;
	}
}

//Adds the process to the second stage of the queue
void addToStage2(Queue * temp)
{
	//If no queue exists
	if (taskQueue == NULL)
		taskQueue = temp;
	
	//If only stage 3 processes exist, add to front
	if (taskQueue->process.stage > 2)
	{
		temp->next=taskQueue;
		taskQueue=temp;
		stage2 = temp;
		return;
	}	
	
	//If no stage 2 process exists
	if (stage2==NULL)
	{
		if (stage1 != NULL)
		{
			Queue *beg = stage1->next;
			stage1->next = temp;
			temp->next=beg;
		}
		stage2 = temp;
	}
	else
	{	
		Queue *beg = stage2->next;
		stage2->next = temp;
		temp->next = beg;
		stage2=temp;
	}
}

//Adds the process to the third stage of the queue
void addToStage3(Queue * temp)
{
	//If no queue exists
	if (taskQueue == NULL)
		taskQueue = temp;
	
	//If no stage 3 process exists
	if (stage3==NULL)
	{
		if (stage2 != NULL)
			stage2->next = temp;
		else if (stage1!=NULL)
			stage1->next = temp;
		stage3 = temp;
	}
	else
	{
		Queue *beg = stage3->next;
		stage3->next = temp;
		temp->next = beg;
		stage3=temp;
	}
}

//Dequeues from the task queue
Queue *removeFromTaskQueue()
{
	if (taskQueue!=NULL)
	{
		if (stage1==taskQueue)
			stage1=NULL;
		if (stage2==taskQueue)
			stage2=NULL;
		if (stage3==taskQueue)
			stage3=NULL;

		Queue *temp = taskQueue;
		taskQueue = taskQueue->next;
		temp->next = NULL;
		return temp;	
	}
	return NULL;
}

/*
*	Inserts a process into the processQueue, based on arrival time, with higher arrival times going further in,
*	clashes are resolved on a FCFS basis
*	INPUT : A single Queue object
*/
void addToProcessQueue (Queue *temp)
{
	if (processQueue == NULL)
		processQueue = temp;
	//If the new node is to be the first
	else if (processQueue->process.ar_time > temp->process.ar_time)
	{
		temp->next=processQueue;
		processQueue=temp;
	}
	else
	{
		Queue *curr=processQueue;
		while (curr->next!=NULL && (curr->next->process.ar_time <= temp->process.ar_time))
			curr=curr->next;
		temp->next=curr->next;
		curr->next=temp;
	}
}

//Dequeues a process object from the front of the queue
Queue* removeFromProcessQueue ()
{
	if (processQueue!=NULL)
	{
		Queue *temp=processQueue;
		processQueue=processQueue->next;
		temp->next=NULL;
		return temp;	
	}
	return NULL;
}

//Function to add to the outgoing queue to print later, sorts by PID
Queue* addToOutQueue(Queue *temp)
{
	if (outQueue == NULL)
		outQueue = temp;

	else if (outQueue->process.pid > temp->process.pid)
	{
		temp->next=outQueue;
		outQueue=temp;
	}
	else
	{
		Queue *curr=outQueue;
		while (curr->next!=NULL && (curr->next->process.pid <= temp->process.pid))
			curr=curr->next;
		temp->next=curr->next;
		curr->next=temp;
	}
}


// Takes input for a process object passed to it by reference, initializes wt_time and stage
void readProcess(Process *process)
{
	printf("Enter PID, Arrival Time and Burst Time\n");
	scanf("%d %d %d",&process->pid,&process->ar_time,&process->bt_time);
	process->wt_time=0;
	process->turn_time = process->bt_time;
	process->stage = 1;
}

/*
*	Displays the relevant details of one instance of a Process object, passed to it as a pointer
*	INPUT : Pointer to a Process object
*/
void displayProcess(Process *process)
{
	printf("PID: %d\tAr_time: %d\tWait Time: %d\tBurst Time: %d\tTurnaround Time: %d\tBurst Time: %d\n",
		process->pid, process->ar_time,	process->wt_time, process->turn_time,
		 process->turn_time + process->wt_time, process->bt_time);
}

/*  
*	Function that takes a linked list of Process objects sorted by arrival time 
*	Implements round robin scheduling based on user given time quantums (in milliseconds)
* 	for 2 stages, then uses FCFS for the final stage
*	Prints a Gantt chart, with a line per time quantum slot
*	Calculates average waiting time and average turnaround time
*	Uses a pair Queues implemented through linked lists with intermediate pointers
*	INPUT: Number of processes size
*/
void multilevelFeedbackQueue(int size)
{

	int timeSlice1, timeSlice2;
	printf("\nEnter the time quantum for stage 1 : ");
	scanf("%d", &timeSlice1);
	printf("\nEnter the time quantum for stage 2 : ");
	scanf("%d", &timeSlice2);
	int stage2timer=timeSlice2;

	float avgWaitingTime = 0.0, avgTurnaroundTime = 0.0;
	//Iterator variables
	int j=0;
	//Variables to keep track of time and current time slot 
	int t=0,t_beg,t_fin; 
	bool flag;

	Queue *temp, *curr;
	//Start by pushing the 1st process to the 
	t+=processQueue->process.ar_time;
	addToStage1(removeFromProcessQueue());
	//Average turnaround time is between first and last arrival
	avgTurnaroundTime -= t;

	//Main loop runs until the processQueue is empty
	while (taskQueue!=NULL)
	{		
		//Decides which stage to allocate process to
		switch (taskQueue->process.stage)
		{
			case 1:
				t_beg = t;
				//Checks if remaining time is lower than the allotted time quantum
				t+=min(timeSlice1, taskQueue->process.bt_time);
				t_fin = t;
				printf("%d to %d: %d\t-- Stage %d\n",t_beg, t_fin, taskQueue->process.pid, taskQueue->process.stage);
				taskQueue->process.bt_time -= t_fin - t_beg;
				taskQueue->process.stage = 2;
				
				//Adds the waiting time to all processes already waiting in the taskQueue 
				curr=taskQueue->next;
				while (curr!=NULL)
				{
		            curr->process.wt_time += (t_fin - t_beg);
					avgWaitingTime += (t_fin - t_beg);
					curr=curr->next;
				}
				temp=removeFromTaskQueue();

				//Adds any processes that have come in the time slot just processed (with appropriate waiting time)
				while (processQueue!=NULL && processQueue->process.ar_time <= t)
				{
					addToStage1(removeFromProcessQueue());
					stage1->process.wt_time += (t_fin - stage1->process.ar_time);
					avgWaitingTime += (t_fin - stage1->process.ar_time);
				}

				//If the process that just got serviced is not finished, adds it to the next stage of the processQueue
				if (temp->process.bt_time != 0)
					addToStage2(temp);
				else
					addToOutQueue(temp);
				break;

			case 2:
				flag = false;
				t_beg = t;
				
				//Checks if remaining time is lower than the allotted time quantum
				if (processQueue!=NULL)
					t+=min(min(timeSlice2, taskQueue->process.bt_time), min((processQueue->process.ar_time - t), stage2timer));
				else
					t+=min(min(timeSlice2, taskQueue->process.bt_time),stage2timer);
				t_fin = t;

				stage2timer -= t_fin-t_beg;

				printf("%d to %d: %d\t-- Stage %d\n", t_beg, t_fin, taskQueue->process.pid, taskQueue->process.stage);
				taskQueue->process.bt_time -= t_fin - t_beg;
				
				//Adds the waiting time to all processes already waiting in the taskQueue 
				curr=taskQueue->next;
				while (curr!=NULL)
				{
		            curr->process.wt_time += (t_fin - t_beg);
					avgWaitingTime += (t_fin - t_beg);
					curr=curr->next;
				}

				if (stage2timer == 0 || taskQueue->process.bt_time == 0)
				{
					temp=removeFromTaskQueue();
					flag=true;
				}

				//Adds any processes that have come in the time slot just processed (with appropriate waiting time)
				while (processQueue!=NULL && processQueue->process.ar_time <= t)
				{
					addToStage1(removeFromProcessQueue());
					stage1->process.wt_time += (t_fin - stage1->process.ar_time);
					avgWaitingTime += (t_fin - stage1->process.ar_time);
				}

				//If the process that just got serviced is not finished, adds it to the end of the processQueue
				if (flag)
				{
					if (stage2timer==0 && temp->process.bt_time != 0)
					{
						temp->process.stage = 3;
						addToStage3(temp);
						stage2timer = timeSlice2;
					
					}
					else if (temp->process.bt_time == 0)
					{
						stage2timer = timeSlice2;
						addToOutQueue(temp);
					}
				}
				break;
			
			case 3:
				t_beg = t;
				if (processQueue!=NULL)
					t+=min(taskQueue->process.bt_time, processQueue->process.ar_time-t);
				else
					t+=taskQueue->process.bt_time;
				t_fin = t;
				
				printf("%d to %d: %d\t-- Stage %d\n",t_beg, t_fin, taskQueue->process.pid, taskQueue->process.stage);
				taskQueue->process.bt_time -= t_fin - t_beg;
				
				//Adds the waiting time to all processes already waiting in the taskQueue 
				curr=taskQueue->next;
				while (curr!=NULL)
				{
		            curr->process.wt_time += (t_fin - t_beg);
					avgWaitingTime += (t_fin - t_beg);
					curr=curr->next;
				}
				
				//Adds any processes that have come in the time slot just processed (with appropriate waiting time)
				while (processQueue!=NULL && processQueue->process.ar_time <= t)
				{
					addToStage1(removeFromProcessQueue());
					stage1->process.wt_time += (t_fin - stage1->process.ar_time);
					avgWaitingTime += (t_fin - stage1->process.ar_time);
				}	
				
				if (taskQueue->process.bt_time == 0)
					addToOutQueue(removeFromTaskQueue());
				break;				
		}
	}

	//Computes turnaround time at the end using waiting time and arrival time of 1st process
	avgTurnaroundTime += avgWaitingTime + t;

	avgWaitingTime /= size;
	avgTurnaroundTime /= size;
	printf("\nAvg Wait Time : %0.3f\tAvg Turnaround Time :  %0.3f\n\n",avgWaitingTime,avgTurnaroundTime);

}


int main()
{
	int size,i,menu=0;
	printf("Enter the number of processes : ");
	scanf("%d",&size);

	//Initializing the queue variables
	processQueue=NULL;
	taskQueue=NULL;
	stage1=stage2=stage3=NULL;
	
	//Taking input and adding to process queue
	for (i=0; i<size ;i++)
	{
		Queue *temp = (Queue *)malloc(sizeof(Queue));
		readProcess(&temp->process);
		temp->next=NULL;
		addToProcessQueue(temp);	
	}
	
	multilevelFeedbackQueue(size);

	//Displays the final process details
	Queue *curr = outQueue;
	while(curr!=NULL)
	{
		displayProcess(&curr->process);
		curr = curr->next;
	}
	return 0;
}
