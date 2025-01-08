//
// Created on 2024/6/6.
//
// Node APIs are not fully supported. To solve the compilation error of the interface cannot be found,
// please include "napi/native_api.h".


#pragma once
#include "common/utils/Array.h"
#include "GLUtil.h"
#include "GLCoord.h"
#include <map>
#include <mutex>
#include <string>

NAMESPACE_DEFAULT

enum DataType {
    BOOL = GL_BOOL,
    INT = GL_INT,
    FLOAT = GL_FLOAT,
    FLOAT_POINTER,
    FVEC2 = GL_FLOAT_VEC2,
    FVEC3 = GL_FLOAT_VEC3,
    FVEC4 = GL_FLOAT_VEC4,
    IVEC2 = GL_INT_VEC2,
    IVEC3 = GL_INT_VEC3,
    IVEC4 = GL_INT_VEC4,
    BVEC2 = GL_BOOL_VEC2,
    BVEC3 = GL_BOOL_VEC3,
    BVEC4 = GL_BOOL_VEC4,
    FMAT2 = GL_FLOAT_MAT2,
    FMAT3 = GL_FLOAT_MAT3,
    FMAT4 = GL_FLOAT_MAT4,
    SAMPLER_2D = GL_SAMPLER_2D,
    SAMPLER_CUBE = GL_SAMPLER_CUBE,
};

class VBO {
public:
    explicit VBO(GLenum usage = GL_STREAM_DRAW) : m_usage(usage) {}
    ~VBO() {
        if (m_size != -1) {
            _WARN("VBO(%d) not released before delete!", m_vbo);
            this->release();
        }
    }

    void bind(const void *points, int byteSize) {
        if (m_size != byteSize) {
            if (m_size != -1) {
                glDeleteBuffers(1, &m_vbo);
            }
            glGenBuffers(1, &m_vbo);

            glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
            glBufferData(GL_ARRAY_BUFFER, byteSize, points, m_usage);
            m_size = byteSize;
        } else {
            glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
            glBufferSubData(GL_ARRAY_BUFFER, 0, byteSize, points);
        }
    }

    void bind() const {
        glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    }

    static void unbind() {
        glBindVertexArray(0);
    }

    void release() {
        if (m_size != -1) {
            glDeleteBuffers(1, &m_vbo);
            _INFO("delete vbo: %d", m_vbo);
            m_size = -1;
        }
    }

private:
    GLenum m_usage;
    unsigned int m_vbo = 0;
    int m_size = -1;
};

class VAO {
public:
    ~VAO() {
        if (m_vao != -1) {
            _WARN("VAO(%d) not released before delete!", m_vao);
            this->release();
        }
    }

    void bind() const {
        if (m_vao == -1) {
            glGenVertexArrays(1, (GLuint *)&m_vao);
        }
        glBindVertexArray(m_vao);
    }

    static void unbind() {
        glBindVertexArray(0);
    }

    void release() {
        if (m_vao != -1) {
            glDeleteVertexArrays(1, (GLuint *)&m_vao);
            _INFO("delete vao: %d", m_vao);
            m_vao = -1;
        }
    }

private:
    GLint m_vao = -1;
};

class Program;

class ProgField {
public:
    ProgField(const char *name, DataType type) : m_name(name), m_type(type) {}

    inline const char *name() const { return m_name.c_str(); }

    inline DataType type() const { return m_type; }

    int dataElements() const {
        switch (m_type) {
        case BOOL :
        case INT :
        case FLOAT :
            return 1;
        case FVEC2 :
        case IVEC2 :
        case BVEC2 :
            return 2;
        case FVEC3 :
        case IVEC3 :
        case BVEC3 :
            return 3;
        case FVEC4 :
        case IVEC4 :
        case BVEC4 :
            return 4;
        case FMAT2 :
            return 4;
        case FMAT3 :
            return 9;
        case FMAT4 :
            return 16;
        case SAMPLER_2D :
        case SAMPLER_CUBE :
            return 1;
        default :
            return 0;
        }
    }

