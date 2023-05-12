#ifndef PLUGIN01_GLOBAL_H
#define PLUGIN01_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(PLUGIN01_LIBRARY)
#  define PLUGIN01_EXPORT Q_DECL_EXPORT
#else
#  define PLUGIN01_EXPORT Q_DECL_IMPORT
#endif

#endif // PLUGIN01_GLOBAL_H
