FROM ubuntu:latest
LABEL authors="LambdaSnail"
ADD cmake-build-debug/music-analyzer-server-0.3.0-Linux.deb .
ADD cmake-build-debug/libwt.so.4.11.4 /usr/local/lib
ADD cmake-build-debug/libwthttp.so.4.11.4 /usr/local/lib
ADD cmake-build-debug/libwtdbo.so.4.11.4 /usr/local/lib
ADD cmake-build-debug/libwtdbosqlite3.so.4.11.4 /usr/local/lib
RUN apt-get update && \
    apt-get -y --no-install-recommends \
      install libboost-thread-dev libboost-filesystem-dev libboost-program-options-dev && \
    apt-get -y --no-install-recommends \
      install ./music-analyzer-server-0.3.0-Linux.deb && \
    apt-get autoremove -y && \
    apt-get clean && \
    rm -r /var/lib/apt/lists/* music-analyzer-server-0.3.0-Linux.deb
WORKDIR /usr/lambda-snail/music-server
ENTRYPOINT ["./music-analyzer-server", \
            "--docroot", ".", \
            "--config", "./config/wt_config.xml", \
            "--http-address", "0.0.0.0", \
            "--http-port", "80"]
EXPOSE 80
EXPOSE 443