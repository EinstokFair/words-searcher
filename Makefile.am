ACLOCAL_AMFLAGS = -I m4

bin_PROGRAMS = words-searcher tools/manifest-signer

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
words_searcher_CXXFLAGS = -std=c++17
words_searcher_LDADD = $(QT6_LIBS)

src/moc_%.cpp: src/%.h
	$(MOC) $< -o $@

EXTRA_DIST = COPYING README.en.md

translations/words-searcher_en.qm: translations/words-searcher_en.ts
	$(LRELEASE) $< -qm $@

BUILT_SOURCES += translations/words-searcher_en.qm
CLEANFILES += translations/words-searcher_en.qm

qrc_resources.cpp: resources.qrc translations/words-searcher_en.qm
	$(RCC) -name resources $< -o $@

BUILT_SOURCES += qrc_resources.cpp
CLEANFILES += qrc_resources.cpp

tools_manifest_signer_SOURCES = tools/manifest-signer.cpp
tools_manifest_signer_CPPFLAGS = $(QT6_CFLAGS)
tools_manifest_signer_CXXFLAGS = -std=c++17
tools_manifest_signer_LDADD = $(QT6_LIBS)
