build_mode=$1
if [ -z $build_mode ];
  then {
    build_mode='release'
  }
fi

echo "Building in $build_mode mode ..."

use_cookies=$2
if [ "$use_cookies" = "true" ];
  then {
    echo 'Embedding cookie file'
    . ./use-cookies.sh $build_mode
  }
fi

cp /usr/local/lib/libwt.so.* "./cmake-build-$build_mode" &&
cp /usr/local/lib/libwthttp.so.* "./cmake-build-$build_mode" &&
docker build . -t lambda-snail:latest --build-arg MODE=$build_mode