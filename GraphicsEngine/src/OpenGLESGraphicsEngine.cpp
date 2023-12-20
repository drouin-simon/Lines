#include "OpenGLESGraphicsEngine.h"

#include <fstream>
#include <sstream>

#include <glm/gtc/type_ptr.hpp>

std::string readShaderFile(const std::string& filePath) {
    std::ifstream file(filePath);
    if (!file.is_open()) {
        return "";
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

OpenGLESGraphicsEngine::OpenGLESGraphicsEngine() {

    projectionMatrix = glm::mat4(1.0f);
    modelViewMatrix = glm::mat4(1.0f);

    this->programId = 0;

}

void OpenGLESGraphicsEngine::initialize() {
    this->openglFunctions = this->context->functions();

    const GLchar* vertexShaderCode = readShaderFile("shader/vertexShader.glsl").c_str();
    const GLchar* fragmentShaderCode = readShaderFile("shader/fragmentShader.glsl").c_str();

    this->vertexShader = this->openglFunctions->glCreateShader(GL_VERTEX_SHADER);
    this->openglFunctions->glShaderSource(this->vertexShader, 1, &vertexShaderCode, nullptr);
    this->openglFunctions->glCompileShader(this->vertexShader);


    this->fragmentShader = this->openglFunctions->glCreateShader(GL_FRAGMENT_SHADER);
    this->openglFunctions->glShaderSource(this->fragmentShader, 1, &fragmentShaderCode, nullptr);
    this->openglFunctions->glCompileShader(this->fragmentShader);

    this->openglFunctions->glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    //// Définir les points pour dessiner la ligne
    //GLfloat vertices[] = {
    //    -0.5f, -0.5f, 0.0f,  // Premier point
    //     0.5f,  0.5f, 0.0f   // Deuxième point
    //};

    //// Activer le pointeur de vertex
    //this->openglFunctions->glEnableClientState(GL_VERTEX_ARRAY);
    //this->openglFunctions->glVertexPointer(3, GL_FLOAT, 0, vertices);

    //// Dessiner la ligne
    //this->openglFunctions->glDrawArrays(GL_LINES, 0, 2);

    //// Désactiver le pointeur de vertex
    //this->openglFunctions->glDisableClientState(GL_VERTEX_ARRAY);
}

void OpenGLESGraphicsEngine::SetViewPort(int x, int y, int width, int height) {
    this->openglFunctions->glViewport(x, y, width, height);
}

void OpenGLESGraphicsEngine::SetProjectionViewPort(int x, int y, int width, int height) {
    this->SetViewPort(x, y, width, height);
    projectionMatrix = glm::ortho(0.0f, static_cast<float>(width), 0.0f, static_cast<float>(height));

}

void OpenGLESGraphicsEngine::SetModelViewOrtho2D(double left, double right, double bottom, double top) {

    modelViewMatrix = glm::ortho(static_cast<float>(left), static_cast<float>(right), static_cast<float>(bottom), static_cast<float>(top));
}
void OpenGLESGraphicsEngine::RenderObject() {

    glUseProgram(programId);

    GLint projMatrixLoc = glGetUniformLocation(programId, "u_projectionMatrix");
    glUniformMatrix4fv(projMatrixLoc, 1, GL_FALSE, &projectionMatrix[0][0]);

    GLint mvMatrixLoc = glGetUniformLocation(programId, "u_modelViewMatrix");
    glUniformMatrix4fv(mvMatrixLoc, 1, GL_FALSE, &modelViewMatrix[0][0]);

}


unsigned int OpenGLESGraphicsEngine::InitTexture(int level, int internalFormat, int width, int height, int border, int format, int type, void* pixels) {
    unsigned int texId;

    glGenTextures(1, &texId);
    glBindTexture(GL_TEXTURE_2D, texId);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D, level, internalFormat, width, height, border, format, type, pixels);
    glBindTexture(GL_TEXTURE_2D, 0);

    return texId;
}

unsigned int OpenGLESGraphicsEngine::InitFrameBuffer(unsigned int texId) {
    unsigned int fbId;

    glGenFramebuffers(1, &fbId);
    glBindFramebuffer(GL_FRAMEBUFFER, fbId);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texId, 0);
    GLenum ret = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    assert(ret == GL_FRAMEBUFFER_COMPLETE);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    return fbId;
}

void OpenGLESGraphicsEngine::DisplayTexture(unsigned int texId, int level, int internalFormat, int width, int height, int border, int format, int type, void* pixels) {
    glBindTexture(GL_TEXTURE_2D, texId);
    glTexImage2D(GL_TEXTURE_2D, level, internalFormat, width, height, border, format, type, pixels);
    glBindTexture(GL_TEXTURE_2D, 0);
}

