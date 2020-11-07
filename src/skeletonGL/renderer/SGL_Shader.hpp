// Author:  AlexHG @ ZEN3X.COM
// License: MIT License
// Website: https://ZEN3X.COM

/**
 * @file    src/skeletonGL/utility/SGL_Shader.hpp
 * @author  AlexHG @ ZEN3X.COM
 * @date    05/11/2020
 * @version 1.92
 *
 * @brief Wraps around all necessary shader functionality
 *
 * @section DESCRIPTION
 *
 * Encapsulates all the required shader functionality
 */

#ifndef SRC_SKELETONGL_RENDERER_SHADERMANAGER_H
#define SRC_SKELETONGL_RENDERER_SHADERMANAGER_H

// C++
#include <string>
// Dependencies
#include <GL/glew.h>
#include "../deps/glm/glm.hpp"
#include "../deps/glm/gtc/type_ptr.hpp"
// SkeletonGL
#include "SGL_OpenGLManager.hpp"
#include "../utility/SGL_DataStructures.hpp"
#include "../utility/SGL_Utility.hpp"


/**
 * @brief What the shader will be used to render
 */
enum class SHADER_TYPE : std::int8_t { NO_SHADER = 0, SPRITE = 1, TEXT = 2, PIXEL = 3, LINE = 4, QUAD = 5, POST_PROCESSOR = 6, END_ENUM = 7};


/**
 * @brief Extra rendering values to be processed by the shader
 */
struct RenderDetails
{
    float deltaTime;                 ///< Time between frames
    float canvasWidth, canvasHeight; ///< Actual drawing proportions
    float mousePosX, mousePosY;      ///< Mouse positioning

    RenderDetails() : deltaTime(0.0f), canvasHeight(0.0f), canvasWidth(0.0f), mousePosX(0), mousePosY(0) {}
};

/**
 * @brief Defines an SGL_Shader
 */
class SGL_Shader
{
private:
    void checkForErrors(SGL_OpenGLManager &oglm, GLuint object, std::string type);
public:
    GLuint ID;                   ///< The shaders internal (OpenGL defined) ID
    SHADER_TYPE shaderType;      ///< What the shader will be used to render
    RenderDetails renderDetails; ///< Extra rendering details
    std::string name;            ///< Shader's identifying (unique) name

    // Constructor
    SGL_Shader();
    // Destructor
    ~SGL_Shader();

    // Activates the shader
    SGL_Shader &use(SGL_OpenGLManager &oglm) noexcept;
    // Deactivates the shader
    void unbind(SGL_OpenGLManager &oglm) const noexcept;
    // Compiles, links, and enables the shader
    void compileShaders(SGL_OpenGLManager &oglm, SHADER_TYPE shaderType, const GLchar *vertexSource, const GLchar *fragmentSource, const GLchar *geometrySource = nullptr) noexcept;

    // Set a single float value
    void setFloat(SGL_OpenGLManager &oglm, const GLchar *name, GLfloat value, GLboolean useShader = false) noexcept;
    // Set a single integer value
    void setInteger(SGL_OpenGLManager &oglm, const GLchar *name, GLint value, GLboolean useShader = false) noexcept;
    // Set a 2D vector of floats
    void setVector2f(SGL_OpenGLManager &oglm, const GLchar *name, GLfloat x, GLfloat y, GLboolean useShader = false) noexcept;
    // Set a 2D vector of floats
    void setVector2f(SGL_OpenGLManager &oglm, const GLchar *name, glm::vec2 value, GLboolean useShader = false) noexcept;
    // Set a 3D vector of floats
    void setVector3f(SGL_OpenGLManager &oglm, const GLchar *name, GLfloat x, GLfloat y, GLfloat z, GLboolean useShader = false) noexcept;
    // Set a 3D vector of floats
    void setVector3f(SGL_OpenGLManager &oglm, const GLchar *name, glm::vec3 value, GLboolean useShader = false) noexcept;
    // Set a 4D vector of floats
    void setVector4f(SGL_OpenGLManager &oglm, const GLchar *name, GLfloat x, GLfloat y, GLfloat z, GLfloat w, GLboolean useShader = false) noexcept;
    // Set a 4D vector of floats
    void setVector4f(SGL_OpenGLManager &oglm, const GLchar *name, glm::vec4 value, GLboolean useShader = false) noexcept;
    // Set a 4x4 matrix of floats
    void setMatrix4(SGL_OpenGLManager &oglm, const GLchar *name, glm::mat4 matrix, GLboolean useShader = false) noexcept;
};


#endif // SRC_SKELETONGL_RENDERER_SHADERMANAGER_H
