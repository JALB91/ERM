#!/bin/bash

if [[ "$OSTYPE" == "darwin"* ]]; then
	./bin/OSX/ERM_Setup_exe $@
elif [[ "$OSTYPE" == "win32" || "$OSTYPE" == "msys" ]]; then
	./bin/WIN/ERM_Setup_exe $@
else
	echo "Invalid OS $OSTYPE"
	exit 1
fi

exit $?
