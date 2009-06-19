#include	<stdio.h>
#include	<stdlib.h>
#include	<math.h>

#include	"allvars.h"
#include	"proto.h"

static	int		last;

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
	
	int		i, subnode, parent, numnodes;
	int		nfree, th, nn;
	NODE	* nfreep;
	double	lenhalf;
	
	subnode	=	0;

/*	create empty root node, size is just boxsize
 */
	nfree	=	NumPart;	/* index of first node */
	nfreep	=	&Nodes[nfree];	/* pointer of first node */

	nfreep->len	=	boxsize;	
	for(i = 0; i < 3; ++i)
		nfreep->center[i]	=	boxhalf;
	for(i = 0; i < 8; ++i)
		nfreep->u.suns[i]	=	-1;
	
	numnodes	=	1;
	nfreep++;
	nfree++;

	parent	=	-1;

/*	next insert all particles one by one
 */
	for(i = 0; i < NumPart; ++i){
		
		th	=	NumPart;

		while(1){

			if(th >= NumPart){
			/* This is a node, continue.
			 */
				subnode	=	0;
				if(P[i].Pos[0] > Nodes[th].center[0])
					subnode	+=	1;
				if(P[i].Pos[1] > Nodes[th].center[1])
					subnode	+=	2;
				if(P[i].Pos[2] > Nodes[th].center[2])
					subnode	+=	4;

				nn	=	Nodes[th].u.suns[subnode];

				if(nn >= 0){
					parent	=	th;
					th	=	nn;
				}else{
					/* OK! We have find an empty slot, insert particle i here.
					 */
					Nodes[th].u.suns[subnode]	=	i;
					break;
				}
			}else{
			/* This is a particle, we have to generate a new node and insert 
			 * the old one
			 */
				Nodes[parent].u.suns[subnode]	=	nfree;

				nfreep->len	=	0.5 * Nodes[parent].len;
				lenhalf	=	0.25 * Nodes[parent].len;

				if(subnode & 1)
					nfreep->center[0]	=	Nodes[parent].center[0] + lenhalf;
				else
					nfreep->center[0]	=	Nodes[parent].center[0] - lenhalf;

				if(subnode & 2)
					nfreep->center[1]	=	Nodes[parent].center[1] + lenhalf;
				else
					nfreep->center[1]	=	Nodes[parent].center[1] - lenhalf;

				if(subnode & 1)
					nfreep->center[2]	=	Nodes[parent].center[2] + lenhalf;
				else
					nfreep->center[2]	=	Nodes[parent].center[2] - lenhalf;

				/* I think here does not use loop is for the sake of speed
				 */
				nfreep->u.suns[0]	=	-1;
				nfreep->u.suns[1]	=	-1;
				nfreep->u.suns[2]	=	-1;
				nfreep->u.suns[3]	=	-1;
				nfreep->u.suns[4]	=	-1;
				nfreep->u.suns[5]	=	-1;
				nfreep->u.suns[6]	=	-1;
				nfreep->u.suns[7]	=	-1;

				subnode	=	0;
				if(P[th].Pos[0] > nfreep->center[0])
					subnode	+=	1;
				if(P[th].Pos[1] > nfreep->center[1])
					subnode	+=	2;
				if(P[th].Pos[2] > nfreep->center[2])
					subnode	+=	4;

				nfreep->u.suns[subnode]	=	th;

				th	=	nfree;

				numnodes++;
				nfree++;
				nfreep++;

				if(numnodes >= MaxNodes){
					printf("maximun number of tree nodes reached\n");
					printf("we'd better stop!\n");
					exit(1);
				}
			}
		}
	}

	/* next computer multipole moments recursively.
	 */
	last	=	-1;
	force_update_node_recursive(All.NumPart, -1, -1);

}

void force_update_node_recursive(int no, int sib, int father){

}



