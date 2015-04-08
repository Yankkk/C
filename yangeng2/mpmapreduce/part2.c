/* 
 * CS 241
 * The University of Illinois
 */

#define _GNU_SOURCE
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>

#include "libmapreduce.h"
#include "sql.h"

query_t* QUERY; // current query to run

void map(int fd, const char *data)
{   
   //TO DO:
 
   int num_tokens;
   char ** value = tokenize(data, '\n', &num_tokens);
   int i;
   int j;
   int k;
   for(i = 0; i < num_tokens; i++){
   		if(QUERY->where_condition(value[i]) == 1){
   			int num;
   			char ** v = tokenize(value[i], ' ', &num);
   			char st[100];
   			char * s = calloc(100, sizeof(1));
   			if(QUERY->group_by_column==-1){
   				for (j = 0; QUERY->selected_columns[j]!=-1; j ++){
   					//strcat(s, " ");
   					strcat(s, v[QUERY->selected_columns[j]]);
   					strcat(s, " ");
   				}
   			int len = snprintf(st, 100, "%s: %s\n", v[0], s);
		    write(fd, st, len);
		    free(s);
   			}
   			else{
   				char st[100];
   				int len = snprintf(st, 100, "%s: %s\n", v[QUERY->group_by_column], v[QUERY->aggregate_func_column]);
   		    	write(fd, st, len);
   			}
   		
   			for(k=0; k< num;k++){
   				free(v[k]);
   			}
   			free(v);
      	}
   }
   for(k = 0; i < num_tokens; k++){
   		free(value[k]);
   }
   free(value);
   close(fd);
	return;
}

static int compare_fns(const void *arg1, const void *arg2) {
  return (*((int*)arg1)) - (*((int*)arg2)); 
}

const char *reduce(const char *value1, const char *value2)
{
   //TO DO:
   	if(QUERY->func == MAX){
   		int i1 = atoi(value1);
		int i2 = atoi(value2);
		int max = i1;
		if(i2 >= i1){
			max = i2;
		}
		char *result;
	    asprintf(&result, "%d", max);
		return result;
   	}
   	else if(QUERY->func == TOP3){
   		int num1;
   		int num2;
   		char ** v1 = tokenize(value1, ' ', &num1);
   		char ** v2 = tokenize(value2, ' ', &num2);
   		int * val = malloc((num1+num2) * sizeof(int));
   		int i;
   		for(i = 0; i < num1; i++){
   			val[i] = atoi(v1[i]);
   		} 
   		for(i = num1; i < num1+num2; i++){
   			val[i] = atoi(v2[i-num1]);
   		}
   		qsort(val, num1+num2,sizeof(int), compare_fns);
   		char * result;
   		if(num1+num2 >= 3){
   			asprintf(&result, "%d %d %d", val[num1+num2-1], val[num1+num2-2], val[num1+num2-3]);
   		}
   		else if(num1+num2 == 2){
   			asprintf(&result, "%d %d", val[1], val[0]);
   		}
   		else if(num1+num2 == 1){
   			asprintf(&result, "%d", val[0]);
   		}
   		free(val);
   		for(i = 0; i < num1; i++){
   			free(v1[i]);
   		}
   		free(v1);
   		for(i = 0; i < num2; i++){
   			free(v2[i]);
   		}
   		free(v2);
   		return result;
   	}
   	else if(QUERY->func == SUM){
   		int i1 = atoi(value1);
		int i2 = atoi(value2);
		char *result;
		asprintf(&result, "%d", (i1 + i2));
		return result;
   	}
   	else if(QUERY->func == COUNT){
   		int i1 = atoi(value1);
		int i2 = atoi(value2);
		char *result;
		asprintf(&result, "%d", (i1 + i2));
		return result;
   	}	
	return NULL;
}


