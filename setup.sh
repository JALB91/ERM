#!/bin/bash

if [[ "$OSTYPE" == "darwin"* ]]; then
	./modules/ERM_Setup/bin/OSX/Release/ERM_Setup_exe $@
elif [[ "$OSTYPE" == "win32" || "$OSTYPE" == "msys" ]]; then
	./modules/ERM_Setup/bin/WIN/Release/ERM_Setup_exe $@
else
	echo "Invalid OS $OSTYPE"
	exit 1
fi

exit $?
