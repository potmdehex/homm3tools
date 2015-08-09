#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "misc.h"
#include "graph.h"

#undef NDEBUG
#include <assert.h>

Graph *
grnew(int n, int m, int edgedim)
{
	Graph *gp;

	assert(edgedim >= 1 && edgedim <= GRMAXDIM);
	gp = xmalloc(sizeof *gp);
	gp->edges = xmalloc(m * sizeof *gp->edges);
	gp->first = xmalloc(n * sizeof *gp->first);
	gp->next = xmalloc(edgedim * m * sizeof *gp->next);
	gp->stack = xmalloc(m * sizeof *gp->stack);
	gp->numnodes = n;
	gp->numedges = m;
	gp->edgedim  = edgedim;
	grdeledges(gp);

	return gp;
}

void
grfree(Graph *gp)
{
	if (!gp)
		return;
	xfree(gp->stack);
	xfree(gp->next);
	xfree(gp->first);
	xfree(gp->edges);
	xfree(gp);
}

static void
addedge(Graph *gp, int v, int e)
{
	assert(v >= 0 && v < gp->numnodes);

	grnextedge(gp, e) = grfirstedge(gp, v);
	grfirstedge(gp, v) = e;
}

void
graddedge(Graph *gp, int e)
{
	int i;

	assert(e >= 0 && e < gp->numedges);

	for (i=0; i<gp->edgedim; i++)
		addedge(gp, gredgenode(gp, e, i), grdenormal(gp, e, i));
}

void
grdeledges(Graph *gp)
{
	int v;

	for (v=0; v<gp->numnodes; v++)
		grfirstedge(gp, v) = -1;
}

static void
deledge(Graph *gp, int v, int e)
{
	int eprev, e1;

	assert(v >= 0 && v < gp->numnodes);

	eprev = -1;
	e1 = grfirstedge(gp, v);
	while (e1 != e)
		e1 = grnextedge(gp, eprev = e1);

	if (eprev < 0)
		grfirstedge(gp, v) = grnextedge(gp, e);
	else
		grnextedge(gp, eprev) = grnextedge(gp, e);
}

void
grdeledge(Graph *gp, int e)
{
	int i;

	assert(e >= 0 && e < gp->numedges);

	for (i=0; i<gp->edgedim; i++)
		deledge(gp, gredgenode(gp, e, i), grdenormal(gp, e, i));
}

static int
degree1(Graph *gp, int v)
{
	int e;

	assert(v >= 0 && v < gp->numnodes);
	return (e = grfirstedge(gp, v)) >= 0 && grnextedge(gp, e) < 0;
}

static void
cyclicdeledge(Graph *gp, int v)
{
	int i, e;

	grdeledge(gp, e = grnormal(gp, grfirstedge(gp, v)));
	grstpush(gp, e);

	for (i=0; i<gp->edgedim; i++)
		if (degree1(gp, v = gredgenode(gp, e, i)))
			cyclicdeledge(gp, v);
}

int
grcyclic(Graph *gp)
{
	int v;

	gp->sp = 0;

	for (v=0; v<gp->numnodes; v++)
		if (degree1(gp, v))
			cyclicdeledge(gp, v);

	return !grstfull(gp);
}
