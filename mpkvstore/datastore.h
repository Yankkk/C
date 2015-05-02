/** @file datastore.h */
#ifndef __DS_H__
#define __DS_H__

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include "datastore_control.h"
/*
typedef struct _datastore_t
{
    void * ds;
} datastore_t;
*/

/*
typedef struct _datastore_t
{
	struct _jsonreq_t ** ds;
	void * addr;
	size_t size;
	size_t current;
	int * probe;

}datastore_t;


int hash( char*  key, int size);
int rehash(char *  key, int size);
size_t find_index(char * key, datastore_t* data);
struct _jsonreq_t * find(char * key, datastore_t* data);
void add(char * key,  char * value, int rev, datastore_t* data);
int Remove(char * key, datastore_t* data);
void resize(datastore_t* data);
*/

/*
typedef struct _datastore_entry_t
{
	const char *key, *value;
	unsigned long rev;
} datastore_entry_t;
*/

typedef struct _datastore_t
{
	void *root;
	pthread_mutex_t mutex;
} datastore_t;

typedef struct _datastore_entry_t
{
	const char *key, *value;
	unsigned long rev;
} datastore_entry_t;

void datastore_init(datastore_t *ds);
unsigned long datastore_put(datastore_t *ds, const char *key, const char *value, const long rev);///
const char *datastore_get(datastore_t *ds, const char *key, unsigned long *revision);
unsigned long datastore_update(datastore_t *ds, const char *key, const char *value, unsigned long known_revision);
unsigned long datastore_delete(datastore_t *ds, const char *key);
void datastore_destroy(datastore_t *ds);


/*
primes[] = {
    17ul, 29ul, 37ul, 53ul, 67ul, 79ul, 97ul, 131ul, 193ul, 257ul, 389ul,
    521ul, 769ul, 1031ul, 1543ul, 2053ul, 3079ul, 6151ul, 12289ul, 24593ul,
    49157ul, 98317ul, 196613ul, 393241ul, 786433ul, 1572869ul, 3145739ul,
    6291469ul, 12582917ul, 25165843ul, 50331653ul, 100663319ul,
    201326611ul, 402653189ul, 805306457ul, 1610612741ul, 3221225473ul,
    4294967291ul
};
*/

#endif
