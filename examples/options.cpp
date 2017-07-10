// -*- mode: c++ -*-

#include <options.hpp>

#include <boost/make_shared.hpp>
#include <boost/program_options.hpp>

#include <iostream>
#include <fstream>
#include <functional>

namespace po = boost::program_options;

static options_t global_options_;

options_t::options_t (int argc, char** argv) {
    {
        auto tmp = std::make_pair (
            "program", po::variable_value (std::string (argv [0]), false));
        map_.insert (tmp);
    }

    po::options_description generic ("Generic options");
    po::options_description config ("Configuration options");

    generic.add_options ()
        ("version,v", "version")
        ("help,h", "this");

    config.add_options ()
        ("display,d", "display frames.")

        ("input,i",   po::value< std::string > ()->default_value ("0"),
         "input (file or stream index).")

        ("algorithm,a", po::value< std::string > ()->default_value ("olbp"),
         "algorithm");

    desc_ = boost::make_shared< po::options_description > ();

    desc_->add (generic);
    desc_->add (config);

    store (po::command_line_parser (argc, argv).options (*desc_).run (), map_);

    notify (map_);
}

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

void
program_options_from (int& argc, char** argv) {
    bool complete_invocation = false;

    options_t program_options (argc, argv);

    if (program_options.have ("version")) {
        std::cout << "OpenCV v3.1\n";
        complete_invocation = true;
    }

    if (program_options.have ("help")) {
        std::cout << program_options.description () << std::endl;
        complete_invocation = true;
    }

    if (complete_invocation)
        exit (0);

    global_options (program_options);
}
