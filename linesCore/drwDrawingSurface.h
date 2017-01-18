#ifndef __drwDrawingSurface_h_
#define __drwDrawingSurface_h_

#include "drwCommand.h"

class drwDrawingSurface
{
public:
    drwDrawingSurface() {}
    ~drwDrawingSurface() {}
    virtual drwCommand::s_ptr CreateMouseCommand( drwMouseCommand::MouseCommandType commandType, QMouseEvent * e ) = 0;
    virtual drwCommand::s_ptr CreateMouseCommand( drwMouseCommand::MouseCommandType commandType, QTabletEvent * e ) = 0;
};

#endif
