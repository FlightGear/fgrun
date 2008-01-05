// io_list.cxx -- I/O option browser functions.
//
// Written by Bernie Bright, started Dec 2002.
//
// Copyright (c) 2002  Bernie Bright - bbright@users.sourceforge.net
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
// $Id$

#include <sstream>
#include <string>
#include <vector>
#include <FL/Fl_File_Chooser.H>
#include <FL/filename.H>

#include "advanced.h"
#include "util.h"
#include "i18n.h"

using std::string;
using std::vector;

void
Advanced::io_list_select_cb( Fl_Browser* o )
{
    int n = o->value();
    if (n <= 0)
	return;

    io_delete->activate();
    io_protocol->activate();
    io_medium->activate();
    io_dir->activate();
    io_hz->activate();

    // --PROTOCOL=MEDIUM,DIR,HZ,...
    // Split i/o option into tokens.
    string buf = o->text(n);
    vector< string > tokens;
    const string delims = "=,";
    string::size_type op = 0;
    string::size_type np;

    while ((np = buf.find_first_of( delims, op )) != buf.npos)
    {
	tokens.push_back( buf.substr( op, np-op ) );
	op = np + 1;
// 	if ((op = buf.find_first_not_of( delims, np )) == buf.npos)
// 	    break;
    }
    tokens.push_back( buf.substr( op, buf.npos ) );

    // Populate dialog controls.

    // Skip leading "--" of protocol token.
    set_choice( io_protocol, tokens[0].c_str() + 2 );
    set_choice( io_medium, tokens[1].c_str() );
    io_medium->do_callback();
    set_choice( io_dir, tokens[2].c_str() );
    io_hz->value( strtod( tokens[3].c_str(), 0 ) );

    generic_group->hide();
    Fl_Group* g = 0;
    n = -1;

    if (tokens[1] == "file")
    {
	io_file_name->value( tokens[4].c_str() );
	g = file_group;
	n = 5;
    }
    else if (tokens[1] == "serial")
    {
	serial_port->value( tokens[4].c_str() );
	serial_baud_rate->value( tokens[5].c_str() );
	g = serial_group;
	n = 6;
    }
    else if (tokens[1] == "socket")
    {
	socket_host->value( tokens[4].c_str() );
	socket_port->value( atoi( tokens[5].c_str() ) );
	if (tokens[6] == "tcp")
	    socket_tcp->setonly();
	else
	    socket_udp->setonly();

	g = socket_group;
	n = 7;
    }

    if (tokens[0] == "--generic" && g != 0)
    {
	io_generic_file->value( tokens[n].c_str() );

	generic_group->position( g->x(), g->y() + g->h() );
	generic_group->show();
    }
}

void
Advanced::io_list_new_cb()
{
    io_list->add("");
    io_list->value( io_list->size() );
    io_delete->activate();
    io_protocol->activate();
    io_medium->activate();
    io_medium->do_callback();
    io_dir->activate();
    io_hz->activate();
}

void
Advanced::io_list_update_cb()
{
    std::ostringstream oss;

    // --PROTOCOL=file,DIR,HZ,FILENAME[,GENERIC]
    // --PROTOCOL=serial,DIR,HZ,DEVICE,BAUD[,GENERIC]
    // --PROTOCOL=socket,DIR,HZ,HOST,PORT,(tcp|udp)[,GENERIC]

    if (strcmp(io_medium->text(), "file") == 0)
    {
	oss << "--" << io_protocol->text()
	    << "=file," << io_dir->text()
	    << "," << int(io_hz->value())
	    << "," << io_file_name->value();
    }
    else if (strcmp(io_medium->text(), "serial") == 0)
    {
	oss << "--" << io_protocol->text()
	    << "=serial," << io_dir->text()
	    << "," << int(io_hz->value())
	    << "," << serial_port->value()
	    << "," <<  serial_baud_rate->value();
    }
    else if (strcmp(io_medium->text(), "socket") == 0)
    {
	oss << "--" << io_protocol->text()
	    << "=socket," << io_dir->text()
	    << "," << int(io_hz->value())
	    << "," << socket_host->value()
	    << "," <<  int(socket_port->value())
	    << (socket_tcp->value() ? ",tcp" : ",udp");
    }

    if (strcmp( io_protocol->text(), "generic" ) == 0)
    {
	oss << ",";
	if (io_generic_file->size() > 0)
	    oss << io_generic_file->value();
    }

    int n = io_list->value();
    if (n > 0)
    {
	io_list->text(n, oss.str().c_str() );
    }
}

void
Advanced::io_medium_update_cb( Fl_Choice* o )
{
    file_group->hide();
    serial_group->hide();
    socket_group->hide();
    generic_group->hide();
    Fl_Group* g = 0;

    if (strcmp(o->text(), "file") == 0)
    {
	file_group->show();
	g = file_group;
	menu_io_dir[2].deactivate();
    }
    else if (strcmp(o->text(), "serial") == 0)
    {
	serial_group->show();
	g = serial_group;
	menu_io_dir[2].activate();
    }
    else if (strcmp(o->text(), "socket") == 0)
    {
	socket_group->show();
	g = socket_group;
	menu_io_dir[2].activate();
    }

    if (strcmp( io_protocol->text(), "generic" ) == 0)
    {
	generic_group->position( g->x(), g->y() + g->h() );
	generic_group->show();
    }

    io_list_update_cb();
}

void
Advanced::io_protocol_update_cb()
{
    io_list_update_cb();

    string protocol( io_protocol->text() );

    if (protocol != "generic")
    {
	generic_group->hide();
	return;
    }

    Fl_Group* g = 0;
    string medium( io_medium->text() );

    // Position the "generic" file selection below the last control.
    if (medium == "file")
    {
	g = file_group;
    }
    else if (medium == "serial")
    {
	g = serial_group;
    }
    else if (medium == "socket")
    {
	g = socket_group;
    }

    generic_group->position( g->x(), g->y() + g->h() );
    generic_group->show();
}

void
Advanced::io_file_cb()
{
}

void
Advanced::io_generic_file_cb()
{
    //static Fl_File_Chooser* fc = 0;
    string dir = fg_root_->value();
    dir += "/Protocol";
    const char* pat = "*.xml";
    const char* message = _("Select protocol file");

    Fl_File_Chooser fc( dir.c_str(), pat, Fl_File_Chooser::SINGLE, message );
    fc.show();

    while ( fc.shown() )
	Fl::wait();

    if (fc.value())
    {
	const char* fname = fl_filename_name( fc.value() );
	const char* ext = fl_filename_ext( fname );
	if (ext != 0 && *ext != 0)
	{
	    string f( fname, ext );
	    io_generic_file->value( f.c_str() );
	    io_list_update_cb();
	}
    }
}

