//
// Created on 2024/6/5.
//
// Node APIs are not fully supported. To solve the compilation error of the interface cannot be found,
// please include "napi/native_api.h".

#pragma once

#include <common/Object.h>

#ifdef __ANDROID__
#include <GLES3/gl3.h>
#include <EGL/egl.h>
#include <EGL/eglext.h>
#elif defined(__HARMONY_OS__)
#include <GLES3/gl3.h>
#include <EGL/egl.h>
#include <EGL/eglext.h>
#else
#include <OpenGL/gl3.h>
#endif

NAMESPACE_DEFAULT

#define INVALID_GL_ID ((GLuint)(-1))

#define CHECK_GL_ERROR { GLenum en = glGetError(); if (en != GL_NO_ERROR) { _ERROR("find gl error: %d", en); }}

class GLUtil {
public:
    static GLuint loadShader(const char *str, int type) {
        GLuint shader = glCreateShader(type);
        _ERROR_RETURN_IF(shader == INVALID_GL_ID, INVALID_GL_ID, "GLUtil::loadShader create shader failed!");

        glShaderSource(shader, 1, &str, nullptr);
        glCompileShader(shader);

        GLint compiled;
        glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);

        if (!compiled) {
            GLint infoLen = 0;
            glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLen);

            if (infoLen > 1) {
                char * infoLog = new char[infoLen+1];
                memset(infoLog, 0, infoLen+1);
                glGetShaderInfoLog(shader, infoLen, nullptr, (GLchar *)infoLog);
                _ERROR("Error compiling shader:%s\n%s", infoLog, str);
                delete [] infoLog;
            }

            glDeleteShader(shader);
            return INVALID_GL_ID;
        }

        return shader;
    }

    static GLuint loadProgram(const char *vstr, const char *fstr) {
        GLuint vertex = loadShader(vstr, GL_VERTEX_SHADER);
        _ERROR_RETURN_IF(vertex == INVALID_GL_ID, INVALID_GL_ID, "loadProgram vertex failed");

        GLuint fragment = loadShader(fstr, GL_FRAGMENT_SHADER);
        if (fragment == INVALID_GL_ID) {
            _ERROR("loadProgram fragment failed");
            glDeleteShader(vertex);
            return INVALID_GL_ID;
        }

        GLuint program = glCreateProgram();
        if (program == INVALID_GL_ID) {
            _ERROR("loadProgram: create program error");
            glDeleteShader(vertex);
            glDeleteShader(fragment);
            return INVALID_GL_ID;
        }

        GLint linked;
        glAttachShader(program, vertex);
        glAttachShader(program, fragment);
        glLinkProgram(program);
        glGetProgramiv(program, GL_LINK_STATUS, &linked);

        if (!linked) {
            _ERROR("loadProgram linked error");
            GLint infoLen = 0;
            glGetProgramiv(program, GL_INFO_LOG_LENGTH, &infoLen);
            if (infoLen > 1) {
                char * infoLog = new char[infoLen+1];
                memset(infoLog, 0, infoLen+1);
                glGetProgramInfoLog(program, infoLen, nullptr, (GLchar *)infoLog);
                _ERROR("Error linking program: %s", infoLog);
                delete [] infoLog;
            }
            glDeleteShader(vertex);
            glDeleteShader(fragment);
            glDeleteProgram(program);
            return INVALID_GL_ID;
        }
        glDeleteShader(vertex);
        glDeleteShader(fragment);
        return program;
    }

    static void clearColor(float r, float g, float b, float a) {
        glClearColor(r, g, b, a);
        glClear(GL_COLOR_BUFFER_BIT);
    }
    
    static GLsync createFenceSync() {
        GLsync gLsync = glFenceSync(GL_SYNC_GPU_COMMANDS_COMPLETE, 0);
        _ERROR_RETURN_IF(gLsync == nullptr, nullptr, "GLUtil::createFenceSync failed");
        return gLsync;
    }
    
    static bool waitFenceSync(GLsync syncObj, GLbitfield flags = GL_SYNC_FLUSH_COMMANDS_BIT) {
        GLenum result = glClientWaitSync(syncObj, flags, GL_TIMEOUT_IGNORED);
        glDeleteSync(syncObj);
        if (result == GL_WAIT_FAILED) {
            _ERROR("GLUtil::waitFenceSync failed");
            return false;
        } else if (result == GL_TIMEOUT_EXPIRED) {
            _ERROR("GLUtil::waitFenceSync timeout");
            return false;
        }
        return true;
    }
    
    static void deleteFenceSync(GLsync syncObj) {
        glDeleteSync(syncObj);
    }
};
NAMESPACE_END
