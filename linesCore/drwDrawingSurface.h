#ifndef __drwDrawingSurface_h_
#define __drwDrawingSurface_h_

class Box2i;

class drwDrawingSurface
{
public:
    drwDrawingSurface() {}
    ~drwDrawingSurface() {}
    virtual void NotifyPlaybackStartStop( bool isStart ) = 0;
    virtual void WaitRenderFinished() = 0;
    virtual void Render() = 0;
    virtual void TryRender() = 0;
    virtual void PostRender() = 0;
};

#endif
