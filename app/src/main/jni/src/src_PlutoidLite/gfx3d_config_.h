/*
#ifndef __GFX3D_CONFIG_H__
#define __GFX3D_CONFIG_H__

#define TARGET_WIN32    1
#define TARGET_GP2XWIZ  2

#if !defined(OPENGL_ES_VERSION)
#	define  OPENGL_ES_VERSION 110
#endif

#if (TARGET_DEVICE == TARGET_BADA)
#include <FGraphicsOpengl.h>
using namespace Osp::Graphics::Opengl;
#else

#if   (OPENGL_ES_VERSION == 110)
#	include <GLES/egl.h>
#	include <GLES/gl.h>
	typedef NativeWindowType  EGLNativeWindowType;
	typedef NativeDisplayType EGLNativeDisplayType;
#elif (OPENGL_ES_VERSION == 200)
#	include <EGL/egl.h>
#	include <GLES2/gl2.h>
#endif

#endif

#if !defined(TARGET_DEVICE)
#	if defined(_WIN32)
#		define TARGET_DEVICE  TARGET_WIN32
#	elif defined(ARM9)
#		define TARGET_DEVICE  TARGET_GP2XWIZ
#	else
#		error  "add -DTARGET_DEVICE=TARGET_??? in your Makefile."
#	endif
#endif

#if   (TARGET_DEVICE == TARGET_WIN32)
#	define SCREEN_WIDTH   320
#	define SCREEN_HEIGHT  240
#elif (TARGET_DEVICE == TARGET_GP2XWIZ)
#	define SCREEN_WIDTH   320
#	define SCREEN_HEIGHT  240
#elif (TARGET_DEVICE == TARGET_BADA)
#	define SCREEN_WIDTH   320
#	define SCREEN_HEIGHT  240
#else
#	error "..."
#endif

#define SCREEN_DEPTH  16

#endif
*/