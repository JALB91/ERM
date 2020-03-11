#!/bin/sh

function print_help {
	echo "ERM Project setup script usage:"
	echo "    -h) Print this message"
	echo "    -C) Setup the project using CMake"
	echo "    -N) Setup the project using Ninja"
	echo "    -t) Set target api [OpenGl, Vulknan (default)]"
	echo "    -r) Release mode"
	echo "    -i) Run cmake in interactive mode"
	echo "    -c) Compile the project"
	echo "    -o) Open in the editor"
	echo "    -f) Fast run the project"
	echo "    -f) Print CMake variables"
}

if [[ "$OSTYPE" == "darwin"* ]]; then
	_GENERATOR_NAME="Xcode"
	_GENERATOR="Xcode"
	_OS="OSX"
elif [[ "$OSTYPE" == "cygwin" || "$OSTYPE" == "msys" || "$OSTYPE" == "win32" ]]; then
	_GENERATOR_NAME="MSVC"
	_GENERATOR="Visual Studio 15 2017"
	_ARCHITECTURE="x64"
	_OS="WIN32"
else
	echo "Unknown OS"
	exit
fi

_BUILD_TYPE="Debug"
_TARGET_API="OpenGl"
_PRINT_VARIABLES="FALSE"
DIR=`dirname $0`

while getopts ":hCNt:ricvof" opt; do
case $opt in
	v)
		_PRINT_VARIABLES="TRUE"
		;;
	C)
		_GENERATOR_NAME="Make"
		_GENERATOR="Unix Makefiles"
		_USE_CMAKE=true
		;;
	N)
		_GENERATOR_NAME="Ninja"
		_GENERATOR="Ninja"
		_USE_NINJA=true
		;;
	t)
		_TARGET_API=$OPTARG
		;;
	r)
		_BUILD_TYPE="Release"
		_EXTRA_DEFINES="$_EXTRA_DEFINES -DNDEBUG"
		;;
	i)
		_INTERACTIVE=true
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

if [[ $_INTERACTIVE ]]; then
	_CMAKE_CMD=ccmake
else
	_CMAKE_CMD=cmake
fi

cd ${DIR}/build/
mkdir "$_GENERATOR_NAME"_"$_TARGET_API"_"$_BUILD_TYPE" &> /dev/null
cd "$_GENERATOR_NAME"_"$_TARGET_API"_"$_BUILD_TYPE"

$_CMAKE_CMD \
	-DPRINT_VARIABLES="$_PRINT_VARIABLES" \
	-DCMAKE_BUILD_TYPE="$_BUILD_TYPE" \
	-DTARGET_API="$_TARGET_API" \
	"$_EXTRA_DEFINES" \
	-G "$_GENERATOR" \
	-A "$_ARCHITECTURE" \
	../.. \
		|| exit

rm ${DIR}/compile_commands.json &> /dev/null
ln compile_commands.json ${DIR}/ &> /dev/null

if [[ $_OPEN ]]; then
	if [[ $_USE_CMAKE || $_USE_NINJA ]]; then
		cd ${DIR} && vim
	else
		cmake --open .
	fi
	exit
fi
if [[ $_COMPILE ]]; then
	cmake --build . || exit 1
fi
if [[ $_FAST_RUN ]]; then
	if [[ $_USE_CMAKE || $_USE_NINJA ]]; then
		cd $_BUILD_TYPE && ./bin/ERM || exit 1
	elif [[ "$_OS" == "OSX" ]]; then
		cd $_BUILD_TYPE/ && ./bin/ERM || exit 1
	elif [[ "$_OS" == "WIN32" ]]; then
		echo "Not ready yet" && exit 1
	fi
fi
