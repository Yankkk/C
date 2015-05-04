/** @file datastore.c */

#define _GNU_SOURCE
#include <search.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

#include "datastore.h"

/*
typedef struct _datastore_entry_t
{
	const char *key, *value;
	unsigned long rev;
} datastore_entry_t;
*/

static int compare(const void *a, const void *b)
{
	const datastore_entry_t * aa = (const datastore_entry_t *) a;
	const datastore_entry_t * bb = (const datastore_entry_t *) b;
	return strcmp(aa->key, bb->key); 
}

static datastore_entry_t *malloc_entry_t(const char *key, const char *value, const long rev)
{
	datastore_entry_t *entry = malloc(sizeof(datastore_entry_t));
	entry->key = key;
	entry->value = value;
	entry->rev = rev;

	return entry;
}


datastore_entry_t *dictionary_tfind(datastore_t *ds, const char *key)
{
	datastore_entry_t tentry = {key, NULL, 0};
	void *tresult = tfind((void *)&tentry, &ds->root, compare);

	if (tresult == NULL)
		return NULL;
	else
		return *((datastore_entry_t **)tresult);
}

static void dictionary_tdelete(datastore_t *ds, const char *key)
{
	datastore_entry_t tentry = {key, NULL, 0};
	tdelete((void *)&tentry, &ds->root, compare);
}

static void destroy_with_element_free(void *ptr)
{
	datastore_entry_t * entry = (datastore_entry_t *) ptr;

	free((void *)entry->key);
	free((void *)entry->value);
	free(entry);
}

/*
void datastore_init(datastore_t *ds)
{
	ds->root = NULL;
	pthread_mutex_init(&ds->mutex, NULL);
}

*/
unsigned long datastore_put(datastore_t *ds, const char *key, const char *value, const long rev)
{
	pthread_mutex_lock(&ds->mutex);

	if (dictionary_tfind(ds, key) == NULL)
	{
		datastore_entry_t *entry = malloc_entry_t(strdup(key), strdup(value), rev);
		tsearch((void *)entry, &ds->root, compare);
		pthread_mutex_unlock(&ds->mutex);
		//datastore_entry_t *entry = dictionary_tfind(ds, key);
		//printf("add: %s %s %d\n", entry->value, entry->key, entry->rev);
		return entry->rev;
	}
	else
	{
		pthread_mutex_unlock(&ds->mutex);

		return 0;
	}
}


const char *datastore_get(datastore_t *ds, const char *key, unsigned long *revision)
{
	pthread_mutex_lock(&ds->mutex);
	datastore_entry_t *entry = dictionary_tfind(ds, key);

	if (entry == NULL)
	{
		pthread_mutex_unlock(&ds->mutex);
		return NULL;
	}
	else
	{
		if (revision != NULL)
			*revision = entry->rev;
		const char *value = strdup(entry->value);
		//const char * value = entry->value;
		pthread_mutex_unlock(&ds->mutex);
		return value;
	}
}


unsigned long datastore_update(datastore_t *ds, const char *key, const char *value, unsigned long known_revision)
{
	pthread_mutex_lock(&ds->mutex);
	datastore_entry_t *entry = dictionary_tfind(ds, key);

	if (entry == NULL)
	{
		// key does not exist
		pthread_mutex_unlock(&ds->mutex);
		return 0;
	}
	else
	{
		// ensure revisions match
		if (entry->rev != known_revision)
		{
			pthread_mutex_unlock(&ds->mutex);
			return -2;
		}

		free((void *)entry->value);
		entry->value = strdup(value);
		unsigned long new_revision = ++(entry->rev);
		//printf("updata: %s %s %d\n", entry->value, entry->key, entry->rev);
		pthread_mutex_unlock(&ds->mutex);
		return new_revision;
	}
}


unsigned long datastore_delete(datastore_t *ds, const char *key)
{
	pthread_mutex_lock(&ds->mutex);
	datastore_entry_t *entry = dictionary_tfind(ds, key);

	if (entry == NULL)
	{
		pthread_mutex_unlock(&ds->mutex);
		return 0;
	}
	else
	{
		/*
		if (entry->rev != known_revision)
		{
			pthread_mutex_unlock(&ds->mutex);
			return 0;
		}
		*/
		dictionary_tdelete(ds, key);

		free((void *)entry->key);
		free((void *)entry->value);
		free(entry);

		pthread_mutex_unlock(&ds->mutex);
		return 1;
	}
}


void datastore_destroy(datastore_t *ds)
{
	tdestroy(ds->root, destroy_with_element_free);
	ds->root = NULL;

	pthread_mutex_destroy(&ds->mutex);
}
