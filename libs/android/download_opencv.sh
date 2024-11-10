#!/usr/bin/env bash

version="4.3.0"
file="opencv-${version}-android-sdk.zip"
url="https://github.com/opencv/opencv/releases/download/${version}/opencv-${version}-android-sdk.zip"
wget -O "$file" "$url"
if [[ -f $file ]]; then
    echo "Downloaded $file"
    unzip $file
    mv OpenCV-android-sdk opencv-$version
    rm $file
else
    echo "Failed to download $file"
    exit 1
fi