#!/usr/bin/env bash

# get-file.sh <url> [<file>]

url="$1"
file="$2"

if [ -z "$url" ]; then
	echo "Usage: $0 <url> [<file>]"
	exit 1
fi

if [ -z "$file" ]; then
	file="$(mktemp)"
fi

curl -sSL "$url" | base64 --decode > "$file"
echo "Decoded $url to $file"
