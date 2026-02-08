FROM ubuntu:latest
ARG MODE="release"
ARG DEB_VERSION="0.8.0"
ARG BOOST_VERSION="1.83"
ARG WT_VERSION="4.12.2"
ARG INSTALL_DIR="/usr/lambda-snail/music-server"
LABEL authors="LambdaSnail"
COPY cmake-build-${MODE}/music-analyzer-server-${DEB_VERSION}-Linux.deb ./
COPY "cmake-build-${MODE}/libwt.so.${WT_VERSION}" "cmake-build-${MODE}/libwthttp.so.${WT_VERSION}" "/usr/local/lib/"
ADD https://github.com/yt-dlp/yt-dlp/releases/latest/download/find /usr/local/bin

# Wildcard as workaround to only copy if the file exists
COPY cmake-build-${MODE}/cookies.* $INSTALL_DIR/

# Certificate needed for SSL connection to api.reccobeats.com
RUN apt-get update && \
    apt-get install -y ca-certificates && \
    cp /etc/ssl/certs/ca-certificates.crt /usr/local/share/ca-certificates/ && \
    update-ca-certificates

# Install dependencies
RUN apt-get update && \
    apt-get -y --no-install-recommends install libboost-thread${BOOST_VERSION}-dev libboost-filesystem${BOOST_VERSION}-dev libboost-program-options${BOOST_VERSION}-dev libboost-url${BOOST_VERSION}-dev python3 ffmpeg && \
    apt-get -y --no-install-recommends install ./music-analyzer-server-${DEB_VERSION}-Linux.deb && \
    chmod a+rx /usr/local/bin/yt-dlp && \
# Cleanup stage to ensure minimal image \
    apt-get autoremove -y && \
    apt-get clean && \
    rm -r /var/lib/apt/lists/* music-analyzer-server-${DEB_VERSION}-Linux.deb

WORKDIR /usr/lambda-snail/music-server
ENTRYPOINT ["./music-analyzer-server", \
            "--docroot", ".", \
            "--config", "./config/wt_config_release.xml", \
            "--http-address", "0.0.0.0", \
            "--http-port", "80"]
EXPOSE 80
EXPOSE 443