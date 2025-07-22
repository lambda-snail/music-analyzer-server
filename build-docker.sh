build_mode=$1
if [ -z $build_mode ];
  then {
    build_mode = "release"
  }
fi

cp /usr/local/lib/libwt.so.* "./cmake-build-$build_mode" &&
cp /usr/local/lib/libwthttp.so.* "./cmake-build-$build_mode" &&
docker build . -t lambda-snail:latest --build-arg MODE=$build_mode