    int dataByteSize() const {
        int unit = sizeof(float);
        return dataElements() * unit;
    }

public:
    template<typename T>
    void put(T *v) {
        std::lock_guard<std::mutex> lock(m_update_mutex);
        m_data.put(v, dataElements());
    }

    template<typename T>
    void set(T v) {
        std::lock_guard<std::mutex> lock(m_update_mutex);
        m_data.put(&v, 1);
    }

    template<typename T>
    void set(T v, T v1) {
        T a[] = {v, v1};
        std::lock_guard<std::mutex> lock(m_update_mutex);
        m_data.put(a, sizeof(a));
    }

    template<typename T>
    void set(T v, T v1, T v2) {
        T a[] = {v, v1, v2};
        std::lock_guard<std::mutex> lock(m_update_mutex);
        m_data.put(a, sizeof(a));
    }

    template<typename T>
    void set(T v, T v1, T v2, T v3) {
        T a[] = {v, v1, v2, v3};
        std::lock_guard<std::mutex> lock(m_update_mutex);
        m_data.put(a, sizeof(a));
    }

protected:
    int ivalue(int i) { return m_data.at<int>(i); }

    float fvalue(int i) { return m_data.at<float>(i); }

    void setLocation(int loc) { m_location = loc; }

protected:
    Array m_data;
    int m_location = -1;

    const std::string m_name;
    const DataType m_type;

    std::mutex m_update_mutex;
};

class Uniform : public ProgField {
public:
    Uniform(const char *name, DataType type, int unitIndex = -1) : ProgField(name, type), m_tex_unit_index(unitIndex) {}

    void input(GLint progId) {
        std::lock_guard<std::mutex> lock(m_update_mutex);
        if (m_location < 0) {
            m_location = glGetUniformLocation(progId, m_name.c_str());
            _ERROR_IF(m_location < 0, "Uniform(%s) location not found! ", m_name.c_str());
        }
//        _INFO("Uniform(%s) location(%d), data type: %d", m_name.c_str(), m_location, m_type);
        GLint loc = m_location;
        switch (m_type) {
        case INT :
            glUniform1i(loc, ivalue(0));
            break;
        case IVEC2 :
            glUniform2i(loc, ivalue(0), ivalue(1));
            break;
        case IVEC3 :
            glUniform3i(loc, ivalue(0), ivalue(1), ivalue(2));
            break;
        case IVEC4 :
            glUniform4i(loc, ivalue(0), ivalue(1), ivalue(2), ivalue(3));
            break;
        case FLOAT :
            glUniform1f(loc, fvalue(0));
            break;
        case FVEC2 :
            glUniform2f(loc, fvalue(0), fvalue(1));
            break;
        case FVEC3 :
            glUniform3f(loc, fvalue(0), fvalue(1), fvalue(2));
            break;
        case FVEC4 :
            glUniform4f(loc, fvalue(0), fvalue(1), fvalue(2), fvalue(3));
            break;
        case FMAT2 :
            glUniformMatrix2fv(loc, 1, GL_FALSE, m_data.data<float>());
            break;
        case FMAT3 :
            glUniformMatrix3fv(loc, 1, GL_FALSE, m_data.data<float>());
            break;
        case FMAT4 :
            glUniformMatrix4fv(loc, 1, GL_FALSE, m_data.data<float>());
            break;
        case SAMPLER_2D : {
            _FATAL_IF(m_tex_unit_index < 0, "Error texture unit index: %d", m_tex_unit_index);
            GLint texId = ivalue(0);
            GLint unit = GL_TEXTURE0 + m_tex_unit_index;
            glActiveTexture(unit);
            glBindTexture(GL_TEXTURE_2D, texId);
            glUniform1i(loc, m_tex_unit_index);
            //_INFO("input texture(%d) unit index(%d)", texId, m_tex_unit_index);
        } break;

        default :
            _FATAL("invalid uniform(%s) data type: %d", m_name.c_str(), m_type);
            return;
        }
    }

private:
    const int m_tex_unit_index;
};

