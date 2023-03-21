#pragma once

// This header includes "public" GL wrapper's headers
// to simplify GL's usage in simple projects

// Since all this headers are pretty small, it
// doesn't hurt compilation speed in any significant way

// Also this can be used for header precompilation (PCH)

// Simple vector library with few simple helpers:
#include "vec.h"

// Necessary GLFW boilerplate boiled down to minimum
#include "opengl-setup.h"

// opengl-wrapper.h and opengl-error-handler.h are
// skipped since raw opengl calls are not meant to
// be used with this library

// gl::window's extension for switchable renderer's 
#include "renderer.h"
#include "renderer-handler-window.h"

// uniforms.h is also excluded, everything it provides
// is abstracted away within gl::shaders::shader_program

// Object oriented representation of OpenGL concepts
#include "vertex-array.h"
#include "vertex-buffer.h"

// Simple way to design vertex array data layouts
#include "vertex-layout.h"

// Container that integrates std::vector with gl::vertex-array
#include "vertex-vector-array.h"

// Simple drawer (window + renderer)
#include "simple-window.h"
#include "simple-drawing-renderer.h"

#include "drawing-manager.h"
