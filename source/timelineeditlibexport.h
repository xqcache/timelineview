#pragma once

#include <QtCore/qglobal.h>

#ifndef TIMELINEEDIT_BUILD_STATIC
#if defined(TIMELINEEDIT_COMPILE_LIB)
#define TIMELINEEDIT_LIB_EXPORT Q_DECL_EXPORT
#else
#define TIMELINEEDIT_LIB_EXPORT Q_DECL_IMPORT
#endif
#else
#define TIMELINEEDIT_LIB_EXPORT
#endif
