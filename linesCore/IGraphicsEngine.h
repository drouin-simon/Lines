#ifndef __IGraphicsEngine_h_
#define __IGraphicsEngine_h_

#include "IncludeGLEW.h"
#include "IncludeGl.h"
#include "PolygonData.h"

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

        // General
    // virtual void clear(int x, int y, int width, int height) = 0;
    // virtual void setupFrame() = 0;

    // // Line
    // virtual int draw(Line line) = 0;
    // virtual void erase(int id) = 0;

    // // Texture
    // virtual void drawTexture() = 0;
    
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
    virtual void color4d(GLdouble red, GLdouble green, GLdouble blue, GLdouble alpha) = 0;
    virtual GLuint createProgram() = 0;
    virtual void attachShader(GLuint glslProgram, GLuint glslVertexShader) = 0;
    virtual void getProgramIv(GLuint program, GLenum pname, GLint *params) = 0;
    virtual void linkProgram(GLuint program) = 0;
    virtual void getProgramInfoLog(GLuint program, GLsizei maxLength, GLsizei *length, GLchar *infoLog ) = 0; 
    virtual void pushName(GLuint name) = 0;
    virtual void popMatrix() = 0;
    virtual void pushMatrix() = 0;
    virtual void popName() = 0;
};

class OpenGLGraphicsEngine : public IGraphicsEngine {

    void SetViewPort(int x, int y, int width, int height) {
        glViewport(x, y, width, height);
    }

    void SetProjectionViewPort(int x, int y, int width, int height) {
        this->SetViewPort(x, y, width, height);
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
    }
    void SetModelViewOrtho2D(double left, double right, double bottom, double top) {
        gluOrtho2D(left, right, bottom, top);
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
    }

