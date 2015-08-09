#ifndef GRAPH_H
#define GRAPH_H

typedef struct Edge	Edge;
typedef struct Graph	Graph;

#define GRMAXDIM	3	/* maximum edge dimension */

struct Edge {
	int	nodes[GRMAXDIM];
};

struct Graph {
	int	numnodes, numedges, edgedim;
	Edge	*edges;
	int	*first, *next;
	int	*stack, sp;
};

extern Graph	*grnew(int, int, int);
extern void	grfree(Graph *);
extern void	graddedge(Graph *, int);
extern void	grdeledge(Graph *, int);
extern void	grdeledges(Graph *);
extern int	grfirstedge(Graph *, int);
extern int	grnextedge(Graph *, int);
extern int	grnodevalue(Graph *, int);
extern int	gredgevalue(Graph *, int);
extern int	grnormal(Graph *, int);
extern int	grdenormal(Graph *, int, int);
extern int	gredgenode(Graph *, int);
extern int	grcyclic(Graph *);
extern void	grstpush(Graph *, int);
extern int	grstpop(Graph *);
extern int	grstfull(Graph *);

#define grfirstedge(gp, v)	(gp->first[v])			/* lvalue */
#define grnextedge(gp, e)	(gp->next[e])			/* lvalue */
#define grnodevalue(gp, v)	grfirstedge(gp, v)		/* lvalue */
#define gredgevalue(gp, e)	(e)
#define gredgenode(gp, e, i)	(gp->edges[e].nodes[i])		/* lvalue */

#define grstpush(gp, e)		(gp->stack[gp->sp++] = e)
#define grstpop(gp)		(gp->stack[--gp->sp])
#define grstfull(gp)		(gp->sp == gp->numedges)

#define grnormal(gp, e)		(e % gp->numedges)
#define grdenormal(gp, e, i)	(e + i*gp->numedges)

#endif
