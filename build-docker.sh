use_cookies=$1
if [ "$use_cookies" = "true" ];
  then {
    echo 'Embedding cookie file'
    . ./use-cookies.sh "release"
  }
fi

cp /usr/local/lib/libwt.so.* "./cmake-build-release" &&
cp /usr/local/lib/libwthttp.so.* "./cmake-build-release" &&
docker build . -t music-analysis-server:latest --build-arg MODE="release"