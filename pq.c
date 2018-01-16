#include <stdio.h>
#include <stdlib.h>

typedef struct node {
	double priority;
	int id;
	int pos;
}NODE;

typedef struct pq_adt {
	NODE** queue;
	int cap;
	int inUse;
	int minFlag;
	NODE** id;
}PQ;

void perc_up(PQ* pq, int inUse) {
	NODE* temp;
	int child = inUse;
	int parent = child / 2;
	int isMin = pq->minFlag;

	while ((parent > 0) && 
		((isMin && (pq->queue[parent]->priority > pq->queue[child]->priority)) || 
		(!isMin && (pq->queue[parent]->priority < pq->queue[child]->priority)))) {
		
		temp = pq->queue[child];
		pq->queue[child] = pq->queue[parent];
		pq->queue[child]->pos = child;
		pq->queue[parent] = temp;
		pq->queue[parent]->pos = parent;
		child = parent;
		parent = child / 2;
	}
}

int minMaxChild(PQ* pq, int root) {
	int lchild = 2 * root;
	int rchild = 2 * root + 1;
	int inUse = pq->inUse;

	if (lchild > inUse) {
		return -1;
	}
	else if (rchild > inUse) {
		return lchild;
	}
	else {
		if (pq->minFlag) {
			return (pq->queue[lchild]->priority < pq->queue[rchild]->priority ? lchild : rchild);
		}
		else {
			return (pq->queue[lchild]->priority > pq->queue[rchild]->priority ? lchild : rchild);
		}
	}

}

void perc_down(PQ* pq, int root) {
	//int capacity = pq->cap;
	int parent = root;
	int isMin = pq->minFlag;
	NODE* temp;

	int child = minMaxChild(pq, parent);
	while ((child != -1) &&
		((isMin && (pq->queue[parent]->priority > pq->queue[child]->priority)) ||
		(!isMin && (pq->queue[parent]->priority < pq->queue[child]->priority)))) {

		temp = pq->queue[child];
		pq->queue[child] = pq->queue[parent];
		pq->queue[child]->pos = child;
		pq->queue[parent] = temp;
		pq->queue[parent]->pos = parent;
		parent = child;
		child = minMaxChild(pq, child);
	}

}


extern PQ * pq_create(int capacity, int min_heap) {
	int i;
	PQ* pq = (PQ*)malloc(sizeof(PQ));
	
	pq->queue = (NODE**)malloc(sizeof(NODE*) * (capacity + 1));
	for (i = 0; i <= capacity; ++i) {
		pq->queue[i] = NULL;
	}

	pq->cap = capacity;
	pq->inUse = 0;
	pq->minFlag = min_heap;

	pq->id = (NODE**)malloc(sizeof(NODE*) * capacity);
	for (i = 0; i < capacity; ++i) {
		pq->id[i] = NULL;
	}

	return pq;
}

extern void pq_free(PQ * pq) {
	int i;
	for (i = 1; i < pq->inUse + 1; ++i) {
		free(pq->queue[i]);
	}
	free(pq->queue);
	free(pq->id);
	free(pq);

}

extern int pq_insert(PQ * pq, int id, double priority) {
	if (!pq || id < 0 || id >= pq->cap || pq->id[id]) {
		return 0;
	}
	int qIn = ((pq->inUse) + 1);
	pq->queue[qIn] = (NODE*)malloc(sizeof(NODE));
	pq->queue[qIn]->id = id;
	pq->queue[qIn]->priority = priority;
	pq->queue[qIn]->pos = qIn;
	pq->id[id] = pq->queue[qIn];
	++(pq->inUse);
	perc_up(pq, pq->inUse);
	return 1;

}

extern int pq_change_priority(PQ * pq, int id, double new_priority) {
	if (!pq || id < 0 || id >= pq->cap || !(pq->id[id])) {
		return 0;
	}
	double oldP = pq->id[id]->priority;
	pq->id[id]->priority = new_priority;
	int qPos = pq->id[id]->pos;
	if (pq->minFlag) {
		new_priority < oldP ? perc_up(pq, qPos) : perc_down(pq, qPos);
	}
	else {
		new_priority > oldP ? perc_up(pq, qPos) : perc_down(pq, qPos);
	}
	return 1;
}

extern int pq_remove_by_id(PQ * pq, int id) {
	if (!pq || id < 0 || id >= pq->cap || !(pq->id[id])) {
		return 0;
	}
	int lastInd = pq->inUse;
	int idInd = pq->id[id]->pos;

	NODE* temp = pq->queue[lastInd];
	pq->queue[lastInd] = pq->queue[idInd];
	pq->queue[lastInd]->pos = lastInd;
	pq->queue[idInd] = temp;
	pq->queue[idInd]->pos = idInd;

	free(pq->queue[lastInd]);
	pq->queue[lastInd] = NULL;
	--(pq->inUse);
	pq->id[id] = NULL;

	perc_down(pq, idInd);
	
	return 1;
}

extern int pq_get_priority(PQ * pq, int id, double *priority) {
	if (!pq || id < 0 || id > pq->inUse || !(pq->id[id])) {
		return 0;
	}
	*priority = pq->id[id]->priority;
	return 1;
}

extern int pq_delete_top(PQ * pq, int *id, double *priority) {
	if (!pq || !pq->queue[1]) {
		return 0;
	}
	else {
		*id = pq->queue[1]->id;
		*priority = pq->queue[1]->priority;
		int lastInd = pq->inUse;
		
		NODE* temp = pq->queue[lastInd];
		pq->queue[lastInd] = pq->queue[1];
		pq->queue[lastInd]->pos = lastInd;
		pq->queue[1] = temp;
		pq->queue[1]->pos = 1;
		
		free(pq->queue[lastInd]);
		pq->queue[lastInd] = NULL;
		--(pq->inUse);
		pq->id[*id] = NULL;
		
		perc_down(pq, 1);
		return 1;
	}
}

extern int pq_peek_top(PQ * pq, int *id, double *priority) {
	if (!pq || !pq->queue[1]) {
		return 0;
	}
	*id = pq->queue[1]->id;
	*priority = pq->queue[1]->priority;
	return 1;

}

extern int pq_capacity(PQ * pq) {
	return (!pq ? 0 : pq->cap);
}

extern int pq_size(PQ * pq) {
	return (!pq ? 0 : pq->inUse);	
}