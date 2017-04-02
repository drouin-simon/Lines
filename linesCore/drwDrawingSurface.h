#ifndef __drwDrawingSurface_h_
#define __drwDrawingSurface_h_

class Box2i;

class drwDrawingSurface
{
public:
    drwDrawingSurface() {}
    ~drwDrawingSurface() {}
    virtual void NeedRedraw() = 0;
    virtual void NotifyDisplaySettingsModified() = 0;
};

#endif
