#ifndef modelutilities_global_h__
#define modelutilities_global_h__
#include <QtGlobal>
#ifndef MODELUTILITIES_STATIC
# if defined(MODELUTILITIES_LIB)
#  define MODELUTILITIES_EXPORT Q_DECL_EXPORT
# else
#  define MODELUTILITIES_EXPORT Q_DECL_IMPORT
# endif
#else
# define MODELUTILITIES_EXPORT
#endif
#endif // modelutilities_global_h__
