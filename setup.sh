#!/bin/bash

if [[ "$OSTYPE" == "darwin"* ]]; then
	./libs/ERM_Setup/bin/MacOS/Release/ERM_Setup $@
elif [[ "$OSTYPE" == "win32" ]]; then
	./libs/ERM_Setup/bin/Windows/Release/ERM_Setup $@
fi

exit $?
