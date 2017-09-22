/* Created by "go tool cgo" - DO NOT EDIT. */

/* package command-line-arguments */

/* Start of preamble from import "C" comments.  */


#line 27 "/Users/milanfranc/Downloads/ipfs-cache-master/src/ipfs_bindings.go"

#include <stdlib.h>
#include <stddef.h>
#include <stdint.h>

// Don't export these functions into C or we'll get "unused function" warnings
// (Or errors saying functions are defined more than once if the're not static).

#if IN_GO
static void execute_void_cb(void* func, void* arg)
{
    ((void(*)(void*)) func)(arg);
}
static void execute_data_cb(void* func, void* data, size_t size, void* arg)
{
    ((void(*)(char*, size_t, void*)) func)(data, size, arg);
}
#endif // if IN_GO

#line 1 "cgo-generated-wrapper"


/* End of preamble from import "C" comments.  */


/* Start of boilerplate cgo prologue.  */
#line 1 "cgo-gcc-export-header-prolog"

#ifndef GO_CGO_PROLOGUE_H
#define GO_CGO_PROLOGUE_H

typedef signed char GoInt8;
typedef unsigned char GoUint8;
typedef short GoInt16;
typedef unsigned short GoUint16;
typedef int GoInt32;
typedef unsigned int GoUint32;
typedef long long GoInt64;
typedef unsigned long long GoUint64;
typedef GoInt64 GoInt;
typedef GoUint64 GoUint;
typedef __SIZE_TYPE__ GoUintptr;
typedef float GoFloat32;
typedef double GoFloat64;
typedef float _Complex GoComplex64;
typedef double _Complex GoComplex128;

/*
  static assertion to make sure the file is being used on architecture
  at least with matching size of GoInt.
*/
typedef char _check_for_64_bit_pointer_matching_GoInt[sizeof(void*)==64/8 ? 1:-1];

typedef struct { const char *p; GoInt n; } GoString;
typedef void *GoMap;
typedef void *GoChan;
typedef struct { void *t; void *v; } GoInterface;
typedef struct { void *data; GoInt len; GoInt cap; } GoSlice;

#endif

/* End of boilerplate cgo prologue.  */

#ifdef __cplusplus
extern "C" {
#endif


extern GoUint8 go_ipfs_cache_start(char* p0);

extern void go_ipfs_cache_stop();

extern void go_ipfs_cache_resolve(char* p0, void* p1, void* p2);

// IMPORTANT: The returned value needs to be explicitly `free`d.

extern char* go_ipfs_cache_ipns_id();

extern void go_ipfs_cache_publish(char* p0, int64_t p1, void* p2, void* p3);

extern void go_ipfs_cache_add(void* p0, size_t p1, void* p2, void* p3);

extern void go_ipfs_cache_cat(char* p0, void* p1, void* p2);

#ifdef __cplusplus
}
#endif
