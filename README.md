# Judy API

## Overview

This is a wrapper for the somewhat error prone default Judy API.  It is intended
to be used in place of the unfortunate Macro API.  The intention is for it to be
easy to use, well documented, and less prone to operator error.  

Judy is an data structure library developed at Hewlett Packard in the
early 2000s.  It has low overhead and high performance, with an eye towards
cache coherency.  

## Error Handling

Error handling has been completely revamped in this release.  There is now a
global error handler callback that can be set to determine what to do when
an error occurs.  Several handlers are included in the API, but you are also
free to define your own.  

To set a handler use `jsl_set_error_handler` like so:

`jsl_set_error_handler(jsl* judy, &jsl_error_stderr);`

The built-in error handlers:

`jsl_error_stderr_exit` Default handler.  Prints the Judy error to standard out
		       and then exits the program with the Judy error code.

`jsl_error_stderr` Prints the Judy error to standard out, but allows the function
		  to return.  The functions have their own error conditions you
		  will have to check for in this case.  Not handling the error
		  will result in undefined behavior.

`jsl_error_stderr_dump_core`: Prints the Judy error to standard error, then dumps
			    core. 

`jsl_error_ignore`: Prints nothing.  As with `jsl_error_stderr` the API call will
		  return an error code that you will need to handle.  

You can also define your own error handling callback.  The prototype is:
`void callback(char* API_Name, char* API_Error, PJError_t* Judy_error);`

The `API_Name` parameter is the API call that produced the error, in string form.
The `API_Error` parameter is a string describing the problem.
`Judy_error` parameter is the error structure returned from Judy, it may be NULL.

Note that not all errors will call the error API.  Some are considered 
nonfatal and will only return error codes via the API interface.  For example
calling `jsl_get` with an index value that is not in the array.  This will not
trigger the error API, but it will return an error value `JSL_NOT_FOUND`.  

## Memory Management

Judy arrays will manage the memory for the keys.  It is not necessary to keep
a copy of the index value yourself.  You can optionally set a cleanup callback
for the objects to have the API clean them up for you.  This is highly 
reccomended as some functions, like `jsl_insert` will leak memory if the 
cleanup callback is not set and the value pointer is to data on the heap.

By default no cleanup is performed.  You can set the cleanup callback using
the following function:

`jsl_set_cleanup_handler(jsl* judy, &jsl_value_cleanup);`

The prototype for the cleanup callback is:
`int callback(void* your_data, char** error_string)`

Your callback should return 0 if the cleanup was successful.  In the event of
and error you should return anything but zero and if possible set the 
`error_string` parameter to something useful.  The error code and string will
be propagated to the error handler you have configured.

## API Usage:

Unlike the base interface, this library uses a struct instead of a bare pointer
to store the Judy array.  All functions that use the struct will take a pointer
to it.  No more remembering if you need a pointer to a pointer or not.
This structure should be considered opaque.

All API calls that return int return 0 on success, and a non-zero error code
otherwise.  The possible error codes are documented with each API call.

### Functions:
`jsl* jsl_init()`

  Returns an initalized but empty jsl object.  

`int jsl_insert(jsl* judy, char* index, void* value)`

  Adds the value 'value' under the key 'index' to the Judy array.  If the index
  already exists then the value is overwritten.  If not the index is created.

`int jsl_create(jsl* judy, char* index, void* value)`

  Adds the value 'value' under the key 'index' to the Judy array.  If the index
  already exists then the value is NOT changed and the API, and the error code
  `JSL_ERROR_EXISTS` is returned. 

`int jsl_update(jsl* judy, char* index, void* value)`

  Updates the field 'index' with 'value'.  If the index value is not in the the
  array the error code `JSL_ERROR_NOT_FOUND` is returned.  Note that the cleanup
  call will be used on the old value first.

`int jsl_delete(jsl* judy, char* index)`

  Removes the value 'index' from the array.  Returns `JSL_ERROR_NOT_FOUND` if the
  index is not in the array.

`int jsl_map(jsl* judy, int (*callback)(const char* index, void* value))`

  Iterates through every index in the array and calls the callback function for
  each entry.  Your callback should return 0 to continue iterating, 1 to stop
  iterating, and anything else to stop iterating and throw an error. 

`int jsl_free(jsl* judy)`

  Destroys all objects in the array, and then cleans up the remnants of the 
  array.

## Iterating over the items
If the map function is too restrictive you can also iterate over the array.
This allows you to change the array while iterating for example.  The way it 
works is you create an iterator object using the from\_start or from\_end 
functions.  This struct should be considered opaque except for two fields, 
index, and value.  Index is a char* of the current index value, and value*
is a pointer to the associated data.  Remember that the iterator is a pointer
value, so to access use iter->index and iter->value.

`jsl_iter* jsl_iter_from_start(jsl* judy, char* starting_point)`

`jsl_iter* jsl_iter_from_end(jsl* judy, char* starting_point)`

  Begins an interation over the array.  The `starting_point` paramter is 
  optional, if specified it will begin the search from that index.  
  Pass NULL as the starting\_point to search from either the start or end, 
  depending on which function you used.
  Searches are alphabetical based on the index value.
 
`int jsl_iter_next(jsl* judy, jsl_iter* iter)`

`int jsl_iter_prev(jsl* judy, jsl_iter* iter)`

  Steps to the next or previous entry in the array.  Can return
  `JSL_END_REACHED` if you attempt to iterate past either end of the array. 

`int jsl_iter_free(jsl_iter* iter)`
  Cleans up a iterator structure.


