!#/bin/bash

cp /usr/local/lib/libwt.so.* ./cmake-build-debug
cp /usr/local/lib/libwthttp.so.* ./cmake-build-debug
cp /usr/local/lib/libwtdbo.so.* ./cmake-build-debug
cp /usr/local/lib/libwtdbosqlite3.so.* ./cmake-build-debug
docker build . -t lambda-snail:latest