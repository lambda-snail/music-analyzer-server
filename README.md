# Running Locally

```bash
 ./todo --docroot . --config ./config/wt_config.xml --http-address 0.0.0.0 --http-port 9090
```

The `resourcesURL` property in `wt_config.xml` should point to the `resources/` directory. The path is relative to
the `docroot` directory.

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