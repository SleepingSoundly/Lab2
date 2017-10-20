#include <stdlib.h>
#include <stdio.h>
#include <omp.h>
#include "gtmp.h"

/*
    From the MCS Paper: A sense-reversing centralized barrier

    shared count : integer := P
    shared sense : Boolean := true
    processor private local_sense : Boolean := true

    procedure central_barrier
        local_sense := not local_sense // each processor toggles its own sense
	if fetch_and_decrement (&count) = 1
	    count := P
	    sense := local_sense // last processor toggles global sense
        else
           repeat until sense = local_sense
*/
#define TRUE 1
#define FALSE 0


int count;
int sense; // so we don't have to include bools, the senses are implemented in binary. 
int local_sense; // 1 is TRUE, 0 is FALSE
int numThreads;

// may need to implement fetch_and* with 
// open MP, #pragma omp atomic makes an action atomic. 
// maybe this applies to functions, not just lines, not sure

int fetch_and_decrement( int* count_ptr){
	int old_count;

	#pragma omp critical
	{
		old_count = (int)(*count_ptr);
		(*count_ptr)--;
	}
	return old_count;
}


void gtmp_init(int num_threads){
	// initialize two global variables, one for the count
	// and one for the "sense, so we're only checking one sense at a time? 
	
//	numThreads = calloc(1, sizeof(int));
//	sense = calloc(1, sizeof(int));
//	local_sense = calloc(1, sizeof(int));
//	count = calloc(1, sizeof(int));		

	numThreads = num_threads--;
	count = num_threads--;
	sense = TRUE; 
	local_sense = TRUE;
	printf(" -- initialized barrier for #%d threads\n", count);
}

void gtmp_barrier(){
	int oldcount;
	// set local_sense to false, it will invalidate other copies of the sense 
	// but we're only reading 'sense' so it mitigates contention
	local_sense = FALSE;
	oldcount = fetch_and_decrement(&count);
	if (oldcount == 0){
		count = numThreads;
		sense = FALSE; 
	}
	else{
		printf(" --- waiting w/ count %d\n", oldcount);
		while( sense != local_sense );
	}
// return once the sense has been flipped


}

void gtmp_finalize(){
	// free up global variables
//	free(numThreads);
//	free(count);
//	free(sense);
//	free(local_sense);	
	printf(" Finished with barrier, exiting\n");
}
