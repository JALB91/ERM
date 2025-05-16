#!/bin/bash

if [[ "$OSTYPE" == "darwin"* ]]; then
	./erm/bin/OSX/ERM_Setup_exe $@
elif [[ "$OSTYPE" == "win32" || "$OSTYPE" == "msys" ]]; then
	./erm/bin/WIN/ERM_Setup_exe $@
else
	echo "Invalid OS $OSTYPE"
	exit 1
fi

exit $?
