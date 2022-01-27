#pragma once

#define EXPAND(x) x

#define ERM_GET_SECOND_ARG(arg1, arg2, ...)					 arg2
#define ERM_GET_THIRD_ARG(arg1, arg2, arg3, ...)			 arg3
#define ERM_GET_FOURTH_ARG(arg1, arg2, arg3, arg4, ...)		 arg4
#define ERM_GET_FIFTH_ARG(arg1, arg2, arg3, arg4, arg5, ...) arg5