class Attribute : public ProgField {
public:
    Attribute(const char *name, DataType type) : ProgField(name, type) {}

    void put(const float *values, int size, int vecSize = 2, bool normalized = false) {
        std::lock_guard<std::mutex> lock(m_update_mutex);
        this->m_vec_size = vecSize;
        this->m_normalized = normalized;
        m_data.put(values, size);
    }

    void put(GLCoord &coords, int vecSize = 2, bool normalized = false) {
        std::lock_guard<std::mutex> lock(m_update_mutex);
        int size = 0;
        const float *values = coords.get(size);
        this->m_vec_size = vecSize;
        this->m_normalized = normalized;
        m_data.put(values, size);
    }

    void bind(GLCoord &coords, int vecSize = 2, bool normalized = false) {
        std::lock_guard<std::mutex> lock(m_update_mutex);
        this->m_vec_size = vecSize;
        this->m_normalized = normalized;
        m_bind_coord = &coords;
    }

    void input(GLint progId, const VAO& vao) {
        std::lock_guard<std::mutex> lock(m_update_mutex);
        if (m_location < 0) {
            m_location = glGetAttribLocation(progId, m_name.c_str());
            _ERROR_IF(m_location < 0, "Attribute(%s) location not found! ", m_name.c_str());
        }
//        _INFO("Attribute(%s) location(%d), data type: %d", m_name.c_str(), m_location, m_type);
        GLint loc = m_location;
        switch (m_type) {
        case FLOAT :
            glVertexAttrib1f(loc, fvalue(0));
            break;
        case FVEC2 :
            glVertexAttrib2f(loc, fvalue(0), fvalue(1));
            break;
        case FVEC3 :
            glVertexAttrib3f(loc, fvalue(0), fvalue(1), fvalue(2));
            break;
        case FVEC4 :
            glVertexAttrib4f(loc, fvalue(0), fvalue(1), fvalue(2), fvalue(3));
            break;
        case FLOAT_POINTER : {
            int dataSize = 0;
            const float *d;
            if (m_bind_coord) {
                d = m_bind_coord->get(dataSize);
            } else {
                d = m_data.data<float>();
                dataSize = m_data.size<float>();
            }

            int unitSize = sizeof(float);
            m_vbo.bind((const void *)d, dataSize*unitSize);
            vao.bind();
            glVertexAttribPointer(loc, m_vec_size,
                                  GL_FLOAT, m_normalized,  0, nullptr);
            glEnableVertexAttribArray(loc);
            VAO::unbind();
            VBO::unbind();
//            CHECK_GL_ERROR
//            _INFO("input(%d), [%f, %f, %f, %f, %f, %f, %f, %f]", dataSize, d[0], d[1], d[2], d[3], d[4], d[5], d[6], d[7]);
            break;
        }
        default :
            _FATAL("Unsupported field type(%d) for Attribute(%s)", m_type, m_name.c_str());
        }
    }

    void release() {
        m_vbo.release();
    }

private:
    // 这里的size不是数组的长度，而是 glVertexAttriPointer 的第二个参数，是 vecX 的维度
    int m_vec_size = 2;
    bool m_normalized = false;
    GLCoord *m_bind_coord = nullptr;

    VBO m_vbo;
};

class Program {
public:
    Program() = default;
    Program(const char *vs, const char *fs) : m_vertex_shader(vs), m_fragment_shader(fs) {}

    ~Program() {
        if (m_id != INVALID_GL_ID) {
            _WARN("GL program(%d) not released before delete!", m_id);
            this->release();
        }
    }

    bool create() { return create(m_vertex_shader.c_str(), m_fragment_shader.c_str()); }

