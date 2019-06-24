AUTOMAKE_OPTIONS=foreign
SUBDIRS=src src/third-party/gumbo-parser
noinst_PROGRAMS=KagamiConsole
KagamiConsole_LDADD=src/libKagami.a src/third-party/gumbo-parser/libgumbo.a
KagamiConsole_LIBS=lpthread -lsqlite3 -lcurl -lboost_system
KagamiConsole_SOURCES=KagamiConsole.cpp
