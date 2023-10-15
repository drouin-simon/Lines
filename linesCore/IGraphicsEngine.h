#include "IncludeGLEW.h"

class IDrawable {

};

class Polygon : IDrawable {

};

class IGraphicsEngine
{
public:

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
    public:
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

// Draw(IGraphicsEngine engine, Line line)
