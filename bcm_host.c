#define _GNU_SOURCE
#include <stdio.h>
#include <stdint.h>
#include <EGL/egl.h>
#include <GLES/gl.h>
#include <GLES2/gl2.h>
#include <SDL.h>

typedef void* DISPMANX_ELEMENT_HANDLE_T;
typedef void* DISPMANX_DISPLAY_HANDLE_T;
typedef void* DISPMANX_UPDATE_HANDLE_T;
typedef void* DISPMANX_RESOURCE_HANDLE_T;
typedef void* DISPMANX_PROTECTION_T;
typedef void* VC_DISPMANX_ALPHA_T;
typedef void* DISPMANX_CLAMP_T;
typedef void* DISPMANX_TRANSFORM_T;
typedef int VC_RECT_T;

typedef struct {
	DISPMANX_ELEMENT_HANDLE_T element;
	int width;   /* This is necessary because dispmanx elements are not queriable. */
	int height;
} EGL_DISPMANX_WINDOW_T;

static SDL_Window *sdl_window = NULL;
static SDL_GLContext *sdl_ctx = NULL;

char const* gl_error_string(GLenum const err)
{
  switch (err)
  {
    // opengl 2 errors (8)
    case GL_NO_ERROR:
      return "GL_NO_ERROR";

    case GL_INVALID_ENUM:
      return "GL_INVALID_ENUM";

    case GL_INVALID_VALUE:
      return "GL_INVALID_VALUE";

    case GL_INVALID_OPERATION:
      return "GL_INVALID_OPERATION";

    case GL_STACK_OVERFLOW:
      return "GL_STACK_OVERFLOW";

    case GL_STACK_UNDERFLOW:
      return "GL_STACK_UNDERFLOW";

    case GL_OUT_OF_MEMORY:
      return "GL_OUT_OF_MEMORY";

    // opengl 3 errors (1)
    case GL_INVALID_FRAMEBUFFER_OPERATION:
      return "GL_INVALID_FRAMEBUFFER_OPERATION";

    // gles 2, 3 and gl 4 error are handled by the switch above
    default:
      fprintf(stderr, "unknown error\n");
      return NULL;
  }
}

int32_t graphics_get_display_size( const uint16_t display_number, uint32_t *width, uint32_t *height) {
	fprintf(stderr, "graphics_get_display_size()\n");

	sdl_window = SDL_CreateWindow("test", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
		480, 320, SDL_WINDOW_OPENGL);

	if (!sdl_window) {
		fprintf(stderr, "Failed to create window. %s\n", SDL_GetError());
		return -1;
	}

	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
	SDL_GL_SetSwapInterval(1);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

	sdl_ctx = SDL_GL_CreateContext(sdl_window);
	if (!sdl_ctx) {
		fprintf(stderr, "Failed to create context. %s\n", SDL_GetError());
		return -1;
	}

	SDL_GL_SetSwapInterval(1);

	SDL_GetWindowSize(sdl_window, width, height);
	return 0;
}

int vc_dispmanx_display_open(int device) {
	fprintf(stderr, "vc_dispmanx_display_open(%d)\n", device);
	return 0;
}

DISPMANX_ELEMENT_HANDLE_T vc_dispmanx_element_add ( DISPMANX_UPDATE_HANDLE_T update,
		 DISPMANX_DISPLAY_HANDLE_T display,
		int32_t layer, const VC_RECT_T *dest_rect, DISPMANX_RESOURCE_HANDLE_T src,
		const VC_RECT_T *src_rect, DISPMANX_PROTECTION_T protection,
		VC_DISPMANX_ALPHA_T *alpha,
		DISPMANX_CLAMP_T *clamp, DISPMANX_TRANSFORM_T transform )  {
	fprintf(stderr, "vc_dispmanx_element_add(%p, %p, %d, %p, %p, %p, %p, %p, %p, %p)\n",
			update, display, layer, dest_rect, src, src_rect, protection,
					      alpha, clamp, transform);
	return NULL;
}

int vc_dispmanx_update_submit_sync( DISPMANX_UPDATE_HANDLE_T update ) {
	fprintf(stderr, "vc_dispmanx_update_submit_sync(%p)\n", update);
	return 0;
}

void bcm_host_deinit(void) {
	fprintf(stderr, "bcm_host_deinit()\n");
	return;
}

int vc_dispmanx_update_start( int32_t priority ) {
	fprintf(stderr, "vc_dispmanx_update_start(%d)\n", priority);
	return 0;
}

extern void init_yyg_fix();
void bcm_host_init(void) {
	SDL_Init(SDL_INIT_EVERYTHING);
	fprintf(stderr, "bcm_host_init()\n");

	init_yyg_fix();
}

void glGenFramebuffersOES(GLsizei n, GLuint* ids) {
	fprintf(stderr, "glGenFramebuffersOES(%d, %p);\n", n, ids);
	glGenFramebuffers(n, ids);
}

void glFramebufferTexture2DOES(GLenum target, GLenum attachment, GLenum textarget, GLuint texture, GLint level) {
	fprintf(stderr, "glFramebufferTexture2DOES(%d, %d, %d, %d, %d);\n", target, attachment, textarget, texture, level);
	glFramebufferTexture2D(target, attachment, textarget, texture, level);
}

void glGenRenderbuffersOES(GLsizei n, GLuint* renderbuffers) {
	glGenRenderbuffers(n, renderbuffers);
}

GLenum glCheckFramebufferStatusOES(GLenum target) {
	return glCheckFramebufferStatus(target);
}

void glBindRenderbufferOES(GLenum target, GLuint renderbuffer) {
	glBindRenderbuffer(target, renderbuffer);
}

void glRenderbufferStorageOES(GLenum target, GLenum internalformat, GLsizei width, GLsizei height) {
	glRenderbufferStorage(target, internalformat, width, height);
}

void glFramebufferRenderbufferOES(GLenum target, GLenum attachment, GLenum renderbuffertarget, GLuint renderbuffer) {
	glFramebufferRenderbuffer(target, attachment, renderbuffertarget, renderbuffer);
}

void glDeleteFramebuffersOES(GLsizei n, const GLuint* framebuffers) {
	glDeleteFramebuffers(n, framebuffers);
}

void glDeleteRenderbuffersOES(GLsizei n, const GLuint* renderbuffers) {
	glDeleteRenderbuffers(n, renderbuffers);
}

EGLAPI EGLBoolean EGLAPIENTRY eglSwapBuffers(EGLDisplay dpy, EGLSurface surface)
{
	if (!sdl_window)
		return EGL_FALSE;

	SDL_GL_SwapWindow(sdl_window);
	return EGL_TRUE;
}