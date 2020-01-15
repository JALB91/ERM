#!/bin/sh

function print_help {
	echo "ERM Project setup script usage:"
	echo "    -h) Print this message"
	echo "    -C) Setup the project using CMake"
	echo "    -c) Compile the project"
	echo "    -o) Open in the editor"
	echo "    -f) Fast run the project"
}

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

OPT="-DPRINT_VARIABLES=FALSE"
DIR=`dirname $0`

while getopts ":hCcvof" opt; do
case $opt in
	v)
		OPT="-DPRINT_VARIABLES=TRUE"
		;;
	C)
		_USE_CMAKE=true
		_GENERATOR="Unix Makefiles"
		;;
	c)
		_COMPILE=true
		;;
	o)
		_OPEN=true
		;;
	f)
		_FAST_RUN=true
		;;
	h)
		print_help
		exit
		;;
	\?)
		echo "Invalid option: -$OPTARG"
		echo
		print_help
		exit 1
		;;
esac
done

cd ${DIR}/build/ && cmake "$OPT" -G "$_GENERATOR" ..

ln compile_commands.json ../ &> /dev/null

if [[ $_OPEN ]]; then
	if [[ $_USE_CMAKE ]]; then
		cd .. && vim
	elif [[ "$_OS" == "OSX" ]]; then
		open ERM.xcodeproj
	elif [[ "$_OS" == "WIN32" ]]; then
		start ERM.sln
	fi
	exit
fi
if [[ $_COMPILE ]]; then
	if [[ $_USE_CMAKE ]]; then
		make || exit 1
	elif [[ "$_OS" == "OSX" ]]; then
		xcodebuild -target ERM || exit 1
	elif [[ "$_OS" == "WIN32" ]]; then
		echo "Not ready yet" && exit 1
	fi
fi
if [[ $_FAST_RUN ]]; then
	if [[ $_USE_CMAKE ]]; then
		(cd .. && ./build/ERM) || exit 1
	elif [[ "$_OS" == "OSX" ]]; then
		(xcodebuild -target ERM && cd Debug/ && ./ERM) || exit 1
	elif [[ "$_OS" == "WIN32" ]]; then
		echo "Not ready yet" && exit 1
	fi
fi
