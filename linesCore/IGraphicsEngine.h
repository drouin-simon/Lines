

class IDrawable {

};

class Line : IDrawable {

};

class Polygon : IDrawable {

};

class IGraphicsEngine
{
public:

    // General
    virtual void clear(int x, int y, int width, int height) = 0;
    virtual void setupFrame() = 0;

    // Line
    virtual int draw(Line line) = 0;
    virtual void erase(int id) = 0;

    // Texture
    virtual void drawTexture() = 0;
};

class OpenGLGraphicsEngine : IGraphicsEngine {
    void Download(unsigned char* buffer) {
        //glBindTexture(GL_TEXTURE_RECTANGLE_ARB, m_texId);
        //glGetTexImage(GL_TEXTURE_RECTANGLE_ARB, 0, m_downloadPixelType, GL_UNSIGNED_BYTE, buffer);
        //glBindTexture(GL_TEXTURE_RECTANGLE_ARB, 0);
    }
};

class VulkanGraphicsEngine : IGraphicsEngine {

};

// Draw(IGraphicsEngine engine, Line line)