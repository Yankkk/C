/** @file datastore_control.c */

#include <stdio.h>
#include <stdlib.h>

#include "datastore_control.h"
#include "datastore.h"

void init()
{


}

void load()
{


}

void save()
{



}

jsonres_t process_request(const char * uri, jsonreq_t request)
{
    jsonres_t response;
    if (uri)
    {
	response.rev = 0;
	response.value = "0";
	response.success = "true";
    }
    return response;


}
