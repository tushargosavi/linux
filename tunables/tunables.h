#ifndef TUNABLES_H
#define TUNABLES_H

/* Error code definations */

#define VALID           0x0001
#define SUCCESS			0x0001

#define INVAL_ENT		0x0001
#define INVAL_TYPE		0x0002
#define INVAL_ENTRY		0x0004
#define UPDATE_ERROR	0x0008
#define VALID_ERROR		0x0010
#define INVAL_OPERATION	0x0020

/* data type supported */
#define INT_TYPE		0x0001
#define FLOAT_TYPE		0x0002
#define LONG_TYPE		0x0004
#define LONG_LONG_TYPE	0x0008
#define STRING_TYPE		0x0010
#define STRUCT_TYPE		0x0020


struct tunable {
	char *name;
	void *value;
	int type;
	int size;
	void *new_value;
	int new_size;
	int (*validate)(struct tunable* t);
	int (*update)(struct tunable* t);
};

struct tunable* search_tunable(char *name);
int set_tunable(char *name,int type,void *value,int size);

#endif
