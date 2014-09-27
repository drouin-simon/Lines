#ifndef __SharedPtrMacro_h_
#define __SharedPtrMacro_h_

//#include <boost/shared_ptr.hpp>
#include <memory>

#define SharedPtrMacro(ClassName) \
typedef std::shared_ptr<ClassName> s_ptr;


#endif