// gl_emulated.cpp
void APIENTRY _glProgramParameteri(GLuint program, GLenum pname, GLint value)
{
  // we only set this when reflecting, so just silently drop it
  if(pname == eGL_PROGRAM_SEPARABLE)
    return;
  
    if(pname == GL_PROGRAM_BINARY_RETRIEVABLE_HINT)
    return;  

  RDCERR("Cannot emulate glProgramParameteri(%s), capture cannot be opened", ToStr(pname).c_str());
}

// gl_shader_funcs.cpp
void WrappedOpenGL::glProgramParameteri(GLuint program, GLenum pname, GLint value)
{
//  if(pname == GL_PROGRAM_BINARY_RETRIEVABLE_HINT) {
//    return;
//  }
}
