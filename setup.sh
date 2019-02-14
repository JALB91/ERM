#!/bin/sh

cd build/ && cmake -G Xcode ..

for i in "$@"
do
	case $i in
		-o|--open)
			open erm.xcodeproj
	esac
done
