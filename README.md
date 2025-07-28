# Overview

This project is made for a friend of mine who needed to expose a simple interface for her students to be able to gather song
data for a statistics course project. This song data was previously provided by Spotify but the API endpoint has been deprecated. However,

However, there is another publicly available API that provides similar data at [Reccobeats](https://reccobeats.com/docs/apis/extract-audio-features).
This application allows users to send mp3 files to this API for analysis. To simplify even further, it also allows the user to choose a YouTube song for
analysis via [yt-dlp](https://github.com/yt-dlp/yt-dlp).

The project is built using C++ and the [Web Toolkit](https://www.webtoolkit.eu/wt/download). Since the purpose is very specific, I have made no
attempt to make the project usable in general. It will compile on Ubuntu using clang (but see the CMake file for dependencies), but I have no idea
if it will work on other platforms - please let me know if you try :)

The source code is made public as per the requirements of the GPL license, and also with a hope that it can serve as a future reference for anyone
who would like to see how to do certain things with WT, such as push notifications.

## Disclaimer

- This application is provided as-is, without any warranty or guarantees, nor do I assume any liability for anything that may happen when using it.
- Although the application download songs from YouTube, the downloaded files are never exposed to the users and are used for analysis purpose only. 
- The project is provided under the GPL license, as per the requirements of the [Web Toolkit](https://www.webtoolkit.eu/wt/download). 

# Requirements

- Linux
    - Linux - Built on an Ubuntu machine, and some things in the Dockerfile may rely on how things are configured in Ubuntu. You may need to tweak some parameters in the dockerfile if your environment differs.
    - Not tested on Windows
- Cpp23 - Built using Clang 19, but may work with GCC as well. 
- Web Toolkit (using wthttpd)
    - Please refer to the [installation instructions](https://www.webtoolkit.eu/wt/doc/reference/html/InstallationUnix.html) for requirements. 

# Running and Deploying

## Run locally

```bash
 ./todo --docroot . --config ./config/wt_config.xml --http-address 0.0.0.0 --http-port 9090
```

The `resourcesURL` property in `wt_config.xml` should point to the `resources/` directory. The path is relative to the `docroot` directory.

## Deploy

To build the docker container, run 

```shell
./build-docker.sh release | debug
```

This copies the WT libraries to the build directory before calling `docker build`. This is so we don't have to build WT from source every time we build the docker image.

The next step depends on the environment you deploy to. In my case I deploy to my own container registry in AWS, so the process looks something like this:

```shell
aws ecr get-login-password --region eu-north-1 | docker login --username AWS --password-stdin XXX.dkr.ecr.eu-north-1.amazonaws.com/your-container-registry
docker tag IMAGE-ID XXX.dkr.ecr.eu-north-1.amazonaws.com/your-container-registry:latest
docker push XXX.dkr.ecr.eu-north-1.amazonaws.com/your-container-registry:latest
```

Where `XXX` is you account id, and `eu--north-1` is the region - your region will probably be different. After this step you can put it in a ECS task and add a load balancer to expose the server to the outside world. These commands will in turn:

1. Provide an access token to `docker` so it can push the image to your container registry
2. Tag the image, and
3. Finally push the new image to your registry

# Features

## Downloading Videos Programmatically

The server executes shell commands to download YouTube videos locally and process them. To accomplish this, first install [yt-dlp](https://github.com/yt-dlp/yt-dlp):

```bash
curl -L https://github.com/yt-dlp/yt-dlp/releases/latest/download/yt-dlp -o ./yt-dlp
chmod a+rx ./yt-dlp
sudo mv ./yt-dlp /usr/local/bin/yt-dlp
```

[More installation instructions](https://github.com/yt-dlp/yt-dlp/wiki/Installation)

A simple download command looks like below. This command outputs nothing to the output stream, and downloads in .webm format.

```bash
yt-dlp -o '%(title)s.%(ext)s' -q btPJPFnesV4
```

To get the filename after download we can issue the following command. This outputs the filename on one line. Note that you will need to manually process the newline at the end somehow when returning from `popen`.

```bash
yt-dlp -o '%(title)s.%(ext)s' --get-filename btPJPFnesV4
```

Then we can convert it to mp3 using ffmpeg.

```bash
ffmpeg -i <file>.webm <file>.mp3
```

Finally, we note that we can also use `yt-dlp` to convert the clip to mp3 directly. This seems to be faster than performing the process above manually,
but the process above gives us more control (if we need it).

```shell
yt-dlp -o '%(title)s.%(ext)s' -t mp3 -q btPJPFnesV4
```

### Cookies

YouTube seems to be blocking data center IPs, so it is possible that `yt-dlp` works when running locally, but breaks when run from the cloud. To work around this, cookies
from a dummy account can be used when making a request using the `--cookies` flag. An example of how cookies can be exported can be found in `use-cookies.sh` which exports
the cookies from a firefox profile with a known name.

Please be careful when exporting cookies, so that you don't export "real" cookies that provide access to sits you may be using.

Please see the following links for more information:

- https://github.com/yt-dlp/yt-dlp/wiki/Extractors#exporting-youtube-cookies
- https://github.com/yt-dlp/yt-dlp/wiki/FAQ#how-do-i-pass-cookies-to-yt-dlp

## UI Updates From Threads

The server processes each song asynchronously, updating the UI as it goes along. To accomplish this using the web toolkit, we need to perform two steps:

1. Enable push updates form the server.
2. Take the UI lock

## Enable Push Updates

Push updates allow the server to push updates to the client. This is a one-liner when setting up the application. 

```c++
app->enableUpdates(true);
```

## UI Lock

Before we attempt to push an update to the client from a background thread, we must first acquire the application lock. The lock is a simple RAII type, just like corresponding locks in the standard library. We then check if we te lock is acquired and if so perform the update.

```c++
Wt::WApplication::UpdateLock uiLock(m_App);
if (uiLock) {
    m_Title->setText(name);
    m_App->triggerUpdate();
}
```

Calling `triggerUpdate()` will push the changes to the client.

## Caveats

One thing to keep in mind when working in a background thread, is that `WApplication::instance()` or the corresponding macro `wApp` seem to use the thread id to identify the session. This means that they will return `nullptr` when called from a background thread, so special care will need to be taken if the background worker needs this.

To work around this, any object or function that needs to be callable from a background thread should either store the application instance as a member variable or take the application as a parameter.

## See Also

- [Server Push Example](https://github.com/emweb/wt/blob/master/examples/feature/serverpush/ServerPush.C)
- [Forum Discussion](https://redmine.emweb.be/boards/2/topics/12601)