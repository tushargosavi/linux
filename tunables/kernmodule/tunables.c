#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/errno.h>

#include "tunables.h"

static int int_validate(struct tunable* t);
static int int_update(struct tunable* t);
static int float_validate(struct tunable* t);
static int float_update(struct tunable* t);
static int long_validate(struct tunable* t);
static int long_update(struct tunable* t);
static int long_long_validate(struct tunable* t);
static int long_long_update(struct tunable* t);
static int string_validate(struct tunable* t);
static int string_update(struct tunable* t);
static int struct_validate(struct tunable* t);
static int struct_update(struct tunable* t);


static struct tunable* tune_list=0;

int add_tunable(struct tunable *tune)
{
	struct tunable *temp=search_tunable(tune->name);
	if(temp) {
		printk("The tunable already exists\n");
		return -EBUSY;
	}
	if(tune_list==0) {
		tune_list=tune;
	}else {
		tune->next=tune_list;
		tune_list=tune;
	}
	return SUCCESS;
}

struct tunable* search_tunable(char *name) {
	struct tunable* temp=tune_list;
	while(temp) {
		if(strcmp(temp->name,name)==0) break;
		temp=temp->next;
	}
	return temp;
}

int delete_tunable(char *name)
{
	struct tunable *temp=tune_list;
	struct tunable *prev=0;
	while(temp) {
		if(strcmp(name,temp->name)==0) {
			if(prev==0) { 
				tune_list=tune_list->next;
			} else {
				prev->next=temp->next; 
				kfree(temp);
				return SUCCESS;
			}
		}else {
			prev=temp;
		}
		temp=temp->next;
	}
	return -EACCES;
}



int set_tunable(char *name,int type,void *value,int size) {
	struct tunable* ptr;
	int data_size=0;

	int (*validate)(struct tunable* t);
	int (*update)(struct tunable *t);

	ptr=search_tunable(name);
	if(ptr==0) {
		printk( "Tunable not found\n");
		return -INVAL_ENT;
	}
	if(ptr->type!=type) {
		printk( "Tunable Type missmatch\n");
		return INVAL_TYPE;
	}

	validate=ptr->validate;
	update=ptr->update;

	if(validate==0 || update==0) {
		printk( "Validate and Update functions can't be NULL\n");
		return -INVAL_ENTRY;
	}

	/* copy the supplied value in the new_value */
	if((ptr->size!=0 && size) && (size < ptr->size)) {
		printk("trying to override fixed data\n");
		return -INVAL_OPERATION;
	}
	data_size=ptr->size? ptr->size : size;
	
	if(ptr->new_value!=0) {
		printk( "somebody is already in this function\n");
		return -INVAL_OPERATION;
	}

	/* allocation space and copy the value The value is copied from the user land*/
	ptr->new_value=(void*)kmalloc(data_size);
	copy_from_user(ptr->new_value,value,data_size);
	
	if(validate(ptr)) {
		if(update(ptr)) {
			if(ptr->new_value) kfree(ptr->new_value);
			ptr->new_size=0;
			return 0;
		}else {
			printk( "Error while updating tunable\n");
			if(ptr->type!=STRING_TYPE && ptr->new_value) free(ptr->new_value);
			ptr->new_size=0;
			return -UPDATE_ERROR;
		}
	}else {
		printk( "Error while validating the data");
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


