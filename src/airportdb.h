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

public:


    AirportDB();
    void load( const char* fname );

    /**
     * 
     */
    const apt_dat_t* find( const char* id ) const;
    int ifind( const char* id ) const;

    const_iterator begin() const { return airports_.begin(); }
    const_iterator end()   const { return airports_.end(); }

private:
    airport_t airports_;
};

#endif // AIRPORT_DB_H_
