#include	<stdio.h>
#include	<stdlib.h>
#include	<math.h>

#include	"allvars.h"
#include	"proto.h"

void gravity_tree(void){

	set_softenings();
	
	if(TreeReconstructFlag){

		force_treebuild(NumPart);

		TreeReconstructFlag	=	0;
	}

	int	i;

	printf("Calculating short range force using tree method...\n");

	for(i = 0; i < NumPart; ++i){
		if(P[i].Ti_endstep == All.Ti_Current){
			force_treeevaluate_shortrange(i);
		}
	}

	printf("done!\n");

}

void force_treeevaluate_shortrange(int target){

}

void force_treeallocate(int maxnodes, int maxpart){

}

void force_treebuild(int npart){

}
