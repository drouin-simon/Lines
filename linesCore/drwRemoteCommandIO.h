#ifndef __drwRemoteCommandSource_h_
#define __drwRemoteCommandSource_h_

class drwRemoteCommandIO
{
public:
    drwRemoteCommandIO() {}
    ~drwRemoteCommandIO() {}

    virtual void SendCommand( drwCommand::s_ptr ) = 0;
    virtual bool IsSharing() = 0;
};

#endif
