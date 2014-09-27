#ifndef __Macros_h_
#define __Macros_h_

#include <string.h>

#define SetMacro(name,type) \
virtual void Set##name( type _arg ) \
{ \
    if( this->name == _arg ) { return; } \
    this->name = _arg; \
}

#define GetMacro(name,type) \
type Get##name() \
{ \
    return this->name; \
}

#define SetVector2Macro(name,type) \
virtual void Set##name (type _arg1, type _arg2) \
{ \
	if ((this->name[0] != _arg1)||(this->name[1] != _arg2)) \
    { \
    	this->name[0] = _arg1; \
    	this->name[1] = _arg2; \
    } \
}; \
void Set##name (type _arg[2]) \
{ \
	this->Set##name (_arg[0], _arg[1]); \
} 

#define GetVector2Macro(name,type) \
virtual type *Get##name () \
{ \
	return this->name; \
} \

#define SetStringMacro(name) \
virtual void Set##name (const char * _arg) \
{ \
  if ( this->name == NULL && _arg == NULL) { return;} \
  if ( this->name && _arg && (!strcmp(this->name,_arg))) { return;} \
  if (this->name) { delete [] this->name; } \
  if (_arg) \
  { \
    this->name = new char[strlen(_arg)+1]; \
    strcpy(this->name,_arg); \
  } \
  else \
  { \
    this->name = NULL; \
  } \
} 

#define GetStringMacro(name) \
virtual char * Get##name () \
{ \
    return this->name; \
}

#endif
