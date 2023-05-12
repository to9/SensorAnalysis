#ifndef PLUGIN02_GLOBAL_H
#define PLUGIN02_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(PLUGIN02_LIBRARY)
#  define PLUGIN02_EXPORT Q_DECL_EXPORT
#else
#  define PLUGIN02_EXPORT Q_DECL_IMPORT
#endif

#endif // PLUGIN02_GLOBAL_H
