#!/bin/sh
# update-version.sh — выпуск новой версии words-searcher.
# 1) bump-version: замена версии в Autotools-файлах и документах
# 2) make dist: tarball
# 3) manifest-signer: Manifest из свежего tarball'а (попадает в тот же коммит)
# 4) git add/commit (тег — вручную, после успешной сборки в CI)
set -e
if [ -z "$1" ]; then echo "Usage: $0 <new-version>"; exit 1; fi
V="$1"

# Переименовать ebuild по версии пакета (words-searcher-<V>.ebuild)
EBUILD_DIR="packaging/gentoo/app-misc/words-searcher"
OLD_EBUILD=$(ls "$EBUILD_DIR"/words-searcher-*.ebuild 2>/dev/null | head -n1)
if [ -n "$OLD_EBUILD" ] && [ "$OLD_EBUILD" != "$EBUILD_DIR/words-searcher-$V.ebuild" ]; then
  git mv "$OLD_EBUILD" "$EBUILD_DIR/words-searcher-$V.ebuild" 2>/dev/null || mv "$OLD_EBUILD" "$EBUILD_DIR/words-searcher-$V.ebuild"
  echo "ebuild renamed to words-searcher-$V.ebuild"
fi

./tools/bump-version "$V" --autotools doc/REQUIREMENTS.ru.html doc/Testing.ru.html

# Пересобрать Makefile из configure.ac/Makefile.am (чтобы TAR/EXTRA_DIST применились)
# Фиксируем SOURCE_DATE_EPOCH (как в build:dist), чтобы tarball был детерминирован
export SOURCE_DATE_EPOCH=0
./configure
make dist
echo "===== [update-version.sh] SHA256 tarball ====="
sha256sum words-searcher-$V.tar.gz
echo "===== [update-version.sh] tar -tvzf (первые 10) ====="
tar -tvzf words-searcher-$V.tar.gz > /tmp/tar-list.txt
head -n 10 /tmp/tar-list.txt
echo "===== [update-version.sh] tar --version / gzip --version ====="
tar --version | head -n1
gzip --version | head -n1
./tools/manifest-signer \
  "words-searcher-$V.tar.gz" \
  "packaging/gentoo/app-misc/words-searcher/words-searcher-$V.ebuild" \
  "packaging/gentoo/app-misc/words-searcher/Manifest"

git add -A
git commit -m "release: $V" || echo "Nothing to commit."
# Тег ставится ВРУЧНУЮ, на ту версию, которая прошла сборку в CI.
# Автоматический тег здесь преждевременен: CI ещё не подтвердил сборку.
# git tag "v$V"

echo "Релиз $V готов (tarball, Manifest, commit). Тег — вручную, после CI."