    unsigned int InitTexture(int level, int internalFormat, int width, int height, int border, int format, int type, void* pixels) {
        unsigned int texId;

        glGenTextures(1, &texId);
        glBindTexture(GL_TEXTURE_RECTANGLE_ARB, texId);
        glTexParameteri(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
        glTexImage2D(GL_TEXTURE_RECTANGLE_ARB, level , internalFormat, width, height, border, format, type, pixels);
        glBindTexture(GL_TEXTURE_RECTANGLE_ARB, 0);

        return texId;
    }

    unsigned int InitFrameBuffer(unsigned int texId) {
        unsigned int fbId;

        int backupFbId = 0;
        glGetIntegerv(GL_FRAMEBUFFER_BINDING, &backupFbId);
        glGenFramebuffersEXT(1, &fbId);
        glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, fbId);
        glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_TEXTURE_RECTANGLE_ARB, texId, 0);
        GLenum ret = glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT);
        assert(ret == GL_FRAMEBUFFER_COMPLETE_EXT);
        glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, backupFbId);

        return fbId;
    }

    void DisplayTexture(unsigned int texId, int level, int internalFormat, int width, int height, int border, int format, int type, void* pixels) {
        glBindTexture(GL_TEXTURE_RECTANGLE_ARB, texId);
        glTexImage2D(GL_TEXTURE_RECTANGLE_ARB, level, internalFormat, width, height, border, format, type, pixels);
        glBindTexture(GL_TEXTURE_RECTANGLE_ARB, 0);
    }

    void ClearTexture(unsigned int fbId) {
        int backupFbId = 0;
        glGetIntegerv(GL_FRAMEBUFFER_BINDING, &backupFbId);
        glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, fbId);
        glClearColor(0.0, 0.0, 0.0, 0.0);
        glClear(GL_COLOR_BUFFER_BIT);
        glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, backupFbId);
    }

    void ReleaseTexture(unsigned int texId, unsigned int fbId) {
        if (fbId)
            glDeleteFramebuffersEXT(1, &fbId);
        if (texId)
            glDeleteTextures(1, &texId);
    }

    void PasteTextureToScreen(unsigned int texId, int texWidth, int texHeight, int x, int y, int screenWidth, int screenHeight) {
        glMatrixMode(GL_PROJECTION);
        glPushMatrix();
        glLoadIdentity();
        gluOrtho2D(0, texWidth, 0, texHeight);

        glBindTexture(GL_TEXTURE_RECTANGLE_ARB, texId);
        glBegin(GL_QUADS);
        {
            glTexCoord2i(x, y);					glVertex2d(x, y);
            glTexCoord2i(x + screenWidth, y);			glVertex2d(x + screenWidth, y);
            glTexCoord2i(x + screenWidth, y + screenHeight);	glVertex2d(x + screenWidth, y + screenHeight);
            glTexCoord2i(x, y + screenHeight);			glVertex2d(x, y + screenHeight);
        }
        glEnd();
        glBindTexture(GL_TEXTURE_RECTANGLE_ARB, 0);

        glPopMatrix();
        glMatrixMode(GL_MODELVIEW);
    }

    void ClearScreen(int texWidth, int texHeight, int x, int y, int screenWidth, int screenHeight) {
        glDisable(GL_BLEND);
        glMatrixMode(GL_PROJECTION);
        glPushMatrix();
        glLoadIdentity();
        gluOrtho2D(0, texWidth, 0, texHeight);

        glColor4d(0.0, 0.0, 0.0, 0.0);
        glBegin(GL_QUADS);
        {
            glVertex2d(x, y);
            glVertex2d(x + screenWidth, y);
            glVertex2d(x + screenWidth, y + screenHeight);
            glVertex2d(x, y + screenHeight);
        }
        glEnd();

        glPopMatrix();
        glMatrixMode(GL_MODELVIEW);
        glEnable(GL_BLEND);
    }

    void Upload(unsigned int texId, int level, int internalFormat, int width, int height, int border, int format, int type,  unsigned char* buffer)
    {
        glBindTexture(GL_TEXTURE_RECTANGLE_ARB, texId);
        glTexImage2D(GL_TEXTURE_RECTANGLE_ARB, level, internalFormat, width, height, border, format, type, buffer);
        glBindTexture(GL_TEXTURE_RECTANGLE_ARB, 0);
    }

    void Download(unsigned int texId, int level, int format, int type, unsigned char* buffer)
    {
        glBindTexture(GL_TEXTURE_RECTANGLE_ARB, texId);
        glGetTexImage(GL_TEXTURE_RECTANGLE_ARB, level, format, type, buffer);
        glBindTexture(GL_TEXTURE_RECTANGLE_ARB, 0);
    }

    void Download(unsigned int texId, int level, int format, int type, unsigned short* buffer)
    {
        glBindTexture(GL_TEXTURE_RECTANGLE_ARB, texId);
        glGetTexImage(GL_TEXTURE_RECTANGLE_ARB, level, format, type, buffer);
        glBindTexture(GL_TEXTURE_RECTANGLE_ARB, 0);
    }

    void Download(unsigned int texId, int level, int format, int type, float* buffer)
    {
        glBindTexture(GL_TEXTURE_RECTANGLE_ARB, texId);
        glGetTexImage(GL_TEXTURE_RECTANGLE_ARB, 0, GL_RED, GL_FLOAT, buffer);
        glGetTexImage(GL_TEXTURE_RECTANGLE_ARB, level, format, type, buffer);
        glBindTexture(GL_TEXTURE_RECTANGLE_ARB, 0);
    }

    void DrawPolygon(PolygonData& data,  Vec4 color) {
        drwVec2Array& points = data.GetPoints();
        if (points.size() > 0)
        {

            glColor4d(color[0], color[1], color[2], color[3]);
            double* pbuffer = points.GetBuffer();
            glVertexPointer(2, GL_DOUBLE, 0, pbuffer);

            drwIndexArray& triangles = data.GetTriangles();
            if (triangles.size() > 0)
            {
                int numberOfIndexes = triangles.size();
                glDrawElements(GL_TRIANGLES, numberOfIndexes, GL_UNSIGNED_INT, triangles.GetBuffer());
            }

            drwIndexArray& lines = data.GetLines();
            if (lines.size() > 0)
            {
                int numberOfIndexes = lines.size();
                glDrawElements(GL_LINE_STRIP, numberOfIndexes, GL_UNSIGNED_INT, lines.GetBuffer());
            }
        }
    }

    void DrawLine(double* pointsBuffer, unsigned int* pointsIndexBuffer, size_t pointsIndexSize, double lineWidth, Vec4 color) {
        glColor4d(color[0], color[1], color[2], color[3]);
        glLineWidth(lineWidth);
        glVertexPointer(2, GL_DOUBLE, 0, pointsBuffer);
        glDrawElements(GL_LINE_STRIP, pointsIndexSize, GL_UNSIGNED_INT, pointsIndexBuffer);
    }

    void FillLine(double* poinstBuffer, unsigned int* indicesBuffer, size_t indicesSize, Vec4 color) {
        glColor4d(color[0], color[1], color[2], color[3]);
        glDisable(GL_BLEND);
        glEnable(GL_COLOR_LOGIC_OP);
        glLogicOp(GL_INVERT);
        glVertexPointer(2, GL_DOUBLE, 0, poinstBuffer);
        glDisableClientState(GL_TEXTURE_COORD_ARRAY);
        glDrawElements(GL_TRIANGLE_FAN, indicesSize, GL_UNSIGNED_INT, indicesBuffer);

        glDisable(GL_COLOR_LOGIC_OP);
    }

    void DrawWideLine(double* verticlesBuffer, unsigned int* indicesBuffer, size_t indicesSize, double* normalsBuffer, double* uvsBuffer, Vec4 color) {
        glColor4d(color[0], color[1], color[2], color[3]);
        glEnable(GL_BLEND);
        glBlendEquation(GL_MAX);

        glVertexPointer(2, GL_DOUBLE, 0, verticlesBuffer);
        glEnableClientState(GL_TEXTURE_COORD_ARRAY);
        glTexCoordPointer(3, GL_DOUBLE, 0, uvsBuffer);
        glEnableClientState(GL_NORMAL_ARRAY);
        glNormalPointer(GL_DOUBLE, 0, normalsBuffer);
        glDrawElements(GL_QUADS, indicesSize, GL_UNSIGNED_INT, indicesBuffer);

        glDisableClientState(GL_TEXTURE_COORD_ARRAY);
        glDisableClientState(GL_NORMAL_ARRAY);
    }

    void SetupPasteTextureToScreen(Vec4 color) {
        glBlendEquation(GL_FUNC_ADD);
        glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ONE);
        glColor4d(color[0], color[1], color[2], color[3]);
    }

    void SetupEraseTextureToScreen(Vec4 color) {
        glBlendEquationSeparate(GL_FUNC_ADD, GL_FUNC_REVERSE_SUBTRACT);
        glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ONE);
        glColor4d(color[0], color[1], color[2], color[3]);
    }

    void ResetTextureState() {
        glBlendEquation(GL_FUNC_ADD);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    }

    void enableScissorTest() override {
        glEnable(GL_SCISSOR_TEST);
    }

    void disableScissorTest() override {
        glDisable(GL_SCISSOR_TEST);
    }

    void enable(GLenum cap) override {
        glEnable(cap);
    }
    
    void scissor(GLint x, GLint y, GLsizei width, GLsizei height) override {
        glScissor( x, y, width, height );
    }
    
    void initializeState() override {
        glDisable( GL_DEPTH_TEST );
        glEnableClientState( GL_VERTEX_ARRAY );
        glEnable( GL_TEXTURE_RECTANGLE_ARB );
        glEnable( GL_BLEND );
        glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
    }
    
    void initializeModelViewMatrix() override { 
        glMatrixMode( GL_MODELVIEW );
        glLoadIdentity();
    }
    
    void clear() override {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }

    void clear(GLbitfield mask) override {
        glClear(mask);
    }

    void clearColor(GLclampf red, GLclampf green, GLclampf blue, GLclampf alpha) override {
        glClearColor(red, green, blue, alpha);
    }

    void blendFunc(GLenum sfactor, GLenum dfactor) override {
        glBlendFunc(sfactor, dfactor);
    }

    void color4d(GLdouble red, GLdouble green, GLdouble blue, GLdouble alpha) override {
        glColor4d(red, green, blue, alpha);
    }

    GLuint createProgram() override {
        return glCreateProgram();
    }

    void linkProgram(GLuint glslProgram) {
        glLinkProgram(glslProgram);
    }

    void attachShader(GLuint glslProgram, GLuint glslVertexShader) override {
        glAttachShader(glslProgram, glslVertexShader);
    }
    
    void getProgramIv(GLuint program, GLenum pname, GLint *params) override {
        glGetProgramiv(program, pname, params);
    }

    void getProgramInfoLog(GLuint program, GLsizei maxLength, GLsizei *length, GLchar *infoLog) override {
        glGetProgramInfoLog(program, maxLength, length, infoLog);
    }

    void Download(unsigned char* buffer) {
        //glBindTexture(GL_TEXTURE_RECTANGLE_ARB, m_texId);
        //glGetTexImage(GL_TEXTURE_RECTANGLE_ARB, 0, m_downloadPixelType, GL_UNSIGNED_BYTE, buffer);
        //glBindTexture(GL_TEXTURE_RECTANGLE_ARB, 0);
    }
    
    void pushName(GLuint name) override {
        glPushName(name);
    }
    
    void pushMatrix() override {
        glPushMatrix();
    }

    void popMatrix() override {
        glPopMatrix();
    }

    void popName() override {
        glPopName();
    }
};

class VulkanGraphicsEngine : IGraphicsEngine {

};

#endif
