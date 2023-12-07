#ifndef __OpenGLESGraphicsEngine_h_
#define __OpenGLESGraphicsEngine_h_

#include "IncludeGLES.h"

#include "IGraphicsEngine.h"
//#include <glm/glm.hpp>
//#include <glm/gtc/matrix_transform.hpp>

class OpenGLESGraphicsEngine : public IGraphicsEngine {

public:
    enum TYPES_MAPPING {
        RED = 0x1903,
        RGB = 0x1907,
        RGBA = 0x1908,
        F32 = 0x1406,
        UBYTE = 0x1401,
        R32F = 0x822E,
        ONE = 1,
        ONE_MINUS_SRC_ALPHA = 0x0303,
        BLEND = 0x0BE2
    };

    OpenGLESGraphicsEngine();

    void initialize() override;

    // Abdellah
    void RenderObject();
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
    void UploadUnsignedByte(unsigned int texId, int level, int internalFormat, int width, int height, int border, int format, unsigned char* buffer) override;
    void DownloadUnsignedByte(unsigned int texId, int level, int format, unsigned char* buffer) override;
    void DownloadUnsignedShort(unsigned int texId, int level, int format, unsigned short* buffer) override;
    void DownloadFloat(unsigned int texId, int level, int format, float* buffer) override;

    // Nicolas
    void DrawPolygon(PolygonData& data, Vec4 color) override;
    void DrawLine(double* pointsBuffer, unsigned int* pointsIndexBuffer, size_t pointsIndexSize, double lineWidth, Vec4 color) override;
    void FillLine(double* poinstBuffer, unsigned int* indicesBuffer, size_t indicesSize, Vec4 color) override;
    void DrawWideLine(double* verticlesBuffer, unsigned int* indicesBuffer, size_t indicesSize, double* normalsBuffer, double* uvsBuffer, Vec4 color) override;
    void SetupPasteTextureToScreen(Vec4 color) override;
    void SetupEraseTextureToScreen(Vec4 color) override;
    void ResetTextureState() override;
    void enableScissorTest() override;
    void disableScissorTest() override;
    void enable(unsigned int cap) override;
    void scissor(int x, int y, int width, int height) override;
    void initializeState() override;
    void initializeModelViewMatrix() override;
    void clear() override;
    void clear(unsigned int mask) override;
    void clearColor(float red, float green, float blue, float alpha) override;
    void blendFunc(unsigned int sfactor, unsigned int dfactor) override;
    void color4d(float red, float green, float blue, float alpha) override;
    unsigned int createProgram() override;
    void linkProgram(unsigned int glslProgram) override;
    void attachShader(unsigned int glslProgram, unsigned int glslVertexShader) override;
    void getProgramIv(unsigned int program, unsigned int pname, int* params) override;
    void getProgramInfoLog(unsigned int program, int maxLength, int* length, char* infoLog) override;
    void pushName(unsigned int name) override;
    void pushMatrix() override;
    void popMatrix() override;
    void popName() override;

    void BindFrameBuffer(unsigned int fbId) override;
    bool SetVariable(unsigned int programId, const char* name, int value) override;
    bool SetVariable(unsigned int programId, const char* name, float value) override;
    void GetVariable(unsigned int name, int* value) override;

    void DeleteShader(unsigned int shaderId) override;
    void DeleteProgram(unsigned int programId) override;

    void VertexPointer(double* verticlesBuffer);

    bool CreateAndCompileShader(unsigned shaderType, unsigned& shaderId, std::vector< std::string >& files, std::vector< std::string >& memSources) override;
    bool CreateAndCompileVertexShader(unsigned& shaderId, std::vector< std::string >& files, std::vector< std::string >& memSources) override;
    bool CreateAndCompileFragmentShader(unsigned& shaderId, std::vector< std::string >& files, std::vector< std::string >& memSources) override;
    bool LoadOneShaderSource(const char* filename, std::string& shaderSource);

    void BlendMaxEquation() override;
    void UseProgram(unsigned int programId) override;

    void DrawToTexture(int& backupFbId) override;

    void Flush() override;


    //GLuint vertexShader;
    //GLuint fragmentShader;
    //unsigned int programId;

    //glm::mat4 projectionMatrix;
    //glm::mat4 modelViewMatrix;

};

#endif // __OpenGLESGraphicsEngine_h_
