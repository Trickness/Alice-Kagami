AUTOMAKE_OPTIONS=foreign
SUBDIRS=src src/third-party/gumbo-parser
#noinst_PROGRAMS=KagamiConsole DebugConsole
noinst_PROGRAMS=DebugConsole

#KagamiConsole_LDADD=src/libKagami.a src/third-party/gumbo-parser/libgumbo.a
#KagamiConsole_LIBS=lpthread -lsqlite3 -lcurl -lboost_system
#KagamiConsole_SOURCES=KagamiConsole.cpp

DebugConsole_LDADD=src/libKagami.a src/third-party/gumbo-parser/libgumbo.a
DebugConsole_LIBS=lpthread -lsqlite3 -lcurl
DebugConsole_SOURCES=DebugConsole.cpp
