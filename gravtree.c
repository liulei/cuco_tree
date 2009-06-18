#include	<stdio.h>
#include	<stdlib.h>
#include	<math.h>

#include	"allvars.h"
#include	"proto.h"

#define	NTAB	1000

static float	shortrange_table[NTAB];

static float	boxsize, boxhalf;

static double	to_grid_fac;

static double	rcut, rcut2, asmth, asmthfac;

static double	h, h_inv, h3_inv;

void set_softenings(void){

	if(All.SofteningHalo * All.Time > All.SofteningHaloMaxPhys){
		All.SofteningTable	=	All.SofteningHaloMaxPhys / All.Time;
	}else{
		All.SofteningTable	=	All.SofteningHalo;
	}

	All.ForceSoftening	=	2.8 * All.SofteningTable;
	
	h	=	All.ForceSoftening;
	h_inv	=	1.0 / h;
	h3_inv	=	h_inv * h_inv * h_inv;
}

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

	MaxNodes	=	maxnodes;
	
	Nodes_base	=	(NODE_BASE *) malloc((MaxNodes + 1) * sizeof(NODE_BASE));

	Extnodes_base	=	(EXTNODE_BASE *) malloc((MaxNodes + 1) * sizeof(EXTNODE_BASE));

	Nodes	=	Nodes_base - NumPart;
	
	Extnodes	=	Extnodes_base - NumPart;

	Nextnode	=	(int *) malloc(NumPart * sizeof(int));

	int		i;
	double	u;

	for(i = 0; i < NTAB; ++i){
		u	=	3.0 / NTAB * (i + 0.5);
		shortrange_table[i]	=	erfc(u) + 2.0 * u / sqrt(M_PI) * exp(-u * u);
	}

	to_grid_fac	=	PMGRID / All.BoxSize;

	rcut	=	All.Rcut;
	rcut2	=	rcut * rcut;

	asmth		=	All.Asmth;
	asmthfac	=	0.5 / asmth * (NTAB / 3.0);

	boxsize	=	All.BoxSize;
	boxhalf	=	boxsize / 2.0;

	printf("to_grid_fac: %g\tasmth: %g\trcut: %g\n", 
			to_grid_fac, asmth, rcut);


}

void force_treebuild(int npart){
	
	int		nfree;
	NODE	* nfreep;

	nfree	=	NumPart;
//	nfreep	=	


}
