#!/bin/bash

if [[ "$OSTYPE" == "darwin"* ]]; then
	./libs/ERM_Setup/bin/OSX/Release/ERM_Setup $@
elif [[ "$OSTYPE" == "win32" || "$OSTYPE" == "msys" ]]; then
	./libs/ERM_Setup/bin/WIN/Release/ERM_Setup $@
else
	echo "Invalid OS $OSTYPE"
	exit 1
fi

exit $?
