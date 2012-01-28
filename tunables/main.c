#include <stdio.h>
#include <stdlib.h>

#include "tunables.h"

int int_val=10;
float float_val=12.34;
long long_val=123;
long long long_long_val=1234;
char *str;

int main(int argc,char **argv)
{
	int new_int=12;
	float new_float=3.1415f;
	long new_long=1234;
	long long new_long_long=123456;
	char *new_str="Hello World";
	char *old_str="Old String";
	str=malloc(strlen(old_str)+1);
	strcpy(str,old_str);

	/* int test */
	printf("value of int_val before set_tunable :%d\n",int_val);
	set_tunable("int_val",INT_TYPE,&new_int,0);
	printf("value of int_val after set_tunable :%d\n",int_val);
	
	/* float test */
	printf("value of float_val before set_tunable :%f\n",float_val);
	set_tunable("float_val",FLOAT_TYPE,&new_float,0);
	printf("value of float_val after set_tunable :%f\n",float_val);

	/* long test */
	printf("value of long_val before set_tunable :%ld\n",long_val);
	set_tunable("long_val",LONG_TYPE,&new_long,0);
	printf("value of long_val after set_tunable :%ld\n",long_val);

	/* long long test */
	printf("value of long_long_val before set_tunable :%Ld\n",long_long_val);
	set_tunable("long_long_val",LONG_LONG_TYPE,&new_long_long,0);
	printf("value of long_long_val after set_tunable :%Ld\n",long_long_val);

	/* string test */
	printf("value of str before set_tunable :%s\n",str);
	set_tunable("str",STRING_TYPE,new_str,strlen(new_str)+1);
	printf("value of str after set_tunable :%s\n",str);
	
	return 0;
}


