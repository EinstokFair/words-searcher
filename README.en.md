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

Version 0.0.1 — skeleton: displays directory tree, four columns (Icon, Date, Name, Size), light/dark themes, font zoom.
