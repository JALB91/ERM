#!/bin/sh

cd build/ && cmake -G Xcode .. && cp -r ../res Debug/

for i in "$@"
do
	case $i in
		-o|--open)
			open erm.xcodeproj
	esac
	case $i in
		-f|--fastrun)
			xcodebuild && Debug/ERM
	esac
done
