// airportdb.h -- Airport database load and query declarations.
//
// Written by Bernie Bright, started Sep 2003.
//
// Copyright (c) 2003  Bernie Bright - bbright@users.sourceforge.net
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

#ifndef AIRPORT_DB_H_
#define AIRPORT_DB_H_

#include <vector>
#include <string>

/**
 * Airport name, ICAO identifier and runways.
 */
struct apt_dat_t
{
    std::string id_;
    std::string name_;
    std::vector< std::string > runways_;
};

/**
 * 
 */
class AirportDB
{
public:
    typedef std::vector< apt_dat_t > airport_t;
    typedef airport_t::iterator iterator;
    typedef airport_t::const_iterator const_iterator;
    typedef airport_t::size_type size_type;

public:


    AirportDB();
    void load( const char* fname );

    /**
     * 
     */
    const apt_dat_t* find( const char* id ) const;

    const_iterator begin() const { return airports_.begin(); }
    const_iterator end()   const { return airports_.end(); }
    size_type size() const { return airports_.size(); }

private:
    airport_t airports_;
};

#endif // AIRPORT_DB_H_
