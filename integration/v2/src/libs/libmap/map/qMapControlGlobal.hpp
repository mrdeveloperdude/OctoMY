#include <QtGlobal>

#ifdef QMAPCONTROL_LIBRARY
//#  define QMAPCONTROL_EXPORT Q_DECL_EXPORT
#else
//#  define QMAPCONTROL_EXPORT Q_DECL_IMPORT
#endif

// TODO: Normalize this on windows
#  define QMAPCONTROL_EXPORT
