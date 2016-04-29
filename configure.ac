AC_PREREQ(2.59)
AC_INIT([skey], [2.0])

AM_INIT_AUTOMAKE([-Wall])

AC_PROG_CC
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_FILES([Makefile lib/Makefile man/Makefile skey/Makefile 
			  include/Makefile])

dnl check required headers
AC_CHECK_HEADER(stdlib.h, , [echo "require stdlib.h"; exit 1])
AC_CHECK_HEADER(sys/quota.h, , [echo "require sys/quota.h"; exit 1])

dnl check endianness
AC_C_BIGENDIAN(,[AC_DEFINE(IS_LITTLE_ENDIAN)])

dnl check if some functions exist
AC_CHECK_FUNC(gethostname, , [AC_DEFINE(NO_GETHOSTNAME)])
AC_CHECK_FUNC(setenv, , [AC_DEFINE(NO_SETENV)])
AC_CHECK_FUNC(getenv, , [AC_DEFINE(NO_GETENV)])

dnl check if where is crypt
AC_CHECK_FUNC(crypt, , AC_CHECK_LIB(crypt, crypt, ,[echo "require crypt"; exit 1]))


AC_OUTPUT
