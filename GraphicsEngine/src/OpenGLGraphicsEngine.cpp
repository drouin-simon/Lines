#include "OpenGLGraphicsEngine.h"

bool OpenGLGraphicsEngine::LoadOneShaderSource(const char* filename, std::string& shaderSource)
{
    // Open shader file for reading
    FILE* f = fopen(filename, "rb");
    if (!f)
    {
        //ReportError("Couldn't open shader file %s\n", filename);
        return false;
    }

    // Get file size
    fseek(f, 0, SEEK_END);
    long length = ftell(f);
    fseek(f, 0, SEEK_SET);

    // Read file into shaderSource string
    char* charShaderSource = new char[length + 1];
    fread(charShaderSource, 1, length, f);
    charShaderSource[length] = '\0';
    shaderSource = charShaderSource;
    delete[] charShaderSource;

    // close the file
    fclose(f);

    return true;
}

void OpenGLGraphicsEngine::initialize() {
    GLenum err = glewInit();
    if (err != GLEW_OK) {
        throw("Failed to initialize GLEW: %s", glewGetErrorString(err));
    }
}

void OpenGLGraphicsEngine::SetViewPort(int x, int y, int width, int height) {
    glViewport(x, y, width, height);
}

void OpenGLGraphicsEngine::SetProjectionViewPort(int x, int y, int width, int height) {
    this->SetViewPort(x, y, width, height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
}
void OpenGLGraphicsEngine::SetModelViewOrtho2D(double left, double right, double bottom, double top) {
    glOrtho(left, right, bottom, top, -1, 1);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

unsigned int OpenGLGraphicsEngine::InitTexture(int level, int internalFormat, int width, int height, int border, int format, int type, void* pixels) {
    unsigned int texId;

    glGenTextures(1, &texId);
    glBindTexture(GL_TEXTURE_RECTANGLE_ARB, texId);
    glTexParameteri(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
    glTexImage2D(GL_TEXTURE_RECTANGLE_ARB, level, internalFormat, width, height, border, format, type, pixels);
    glBindTexture(GL_TEXTURE_RECTANGLE_ARB, 0);

    return texId;
}

unsigned int OpenGLGraphicsEngine::InitFrameBuffer(unsigned int texId) {
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

void OpenGLGraphicsEngine::DisplayTexture(unsigned int texId, int level, int internalFormat, int width, int height, int border, int format, int type, void* pixels) {
    glBindTexture(GL_TEXTURE_RECTANGLE_ARB, texId);
    glTexImage2D(GL_TEXTURE_RECTANGLE_ARB, level, internalFormat, width, height, border, format, type, pixels);
    glBindTexture(GL_TEXTURE_RECTANGLE_ARB, 0);
}

void OpenGLGraphicsEngine::ClearTexture(unsigned int fbId) {
    int backupFbId = 0;
    glGetIntegerv(GL_FRAMEBUFFER_BINDING, &backupFbId);
    glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, fbId);
    glClearColor(0.0, 0.0, 0.0, 0.0);
    glClear(GL_COLOR_BUFFER_BIT);
    glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, backupFbId);
}

void OpenGLGraphicsEngine::ReleaseTexture(unsigned int texId, unsigned int fbId) {
    if (fbId)
        glDeleteFramebuffersEXT(1, &fbId);
    if (texId)
        glDeleteTextures(1, &texId);
}

void OpenGLGraphicsEngine::PasteTextureToScreen(unsigned int texId, int texWidth, int texHeight, int x, int y, int screenWidth, int screenHeight) {
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    glOrtho(0, texWidth, 0, texHeight, -1, 1);

    glBindTexture(GL_TEXTURE_RECTANGLE_ARB, texId);
    glBegin(GL_QUADS);
    {
        glTexCoord2i(x, y);					
        glVertex2d(x, y);

        glTexCoord2i(x + screenWidth, y);			
        glVertex2d(x + screenWidth, y);

        glTexCoord2i(x + screenWidth, y + screenHeight);	
        glVertex2d(x + screenWidth, y + screenHeight);

        glTexCoord2i(x, y + screenHeight);			
        glVertex2d(x, y + screenHeight);
    }
    glEnd();
    glBindTexture(GL_TEXTURE_RECTANGLE_ARB, 0);

    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
}

void OpenGLGraphicsEngine::ClearScreen(int texWidth, int texHeight, int x, int y, int screenWidth, int screenHeight) {
    glDisable(GL_BLEND);
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    glOrtho(0, texWidth, 0, texHeight, -1, 1);

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

void OpenGLGraphicsEngine::UploadUnsignedByte(unsigned int texId, int level, int internalFormat, int width, int height, int border, int format, unsigned char* buffer)
{
    glBindTexture(GL_TEXTURE_RECTANGLE_ARB, texId);
    glTexImage2D(GL_TEXTURE_RECTANGLE_ARB, level, internalFormat, width, height, border, format, GL_UNSIGNED_BYTE, buffer);
    glBindTexture(GL_TEXTURE_RECTANGLE_ARB, 0);
}

void OpenGLGraphicsEngine::DownloadUnsignedByte(unsigned int texId, int level, int format, unsigned char* buffer)
{
    glBindTexture(GL_TEXTURE_RECTANGLE_ARB, texId);
    glGetTexImage(GL_TEXTURE_RECTANGLE_ARB, level, format, GL_UNSIGNED_BYTE, buffer);
    glBindTexture(GL_TEXTURE_RECTANGLE_ARB, 0);
}

void OpenGLGraphicsEngine::DownloadUnsignedShort(unsigned int texId, int level, int format, unsigned short* buffer)
{
    glBindTexture(GL_TEXTURE_RECTANGLE_ARB, texId);
    glGetTexImage(GL_TEXTURE_RECTANGLE_ARB, level, format, GL_UNSIGNED_SHORT, buffer);
    glBindTexture(GL_TEXTURE_RECTANGLE_ARB, 0);
}

void OpenGLGraphicsEngine::DownloadFloat(unsigned int texId, int level, int format, float* buffer)
{
    glBindTexture(GL_TEXTURE_RECTANGLE_ARB, texId);
    glGetTexImage(GL_TEXTURE_RECTANGLE_ARB, 0, GL_RED, GL_FLOAT, buffer);
    glGetTexImage(GL_TEXTURE_RECTANGLE_ARB, level, format, GL_FLOAT, buffer);
    glBindTexture(GL_TEXTURE_RECTANGLE_ARB, 0);
}

void OpenGLGraphicsEngine::DrawPolygon(PolygonData& data, Vec4 color) {
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

void OpenGLGraphicsEngine::DrawLine(double* pointsBuffer, unsigned int* pointsIndexBuffer, size_t pointsIndexSize, double lineWidth, Vec4 color) {
    glColor4d(color[0], color[1], color[2], color[3]);
    glLineWidth(lineWidth);
    glVertexPointer(2, GL_DOUBLE, 0, pointsBuffer);
    glDrawElements(GL_LINE_STRIP, pointsIndexSize, GL_UNSIGNED_INT, pointsIndexBuffer);
}

void OpenGLGraphicsEngine::FillLine(double* poinstBuffer, unsigned int* indicesBuffer, size_t indicesSize, Vec4 color) {
    glColor4d(color[0], color[1], color[2], color[3]);
    glDisable(GL_BLEND);
    glEnable(GL_COLOR_LOGIC_OP);
    glLogicOp(GL_INVERT);
    glVertexPointer(2, GL_DOUBLE, 0, poinstBuffer);
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);
    glDrawElements(GL_TRIANGLE_FAN, indicesSize, GL_UNSIGNED_INT, indicesBuffer);

    glDisable(GL_COLOR_LOGIC_OP);
}

void OpenGLGraphicsEngine::DrawWideLine(double* verticlesBuffer, unsigned int* indicesBuffer, size_t indicesSize, double* normalsBuffer, double* uvsBuffer, Vec4 color) {
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

void OpenGLGraphicsEngine::SetupPasteTextureToScreen(Vec4 color) {
    glBlendEquation(GL_FUNC_ADD);
    glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ONE);
    glColor4d(color[0], color[1], color[2], color[3]);
}

void OpenGLGraphicsEngine::SetupEraseTextureToScreen(Vec4 color) {
    glBlendEquationSeparate(GL_FUNC_ADD, GL_FUNC_REVERSE_SUBTRACT);
    glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ONE);
    glColor4d(color[0], color[1], color[2], color[3]);
}

void OpenGLGraphicsEngine::ResetTextureState() {
    glBlendEquation(GL_FUNC_ADD);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void OpenGLGraphicsEngine::enableScissorTest() {
    glEnable(GL_SCISSOR_TEST);
}

void OpenGLGraphicsEngine::disableScissorTest() {
    glDisable(GL_SCISSOR_TEST);
}

void OpenGLGraphicsEngine::enable(unsigned int cap) {
    glEnable(cap);
}

void OpenGLGraphicsEngine::scissor(int x, int y, int width, int height) {
    glScissor(x, y, width, height);
}

void OpenGLGraphicsEngine::initializeState() {
    glDisable(GL_DEPTH_TEST);
    glEnableClientState(GL_VERTEX_ARRAY);
    glEnable(GL_TEXTURE_RECTANGLE_ARB);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void OpenGLGraphicsEngine::initializeModelViewMatrix() {
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

void OpenGLGraphicsEngine::clear() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void OpenGLGraphicsEngine::clear(unsigned int mask) {
    glClear(mask);
}

void OpenGLGraphicsEngine::clearColor(float red, float green, float blue, float alpha) {
    glClearColor(red, green, blue, alpha);
}

void OpenGLGraphicsEngine::blendFunc(unsigned int sfactor, unsigned int dfactor) {
    glBlendFunc(sfactor, dfactor);
}

void OpenGLGraphicsEngine::color4d(float red, float green, float blue, float alpha) {
    glColor4d(red, green, blue, alpha);
}

unsigned int OpenGLGraphicsEngine::createProgram() {
    return glCreateProgram();
}

void OpenGLGraphicsEngine::linkProgram(unsigned int glslProgram) {
    glLinkProgram(glslProgram);
}

void OpenGLGraphicsEngine::attachShader(unsigned int glslProgram, unsigned int glslVertexShader) {
    glAttachShader(glslProgram, glslVertexShader);
}

void OpenGLGraphicsEngine::getProgramIv(unsigned int program, unsigned int pname, int* params) {
    glGetProgramiv(program, pname, params);
}

void OpenGLGraphicsEngine::getProgramInfoLog(unsigned int program, int maxLength, int* length, char* infoLog) {
    glGetProgramInfoLog(program, maxLength, length, infoLog);
}

void OpenGLGraphicsEngine::pushName(unsigned int name) {
    glPushName(name);
}

void OpenGLGraphicsEngine::pushMatrix() {
     glPushMatrix();
}

void OpenGLGraphicsEngine::popMatrix() {
    glPopMatrix();
}

void OpenGLGraphicsEngine::popName() {
    glPopName();
}

void OpenGLGraphicsEngine::BindFrameBuffer(unsigned int fbId) {
    glBindFramebuffer(GL_FRAMEBUFFER, fbId);
}

bool OpenGLGraphicsEngine::SetVariable(unsigned int programId, const char* name, int value) {
    int location = glGetUniformLocation(programId, name);
    if (location != -1)
    {
        glUniform1i(location, value);
        return true;
    }
    return false;
}

bool OpenGLGraphicsEngine::SetVariable(unsigned int programId, const char* name, float value) {
    int location = glGetUniformLocation(programId, name);
    if (location != -1)
    {
        glUniform1f(location, value);
        return true;
    }
    return false;
}

void OpenGLGraphicsEngine::GetVariable(unsigned int name, int* value) {
    glGetIntegerv(name, value);
}

void OpenGLGraphicsEngine::DeleteShader(unsigned int shaderId) {
    glDeleteShader(shaderId);
}

void OpenGLGraphicsEngine::DeleteProgram(unsigned int programId) {
    glDeleteProgram(programId);
}

bool OpenGLGraphicsEngine::CreateAndCompileVertexShader(unsigned& shaderId, std::vector< std::string >& files, std::vector< std::string >& memSources) {
    return CreateAndCompileShader(GL_VERTEX_SHADER, shaderId, files, memSources);
}
bool OpenGLGraphicsEngine::CreateAndCompileFragmentShader(unsigned& shaderId, std::vector< std::string >& files, std::vector< std::string >& memSources) {
    return CreateAndCompileShader(GL_FRAGMENT_SHADER, shaderId, files, memSources);
}

bool OpenGLGraphicsEngine::CreateAndCompileShader(unsigned shaderType, unsigned& shaderId, std::vector< std::string >& files, std::vector< std::string >& memSources) {
    // Add sources from files to m_memSources vector
    for (unsigned i = 0; i < files.size(); ++i)
    {
        std::string newSource;
        if (!LoadOneShaderSource(files[i].c_str(), newSource))
            return false;
        memSources.push_back(newSource);
    }

    // put all the sources in an array of const GLchar*
    const GLchar** shaderStringPtr = new const GLchar * [memSources.size()];
    for (unsigned i = 0; i < memSources.size(); ++i)
    {
        shaderStringPtr[i] = memSources[i].c_str();
    }

    // Create the shader and set its source
    shaderId = glCreateShader(shaderType);
    glShaderSource(shaderId, memSources.size(), shaderStringPtr, NULL);

    delete[] shaderStringPtr;

    // Compile the shader
    GLint success = 0;
    glCompileShader(shaderId);
    glGetShaderiv(shaderId, GL_COMPILE_STATUS, &success);
    //if (!success)
    //{
    //    GLint logLength = 0;
    //    glGetShaderiv(shaderId, GL_INFO_LOG_LENGTH, &logLength);
    //    GLchar* infoLog = new GLchar[logLength + 1];
    //    glGetShaderInfoLog(shaderId, logLength, NULL, infoLog);
    //    ReportError("Error in shader complilation: \n %s\n", infoLog);
    //    delete[] infoLog;
    //    return false;
    //}
    return true;
}

void OpenGLGraphicsEngine::BlendMaxEquation() {
    glBlendEquation(GL_MAX);
}

void OpenGLGraphicsEngine::UseProgram(unsigned int programId) {
    glUseProgram(programId);
}

void OpenGLGraphicsEngine::DrawToTexture(unsigned int& fbId, int &backupFbId) {
    this->GetVariable(GL_FRAMEBUFFER_BINDING, &backupFbId);
    this->BindFrameBuffer(fbId);
}

void OpenGLGraphicsEngine::Flush() {
    glFlush();
}