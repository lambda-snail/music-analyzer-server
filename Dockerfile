FROM ubuntu:latest
LABEL authors="LambdaSnail"
COPY cmake-build-debug/music-analyzer-server-0.3.0-Linux.deb ./
COPY "cmake-build-debug/libwt.so.4.11.4" "cmake-build-debug/libwthttp.so.4.11.4" "cmake-build-debug/libwtdbo.so.4.11.4" "cmake-build-debug/libwtdbosqlite3.so.4.11.4" "/usr/local/lib/"
ADD https://github.com/yt-dlp/yt-dlp/releases/latest/download/yt-dlp /usr/local/bin

# Install dependencies
RUN apt-get update && \
    apt-get -y --no-install-recommends install libboost-thread-dev libboost-filesystem-dev libboost-program-options-dev ffmpeg && \
    apt-get -y --no-install-recommends install ./music-analyzer-server-0.3.0-Linux.deb && \
    chmod a+rx /usr/local/bin/yt-dlp && \
# Cleanup stage to ensure minimal image
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