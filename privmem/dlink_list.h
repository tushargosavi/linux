#ifndef MISC_H
#define MISC_H

typedef struct {
	LOCK_T lock;
	void *list_ptr ;
	int count;
}dlist_t;

typedef struct {
	void *next;
	void *prev;
}dlist_element_t;
	

#define DLIST_RESET(dlist) { dlist->list_ptr=dlist->count=0; }
#define DLIST_APPEND(dlist,node) \
{ \
	if(list_ptr==0) { \
		list_ptr=node;  \
	else { \
		(dlist_element*)node->next=dlist->list_ptr; \
		(dlist_element*)list_ptr->prev=node; \
		list_ptr=(void*)node; \
	} \
	dlist->count++; \
}	


#define DLIST_REMOVE(dlist,node) \
{ \
	dlist_element_t *prev; \
	(node->prev!=0) { \
		/* first element */ \
		dlist->list_ptr =(dlist_element_t*)node->next; \
		dlist->list_ptr->prev=0; \
	}else { \
		prev=(dlist_element_t*)node->prev; \
		prev->next=node->next; \
		node->next->prev=prev; \
		MEM_FREE(node);  \
	} \
}
		
#endif

