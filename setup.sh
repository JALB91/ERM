#!/bin/sh

OPT="-DPRINT_VARIABLES=FALSE"
CMAKE=false
DIR=`dirname $0`

for i in "$@"
do
	case $i in
		-v|--verbose)
			OPT="-DPRINT_VARIABLES=TRUE"
	esac
	case $i in
		--cmake)
			CMAKE=true
	esac
done

_GENERATOR=""
_OS=""

if [[ "$OSTYPE" == "linux-gnu" ]]; then
	_OS="Linux"
elif [[ "$OSTYPE" == "darwin"* ]]; then
	_GENERATOR="Xcode"
	_OS="OSX"
elif [[ "$OSTYPE" == "cygwin" || "$OSTYPE" == "msys" || "$OSTYPE" == "win32" ]]; then
	_GENERATOR="Visual Studio 15 2017"
	_OS="WIN32"
else
	echo "Unknown OS"
	exit
fi

if $CMAKE; then
	_GENERATOR="Unix Makefiles"
fi

cd ${DIR}/build/ && cmake "$OPT" -G "$_GENERATOR" ..

if $CMAKE; then
	exit
fi

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
	case $i in
		-c|--compile)
			if [[ "$_OS" == "OSX" ]]; then
				xcodebuild -target ERM
			elif [[ "$_OS" == "WIN32" ]]; then
				echo "Not ready yet"
			fi
	esac
done