void OpenGLESGraphicsEngine::ClearTexture(unsigned int fbId) {
    int backupFbId = 0;
    glGetIntegerv(GL_FRAMEBUFFER_BINDING, &backupFbId);
    glBindFramebuffer(GL_FRAMEBUFFER, fbId);
    glClearColor(0.0, 0.0, 0.0, 0.0);
    glClear(GL_COLOR_BUFFER_BIT);
    glBindFramebuffer(GL_FRAMEBUFFER, backupFbId);
}

void OpenGLESGraphicsEngine::ReleaseTexture(unsigned int texId, unsigned int fbId) {
    if (fbId)
        glDeleteFramebuffers(1, &fbId);
    if (texId)
        glDeleteTextures(1, &texId);
}

void OpenGLESGraphicsEngine::PasteTextureToScreen(unsigned int texId, int texWidth, int texHeight, int x, int y, int screenWidth, int screenHeight) {

    glUseProgram(programId);

    glm::mat4 projection = glm::ortho(0.0f, static_cast<float>(texWidth), 0.0f, static_cast<float>(texHeight));
    GLint projMatrixLoc = glGetUniformLocation(programId, "u_projectionMatrix");
    glUniformMatrix4fv(projMatrixLoc, 1, GL_FALSE, glm::value_ptr(projection));


    glm::mat4 modelView = glm::mat4(1.0f);
    GLint mvMatrixLoc = glGetUniformLocation(programId, "u_modelViewMatrix");
    glUniformMatrix4fv(mvMatrixLoc, 1, GL_FALSE, glm::value_ptr(modelView));


    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texId);


    glBindTexture(GL_TEXTURE_2D, 0);
    glUseProgram(0);
}

void OpenGLESGraphicsEngine::ClearScreen(int texWidth, int texHeight, int x, int y, int screenWidth, int screenHeight) {
    glEnable(GL_SCISSOR_TEST);
    glScissor(x, y, screenWidth, screenHeight);
    glClearColor(0.0, 0.0, 0.0, 0.0);
    glClear(GL_COLOR_BUFFER_BIT);
    glDisable(GL_SCISSOR_TEST);
}

void OpenGLESGraphicsEngine::UploadUnsignedByte(unsigned int texId, int level, int internalFormat, int width, int height, int border, int format, unsigned char* buffer)
{
    glBindTexture(GL_TEXTURE_2D, texId);
    glTexImage2D(GL_TEXTURE_2D, level, internalFormat, width, height, border, format, GL_UNSIGNED_BYTE, buffer);
    glBindTexture(GL_TEXTURE_2D, 0);
}

void OpenGLESGraphicsEngine::DownloadUnsignedByte(unsigned int texId, int level, int format, unsigned char* buffer)
{

    GLuint fbId;
    glGenFramebuffers(1, &fbId);
    glBindFramebuffer(GL_FRAMEBUFFER, fbId);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texId, level);
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE) {
        int width, height;  // TODO: Initialization
        glReadPixels(0, 0, width, height, format, GL_UNSIGNED_BYTE, buffer);
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glDeleteFramebuffers(1, &fbId);
}

void OpenGLESGraphicsEngine::DownloadUnsignedShort(unsigned int texId, int level, int format, unsigned short* buffer)
{

    GLuint fbId;
    glGenFramebuffers(1, &fbId);
    glBindFramebuffer(GL_FRAMEBUFFER, fbId);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texId, level);
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE) {
        int width, height; // TODO: Initialization
        glReadPixels(0, 0, width, height, format, GL_SHORT, buffer);
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glDeleteFramebuffers(1, &fbId);
}

void OpenGLESGraphicsEngine::DownloadFloat(unsigned int texId, int level, int format, float* buffer)
{

    GLuint fbId;
    glGenFramebuffers(1, &fbId);
    glBindFramebuffer(GL_FRAMEBUFFER, fbId);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texId, level);
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE) {
        int width, height;  // TODO: Initialization
        glReadPixels(0, 0, width, height, format, GL_FLOAT, buffer);
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glDeleteFramebuffers(1, &fbId);
}

