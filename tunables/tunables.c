#include <stdio.h>
#include <stdlib.h>

#include "tunables.h"

int int_validate(struct tunable* t);
int int_update(struct tunable* t);
int float_validate(struct tunable* t);
int float_update(struct tunable* t);
int long_validate(struct tunable* t);
int long_update(struct tunable* t);
int long_long_validate(struct tunable* t);
int long_long_update(struct tunable* t);
int string_validate(struct tunable* t);
int string_update(struct tunable* t);
int struct_validate(struct tunable* t);
int struct_update(struct tunable* t);

extern int int_val;
extern int float_val;
extern long long_val;
extern long long long_long_val;
extern const char* str;


struct tunable tunables[] ={
	{"int_val",&int_val,INT_TYPE,sizeof(int),0,0,int_validate,int_update},
	{"float_val",&float_val,FLOAT_TYPE,sizeof(float),0,0,float_validate,float_update},
	{"long_val",&long_val,LONG_TYPE,sizeof(long),0,0,long_validate,long_update},
	{"long_long_val",&long_long_val,LONG_LONG_TYPE,sizeof(long long),0,0,long_long_validate,long_long_update},
	{"str",&str,STRING_TYPE,0,0,0,string_validate,string_update},
	{NULL}
};


struct tunable* search_tunable(char *name) {
	struct tunable* list=tunables;
	while(list->name) {
		if(strcmp(name,list->name)==0) 
			return list;
		list++;
	}
	return 0;
}


int set_tunable(char *name,int type,void *value,int size) {
	
	struct tunable* ptr;
	int data_size=0;

	int (*validate)(struct tunable* t);
	int (*update)(struct tunable *t);

	ptr=search_tunable(name);
	if(ptr==0) {
		fprintf(stderr,"Tunable not found\n");
		return -INVAL_ENT;
	}
	if(ptr->type!=type) {
		fprintf(stderr,"Tunable Type missmatch\n");
		return INVAL_TYPE;
	}

	validate=ptr->validate;
	update=ptr->update;

	if(validate==0 || update==0) {
		fprintf(stderr,"Validate and Update functions can't be NULL\n");
		return -INVAL_ENTRY;
	}

	/* copy the supplied value in the new_value */
	if((ptr->size!=0 && size) && (size < ptr->size)) {
		fprintf(stderr,"trying to override fixed data\n");
		return -INVAL_OPERATION;
	}
	data_size=ptr->size? ptr->size : size;
	
	if(ptr->new_value!=0) {
		fprintf(stderr,"somebody is already in this function\n");
		return -INVAL_OPERATION;
	}

	/* allocation space and copy the value */
	ptr->new_value=(void*)malloc(data_size);
	memcpy(ptr->new_value,value,data_size);
	
	if(validate(ptr)) {
		if(update(ptr)) {
			if(ptr->new_value) free(ptr->new_value);
			ptr->new_size=0;
			return 0;
		}else {
			fprintf(stderr,"Error while updating tunable\n");
			if(ptr->type!=STRING_TYPE && ptr->new_value) free(ptr->new_value);
			ptr->new_size=0;
			return -UPDATE_ERROR;
		}
	}else {
		fprintf(stderr,"Error while validating the data");
		return -VALID_ERROR;
	}
}


int int_validate(struct tunable *t)
{
	return VALID;
}

int int_update(struct tunable *t)
{
	*((int*)(t->value))=*((int*)(t->new_value));
	return (SUCCESS);
}

int float_validate(struct tunable *t)
{
	return VALID;
}

int float_update(struct tunable *t)
{
	*((float*)(t->value))=*((float*)(t->new_value));
	return (SUCCESS);
}

int long_validate(struct tunable *t)
{
	return VALID;
}

int long_update(struct tunable *t)
{
	*((long*)(t->value))=*((long*)(t->new_value));
	return (SUCCESS);
}

int long_long_validate(struct tunable* t)
{
	return VALID;
}

int long_long_update(struct tunable *t)
{
	*((long long*)(t->value))=*((long long*)(t->new_value));
	return (SUCCESS);
}

int string_validate(struct tunable *t)
{
	return VALID;
}

/* The string pointer are modified instead of coping the value */
int string_update(struct tunable *t)
{
	void *temp=*(char**)(t->value);
	*((char**)(t->value))=t->new_value;
	t->new_value=temp;
	return SUCCESS;
}

int struct_validate(struct tunable *t)
{
	return VALID;
}

int struct_update(struct tunable *t)
{
	return SUCCESS;
}


