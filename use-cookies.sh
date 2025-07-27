#!/usr/bin/bash

# Requires a profile in firefox called 'Music Analysis' with relevant cookies
# This script is mostly for my own use, not designed to be generally applicable to any scenario

build_mode=$1
if [ -z $build_mode ];
  then {
    build_mode = "release"
  }
fi

yt-dlp -q --cookies-from-browser firefox:"/home/niclas/.mozilla/firefox/dzd5mwav.Music Analysis" --cookies "cmake-build-${build_mode}/cookies.txt"