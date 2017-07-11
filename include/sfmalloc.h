#ifndef __MY_MALLOC__
#define __MY_MALLOC__

#ifdef __GNUC__
extern void *MALLOC(int nBytes, char *filename, const char *function, int line);
extern void *CALLOC(int nBytes, int unitSize,
        char *filename, const char *function, int line);
extern void FREE(void *data, char *filename, const char *function, int line);
extern void *REALLOC(void *data, int nBytes, char *filename, 
	const char *function, int line);

extern void *safeMalloc(int nBytes, char *filename, const char *function, int line);
extern void *safeCalloc(int nBytes, int unitSize, char *filename, const char *function, int line);
extern void *safeRealloc(
		void *data, 
		int nBytes, 
		char *filename, 
		const char *function,	
		int line);

#else
extern void *MALLOC(int nBytes, char *filename, int line);
extern void *CALLOC(int nBytes, int unitSize, char *filename, int line);
extern void FREE(void *data, char *filename, int line);
extern void *REALLOC(void *data, int nBytes, char *filename, int line);

extern void *safeMalloc(int nBytes, char *filename, int line);
extern void *safeCalloc(int nBytes, int unitSize, char *filename, int line);
extern void *safeRealloc(void *data, int nBytes, char *filename, int line);
#endif

#ifdef SFMALLOC

#ifdef __GNUC__
#define Malloc(nBytes) MALLOC(nBytes, __FILE__, __FUNCTION__, __LINE__)
#define Calloc(nBytes, unitSize) CALLOC(nBytes, unitSize, __FILE__, __FUNCTION__, __LINE__)
#define Free(data) FREE(data, __FILE__, __FUNCTION__, __LINE__)
#define Realloc(data, nBytes) REALLOC(data, nBytes, __FILE__, __FUNCTION__, __LINE__)
#else
#define Malloc(nBytes) MALLOC(nBytes, __FILE__, __LINE__)
#define Calloc(nBytes, unitSize) CALLOC(nBytes, unitSize,  __FILE__, __LINE__)
#define Free(data) FREE(data, __FILE__, __LINE__)
#define Realloc(data, nBytes) REALLOC(data, nBytes, __FILE__, __LINE__)
#endif



#else

#ifdef __GNUC__
#define Malloc(nBytes) safeMalloc(nBytes, __FILE__,__FUNCTION__,__LINE__)
#define Calloc(nBytes, unitSize) safeCalloc(nBytes, unitSize, __FILE__,__FUNCTION__,__LINE__)
#define Free(data) free(data)
#define Realloc(data, nBytes) safeRealloc(data, nBytes,__FILE__,__FUNCTION__, __LINE__)
#else
#define Malloc(nBytes) safeMalloc(nBytes, __FILE__,__LINE__)
#define Calloc(nBytes, unitSize) safeCalloc(nBytes, unitSize,  __FILE__,__LINE__)
#define Free(data) free(data)
#define Realloc(data, nBytes) safeRealloc(data, nBytes,__FILE__, __LINE__)
#endif


#endif


#endif
