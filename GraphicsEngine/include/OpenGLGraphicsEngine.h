#ifndef __OpenGLGraphicsEngine_h_
#define __OpenGLGraphicsEngine_h_

#include "IncludeGLEW.h"
#include "IncludeGl.h"

#include "IGraphicsEngine.h"

class OpenGLGraphicsEngine : public IGraphicsEngine {

public:
    enum TYPES_MAPPING {
        RED = GL_RED,
        RGB = GL_RGB,
        RGBA = GL_RGBA,
        F32 = GL_FLOAT,
        UBYTE = GL_UNSIGNED_BYTE,
        R32F = GL_R32F,
        ONE = GL_ONE,
        ONE_MINUS_SRC_ALPHA = GL_ONE_MINUS_SRC_ALPHA,
        BLEND = GL_BLEND
    };

    void initialize() ;

    void SetViewPort(int x, int y, int width, int height) ;
    void SetProjectionViewPort(int x, int y, int width, int height);
    void SetModelViewOrtho2D(double left, double right, double bottom, double top) ;
    unsigned int InitTexture(int level, int internalFormat, int width, int height, int border, int format, int type, void* pixels) ;
    unsigned int InitFrameBuffer(unsigned int texId) ;
    void DisplayTexture(unsigned int texId, int level, int internalFormat, int width, int height, int border, int format, int type, void* pixels) ;
    void ClearTexture(unsigned int fbId) ;
    void ReleaseTexture(unsigned int texId, unsigned int fbId) ;
    void PasteTextureToScreen(unsigned int texId, int texWidth, int texHeight, int x, int y, int screenWidth, int screenHeight) ;
    void ClearScreen(int texWidth, int texHeight, int x, int y, int screenWidth, int screenHeight) ;
    void UploadUnsignedByte(unsigned int texId, int level, int internalFormat, int width, int height, int border, int format, unsigned char* buffer) ;
    void DownloadUnsignedByte(unsigned int texId, int level, int format, unsigned char* buffer) ;
    void DownloadUnsignedShort(unsigned int texId, int level, int format, unsigned short* buffer) ;
    void DownloadFloat(unsigned int texId, int level, int format, float* buffer) ;
    void DrawPolygon(PolygonData& data, Vec4 color) ;
    void DrawLine(double* pointsBuffer, unsigned int* pointsIndexBuffer, size_t pointsIndexSize, double lineWidth, Vec4 color) ;
    void FillLine(double* poinstBuffer, unsigned int* indicesBuffer, size_t indicesSize, Vec4 color) ;
    void DrawWideLine(double* verticlesBuffer, unsigned int* indicesBuffer, size_t indicesSize, double* normalsBuffer, double* uvsBuffer, Vec4 color) ;
    void SetupPasteTextureToScreen(Vec4 color) ;
    void SetupEraseTextureToScreen(Vec4 color) ;
    void ResetTextureState() ;
    void enableScissorTest() ;
    void disableScissorTest() ;
    void enable(unsigned int cap) ;
    void scissor(int x, int y, int width, int height) ;
    void initializeState() ;
    void initializeModelViewMatrix() ;
    void clear() ;
    void clear(unsigned int mask) ;
    void clearColor(float red, float green, float blue, float alpha) ;
    void blendFunc(unsigned int sfactor, unsigned int dfactor) ;
    void color4d(float red, float green, float blue, float alpha) ;
    unsigned int createProgram() ;
    void linkProgram(unsigned int glslProgram) ;
    void attachShader(unsigned int glslProgram, unsigned int glslVertexShader) ;
    void getProgramIv(unsigned int program, unsigned int pname, int* params) ;
    void getProgramInfoLog(unsigned int program, int maxLength, int* length, char* infoLog) ;
    void pushName(unsigned int name) ;
    void pushMatrix() ;
    void popMatrix() ;
    void popName() ;

    void BindFrameBuffer(unsigned int fbId) ;
    bool SetVariable(unsigned int programId, const char* name, int value) ;
    bool SetVariable(unsigned int programId, const char* name, float value) ;
    bool SetVariable(unsigned int programId, const char* name, Vec4 value) ;
    void GetVariable(unsigned int name, int* value) ;

    void DeleteShader(unsigned int shaderId) ;
    void DeleteProgram(unsigned int programId) ;

    bool CreateAndCompileShader(unsigned shaderType, unsigned& shaderId, std::vector< std::string >& files, std::vector< std::string >& memSources) ;
    bool CreateAndCompileVertexShader(unsigned& shaderId, std::vector< std::string >& files, std::vector< std::string >& memSources) ;
    bool CreateAndCompileFragmentShader(unsigned& shaderId, std::vector< std::string >& files, std::vector< std::string >& memSources) ;
    bool LoadOneShaderSource(const char* filename, std::string& shaderSource);

    void BlendMaxEquation();
    void UseProgram(unsigned int programId);
    void DrawToTexture(unsigned int& fbId, int& backupFbId);
    void Flush() ;
};

#endif // __OpenGLGraphicsEngine_h_