    bool create(const char *vs, const char *fs) {
        if (m_id == INVALID_GL_ID) {
            m_id = GLUtil::loadProgram(vs, fs);
            _ERROR_RETURN_IF(m_id == INVALID_GL_ID, false, "create gl program failed:\n%s\n---\n%s\n", vs, fs);

            m_vertex_shader = vs;
            m_fragment_shader = fs;
        } else if (m_vertex_shader != vs || m_fragment_shader != fs) {
            _WARN("recreate gl program!!");
            release();

            m_id = GLUtil::loadProgram(vs, fs);
            _ERROR_RETURN_IF(m_id == INVALID_GL_ID, false, "create gl program failed:\n%s\n---\n%s\n", vs, fs);

            m_vertex_shader = vs;
            m_fragment_shader = fs;
        }
        _INFO("gl program created successfully, id: %d", m_id);
        return true;
    }

    inline GLuint id() const { return m_id; }

    inline bool valid() const { return m_id != INVALID_GL_ID; }

    bool isAttached() const { return m_attached; }

    bool attach() {
        if (!m_attached) {
            if (m_id == INVALID_GL_ID) {
                if (!create()) {
                    _ERROR("create gl program failed while attach()");
                    return false;
                }
            }
            glUseProgram(m_id);
            m_attached = true;
        } else {
            _WARN("gl program(%d) already attached", m_id);
        }
        return m_attached;
    }

    void input() {
        _FATAL_IF(!m_attached, "gl program(%d) not attached while input", m_id);

        for (auto &kv : m_attr_map) {
            kv.second->input(m_id, m_vao);
        }
        for (auto &kv : m_uniform_map) {
            kv.second->input(m_id);
        }
        m_vao.bind();
    }

    void detach() {
        if (m_attached) {
            VAO::unbind();
            glUseProgram(0);
            m_attached = false;
        } else {
            _WARN("gl program(%d) already detached", m_id);
        }
    }

    Attribute *defAttribute(const char *name, DataType type) {
        auto it = m_attr_map.find(name);
        if (it != m_attr_map.end()) {
            return it->second;
        }
        Attribute *attr = new Attribute(name, type);
        m_attr_map[name] = attr;
        return attr;
    }

    Attribute *attribute(const char *name) {
        auto it = m_attr_map.find(name);
        if (it != m_attr_map.end()) {
            return it->second;
        }
        return nullptr;
    }

    Uniform *defUniform(const char *name, DataType type) {
        auto it = m_uniform_map.find(name);
        if (it != m_uniform_map.end()) {
            return it->second;
        }

        Uniform *uni = type == SAMPLER_2D ? new Uniform(name, type, nextTexUnitIndex()) : new Uniform(name, type);
        m_uniform_map[name] = uni;
        return uni;
    }

    Uniform *uniform(const char *name) {
        auto it = m_uniform_map.find(name);
        if (it != m_uniform_map.end()) {
            return it->second;
        }
        return nullptr;
    }

    void release() {
        if (m_id != INVALID_GL_ID) {
            glDeleteProgram(m_id);
            _INFO("release gl program(%d)", m_id);
            m_id = INVALID_GL_ID;
        }

        // 释放 m_attr_map 中的指针内存
        for (auto &pair : m_attr_map) {
            pair.second->release();
            delete pair.second;
        }
        m_attr_map.clear();

        // 释放 m_uniform_map 中的指针内存
        for (auto &pair : m_uniform_map) {
            delete pair.second;
        }
        m_uniform_map.clear();

        m_vao.release();
    }

private:
    int nextTexUnitIndex() {
        int i = m_uniform_texture_count;
        m_uniform_texture_count += 1;
        return i;
    }

private:
    GLuint m_id = INVALID_GL_ID;

    std::string m_vertex_shader;
    std::string m_fragment_shader;

    bool m_attached = false;
    int m_uniform_texture_count = 0;

    VAO m_vao;
    std::map<std::string, Attribute *> m_attr_map;
    std::map<std::string, Uniform *> m_uniform_map;
};

NAMESPACE_END
