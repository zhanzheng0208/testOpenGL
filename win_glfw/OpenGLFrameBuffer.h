#include <stdint.h>
#include "3rdParty/deps/glad/gl.h"

class OpenGLFrameBuffer
{
public:
    void createBuffers(int32_t p_width, int32_t p_height);
    void bind();
    void unBind();

    uint32_t getTexture();

private:
    uint32_t _fbo = 0;
    uint32_t _texId = 0;
    int32_t _width = 0;
    int32_t _height = 0;
};