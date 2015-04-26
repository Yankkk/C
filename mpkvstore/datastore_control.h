/** @file datastore_control.h */
#ifndef __DS_CTRL_H__
#define __DS_CTRL_H__

#include "datastore.h"

typedef struct _jsonreq_t
{
  char * key;
  char * value;
  unsigned long rev;

} jsonreq_t;

typedef struct _jsonres_t
{
  char * success;
  char * value;
  unsigned long rev;

} jsonres_t;

void init();
void load();
void save();

jsonres_t process_request(const char * uri, jsonreq_t request);


#endif
