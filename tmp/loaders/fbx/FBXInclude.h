#if defined(ERM_HOST_OSX)
#	pragma clang diagnostic push
#	pragma clang diagnostic ignored "-Wall"
#	pragma clang diagnostic ignored "-Wextra"
#	pragma clang diagnostic ignored "-pedantic-errors"
#	pragma clang diagnostic ignored "-pedantic"
#	pragma clang diagnostic ignored "-Wnested-anon-types"
#	pragma clang diagnostic ignored "-Wextra-semi"
#	include <fbxsdk.h>
#	pragma clang diagnostic pop
#elif defined(ERM_HOST_WINDOWS)
#	include <fbxsdk.h>
#endif
