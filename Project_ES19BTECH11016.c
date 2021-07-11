/* The program takes too long to run for large number of vertices */ 

#include <stdio.h>								//Including the preprocessor directives
#include <stdlib.h>
#include <time.h>


#define MAX_VERTICES 10000						//This program takes a lot of time to run for 10000 nodes and 3000 edges
#define MAX_EDGES 500
#define MAX_DAYS 300

struct neighbour								//Declaring a structure to store neighbours of a particular node
{
	int neighbour_id;							//Stores the id of the neighbour
	
	struct neighbour * next;					//Stores the address of the next neighbour
};

struct person									//Structure for listing the vertices
{
	int id;										//Stores the id of a particular node 
	
	struct neighbour *neighbour;				//Stores address of the head of neighbour list
	
	int pred_inf_time;							//Stores the predicted infection time
	
	int rec_time;								//Stores recovery time
	
	char status;								//Stores the status of the node
	
	int number_of_neighbours;					//Stores the number of neighbours of a particular node
};

struct event									//Structure for the priority queue
{
	int time;									//Stores the event time 
	
	char action;								//Stores the action, whether transmit or recover
	
	struct person * node;						//Stores the address of a particular node
	
	struct event * next;						

};

void fast_SIR(struct person * human[], int initial_infecteds[], int total_people, int infected_people, int t_max);							//Declaring functions

void process_trans_SIR(struct event * heap[], int count, struct person * human[], int node_id, int time, int times[], int total_people, int S[], int I[], int R[], int t_max);

void find_trans_SIR(struct event * heap[], int count, struct person * human[], struct person * source, int time, int neighbour_id, int t_max);

void process_rec_SIR(struct event * heap[], int count, struct person * u, int time, int times[], int vertices, int S[], int I[], int R[]);

void push(struct event * heap[], int count, struct person * u, int times, char act);

void pop(struct event * heap[], int count);

void add_neighbour(struct person * human, int id);

void print_result(int times[], int S[],int I[],int R[]);

int probability();

int check_heap(struct event * heap[], struct person * human[], int id, int count);

int S_List, I_List, R_List, times_count;										//Counters to keep count of the lists

int rep_check;

int Days = 1;

int heap_count = 1;																//Counter to count the number of elements in the heap


int main()
{
	srand(time(NULL));																//Time seed for the rand function
	
	int v, e, i, j, r, k=0, t_max;											//Declaring the variables
	
	v = (rand() % MAX_VERTICES) + 1 ;									//Generating random number vertices
	
	e = (rand() % MAX_EDGES) + 1;												//Generating random number of edges
	
	struct person * human[v];													//This array stores the addresses of the vertices
	int mat[v];
	int rand_infecteds;
	
	for(i=0;i<v;i++)															//Initialising the array
	{
		human[i] = (struct person *)malloc(sizeof(struct person));
		human[i]->id = i;
		human[i]->number_of_neighbours = 0;
		human[i]->neighbour = (struct neighbour *)malloc(sizeof(struct neighbour));
		human[i]->neighbour->next = NULL;
	}
	
	for(i=0;i<v;i++)																		//Generating a random undirected graph
	{
		if(human[i]->number_of_neighbours < e)
		{
			for(j=i;j<v;j++)
			{
				if(i == j)																	//A vertex cannot be its own neighbour
				{
					continue;
				}
				
				if(probability())																//If the probability() ufnction returns 1, a particu;ar node becomes a neighbour
				{
					if(human[i]->number_of_neighbours < e && human[j]->number_of_neighbours < e)
					{
						add_neighbour(human[i], human[j]->id);
						human[i]->number_of_neighbours++;
						add_neighbour(human[j], human[i]->id);
						human[j]->number_of_neighbours++;
					}
				}
			}
		}
	}

	for(i=0;i<v;i++)
	{
		mat[i] = -1;
	}
	
	rand_infecteds = (rand() % v) + 1;
	
	for(i=0;i<rand_infecteds;i++)
	{
		if(probability())
		{
			mat[k] = human[i]->id;
			k++;	
		}
	}
	int initial_infecteds[k];														//Generating a random list infecteds
	for(i=0;i<k;i++)
	{
		initial_infecteds[i] = mat[i];
	}
	
	t_max = MAX_DAYS;
	fast_SIR(human, initial_infecteds, v, k, t_max);								//Calling the fast_SIR function
	
	return 0;
}

int probability()																//Returns 1 if the number generated is less than or equal to 49
{
	int r;
    
    r = rand() % 100;
    if(r > 49)
    {
    	return 0;
    }
    return 1;
}

