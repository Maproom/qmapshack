Patching files of Routino build files in order to reduce it for MacOS builds only:

This directory contains:
- Makefile.conf.diff
- Makefile.diff
- src-Makefile.diff

Use the patch command:

   patch Makefile.conf.diff $ROUTINO_DIR/trunk/Makefile.conf
   patch Makefile.diff $ROUTINO_DIR/trunk/Makefile
   patch src-Makefile.diff $ROUTINO_DIR/trunk/src/Makefile
   
