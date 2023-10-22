#ifndef __OpenGLGraphicsEngine_h_
#define __OpenGLGraphicsEngine_h_

#include "IGraphicsEngine.h"

class OpenGLGraphicsEngine : public IGraphicsEngine {
    void SetViewPort(int x, int y, int width, int height) override;
    void SetProjectionViewPort(int x, int y, int width, int height);
    void SetModelViewOrtho2D(double left, double right, double bottom, double top) override;
    unsigned int InitTexture(int level, int internalFormat, int width, int height, int border, int format, int type, void* pixels) override;
    unsigned int InitFrameBuffer(unsigned int texId) override;
    void DisplayTexture(unsigned int texId, int level, int internalFormat, int width, int height, int border, int format, int type, void* pixels) override;
    void ClearTexture(unsigned int fbId) override;
    void ReleaseTexture(unsigned int texId, unsigned int fbId) override;
    void PasteTextureToScreen(unsigned int texId, int texWidth, int texHeight, int x, int y, int screenWidth, int screenHeight) override;
    void ClearScreen(int texWidth, int texHeight, int x, int y, int screenWidth, int screenHeight) override;
    void Upload(unsigned int texId, int level, int internalFormat, int width, int height, int border, int format, int type, unsigned char* buffer) override;
    void Download(unsigned int texId, int level, int format, int type, unsigned char* buffer) override;
    void Download(unsigned int texId, int level, int format, int type, unsigned short* buffer) override;
    void Download(unsigned int texId, int level, int format, int type, float* buffer) override;
    void DrawPolygon(PolygonData& data, Vec4 color) override;
    void DrawLine(double* pointsBuffer, unsigned int* pointsIndexBuffer, size_t pointsIndexSize, double lineWidth, Vec4 color) override;
    void FillLine(double* poinstBuffer, unsigned int* indicesBuffer, size_t indicesSize, Vec4 color) override;
    void DrawWideLine(double* verticlesBuffer, unsigned int* indicesBuffer, size_t indicesSize, double* normalsBuffer, double* uvsBuffer, Vec4 color) override;
    void SetupPasteTextureToScreen(Vec4 color) override;
    void SetupEraseTextureToScreen(Vec4 color) override;
    void ResetTextureState() override;
    void enableScissorTest() override;
    void disableScissorTest() override;
    void enable(GLenum cap) override;
    void scissor(GLint x, GLint y, GLsizei width, GLsizei height) override;
    void initializeState() override;
    void initializeModelViewMatrix() override;
    void clear() override;
    void clear(GLbitfield mask) override;
    void clearColor(GLclampf red, GLclampf green, GLclampf blue, GLclampf alpha) override;
    void blendFunc(GLenum sfactor, GLenum dfactor) override;
    void color4d(GLdouble red, GLdouble green, GLdouble blue, GLdouble alpha) override;
    GLuint createProgram() override;
    void linkProgram(GLuint glslProgram) override;
    void attachShader(GLuint glslProgram, GLuint glslVertexShader) override;
    void getProgramIv(GLuint program, GLenum pname, GLint* params) override;
    void getProgramInfoLog(GLuint program, GLsizei maxLength, GLsizei* length, GLchar* infoLog) override;
    void pushName(GLuint name) override;
    void pushMatrix() override;
    void popMatrix() override;
    void popName() override;
};

#endif // __OpenGLGraphicsEngine_h_