#!/bin/sh
# build.sh — сборка words-searcher из исходников (для разработчиков, скачавших с GitHub/GitLab).
#
# Использование:
#   ./build.sh          — собрать (autoreconf, configure, make)
#   ./build.sh clean    — очистить сгенерированные файлы (make distclean и удаление Autotools-артефактов)
#   ./build.sh dist     — собрать дистрибутивный tarball (make dist)
#
# Требования: autoconf, automake, libtool, pkg-config, Qt6 (base dev), make, g++.

set -e

case "${1:-}" in
  clean)
    echo "Очистка..."
    if [ -f Makefile ]; then
      make distclean || true
    fi
    rm -rf autom4te.cache build-aux m4 aclocal.m4 configure config.log config.status
    rm -f Makefile Makefile.in
    find . -name '*.o' -delete 2>/dev/null || true
    find . -name 'moc_*.cpp' -delete 2>/dev/null || true
    rm -f qrc_resources.cpp
    echo "Готово."
    ;;
  dist)
    echo "Сборка tarball..."
    autoreconf -fi
    ./configure
    make dist
    echo "Готово: $(ls -1 words-searcher-*.tar.gz 2>/dev/null | tail -1)"
    ;;
  *)
    echo "Сборка..."
    autoreconf -fi
    ./configure
    make -j"$(nproc)"
    echo "Готово: ./words-searcher"
    ;;
esac
