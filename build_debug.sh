#!/bin/bash
echo "welcome"
qmake CONFIG+=debug
make
echo "build project with debug mode successfully"