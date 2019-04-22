$ git diff gl_renderstate.cpp
diff --git a/renderdoc/driver/gl/gl_renderstate.cpp b/renderdoc/driver/gl/gl_renderstate.cpp
index 7733e8fe8..671612141 100644
--- a/renderdoc/driver/gl/gl_renderstate.cpp
    +++ b/renderdoc/driver/gl/gl_renderstate.cpp
    @@ -926,17 +926,18 @@ void GLRenderState::FetchState(WrappedOpenGL *driver)
if(prog == 0)
    continue;

    -      GLint numSubroutines = 0;
    -      GL.glGetProgramStageiv(prog, shs[s], eGL_ACTIVE_SUBROUTINES, &numSubroutines);
    +      // delete by zhouhm
    +      //GLint numSubroutines = 0;
    +      //GL.glGetProgramStageiv(prog, shs[s], eGL_ACTIVE_SUBROUTINES, &numSubroutines);

-      if(numSubroutines == 0)
    -        continue;
    +      //if(numSubroutines == 0)
    +      //  continue;

    -      GL.glGetProgramStageiv(prog, shs[s], eGL_ACTIVE_SUBROUTINE_UNIFORM_LOCATIONS,
            -                             &Subroutines[s].numSubroutines);
+      //GL.glGetProgramStageiv(prog, shs[s], eGL_ACTIVE_SUBROUTINE_UNIFORM_LOCATIONS,
+      //                       &Subroutines[s].numSubroutines);

-      for(GLint i = 0; i < Subroutines[s].numSubroutines; i++)
-        GL.glGetUniformSubroutineuiv(shs[s], i, &Subroutines[s].Values[0]);
+      //for(GLint i = 0; i < Subroutines[s].numSubroutines; i++)
+      //  GL.glGetUniformSubroutineuiv(shs[s], i, &Subroutines[s].Values[0]);
}
}
    else
@@ -1162,7 +1163,7 @@ void GLRenderState::FetchState(WrappedOpenGL *driver)

if(HasExt[EXT_depth_bounds_test])
{
    -    GL.glGetDoublev(eGL_DEPTH_BOUNDS_TEST_EXT, &DepthBounds.nearZ);
    +    //GL.glGetDoublev(eGL_DEPTH_BOUNDS_TEST_EXT, &DepthBounds.nearZ);
}
else
{

