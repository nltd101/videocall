#!/bin/bash
echo "welcome"
qmake CONFIG+=release
make
echo "build project with debug mode successfully"