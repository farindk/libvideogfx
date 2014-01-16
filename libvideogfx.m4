# Configure paths for LibVideoGfx
# shamelessly stolen from Owen Taylor

dnl AM_PATH_LIBVIDEOGFX([MINIMUM-VERSION, [ACTION-IF-FOUND [, ACTION-IF-NOT-FOUND [, MODULES]]]])
dnl Test for LibVideoGfx, and define LIBVIDEOGFX_CFLAGS and LIBVIDEOGFX_LIBS
dnl
AC_DEFUN(AM_PATH_LIBVIDEOGFX,
[dnl 
dnl Get the cflags and libraries from the libvideogfx-config script
dnl
AC_ARG_WITH(libvideogfx-prefix,[  --with-libvideogfx-prefix=PFX   Prefix where LibVideoGfx is installed (optional)],
            libvideogfx_config_prefix="$withval", libvideogfx_config_prefix="")
AC_ARG_WITH(libvideogfx-exec-prefix,[  --with-libvideogfx-exec-prefix=PFX Exec prefix where LibVideoGfx is installed (optional)],
            libvideogfx_config_exec_prefix="$withval", libvideogfx_config_exec_prefix="")
AC_ARG_ENABLE(libvideogfxtest, [  --disable-libvideogfxtest       Do not try to compile and run a test LibVideoGfx program],
		    , enable_libvideogfxtest=yes)

dnl  for module in . $4
dnl  do
dnl      case "$module" in
dnl         gthread) 
dnl             gtk_config_args="$gtk_config_args gthread"
dnl         ;;
dnl      esac
dnl  done

  if test x$libvideogfx_config_exec_prefix != x ; then
     libvideogfx_config_args="$libvideogfx_config_args --exec-prefix=$libvideogfx_config_exec_prefix"
     if test x${LIBVIDEOGFX_CONFIG+set} != xset ; then
        LIBVIDEOGFX_CONFIG=$libvideogfx_config_exec_prefix/bin/libvideogfx-config
     fi
  fi
  if test x$libvideogfx_config_prefix != x ; then
     libvideogfx_config_args="$libvideogfx_config_args --prefix=$libvideogfx_config_prefix"
     if test x${LIBVIDEOGFX_CONFIG+set} != xset ; then
        LIBVIDEOGFX_CONFIG=$libvideogfx_config_prefix/bin/libvideogfx-config
     fi
  fi

  AC_PATH_PROG(LIBVIDEOGFX_CONFIG, libvideogfx-config, no)
  min_libvideogfx_version=ifelse([$1], ,0.0.0,$1)
  AC_MSG_CHECKING(for LibVideoGfx - version >= $min_libvideogfx_version)
  no_libvideogfx=""
  if test "$LIBVIDEOGFX_CONFIG" = "no" ; then
    no_libvideogfx=yes
  else
    LIBVIDEOGFX_CFLAGS=`$LIBVIDEOGFX_CONFIG $libvideogfx_config_args --cflags`
    LIBVIDEOGFX_LIBS=`$LIBVIDEOGFX_CONFIG $libvideogfx_config_args --libs`
    libvideogfx_config_major_version=`$LIBVIDEOGFX_CONFIG $libvideogfx_config_args --version | \
           sed 's/\([[0-9]]*\).\([[0-9]]*\).\([[0-9]]*\)/\1/'`
    libvideogfx_config_minor_version=`$LIBVIDEOGFX_CONFIG $libvideogfx_config_args --version | \
           sed 's/\([[0-9]]*\).\([[0-9]]*\).\([[0-9]]*\)/\2/'`
    libvideogfx_config_micro_version=`$LIBVIDEOGFX_CONFIG $libvideogfx_config_args --version | \
           sed 's/\([[0-9]]*\).\([[0-9]]*\).\([[0-9]]*\)/\3/'`
    if test "x$enable_libvideogfxtest" = "xyes" ; then
      ac_save_CFLAGS="$CFLAGS"
      ac_save_LIBS="$LIBS"
      CFLAGS="$CFLAGS $LIBVIDEOGFX_CFLAGS"
      LIBS="$LIBVIDEOGFX_LIBS $LIBS"
dnl
dnl Now check if the installed LibVideoGfx is sufficiently new. (Also sanity
dnl checks the results of gtk-config to some extent
dnl
dnl      rm -f conf.libvideogfxtest
dnl      AC_TRY_RUN([
dnl #include <libvideogfx/init.hh>
dnl #include <stdio.h>
dnl #include <stdlib.h>
dnl
dnl int 
dnl main ()
dnl {
dnl   int major, minor, micro;
dnl   char *tmp_version;
dnl 
dnl   system ("touch conf.gtktest");
dnl 
dnl   /* HP/UX 9 (%@#!) writes to sscanf strings */
dnl   tmp_version = g_strdup("$min_gtk_version");
dnl   if (sscanf(tmp_version, "%d.%d.%d", &major, &minor, &micro) != 3) {
dnl      printf("%s, bad version string\n", "$min_gtk_version");
dnl      exit(1);
dnl    }
dnl 
dnl   if ((gtk_major_version != $gtk_config_major_version) ||
dnl       (gtk_minor_version != $gtk_config_minor_version) ||
dnl       (gtk_micro_version != $gtk_config_micro_version))
dnl     {
dnl       printf("\n*** 'gtk-config --version' returned %d.%d.%d, but GTK+ (%d.%d.%d)\n", 
dnl              $gtk_config_major_version, $gtk_config_minor_version, $gtk_config_micro_version,
dnl              gtk_major_version, gtk_minor_version, gtk_micro_version);
dnl       printf ("*** was found! If gtk-config was correct, then it is best\n");
dnl       printf ("*** to remove the old version of GTK+. You may also be able to fix the error\n");
dnl       printf("*** by modifying your LD_LIBRARY_PATH enviroment variable, or by editing\n");
dnl       printf("*** /etc/ld.so.conf. Make sure you have run ldconfig if that is\n");
dnl       printf("*** required on your system.\n");
dnl       printf("*** If gtk-config was wrong, set the environment variable GTK_CONFIG\n");
dnl       printf("*** to point to the correct copy of gtk-config, and remove the file config.cache\n");
dnl       printf("*** before re-running configure\n");
dnl     } 
dnl #if defined (GTK_MAJOR_VERSION) && defined (GTK_MINOR_VERSION) && defined (GTK_MICRO_VERSION)
dnl   else if ((gtk_major_version != GTK_MAJOR_VERSION) ||
dnl 	   (gtk_minor_version != GTK_MINOR_VERSION) ||
dnl            (gtk_micro_version != GTK_MICRO_VERSION))
dnl     {
dnl       printf("*** GTK+ header files (version %d.%d.%d) do not match\n",
dnl 	     GTK_MAJOR_VERSION, GTK_MINOR_VERSION, GTK_MICRO_VERSION);
dnl       printf("*** library (version %d.%d.%d)\n",
dnl 	     gtk_major_version, gtk_minor_version, gtk_micro_version);
dnl     }
dnl #endif /* defined (GTK_MAJOR_VERSION) ... */
dnl   else
dnl     {
dnl       if ((gtk_major_version > major) ||
dnl         ((gtk_major_version == major) && (gtk_minor_version > minor)) ||
dnl         ((gtk_major_version == major) && (gtk_minor_version == minor) && (gtk_micro_version >= micro)))
dnl       {
dnl         return 0;
dnl        }
dnl      else
dnl       {
dnl         printf("\n*** An old version of GTK+ (%d.%d.%d) was found.\n",
dnl                gtk_major_version, gtk_minor_version, gtk_micro_version);
dnl         printf("*** You need a version of GTK+ newer than %d.%d.%d. The latest version of\n",
dnl 	       major, minor, micro);
dnl         printf("*** GTK+ is always available from ftp://ftp.gtk.org.\n");
dnl         printf("***\n");
dnl         printf("*** If you have already installed a sufficiently new version, this error\n");
dnl         printf("*** probably means that the wrong copy of the gtk-config shell script is\n");
dnl         printf("*** being found. The easiest way to fix this is to remove the old version\n");
dnl         printf("*** of GTK+, but you can also set the GTK_CONFIG environment to point to the\n");
dnl         printf("*** correct copy of gtk-config. (In this case, you will have to\n");
dnl         printf("*** modify your LD_LIBRARY_PATH enviroment variable, or edit /etc/ld.so.conf\n");
dnl         printf("*** so that the correct libraries are found at run-time))\n");
dnl       }
dnl     }
dnl   return 1;
dnl }
dnl ],, no_gtk=yes,[echo $ac_n "cross compiling; assumed OK... $ac_c"])
       CFLAGS="$ac_save_CFLAGS"
       LIBS="$ac_save_LIBS"
     fi
  fi
  if test "x$no_libvideogfx" = x ; then
     AC_MSG_RESULT(yes)
     ifelse([$2], , :, [$2])     
  else
     AC_MSG_RESULT(no)
     if test "$LIBVIDEOGFX_CONFIG" = "no" ; then
       echo "*** The libvideogfx-config script installed by LibVideoGfx could not be found"
       echo "*** If LibVideoGfx was installed in PREFIX, make sure PREFIX/bin is in"
       echo "*** your path, or set the LIBVIDEOGFX_CONFIG environment variable to the"
       echo "*** full path to libvideogfx-config."
     else
       if test -f conf.libvideogfxtest ; then
        :
       else
          echo "*** Could not run LibVideoGfx test program, checking why..."
          CFLAGS="$CFLAGS $LIBVIDEOGFX_CFLAGS"
          LIBS="$LIBS $LIBVIDEOGFX_LIBS"
          AC_TRY_LINK([
#include <libvideogfx/init.hh>
#include <stdio.h>
],      [ return ((libvideogfx_major_version) || (libvideogfx_minor_version) || (libvideogfx_micro_version)); ],
        [ echo "*** The test program compiled, but did not run. This usually means"
          echo "*** that the run-time linker is not finding LibVideoGfx or finding the wrong"
          echo "*** version of LibVideoGfx. If it is not finding LibVideoGfx, you'll need to set your"
          echo "*** LD_LIBRARY_PATH environment variable, or edit /etc/ld.so.conf to point"
          echo "*** to the installed location  Also, make sure you have run ldconfig if that"
          echo "*** is required on your system"
	  echo "***"
          echo "*** If you have an old version installed, it is best to remove it, although"
          echo "*** you may also be able to get things to work by modifying LD_LIBRARY_PATH" ],
        [ echo "*** The test program failed to compile or link. See the file config.log for the"
          echo "*** exact error that occured. This usually means LibVideoGfx was incorrectly installed"
          echo "*** or that you have moved LibVideoGfx since it was installed. In the latter case, you"
          echo "*** may want to edit the libvideogfx-config script: $LIBVIDEOGFX_CONFIG" ])
          CFLAGS="$ac_save_CFLAGS"
          LIBS="$ac_save_LIBS"
       fi
     fi
     LIBVIDEOGFX_CFLAGS=""
     LIBVIDEOGFX_LIBS=""
     ifelse([$3], , :, [$3])
  fi
  AC_SUBST(LIBVIDEOGFX_CFLAGS)
  AC_SUBST(LIBVIDEOGFX_LIBS)
  rm -f conf.libvideogfxtest
])
