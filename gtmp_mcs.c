#include <stdlib.h>
#include <stdio.h>
#include <omp.h>
#include "gtmp.h"

/*
    From the MCS Paper: A scalable, distributed tree-based barrier with only local spinning.

    type treenode = record
        parentsense : Boolean
	parentpointer : ^Boolean
	childpointers : array [0..1] of ^Boolean
	havechild : array [0..3] of Boolean
	childnotready : array [0..3] of Boolean
	dummy : Boolean //pseudo-data

    shared nodes : array [0..P-1] of treenode
        // nodes[vpid] is allocated in shared memory
        // locally accessible to processor vpid
    processor private vpid : integer // a unique virtual processor index
    processor private sense : Boolean

    // on processor i, sense is initially true
    // in nodes[i]:
    //    havechild[j] = true if 4 * i + j + 1 < P; otherwise false
    //    parentpointer = &nodes[floor((i-1)/4].childnotready[(i-1) mod 4],
    //        or dummy if i = 0
    //    childpointers[0] = &nodes[2*i+1].parentsense, or &dummy if 2*i+1 >= P
    //    childpointers[1] = &nodes[2*i+2].parentsense, or &dummy if 2*i+2 >= P
    //    initially childnotready = havechild and parentsense = false
	
    procedure tree_barrier
        with nodes[vpid] do
	    repeat until childnotready = {false, false, false, false}
	    childnotready := havechild //prepare for next barrier
	    parentpointer^ := false //let parent know I'm ready
	    // if not root, wait until my parent signals wakeup
	    if vpid != 0
	        repeat until parentsense = sense
	    // signal children in wakeup tree
	    childpointers[0]^ := sense
	    childpointers[1]^ := sense
	    sense := not sense
*/

#define TRUE  1
#define FALSE 0

#define NO_PARENT 99


int floor( int val ){
	// TODO implement floor from math.h	


}


typedef struct _node_t{

	// TODO these may need to be alligned 
	// along cache lines
	int haveChildren[4];
	int childNotReady[4];
	int* childPointer[2];
	int parentSense;
	struct node_t* parentPointer;

} node_t;

static int num_leaves;
static node_t* tree;

//
//	initialize tree structure
//		args: number of threads required for barrier
//
//
//		creates and initializes an array to represent
//		the mcs 4-ary tree and the binary wake up tree
//

void gtmp_init(int num_threads){

	int i, j;
	node_t* curnode;
	num_leaves = num_threads - 1; // user input non-zero indexed

	// allocate appropriate memory for an array of nodes
	tree = (node_t*)malloc(num_nodes * sizeof(node_t));

	for(i = 0; i < num_leaves; i++){
		curnode = &nodes[i];
		// for the potential children in the tree
		// for which there can be 4, find out if the
		// slot will exist within our bounds. 
		// if so, assign it to the "have child array"
		for(j = 0; j < 4; j++){
			if( i * 4 + j + 1 < num_leaves){
				// this will be a child, TRUE
				curnode->haveChildren[j] = TRUE;
			}

			// this whole array should be true
			// even if it doesn't have children
			curnode->childNotReady[j] = TRUE;
		}

		// for parent pointers for a node, check 
		// the index
		if( i == 0) {
			curnode->parentPointer = NO_PARENT;
		}
		else{
			// set your parent pointer 
			//  to point to your position
			// in the "child not ready array"
			// of your parent
			curnode->parentPointer = &tree[ floor((i-1)/4) ]->childNotReady[(i-1)%4];


		}
		
		// for the child pointer array
		// it is binary, only 2 bits long
		// sets childPointer to the pointer to the
		// individual nodes within the tree that are 
		// TODO where are nodes 3 and 4????
		curnode->childPointer[0] = &tree[ 2 * i + 1 ];
		curnode->childPointer[1] = &tree[ 2 * i + 2 ];
		
		// Parent sense 
		parentSense = FALSE;

	}
	
}


//	Barrier
//		Bookkeeping and spin
//

void gtmp_barrier(){

	int itr;

	// need a vpid
	for(itr = 0; itr < 4; itr++){
	// iterate until you hit the first "non-ready"
		while(tree[vpid][j] != FALSE);
	}		
	// when this condition is met, all the children 
	// have hit the barrier. 

	// flip your own sense
	tree[vpid]->parentPointer = FALSE;




}

void gtmp_finalize(){

}
