ACLOCAL_AMFLAGS = -I m4

bin_PROGRAMS = words-searcher

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
    src/moc_FsProxyModel.cpp

BUILT_SOURCES = $(nodist_words_searcher_SOURCES)
CLEANFILES = $(nodist_words_searcher_SOURCES)

words_searcher_CPPFLAGS = $(QT6_CFLAGS) -Isrc
words_searcher_CXXFLAGS = -std=c++17
words_searcher_LDADD = $(QT6_LIBS)

src/moc_%.cpp: src/%.h
	$(MOC) $< -o $@
