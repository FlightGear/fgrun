#!/bin/sh

aclocal -I . \
  && autoheader \
  && automake --add-missing --copy \
  && autoconf

