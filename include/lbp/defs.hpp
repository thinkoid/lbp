// -*- mode: c++ -*-

#ifndef LBP_DEFS_HPP
#define LBP_DEFS_HPP

#include <lbp/config.hpp>

#include <boost/assert.hpp>

#define LBP_CHECK  BOOST_CHECK
#define LBP_VERIFY BOOST_VERIFY

#define LBP_STATIC_ASSERT(x) BOOST_STATIC_ASSERT (x)
#define LBP_STATIC_ASSERT_MSG(x, msg) BOOST_STATIC_ASSERT_MSG (x, msg)
#define LBP_ASSERT(x) BOOST_ASSERT (x)
#define LBP_UNUSED(x) ((void)x)

namespace lbp { }

#endif // LBP_DEFS_HPP
