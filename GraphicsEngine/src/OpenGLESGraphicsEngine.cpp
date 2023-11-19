/*
#include "OpenGLESGraphicsEngine.h"

void OpenGLESGraphicsEngine::SetViewPort(int x, int y, int width, int height) {
    glViewport(x, y, width, height);
    
}

void OpenGLESGraphicsEngine::SetProjectionViewPort(int x, int y, int width, int height) {
    this->SetViewPort(x, y, width, height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
}
void OpenGLESGraphicsEngine::SetModelViewOrtho2D(double left, double right, double bottom, double top) {
    gluOrtho2D(left, right, bottom, top);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

unsigned int OpenGLESGraphicsEngine::InitTexture(int level, int internalFormat, int width, int height, int border, int format, int type, void* pixels) {
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

unsigned int OpenGLESGraphicsEngine::InitFrameBuffer(unsigned int texId) {
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

void OpenGLESGraphicsEngine::DisplayTexture(unsigned int texId, int level, int internalFormat, int width, int height, int border, int format, int type, void* pixels) {
    glBindTexture(GL_TEXTURE_RECTANGLE_ARB, texId);
    glTexImage2D(GL_TEXTURE_RECTANGLE_ARB, level, internalFormat, width, height, border, format, type, pixels);
    glBindTexture(GL_TEXTURE_RECTANGLE_ARB, 0);
}

void OpenGLESGraphicsEngine::ClearTexture(unsigned int fbId) {
    int backupFbId = 0;
    glGetIntegerv(GL_FRAMEBUFFER_BINDING, &backupFbId);
    glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, fbId);
    glClearColor(0.0, 0.0, 0.0, 0.0);
    glClear(GL_COLOR_BUFFER_BIT);
    glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, backupFbId);
}

void OpenGLESGraphicsEngine::ReleaseTexture(unsigned int texId, unsigned int fbId) {
    if (fbId)
        glDeleteFramebuffersEXT(1, &fbId);
    if (texId)
        glDeleteTextures(1, &texId);
}

void OpenGLESGraphicsEngine::PasteTextureToScreen(unsigned int texId, int texWidth, int texHeight, int x, int y, int screenWidth, int screenHeight) {
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    gluOrtho2D(0, texWidth, 0, texHeight);

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

void OpenGLESGraphicsEngine::ClearScreen(int texWidth, int texHeight, int x, int y, int screenWidth, int screenHeight) {
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

void OpenGLESGraphicsEngine::Upload(unsigned int texId, int level, int internalFormat, int width, int height, int border, int format, int type, unsigned char* buffer)
{
    glBindTexture(GL_TEXTURE_RECTANGLE_ARB, texId);
    glTexImage2D(GL_TEXTURE_RECTANGLE_ARB, level, internalFormat, width, height, border, format, type, buffer);
    glBindTexture(GL_TEXTURE_RECTANGLE_ARB, 0);
}

void OpenGLESGraphicsEngine::Download(unsigned int texId, int level, int format, int type, unsigned char* buffer)
{
    glBindTexture(GL_TEXTURE_RECTANGLE_ARB, texId);
    glGetTexImage(GL_TEXTURE_RECTANGLE_ARB, level, format, type, buffer);
    glBindTexture(GL_TEXTURE_RECTANGLE_ARB, 0);
}

void OpenGLESGraphicsEngine::Download(unsigned int texId, int level, int format, int type, unsigned short* buffer)
{
    glBindTexture(GL_TEXTURE_RECTANGLE_ARB, texId);
    glGetTexImage(GL_TEXTURE_RECTANGLE_ARB, level, format, type, buffer);
    glBindTexture(GL_TEXTURE_RECTANGLE_ARB, 0);
}

void OpenGLESGraphicsEngine::Download(unsigned int texId, int level, int format, int type, float* buffer)
{
    glBindTexture(GL_TEXTURE_RECTANGLE_ARB, texId);
    glGetTexImage(GL_TEXTURE_RECTANGLE_ARB, 0, GL_RED, GL_FLOAT, buffer);
    glGetTexImage(GL_TEXTURE_RECTANGLE_ARB, level, format, type, buffer);
    glBindTexture(GL_TEXTURE_RECTANGLE_ARB, 0);
}

void OpenGLESGraphicsEngine::DrawPolygon(PolygonData& data, Vec4 color) {
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

void OpenGLESGraphicsEngine::DrawLine(double* pointsBuffer, unsigned int* pointsIndexBuffer, size_t pointsIndexSize, double lineWidth, Vec4 color) {
    glColor4d(color[0], color[1], color[2], color[3]);
    glLineWidth(lineWidth);
    glVertexPointer(2, GL_DOUBLE, 0, pointsBuffer);
    glDrawElements(GL_LINE_STRIP, pointsIndexSize, GL_UNSIGNED_INT, pointsIndexBuffer);
}

void OpenGLESGraphicsEngine::FillLine(double* poinstBuffer, unsigned int* indicesBuffer, size_t indicesSize, Vec4 color) {
    glColor4d(color[0], color[1], color[2], color[3]);
    glDisable(GL_BLEND);
    glEnable(GL_COLOR_LOGIC_OP);
    glLogicOp(GL_INVERT);
    glVertexPointer(2, GL_DOUBLE, 0, poinstBuffer);
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);
    glDrawElements(GL_TRIANGLE_FAN, indicesSize, GL_UNSIGNED_INT, indicesBuffer);

    glDisable(GL_COLOR_LOGIC_OP);
}

void OpenGLESGraphicsEngine::DrawWideLine(double* verticlesBuffer, unsigned int* indicesBuffer, size_t indicesSize, double* normalsBuffer, double* uvsBuffer, Vec4 color) {
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

void OpenGLESGraphicsEngine::SetupPasteTextureToScreen(Vec4 color) {
    glBlendEquation(GL_FUNC_ADD);
    glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ONE);
    glColor4d(color[0], color[1], color[2], color[3]);
}

void OpenGLESGraphicsEngine::SetupEraseTextureToScreen(Vec4 color) {
    glBlendEquationSeparate(GL_FUNC_ADD, GL_FUNC_REVERSE_SUBTRACT);
    glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ONE);
    glColor4d(color[0], color[1], color[2], color[3]);
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
    glDisable(GL_DEPTH_TEST);
    glEnableClientState(GL_VERTEX_ARRAY);
    glEnable(GL_TEXTURE_RECTANGLE_ARB);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void OpenGLESGraphicsEngine::initializeModelViewMatrix() {
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

void OpenGLESGraphicsEngine::clear() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void OpenGLESGraphicsEngine::clear(GLbitfield mask) {
    glClear(mask);
}

void OpenGLESGraphicsEngine::clearColor(float red, float green, float blue, float alpha) {
    glClearColor(red, green, blue, alpha);
}

void OpenGLESGraphicsEngine::blendFunc(unsigned int sfactor, unsigned int dfactor) {
    glBlendFunc(sfactor, dfactor);
}

void OpenGLESGraphicsEngine::color4d(float red, float green, float blue, float alpha) {
    glColor4d(red, green, blue, alpha);
}

GLuint OpenGLESGraphicsEngine::createProgram() {
    return glCreateProgram();
}

void OpenGLESGraphicsEngine::linkProgram(unsigned int glslProgram) {
    glLinkProgram(glslProgram);
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

//void OpenGLGraphicsEngine::Download(unsigned char* buffer) {
//    //glBindTexture(GL_TEXTURE_RECTANGLE_ARB, m_texId);
//    //glGetTexImage(GL_TEXTURE_RECTANGLE_ARB, 0, m_downloadPixelType, GL_UNSIGNED_BYTE, buffer);
//    //glBindTexture(GL_TEXTURE_RECTANGLE_ARB, 0);
//}

void OpenGLESGraphicsEngine::pushName(unsigned int name) {
    glPushName(name);
}

void OpenGLESGraphicsEngine::pushMatrix() {
    glPushMatrix();
}

void OpenGLESGraphicsEngine::popMatrix() {
    glPopMatrix();
}

void OpenGLESGraphicsEngine::popName() {
    glPopName();
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

void OpenGLESGraphicsEngine::GetVariable(unsigned int name, int* value) {
    glGetIntegerv(name, value);
}

void OpenGLESGraphicsEngine::DeleteShader(unsigned int shaderId) {
    glDeleteShader(shaderId);
}

void OpenGLESGraphicsEngine::DeleteProgram(unsigned int programId) {
    glDeleteProgram(programId);
}

*/