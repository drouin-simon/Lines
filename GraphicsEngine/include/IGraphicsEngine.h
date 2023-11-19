#ifndef __IGraphicsEngine_h_
#define __IGraphicsEngine_h_

#include "IncludeGLEW.h"
#include "IncludeGl.h"

#include "../linesCore/PolygonData.h"

#include <SVL.h>
#include <assert.h>

// TODO: Use SVL to make Vec3/4
class IGraphicsEngine
{
public:
    // drwCamera.cpp
    virtual void SetViewPort(int x, int y, int width, int height) = 0;
    virtual void SetProjectionViewPort(int x, int y, int width, int height) = 0;
    virtual void SetModelViewOrtho2D(double left, double right, double bottom, double top) = 0;

    // drwDrawableTexture.cpp
    virtual unsigned int InitTexture(int level, int internalFormat, int width, int height, int border, int format, int type, void* pixels) = 0;
    virtual unsigned int InitFrameBuffer(unsigned int texId) = 0;
    virtual void DisplayTexture(unsigned int texId, int level, int internalFormat, int width, int height, int border, int format, int type, void* pixels) = 0;
    virtual void ClearTexture(unsigned int fbId) = 0;
    virtual void ReleaseTexture(unsigned int texId, unsigned int fbId) = 0;
    virtual void PasteTextureToScreen(unsigned int texId, int texWidth, int texHeight, int x, int y, int screenWidth, int screenHeight) = 0;
    virtual void ClearScreen(int texWidth, int texHeight, int x, int y, int screenWidth, int screenHeight) = 0;
    
    virtual void Upload(unsigned int texId, int level, int internalFormat, int width, int height, int border, int format, int type, unsigned char* buffer) = 0;
    virtual void Download(unsigned int texId, int level, int format, int type, unsigned char* buffer) = 0;
    virtual void Download(unsigned int texId, int level, int format, int type, unsigned short* buffer) = 0;
    virtual void Download(unsigned int texId, int level, int format, int type, float* buffer) = 0;

    // lcCircle.cpp
    virtual void DrawPolygon(PolygonData& data, Vec4 color) = 0;
    
    // line.cpp
    virtual void DrawLine(double* pointsBuffer, unsigned int* pointsIndexBuffer, size_t pointsIndexSize, double lineWidth, Vec4 color) = 0;

    // wideline.cpp
    virtual void FillLine(double* poinstBuffer, unsigned int* indicesBuffer, size_t indicesSize, Vec4 color) = 0;
    virtual void DrawWideLine(double* verticlesBuffer, unsigned int* indicesBuffer, size_t indicesSize, double* normalsBuffer, double* uvsBuffer, Vec4 color) = 0;
    virtual void SetupPasteTextureToScreen(Vec4 color) = 0;
    virtual void SetupEraseTextureToScreen(Vec4 color = Vec4{ 0.0, 0.0, 0.0, 1.0 }) = 0;
    virtual void ResetTextureState() = 0;
    
    // drwGLRenderer.h
    virtual void enableScissorTest() = 0;
    virtual void disableScissorTest() = 0;
    virtual void enable(GLenum cap) = 0;
    virtual void scissor(GLint x, GLint y, GLsizei width, GLsizei height) = 0;
    virtual void initializeState() = 0;
    virtual void initializeModelViewMatrix() = 0;
    virtual void clear() = 0;
    virtual void clear(GLbitfield mask) = 0;
    virtual void clearColor(GLclampf red, GLclampf green, GLclampf blue, GLclampf alpha) = 0;
    virtual void blendFunc(GLenum sfactor, GLenum dfactor) = 0;
    virtual void color4d(float red, float green, float blue, float alpha) = 0;
    //virtual void color4d(GLdouble red, GLdouble green, GLdouble blue, GLdouble alpha) = 0;
    virtual GLuint createProgram() = 0;
    virtual void attachShader(GLuint glslProgram, GLuint glslVertexShader) = 0;
    virtual void getProgramIv(GLuint program, GLenum pname, GLint *params) = 0;
    virtual void linkProgram(GLuint program) = 0;
    virtual void getProgramInfoLog(GLuint program, GLsizei maxLength, GLsizei *length, GLchar *infoLog ) = 0; 
    virtual void pushName(GLuint name) = 0;
    virtual void popMatrix() = 0;
    virtual void pushMatrix() = 0;
    virtual void popName() = 0;

    // Other
    virtual void BindFrameBuffer(unsigned int fbId) = 0;
    virtual bool SetVariable(unsigned int programId, const char* name, int value) = 0;
    virtual bool SetVariable(unsigned int programId, const char* name, float value) = 0;

    virtual void DeleteShader(unsigned int shaderId) = 0;
    virtual void DeleteProgram(unsigned int programId) = 0;
};

#endif
