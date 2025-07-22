cp /usr/local/lib/libwt.so.* ./cmake-build-debug &&
cp /usr/local/lib/libwthttp.so.* ./cmake-build-debug &&
docker build . -t lambda-snail:latest