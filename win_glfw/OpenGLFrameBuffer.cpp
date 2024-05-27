#include <iostream>
#include "OpenGLFrameBuffer.h"


void OpenGLFrameBuffer::createBuffers(int32_t p_width, int32_t p_height)
{
    _width = p_width;
    _height = p_height;

    glGenFramebuffers(1, &_fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, _fbo);
    // create a color attachment texture
    glGenTextures(1, &_texId);
    glBindTexture(GL_TEXTURE_2D, _texId);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, _width, _height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, _texId, 0);
    // create a renderbuffer object for depth and stencil attachment (we won't be sampling these)
    unsigned int rbo;
    glGenRenderbuffers(1, &rbo);
    glBindRenderbuffer(GL_RENDERBUFFER, rbo);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, _width, _height); // use a single renderbuffer object for both a depth AND stencil buffer.
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo); // now actually attach it
    // now that we actually created the framebuffer and added all attachments we want to check if it is actually complete now
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void OpenGLFrameBuffer::bind()
{
    glBindFramebuffer(GL_FRAMEBUFFER, _fbo);
}

void OpenGLFrameBuffer::unBind()
{
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

uint32_t OpenGLFrameBuffer::getTexture()
{
    return _texId;
}
