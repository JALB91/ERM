#!/bin/sh

OPT="-DPRINT_VARIABLES=FALSE"
DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && pwd )"

for i in "$@"
do
	case $i in
		-v|--verbose)
			OPT="-DPRINT_VARIABLES=TRUE"
	esac
done

GENERATOR=""
_OS=""

if [[ "$OSTYPE" == "linux-gnu" ]]; then
	_OS="Linux"
elif [[ "$OSTYPE" == "darwin"* ]]; then
	GENERATOR="XCode"
	_OS="OSX"
elif [[ "$OSTYPE" == "cygwin" || "$OSTYPE" == "msys" || "$OSTYPE" == "win32" ]]; then
	GENERATOR="Visual Studio 15 2017"
	_OS="WIN32"
else
	echo "Unknown OS"
	return
fi

cd ${DIR}/build/ && cmake "$OPT" -G "$GENERATOR" ..

for i in "$@"
do
	case $i in
		-o|--open)
			if [[ "$_OS" == "OSX" ]]; then
				open ERM.xcodeproj
			elif [[ "$_OS" == "WIN32" ]]; then
				start ERM.sln
			fi
	esac
	case $i in
		-f|--fastrun)
			if [[ "$_OS" == "OSX" ]]; then
				xcodebuild -target ERM && cd Debug/ && ./ERM
			elif [[ "$_OS" == "WIN32" ]]; then
				echo "Not ready yet"
			fi
	esac
done