void OpenGLESGraphicsEngine::DrawPolygon(PolygonData& data, Vec4 color) {
    drwVec2Array& points = data.GetPoints();
    if (points.size() > 0)
    {

        this->color4d(color[0], color[1], color[2], color[3]);
        double* pbuffer = points.GetBuffer();

        this->VertexPointer(pbuffer);

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

void OpenGLESGraphicsEngine::DrawLine(double* pointsBuffer, unsigned int* pointsIndexBuffer, size_t pointsIndexSize, double lineWidth, Vec4 color) {

    this->color4d(color[0], color[1], color[2], color[3]);
    glLineWidth(lineWidth);

    this->VertexPointer(pointsBuffer);
    glDrawElements(GL_LINE_STRIP, pointsIndexSize, GL_UNSIGNED_INT, pointsIndexBuffer);
}

void OpenGLESGraphicsEngine::FillLine(double* poinstBuffer, unsigned int* indicesBuffer, size_t indicesSize, Vec4 color) {
    //glColor4d(color[0], color[1], color[2], color[3]);
    //glDisable(GL_BLEND);
    //glEnable(GL_COLOR_LOGIC_OP);
    //glLogicOp(GL_INVERT);
    //glVertexPointer(2, GL_DOUBLE, 0, poinstBuffer);
    //glDisableClientState(GL_TEXTURE_COORD_ARRAY);
    //glDrawElements(GL_TRIANGLE_FAN, indicesSize, GL_UNSIGNED_INT, indicesBuffer);

    //glDisable(GL_COLOR_LOGIC_OP);
}

void OpenGLESGraphicsEngine::DrawWideLine(double* verticlesBuffer, unsigned int* indicesBuffer, size_t indicesSize, double* normalsBuffer, double* uvsBuffer, Vec4 color) {
    //glColor4d(color[0], color[1], color[2], color[3]);
    //glEnable(GL_BLEND);
    //glBlendEquation(GL_MAX);

    //glVertexPointer(2, GL_DOUBLE, 0, verticlesBuffer);
    //glEnableClientState(GL_TEXTURE_COORD_ARRAY);
    //glTexCoordPointer(3, GL_DOUBLE, 0, uvsBuffer);
    //glEnableClientState(GL_NORMAL_ARRAY);
    //glNormalPointer(GL_DOUBLE, 0, normalsBuffer);
    //glDrawElements(GL_QUADS, indicesSize, GL_UNSIGNED_INT, indicesBuffer);

    //glDisableClientState(GL_TEXTURE_COORD_ARRAY);
    //glDisableClientState(GL_NORMAL_ARRAY);
}

void OpenGLESGraphicsEngine::SetupPasteTextureToScreen(Vec4 color) {
    glBlendEquation(GL_FUNC_ADD);
    glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ONE);

    this->color4d(color[0], color[1], color[2], color[3]);
}

void OpenGLESGraphicsEngine::SetupEraseTextureToScreen(Vec4 color) {
    glBlendEquationSeparate(GL_FUNC_ADD, GL_FUNC_REVERSE_SUBTRACT);
    glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ONE);

    this->color4d(color[0], color[1], color[2], color[3]);
}

