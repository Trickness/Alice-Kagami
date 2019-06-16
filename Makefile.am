AUTOMAKE_OPTIONS=foreign
SUBDIRS=src src/third-party/gumbo-parser
noinst_PROGRAMS=KagamiConsole
KagamiConsole_LDADD=src/libKagami.a src/libAdaptor.a src/libCastBook.a src/third-party/gumbo-parser/libgumbo.a
KagamiConsole_LIBS=lpthread -lsqlite3
KagamiConsole_SOURCES=KagamiConsole.cpp
