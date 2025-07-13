# Running Locally

```bash
 ./todo --docroot . --config ./config/wt_config.xml --http-address 0.0.0.0 --http-port 9090
```

The `resourcesURL` property in `wt_config.xml` should point to the `resources/` directory. The path is relative to
the `docroot` directory.

# Downloading Videos Programmatically

Install [yt-dlp](https://github.com/yt-dlp/yt-dlp):

```bash
curl -L https://github.com/yt-dlp/yt-dlp/releases/latest/download/yt-dlp -o ./yt-dlp
chmod a+rx ./yt-dlp
sudo mv ./yt-dlp /usr/local/bin/yt-dlp
```

[More installation instructions](https://github.com/yt-dlp/yt-dlp/wiki/Installation)

Simple download (no output, webm-format)

```bash
yt-dlp -o '%(title)s.%(ext)s' -q btPJPFnesV4
```

Get the filename after download (one line output)

```bash
yt-dlp -o '%(title)s.%(ext)s' --get-filename btPJPFnesV4
```