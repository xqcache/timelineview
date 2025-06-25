#pragma once

#include <QtCore/qglobal.h>

#ifndef TIMELINE_BUILD_STATIC
#if defined(TIMELINE_COMPILE_LIB)
#define TIMELINE_LIB_EXPORT Q_DECL_EXPORT
#else
#define TIMELINE_LIB_EXPORT Q_DECL_IMPORT
#endif
#else
#define TIMELINE_LIB_EXPORT
#endif
