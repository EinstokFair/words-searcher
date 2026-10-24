ACLOCAL_AMFLAGS = -I m4

# -fPIC для всех целей (Qt6 требует PIC)
AM_CXXFLAGS = -std=c++17 -fPIC

# Детерминированный dist: фиксированные timestamp/owner/порядок (GNU tar)
TAR_OPTIONS = --sort=name --mtime=@0 --owner=0 --group=0 --numeric-owner --format=posix --mode=go+u,go-w
export TAR_OPTIONS



# gzip без timestamp/имени в заголовке (воспроизводимый .tar.gz)
GZIP_ENV = -9 -n

bin_PROGRAMS = words-searcher tools/manifest-signer tools/bump-version

words_searcher_SOURCES = \
    src/main.cpp \
    src/MainWindow.cpp \
    src/MainWindow.h \
    src/SettingsDialog.cpp \
    src/SettingsDialog.h \
    src/FsProxyModel.cpp \
    src/FsProxyModel.h

nodist_words_searcher_SOURCES = \
    src/moc_MainWindow.cpp \
    src/moc_SettingsDialog.cpp \
    src/moc_FsProxyModel.cpp \
    qrc_resources.cpp

BUILT_SOURCES = $(nodist_words_searcher_SOURCES)
CLEANFILES = $(nodist_words_searcher_SOURCES)

words_searcher_CPPFLAGS = $(QT6_CFLAGS) -Isrc
words_searcher_LDADD = $(QT6_LIBS)

src/moc_%.cpp: src/%.h
	$(MOC) $< -o $@

EXTRA_DIST = COPYING README.en.md \
             resources.qrc \
             translations/words-searcher_en.ts \
             translations/words-searcher_ru.ts


# Переводы: установка .qm (ru всегда, en — если HAVE_EN)
translationsdir = $(datadir)/words-searcher/translations
translations_DATA = translations/words-searcher_ru.qm
if HAVE_EN
translations_DATA += translations/words-searcher_en.qm
endif
translations/words-searcher_ru.qm: translations/words-searcher_ru.ts
	$(LRELEASE) $< -qm $@
translations/words-searcher_en.qm: translations/words-searcher_en.ts
	$(LRELEASE) $< -qm $@
CLEANFILES += translations/words-searcher_en.qm

qrc_resources.cpp: resources.qrc
	$(RCC) -name resources $< -o $@

BUILT_SOURCES += qrc_resources.cpp
CLEANFILES += qrc_resources.cpp

tools_manifest_signer_SOURCES = tools/manifest-signer.cpp
tools_manifest_signer_CPPFLAGS = $(QT6_CFLAGS)
tools_manifest_signer_LDADD = $(QT6_LIBS)
tools_bump_version_SOURCES = tools/bump-version.cpp
tools_bump_version_CPPFLAGS = $(QT6_CFLAGS)
tools_bump_version_LDADD = $(QT6_LIBS)
