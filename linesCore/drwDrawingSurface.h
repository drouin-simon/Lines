#ifndef __drwDrawingSurface_h_
#define __drwDrawingSurface_h_

class drwDrawingSurface
{
public:
    drwDrawingSurface() {}
    ~drwDrawingSurface() {}
    virtual void NeedRedraw() = 0;
    virtual void NeedRedraw( int x, int y, int width, int height ) = 0;
    virtual void NeedRedrawOverlay() = 0;
    virtual void NeedRedrawOverlay( int x, int y, int width, int height ) = 0;
};

#endif