void add_neighbour(struct person * human, int id)							//Adds neighbour to the neighbours list
{
	struct neighbour * new_node;
	struct neighbour *temp;
	
	new_node = (struct neighbour *)malloc(sizeof(struct neighbour));
	
	temp = human->neighbour;
	
	while(temp->next != NULL)
	{
		temp = temp->next;
	}
	
	temp->neighbour_id = id;
	
	temp->next = new_node;
	
	new_node->next = NULL;
}

void fast_SIR(struct person * human[], int initial_infecteds[], int total_people, int infected_people, int t_max)
{
	int i;
	int days[1000 * t_max];
	int S[total_people], I[total_people], R[total_people]; 
	struct event * heap[total_people];												//Declaring an array for heap
	
	heap[0] = NULL;																		//The first element is initialised to NULL
	
	for(i=0;i<total_people;i++)															//Adds elements to the S list, initially everyone is susceptible
	{
		human[i]->status = 'S';
		human[i]->pred_inf_time = 3000;
		S[i] = human[i]->id;
		S_List++;
	}
	
	printf("Day 0: Number of Susceptible: %d , Number of Infected: %d, Number of Recovered: %d\n", S_List, I_List,R_List);			//Prints the initial list
	printf("SUSCEPTIBLE: ");
	for(i=0;i<S_List;i++)
	{
		printf("%d ", S[i]);
	}
	printf("\n");
	printf("INFECTED: ");
	for(i=0;i<I_List;i++)
	{
		printf("%d", I_List);
	}
	printf("\n");
	printf("RECOVERED: ");
	for(i=0;i<R_List;i++)
	{
		printf("%d", R_List);
	}
	printf("\n\n");
	
	for(i=0;i<infected_people;i++)											//Pushes the infected into the queue
	{
		human[initial_infecteds[i]]->pred_inf_time = 0;
		push(heap, heap_count, human[initial_infecteds[i]], 0, 'T');
		heap_count++;			
	}
	
	while(heap[1] != NULL)
	{
		if(heap[1]->action == 'T')									//If the node transmits
		{
			if(heap[1]->node->status == 'S')							//If the node is susceptible 
			{
				process_trans_SIR(heap, heap_count, human, heap[1]->node->id, heap[1]->time, days, total_people, S, I, R, t_max);			//Calls the process_trans_SIR() function
			}
			else										//If the node is Infected comes out of the while loop
			{
			    break;
			}
		}
		
		else
		{
			process_rec_SIR(heap, heap_count, heap[1]->node, heap[1]->time, days, total_people, S, I, R);			//Calls process_rec_SIR() function if action is recover
		}
	}
	print_result(days, S, I , R);
}

void process_trans_SIR(struct event * heap[], int count, struct person * human[], int node_id, int time, int times[], int total_people, int S[], int I[], int R[], int t_max)
{
	int day_count, i, r, j;
	day_count = 1;													//Counts the number of days
	struct person * u;
	struct neighbour *temp;

	u = human[node_id];												
	
	if(rep_check != time)								//This if condition is for checking multi-tasks in a single day
	{
		rep_check = time;
		if(times_count < t_max)							//The times[] can be accessed if times_count is less than maximum days
		{
			times[times_count] = time;
			times_count++;
		}
		print_result(times, S, I , R);
	}
	
	for(i=0;i<S_List;i++)										//Removes an element from the susceptible list  
	{
		if(S[i] == u->id)
		{
			for(j=i;j<S_List - 1;j++)
			{
				S[j] = S[j+1];
			}
			S[S_List-1] = -1;
			S_List--;
			break;
		}
		
	}
	
	I[I_List] = u->id;											//Adds element to the infected list 
	I_List++;
	
	u->status = 'I';											//Status of the node is changed to infected 
	
	r = rand() % 10000;
		
	while(r > 2000)													//The recovery time is calculated using the probability, the probabilty is 20%
	{
		r = rand() % 10000;
		day_count++;
	}

	u->rec_time = time + day_count;
	if(u->rec_time < t_max)										//It is added to the list only if the recovery time is less than the maximum days
	{
		pop(heap, heap_count - 1);
		heap_count--;
		push(heap, heap_count, u, u->rec_time, 'R');
		heap_count++;
	}
	
	temp = u->neighbour;											
	
	while(temp->next != NULL)													//Infects the neighbours
	{
		if(check_heap(heap, human, temp->neighbour_id, heap_count))
		{
				find_trans_SIR(heap, heap_count, human, u, time, temp->neighbour_id, t_max);			//Calls the find_trans_SIR() function
		}
		temp = temp->next;
	}
	
}

