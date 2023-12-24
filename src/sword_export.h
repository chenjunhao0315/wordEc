
#ifndef SWORD_EXPORT_H
#define SWORD_EXPORT_H

#ifdef SWORD_STATIC_DEFINE
#  define SWORD_EXPORT
#  define SWORD_NO_EXPORT
#else
#  ifndef SWORD_EXPORT
#    ifdef sword_EXPORTS
        /* We are building this library */
#      define SWORD_EXPORT 
#    else
        /* We are using this library */
#      define SWORD_EXPORT 
#    endif
#  endif

#  ifndef SWORD_NO_EXPORT
#    define SWORD_NO_EXPORT 
#  endif
#endif

#ifndef SWORD_DEPRECATED
#  define SWORD_DEPRECATED __attribute__ ((__deprecated__))
#endif

#ifndef SWORD_DEPRECATED_EXPORT
#  define SWORD_DEPRECATED_EXPORT SWORD_EXPORT SWORD_DEPRECATED
#endif

#ifndef SWORD_DEPRECATED_NO_EXPORT
#  define SWORD_DEPRECATED_NO_EXPORT SWORD_NO_EXPORT SWORD_DEPRECATED
#endif

#if 0 /* DEFINE_NO_DEPRECATED */
#  ifndef SWORD_NO_DEPRECATED
#    define SWORD_NO_DEPRECATED
#  endif
#endif

#endif /* SWORD_EXPORT_H */
