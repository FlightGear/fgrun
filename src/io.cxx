#include "UserInterface.h"

void
UserInterface::io_list_select_cb( Fl_Browser* o )
{
    int n = o->value();
    if (n > 0)
    {
	io_delete->activate();
	io_protocol->activate();
	io_medium->activate();
	io_dir->activate();
	io_hz->activate();

	const char* p = o->text(n);
	char protocol[32];
	char medium[32];
	char dir[32];
	int hz = 0;
	char args[256];

	sscanf( p, "--%[^=]=%[^,],%[^,],%d,%s",
		protocol, medium, dir, &hz, args );
	set_choice( io_protocol, protocol );
	set_choice( io_medium, medium );
	io_medium->do_callback();
	set_choice( io_dir, dir );
	io_hz->value( hz );

	if (strcmp(medium, "file") == 0)
	{
	    file_name->value( args );
	}
	else if (strcmp(medium, "serial") == 0)
	{
	    char device[256];
	    char baud[32];
	    sscanf(args, "%[^,],%s", device, baud );
	    serial_port->value( device );
	    serial_baud_rate->value( baud );
	}
	else if (strcmp(medium, "socket") == 0)
	{
	    char host[128];
	    int port;
	    char type[32];
	    if (args[0] == ',')
	    {
		host[0] = 0;
		sscanf(args, ",%d,%s", &port, type );
	    }
	    else
	    {
		sscanf(args, "%[^,],%d,%s", host, &port, type );
	    }
	    socket_host->value(host);
	    socket_port->value( port );
	    if (strcmp(type, "tcp") == 0)
		socket_tcp->setonly();
	    else
		socket_udp->setonly();
	}
    }
}

void
UserInterface::io_list_new_cb()
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
UserInterface::io_list_update_cb()
{
    char buf[256];

    // --protocol=medium,dir,hz,filename|(device,baud)|(host,port,tcp|udp)

    if (strcmp(io_medium->text(), "file") == 0)
    {
	snprintf( buf, sizeof(buf), "--%s=file,%s,%d,%s",
		  io_protocol->text(), io_dir->text(), int(io_hz->value()),
		  file_name->value());
    }
    else if (strcmp(io_medium->text(), "serial") == 0)
    {
	snprintf( buf, sizeof(buf), "--%s=serial,%s,%d,%s,%s",
		  io_protocol->text(), io_dir->text(), int(io_hz->value()),
		  serial_port->value(), serial_baud_rate->value());
    }
    else if (strcmp(io_medium->text(), "socket") == 0)
    {
	snprintf( buf, sizeof(buf), "--%s=socket,%s,%d,%s,%d,%s",
		  io_protocol->text(), io_dir->text(), int(io_hz->value()),
		  socket_host->value(), int(socket_port->value()),
		  socket_tcp->value() ? "tcp" : "udp" );
    }

    int n = io_list->value();
    if (n > 0)
    {
	io_list->text(n, buf );
    }
}

void
UserInterface::io_list_medium_update_cb( Fl_Choice* o )
{
    if (strcmp(o->text(), "file") == 0)
    {
	file_group->show();
	menu_io_dir[2].deactivate();
	serial_group->hide();
	socket_group->hide();
    }
    else if (strcmp(o->text(), "serial") == 0)
    {
	file_group->hide();
	serial_group->show();
	socket_group->hide();
	menu_io_dir[2].activate();
    }
    else if (strcmp(o->text(), "socket") == 0)
    {
	file_group->hide();
	serial_group->hide();
	socket_group->show();
	menu_io_dir[2].activate();
    }

    io_list_update_cb();
}
