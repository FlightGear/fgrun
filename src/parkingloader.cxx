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
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
//

#include <cstdlib>

#include "parkingloader.h"

ParkingLoader::ParkingLoader( apt_dat_t &d ):
    XMLVisitor(), apt_data(d) {}

void  ParkingLoader::startXML () {
  //cout << "Start XML" << endl;
}

void  ParkingLoader::endXML () {
  //cout << "End XML" << endl;
}

void  ParkingLoader::startElement( const char * name, const XMLAttributes &atts ) {
    if (name == string("Parking"))
    {
        std::string type, gateName, gateNumber;
        for (int i = 0; i < atts.size(); i++)
	{
            std::string attname = atts.getName( i );
	    if ( attname == "type" )
	        type = atts.getValue( i );
	    else if ( attname == "name" )
	        gateName = atts.getValue( i );
	    else if ( attname == "number" )
	        gateNumber = atts.getValue( i );
	}
        apt_data.parking_.insert( gateName + gateNumber );
    }
}

void  ParkingLoader::endElement (const char * name) {
}

void  ParkingLoader::data (const char * s, int len) {
}

void  ParkingLoader::pi (const char * target, const char * data) {
}

void  ParkingLoader::warning (const char * message, int line, int column) {
}

void  ParkingLoader::error (const char * message, int line, int column) {
}
