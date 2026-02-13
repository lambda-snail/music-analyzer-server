FROM ubuntu:questing
ARG MODE="release"
ARG DEB_VERSION="0.9.0"
ARG BOOST_VERSION="1.88"
ARG WT_VERSION="4.12.2"
ARG INSTALL_DIR="/usr/lambda-snail/music-server"
LABEL authors="LambdaSnail"
COPY cmake-build-${MODE}/music-analyzer-server-${DEB_VERSION}-Linux.deb ./
COPY "cmake-build-${MODE}/libwt.so.${WT_VERSION}" "cmake-build-${MODE}/libwthttp.so.${WT_VERSION}" "/usr/local/lib/"

# Wildcard as workaround to only copy if the file exists
COPY cmake-build-${MODE}/cookies.* $INSTALL_DIR/

# Certificate needed for SSL connection to api.reccobeats.com
RUN apt-get update && \
    apt-get install -y ca-certificates && \
    cp /etc/ssl/certs/ca-certificates.crt /usr/local/share/ca-certificates/ && \
    update-ca-certificates

# Install dependencies

# Comment in the following two rows for youtube support
#ADD https://github.com/yt-dlp/yt-dlp/releases/download/2026.02.04/yt-dlp /usr/local/bin
#RUN apt-get update && \
#    apt-get -y --no-install-recommends install curl unzip ffmpeg python3 && \
#    chmod a+rx /usr/local/bin/yt-dlp && \
#    curl -fsSL https://deno.land/install.sh | sh -s -- -y && \
#    ln /root/.deno/bin/deno /usr/bin && \
#    apt-get remove -y curl unzip

RUN apt-get update && \
    apt-get -y --no-install-recommends install libboost-thread${BOOST_VERSION} libboost-filesystem${BOOST_VERSION} libboost-program-options${BOOST_VERSION} libboost-url${BOOST_VERSION} && \
    apt-get -y --no-install-recommends install ./music-analyzer-server-${DEB_VERSION}-Linux.deb && \
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