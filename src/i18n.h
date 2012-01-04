// i18n.h -- Internationalisation file.
//
// Written by Frederic Bouvier, started Dec 2007.
//
// Copyright (c) 2007  Frederic Bouvier - fredb@users.sourceforge.net
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License as
// published by the Free Software Foundation; either version 2 of the
// License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
//
// $Id:$

#ifndef i18n_h_included
#define i18n_h_included

#if defined( HAVE_CONFIG_H )
#  include "config.h"
#endif

#if !defined( ENABLE_NLS )

#define _(String) (String)
#define N_(String) String
#define textdomain(Domain)
#define bindtextdomain(Package, Directory)

#else

#include <libintl.h>
#define _(String) gettext (String)
#define gettext_noop(String) String
#define N_(String) gettext_noop (String)

#endif

#endif
