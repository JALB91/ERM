/*==============================================================================
FMOD Example Framework
Copyright (c), Firelight Technologies Pty, Ltd 2012-2021.
==============================================================================*/
#include <assert.h>
#include <pthread.h>

#define COMMON_PLATFORM_SUPPORTS_FOPEN
#define Common_TTY(format, ...) fprintf(stderr, format, __VA_ARGS__)

typedef pthread_mutex_t Common_Mutex;

inline void Common_Mutex_Create(Common_Mutex* mutex)
{
	int err;
	pthread_mutexattr_t mutexattr;
	pthread_mutexattr_init(&mutexattr);

	err = pthread_mutexattr_settype(&mutexattr, PTHREAD_MUTEX_RECURSIVE);
	assert(err == 0);

	err = pthread_mutex_init(mutex, &mutexattr);
	assert(err == 0);
}

inline void Common_Mutex_Destroy(Common_Mutex* mutex)
{
	pthread_mutex_destroy(mutex);
}

inline void Common_Mutex_Enter(Common_Mutex* mutex)
{
	int err = pthread_mutex_lock(mutex);
	assert(err == 0);
}

inline void Common_Mutex_Leave(Common_Mutex* mutex)
{
	int err = pthread_mutex_unlock(mutex);
	assert(err == 0);
}
