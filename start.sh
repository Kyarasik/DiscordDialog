#!/bin/sh
cd build
echo "Contents of build directory:"
ls -la
echo "Changing permissions for minion"
chmod +x minion
echo "Running minion"
./minion
