# words-searcher

[![Build](https://github.com/EinstokFair/words-searcher/actions/workflows/build.yml/badge.svg?branch=master)](https://github.com/EinstokFair/words-searcher/actions/workflows/build.yml)

Directory tree viewer with future search functionality.

## Build

Requirements (Debian/Ubuntu):

    sudo apt install build-essential qt6-base-dev qt6-base-dev-tools \
        autoconf automake libtool pkg-config

Local build:

    autoreconf -fi
    ./configure
    make

## Run

    ./words-searcher

## Status

Version 0.0.6 — skeleton: displays directory tree, four columns (Icon, Date, Name, Size), light/dark themes, font zoom.

## License

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <https://www.gnu.org/licenses/>.
