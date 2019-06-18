#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.69])
AC_INIT(AliceNoKagami, 0.1, sternwzhang@outlook.com)
AC_CONFIG_SRCDIR([src/AliceNoKagami.cpp])
AC_CONFIG_HEADERS([config.h])
AM_INIT_AUTOMAKE([foreign -Wall])
AC_PROG_RANLIB
AM_PROG_AR
AC_LANG_PUSH([C++])

# Checks for programs.
AC_PROG_CXX
AC_PROG_CC

# Checks for libraries.
AC_CHECK_LIB(pthread,pthread_create,[],[AC_MSG_ERROR([No pthread support])],[])
AC_CHECK_LIB(sqlite3,sqlite3_open,[],[AC_MSG_ERROR([No sqlite3 support])],[])
AC_CHECK_LIB(curl,curl_easy_init,[],[AC_MSG_ERROR([No curl support])],[])
# Checks for header files.
AC_CHECK_HEADER(boost/asio.hpp,[],[AC_MSG_ERROR([No Boots::ASIO libraries, please install boost.])])
AC_CHECK_HEADER(sqlite3.h,[],[AC_MSG_ERROR([No header file for sqlite3])])
AC_CHECK_HEADERS([stddef.h stdint.h stdlib.h string.h strings.h])
# Checks for typedefs, structures, and compiler characteristics.
AC_CHECK_HEADER_STDBOOL
AC_C_INLINE
AC_TYPE_SIZE_T
AC_TYPE_UINT32_T
AC_TYPE_UINT64_T
AC_TYPE_UINT8_T

# Checks for library functions.
AC_CONFIG_FILES([Makefile
                 src/Makefile
                 src/third-party/gumbo-parser/Makefile])
AC_OUTPUT
