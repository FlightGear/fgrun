#!/bin/sh

aclocal -I . -I m4 \
  && autoheader \
  && automake --add-missing --copy \
  && autoconf

