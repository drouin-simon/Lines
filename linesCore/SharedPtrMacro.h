#ifndef __SharedPtrMacro_h_
#define __SharedPtrMacro_h_

#ifdef USE_BOOST
#include <boost/shared_ptr.hpp>
#define SharedPtrMacro(ClassName) \
typedef boost::shared_ptr<ClassName> s_ptr;
#else
#include <memory>
#define SharedPtrMacro(ClassName) \
typedef std::shared_ptr<ClassName> s_ptr;
#endif

#endif
