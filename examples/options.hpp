// -*- mode: c++ -*-

#ifndef LBP_OPTIONS_HPP
#define LBP_OPTIONS_HPP

#include <boost/program_options.hpp>
#include <boost/program_options/variables_map.hpp>

struct options_t {
    typedef boost::program_options::variable_value value_type;

    typedef value_type& reference;
    typedef const value_type& const_reference;

public:
    options_t ();
    options_t (int, char**);

    bool
    have (const char*) const;

    const_reference
    operator[] (const char* key) const;

    const boost::program_options::options_description&
    description () const {
        return *desc_;
    }

private:
    boost::program_options::variables_map map_;
    boost::shared_ptr< boost::program_options::options_description > desc_;
};

const options_t&
global_options ();

options_t
global_options (options_t);

inline bool
have_global_option (const char* s) {
    return global_options ().have (s);
}

inline bool
have_global_option (const std::string& s) {
    return have_global_option (s.c_str ());
}

#endif // LBP_OPTIONS_HPP
