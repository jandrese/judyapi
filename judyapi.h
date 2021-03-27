#ifndef JUDYAPI_H
#define JUDYAPI_H

#include <Judy.h>

#ifndef JUDY_API_STORAGE
extern char* jhash_error_string;
#endif

typedef struct
{
	Pvoid_t	array;			/* Pointer to the Judy structure */
	size_t size;			/* Number of elements in the Judy array */
	size_t maxindexlen;		/* Maximum index length we have seen. */
	extern void* jsl_error_handler; /* Callback to error handler function */
	extern void* jsl_cleanup;	/* Callback to node cleanup function */
} jhash;

typedef struct
{
	char* index;
	void* value;
	size_t maxindexlen;
} jhash_iter;


jhash* jhash_init();
void*     jhash_get(jhash* judy, char* index);
int    jhash_insert(jhash* judy, char* index, void* value);
int    jhash_create(jhash* judy, char* index, void* value);
int    jhash_update(jhash* judy, char* index, void* value);
int    jhash_delete(jhash* judy, char* index);
int       jhash_map(jhash* judy, int (*callback)(const char*, void*));
int      jhash_free(jhash* judy);

jhash_iter* jhash_iter_from_start(jhash* judy, char* starting_point);
jhash_iter*   jhash_iter_from_end(jhash* judy, char* starting_point);
int    jhash_iter_next(jhash* judy, jhash_iter* iter);
int    jhash_iter_prev(jhash* judy, juash_iter* iter);
int    jhash_iter_free(jhash_iter* iter);

void error_stderr(char* API_Name, char* API_Error, PJError_t* Judy_error);
void error_stderr_exit(char* API_Name, char* API_Error, PJError_t* Judy_error);
void error_stderr_dump_core(char* API_Name, char* API_Error, PJError_t*
Judy_error);
void error_ignore(char* API_Name, char* API_Error, PJError_t* Judy_error);

#endif