void OpenGLESGraphicsEngine::ResetTextureState() {
    glBlendEquation(GL_FUNC_ADD);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void OpenGLESGraphicsEngine::enableScissorTest() {
    glEnable(GL_SCISSOR_TEST);
}

void OpenGLESGraphicsEngine::disableScissorTest() {
    glDisable(GL_SCISSOR_TEST);
}

void OpenGLESGraphicsEngine::enable(unsigned int cap) {
    glEnable(cap);
}

void OpenGLESGraphicsEngine::scissor(int x, int y, int width, int height) {
    glScissor(x, y, width, height);
}

void OpenGLESGraphicsEngine::initializeState() {
    //glDisable(GL_DEPTH_TEST);
    //glEnableClientState(GL_VERTEX_ARRAY);
    //glEnable(GL_TEXTURE_2D);
    //glEnable(GL_BLEND);
    //glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void OpenGLESGraphicsEngine::initializeModelViewMatrix() {
    //glMatrixMode(GL_MODELVIEW);
    //glLoadIdentity();

    /*GLuint modelViewMatrixLocation = glGetUniformLocation(this->programId, "u_modelViewMatrix");
    glm::mat4 modelViewMatrix = glm::mat4(1.0f);
    glUniformMatrix4fv(modelViewMatrixLocation, 1, GL_FALSE, glm::value_ptr(modelViewMatrix));*/
}

void OpenGLESGraphicsEngine::clear() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void OpenGLESGraphicsEngine::clear(unsigned int mask) {
    glClear(mask);
}

void OpenGLESGraphicsEngine::clearColor(float red, float green, float blue, float alpha) {
    glClearColor(red, green, blue, alpha);
}

void OpenGLESGraphicsEngine::blendFunc(unsigned int sfactor, unsigned int dfactor) {
    glBlendFunc(sfactor, dfactor);
}

void OpenGLESGraphicsEngine::color4d(float red, float green, float blue, float alpha) {

    GLint colorLoc = glGetUniformLocation(this->programId, "u_color");
    glUniform4f(colorLoc, red, green, blue, alpha);
}

unsigned int OpenGLESGraphicsEngine::createProgram() {
    return glCreateProgram();
}

void OpenGLESGraphicsEngine::linkProgram(unsigned int glslProgram) {
    glLinkProgram(glslProgram);
    this->programId = glslProgram;
}

void OpenGLESGraphicsEngine::attachShader(unsigned int glslProgram, unsigned int glslVertexShader) {
    glAttachShader(glslProgram, glslVertexShader);
}

void OpenGLESGraphicsEngine::getProgramIv(unsigned int program, unsigned int pname, int* params) {
    glGetProgramiv(program, pname, params);
}

void OpenGLESGraphicsEngine::getProgramInfoLog(unsigned int program, int maxLength, int* length, char* infoLog) {
    glGetProgramInfoLog(program, maxLength, length, infoLog);
}

void OpenGLESGraphicsEngine::pushName(unsigned int name) {
    //glPushName(name);
}

void OpenGLESGraphicsEngine::pushMatrix() {
    //glPushMatrix();
}

void OpenGLESGraphicsEngine::popMatrix() {
    //glPopMatrix();
}

void OpenGLESGraphicsEngine::popName() {
    //glPopName();
}

void OpenGLESGraphicsEngine::BindFrameBuffer(unsigned int fbId) {
    glBindFramebuffer(GL_FRAMEBUFFER, fbId);
}

bool OpenGLESGraphicsEngine::SetVariable(unsigned int programId, const char* name, int value) {
    int location = glGetUniformLocation(programId, name);
    if (location != -1)
    {
        glUniform1i(location, value);
        return true;
    }
    return false;
}

bool OpenGLESGraphicsEngine::SetVariable(unsigned int programId, const char* name, float value) {
    int location = glGetUniformLocation(programId, name);
    if (location != -1)
    {
        glUniform1f(location, value);
        return true;
    }
    return false;
}

bool OpenGLESGraphicsEngine::SetVariable(unsigned int programId, const char* name, Vec4 value) {
    int location = glGetUniformLocation(programId, name);
    if (location != -1)
    {
        glUniform4f(location, value[0], value[1], value[2], value[3]);
        return true;
    }
    return false;
}

void OpenGLESGraphicsEngine::GetVariable(unsigned int name, int* value) {
    glGetIntegerv(name, value);
}

void OpenGLESGraphicsEngine::DeleteShader(unsigned int shaderId) {
    glDeleteShader(shaderId);
}

void OpenGLESGraphicsEngine::DeleteProgram(unsigned int programId) {
    glDeleteProgram(programId);
}

void OpenGLESGraphicsEngine::VertexPointer(double* verticlesBuffer) {
    //GLint positionLoc = glGetAttribLocation(this->programId, "a_position");
    //glVertexAttribPointer(positionLoc, 2, GL_FLOAT, GL_FALSE, 0, verticlesBuffer);
    //glEnableVertexAttribArray(positionLoc);
}

bool OpenGLESGraphicsEngine::CreateAndCompileVertexShader(unsigned& shaderId, std::vector< std::string >& files, std::vector< std::string >& memSources) {
    return CreateAndCompileShader(GL_VERTEX_SHADER, shaderId, files, memSources);
}
bool OpenGLESGraphicsEngine::CreateAndCompileFragmentShader(unsigned& shaderId, std::vector< std::string >& files, std::vector< std::string >& memSources) {
    return CreateAndCompileShader(GL_FRAGMENT_SHADER, shaderId, files, memSources);
}

bool OpenGLESGraphicsEngine::CreateAndCompileShader(unsigned shaderType, unsigned& shaderId, std::vector< std::string >& files, std::vector< std::string >& memSources) {
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

void OpenGLESGraphicsEngine::BlendMaxEquation() {
   glBlendEquation(GL_MAX);
}

void OpenGLESGraphicsEngine::UseProgram(unsigned int programId) {
    glUseProgram(programId);
}

void OpenGLESGraphicsEngine::DrawToTexture(unsigned int& fbId, int& backupFbId) {
    this->GetVariable(GL_FRAMEBUFFER_BINDING, &backupFbId);
    this->BindFrameBuffer(backupFbId);
}

void OpenGLESGraphicsEngine::Flush() {
    glFlush();
}

bool OpenGLESGraphicsEngine::LoadOneShaderSource(const char* filename, std::string& shaderSource)
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