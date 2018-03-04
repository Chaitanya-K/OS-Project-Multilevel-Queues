/*
*	AUTHOR : Chaitanya Kumar
*	A simple C program to simulate a multilevel queue
*	for a set of processes, given their details
*/

#include <stdio.h>
#include <stdlib.h>
#define SIZE 50

/*
*	Structure to hold all relevant process details
*	Arrival time, Burst time and Turnaround time all in milliseconds
*	Priority ranges from 0 to 9, 0 being the highest priority
*/

typedef struct process
{
	int pid;
	int ar_time;
	int bt_time;
	int wt_time;
	int algo;
	int pri;
}PROCESS;

/*
*	PROCESS arrays created for all scheduling algorithms
*	Count variables to keep track of the number of processes in each algorithm
*/

PROCESS QFCFS[10];
PROCESS QSJF[10];
PROCESS QSRTF[10];
PROCESS QNPP[10];
int count1 = 0, count2 = 0, count3 = 0, count4 = 0, time=0;

/*
*	Scheduling algorithm: First Come First Serve
*	Serves requests in order of their arrival
*/

void FCFS()
{
	int i, j;
	for (i=count1-1; i>=0; i--)
	{
		for (j=0; j<i; j++)
		{
			if (QFCFS[j].ar_time > QFCFS[j+1].ar_time)
			{
				PROCESS temp = QFCFS[j];
				QFCFS[j] = QFCFS[j+1];
				QFCFS[j+1] = temp;
			}
		}
	}
	printf("\nQUEUE 1: First Come First Serve:\n");
	for (i=0; i<count1; i++)
	{
		printf("\nfrom %d to %d: process %d", time, time + QFCFS[i].bt_time, QFCFS[i].pid);
		time+=QFCFS[i].bt_time;
	}
}

/*
*	Scheduling algorithm: Shortest Job First
*	Serves the process with the shortest burst time in the queue at that instant.
*	No preemption
*/

void SJF()
{
	int i, j;
	printf("\nQUEUE 2: Shortest Job First:\n");
	int time1=0;
	for (i=0; i<count2; i++)
	{
		time1+=QSJF[i].bt_time;
	}
	int min , pos, ct = 0;
	i=0;
	while (i<time1)
	{
	    min = 9999;
		for (j=ct;j<count2; j++)
		{
			if (QSJF[j].ar_time <= i && QSJF[j].bt_time < min)
			{
				min = QSJF[j].bt_time;
				pos = j;
			}
		}
		printf("\nfrom %d to %d: process %d", i+time, i+min+time, QSJF[pos].pid);
		PROCESS temp = QSJF[ct];
		QSJF[ct] = QSJF [pos];
		QSJF[pos] = temp;
		ct++;
		i+=min;
	}
	time+=time1;
}

/*
*	Scheduling algorithm: Shortest Remaining Time First
*	Serves the process which has the least reaining burst time at that instant
*	Preemptive form of Shortest Job First Algorithm
*/

void SRTF()
{
	printf("\nQUEUE 3: Shortest Remaining Time First\n");
	int i, j, time1=0;
	for (i=0; i<count3; i++)
	{
		time1 += QSRTF[i].bt_time;
	}
	for (i=count3-1; i>=0; i--)
	{
		for (j=0; j<i; j++)
		{
			if (QSRTF[j].ar_time > QSRTF[j+1].ar_time)
			{
				PROCESS temp = QSRTF[j];
				QSRTF[j] = QSRTF[j+1];
				QSRTF[j+1] = temp;
			}
		}
	}
	i=0;
	int prev = -1;
	while (i<time1)
	{
		int min = 9999, pos;
		for (j=0; j<count3; j++)
		{
			if (QSRTF[j].ar_time<=i && QSRTF[j].bt_time<min && QSRTF[j].bt_time>0)
			{
				min = QSRTF[j].bt_time;
				pos = j;
			}
		}
		if (prev != pos)
		{
			if (prev != -1)
				printf("%d: process %d", i+time, QSRTF[prev].pid);
			printf("\nfrom %d to ", i+time);
		}
		prev = pos;
		QSRTF[pos].bt_time--;
		i++;
	}
	printf("%d: process %d", i+time, QSRTF[prev].pid);
	time+=time1;
}

/*
*	Scheduling algorithm: Non-Preemptive Priority
*	Serves requests in order of their priority
*/

void NPP()
{
    int i, j; 
	printf("\nQUEUE 4: Non Preemptive Priority:\n");
	int time1=0;
	for (i=0; i<count4; i++)
	{
		time1+=QNPP[i].bt_time;
	}
	int min , pos, ct = 0;
	i=0;
	while (i<time1)
	{
	    min = 9999;
		for (j=ct;j<count4; j++)
		{
			if (QNPP[j].ar_time <= i && QNPP[j].pri < min)
			{
				min = QNPP[j].pri;
				pos = j;
			}
		}
		printf("\nfrom %d to %d: process %d", i+time, i+QNPP[pos].bt_time+time, QNPP[pos].pid);
		i+=QNPP[pos].bt_time;
		PROCESS temp = QNPP[ct];
		QNPP[ct] = QNPP [pos];
		QNPP[pos] = temp;
		ct++;
	}
	time+=time1;
}

/*
*	Function to read a process
*	After reading, adds process to the queue of the scheduling algorithm requested
*/

PROCESS readProcess (PROCESS p)
{
	printf("\nEnter Process ID, Arrival Time, Burst Time and Algorithm \n(1 for FCFS, 2 for SJF, 3 for SRTF, 4 for Non-Preemptive Priority)\n");
	scanf("%d %d %d %d", &p.pid, &p.ar_time, &p.bt_time, &p.algo);
	p.pri = 0;
	if (p.algo == 4)
	{
		printf("Enter priority: ");
		scanf("%d", &p.pri);
	}
	p.wt_time=0;
	switch (p.algo)
	{
		case 1:	QFCFS[count1++]=p;
		break;
		case 2:	QSJF[count2++]=p;
		break;
		case 3:	QSRTF[count3++]=p;
		break;
		case 4:	QNPP[count4++]=p;
		break;
		default:printf("\nInvalid selection\n");
	}
	return p;
}

/*
*	Function to display all the necessary details of a process, as entered by the user
*/

void dispProc(PROCESS p)
{
	printf("\nPID: %d\tArrival time: %d\tBurst time: %d\tAlgorithm: %d", p.pid, p.ar_time, p.bt_time, p.algo);
	if (p.algo==4)
    {
        printf("\tPriority: %d", p.pri);
    }
    printf("\n");
}

void main()
{
	int i, n;
	printf("\nEnter the number of processes: ");
	scanf("%d", &n);
	PROCESS parr[n];
	for (i=0;i<n;i++)
	{
		parr[i] = readProcess(parr[i]);
	}
	printf("\nEntered details are: \n");
	for (i=0;i<n;i++)
	{
		dispProc(parr[i]);
	}
	printf("\nPriority between queues:\n1. FCFS\n2. SJF\n3. SRTF\n4. NPP");
	FCFS();
	SJF();
	SRTF();
	NPP();
}

