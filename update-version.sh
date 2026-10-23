#!/bin/sh
# update-version.sh — выпуск новой версии words-searcher.
# 1) bump-version: замена версии в Autotools-файлах и документах
# 2) make dist: tarball
# 3) manifest-signer: Manifest из свежего tarball'а (попадает в тот же коммит)
# 4) git add/commit (тег — вручную, после успешной сборки в CI)
set -e
if [ -z "$1" ]; then echo "Usage: $0 <new-version>"; exit 1; fi
V="$1"

./tools/bump-version "$V" --autotools doc/REQUIREMENTS.ru.html doc/Testing.ru.html

make dist
./tools/manifest-signer \
  "words-searcher-$V.tar.gz" \
  "packaging/gentoo/app-misc/words-searcher/words-searcher-0.1.ebuild" \
  "packaging/gentoo/app-misc/words-searcher/Manifest"

git add -A
git commit -m "release: $V" || echo "Nothing to commit."
# Тег ставится ВРУЧНУЮ, на ту версию, которая прошла сборку в CI.
# Автоматический тег здесь преждевременен: CI ещё не подтвердил сборку.
# git tag "v$V"

echo "Релиз $V готов (tarball, Manifest, commit). Тег — вручную, после CI."
