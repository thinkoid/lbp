// -*- mode: c++ -*-

#include <options.hpp>

#include <boost/make_shared.hpp>
#include <boost/program_options.hpp>

#include <iostream>
#include <fstream>
#include <functional>

namespace po = boost::program_options;

static options_t global_options_;

const options_t&
global_options () {
    return global_options_;
}

options_t
global_options (options_t arg) {
    return std::swap (arg, global_options_), std::move (arg);
}

options_t::options_t () { }

options_t::const_reference
options_t::operator[] (const char* s) const {
    return map_ [s];
}

bool
options_t::have (const char* key) const {
    return bool (map_.count (key));
}
