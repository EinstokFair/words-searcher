#!/bin/sh
set -e
if [ -z "$1" ]; then echo "Usage: $0 <new-version>"; exit 1; fi
./tools/bump-version "$1" --autotools doc/REQUIREMENTS.ru.html doc/Testing.ru.html