void find_trans_SIR(struct event * heap[], int count, struct person * human[], struct person * source, int time, int neighbour_id, int t_max)
{
	int day_count = 1, inf_time, r;
	struct person *target;
	target = human[neighbour_id];
	if(target->status == 'S')												//A node is infected only if it is susceptible
	{
		r = rand() % 10000;
			
		while(r > 5000)													//The number of days it takes to get infected is based on probability of 50%
		{
			r = rand() % 10000;
			day_count++;
		}
		
		inf_time = time + day_count;
		
		if(inf_time < source->rec_time && inf_time < target->pred_inf_time && inf_time < t_max)				
		{
			push(heap, heap_count, target, inf_time, 'T');							//Inserting the node into the priority queue
			heap_count++;
			target->pred_inf_time  = inf_time;
		}	
	}
}

void process_rec_SIR(struct event * heap[], int count, struct person * u, int time, int times[], int vertices, int S[], int I[], int R[])
{
	int i, j;
	
	if(rep_check != time)
	{
		rep_check = time;
		if(times_count < MAX_DAYS)							//The times[] can only be accessed if event time is non zero, and times_counter is less than the max days
		{
			times[times_count] = time;
			times_count++;
		}
		print_result(times, S, I , R);
	}
	
	pop(heap, heap_count - 1);									//Deletes from the priority queue
	
	heap_count--;
	
	for(i=0;i<I_List;i++)										//Delete for I list 
	{
		if(I[i] == u->id)
		{
			for(j=i;j<I_List - 1;j++)
			{
				I[j] = I[j+1];
			}
			I[I_List-1] = -1;
			I_List--;
			break;
		}
	}
	
	R[R_List] = u->id;										//Add into R list 
	R_List++;
	
	u->status = 'R';
	
}

int check_heap(struct event * heap[], struct person * human[], int id, int count)					//Checks for repeated elements in the heap
{
	int i;
	
	for(i=1;i<count;i++)
	{
		if(heap[i]->node == human[id])
		{
			return 0;																			//Returns 0 if the element is already present in the heap 
		}
	}
	
	return 1;																					//Returns 1 if the element is not present in the heap
}

void push(struct event * heap[], int count, struct person * u, int times, char act) 
{ 
  	struct event *temp;
  	heap[count] = (struct event *)malloc(sizeof(struct event));							//Stores the passed parameters
  	heap[count]->node = u;
  	heap[count]->time = times;
  	heap[count]->action = act;
  	heap[count]->next = NULL;
  	
	int i = count;

	temp = heap[i];
	
	while(i>1 && temp->time < heap[i/2]->time)							//Inserts in the heap
	{
		heap[i] = heap[i/2];
		i = i/2; 
	}
	
	heap[i] = temp;
} 

void pop(struct event * heap[], int count) 							//Deletes form the heap
{ 
	int i, j;
	struct event *temp;
	
    heap[1] = heap[count];											//The first element is replaced by the last element
    heap[count] = NULL;												//The last element is replaced with NULL
    i = 1; 
    j = i * 2;
    while(j <= count-1)
    {
    	if(j<count-1 && j+1 <= count - 1 && heap[j+1]->time <= heap[j]->time)			//Compares the children of the parent element
    	{
    		j = j+1;
		}
		if(heap[i]->time >= heap[j]->time)										//If parent element is greater than child, they are swapped
		{
			temp = heap[i];
			heap[i] = heap[j];
			heap[j] = temp;
			i = j;
			j = 2 * j;
		}
		else
		{
			break;
		}
	}
} 

void print_result(int times[], int S[],int I[],int R[])										//Prints all the arrays
{
    
	if(times_count >= 1)
	{	
		do
 	   {		
    	    printf("Day %d: ",Days); 																	//Prints the specific day
			printf("Number of Suceptible: %d, Number of Infected: %d, Number of Recovered: %d\n", S_List, I_List, R_List);			//Prints number of elements in respective lists
  	    	int j=0,k=0,l=0;
    	    printf("SUSCEPTIBLE: ");
        	for(j=0;j<S_List;j++)    													//Iterates through the list list and prints the elements
        	{
        	    printf("%d ", S[j]);
        	}
        	printf("\n");
        	printf("INFECTED: ");
        	for(k=0;k<I_List;k++)
        	{
            	printf("%d ", I[k]);
        	}
        	printf("\n");
        	printf("RECOVERED: ");
        	for(l=0;l<R_List;l++)
        	{
        	    printf("%d ", R[l]);
        	}
        	printf("\n\n");

        	Days++;

    	}while(Days < times[times_count - 1]);  												//Prints the lists on the days between two events
	}
}




