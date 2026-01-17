/*  tccdefs.h

    Nothing is defined before this file except target machine, target os
    and the few things related to option settings in tccpp.c:tcc_predefs().

    This file is either included at runtime as is, or converted and
    included as C-strings at compile-time (depending on CONFIG_TCC_PREDEFS).

    Note that line indent matters:

    - in lines starting at column 1, platform macros are replaced by
      corresponding TCC target compile-time macros.  See conftest.c for
      the list of platform macros supported in lines starting at column 1.

    - only lines indented >= 4 are actually included into the executable,
      check tccdefs_.h.
*/

#if __SIZEOF_POINTER__ == 4
    /* 32bit systems. */
    #define __SIZE_TYPE__ unsigned int
    #define __PTRDIFF_TYPE__ int
    #define __ILP32__ 1
    #define __INT64_TYPE__ long long
#elif __SIZEOF_LONG__ == 4
    /* 64bit Windows. */
    #define __SIZE_TYPE__ unsigned long long
    #define __PTRDIFF_TYPE__ long long
    #define __LLP64__ 1
    #define __INT64_TYPE__ long long
#else
    /* Other 64bit systems. */
    #define __SIZE_TYPE__ unsigned long
    #define __PTRDIFF_TYPE__ long
    #define __LP64__ 1
    #define __INT64_TYPE__ long
#endif
    #define __SIZEOF_INT__ 4
    #define __INT_MAX__ 0x7fffffff
#if __SIZEOF_LONG__ == 4
    #define __LONG_MAX__ 0x7fffffffL
#else
    #define __LONG_MAX__ 0x7fffffffffffffffL
#endif
    #define __SIZEOF_LONG_LONG__ 8
    #define __LONG_LONG_MAX__ 0x7fffffffffffffffLL
    #define __CHAR_BIT__ 8
    #define __ORDER_LITTLE_ENDIAN__ 1234
    #define __ORDER_BIG_ENDIAN__ 4321
    #define __BYTE_ORDER__ __ORDER_LITTLE_ENDIAN__
#if defined __linux__
    #define __WCHAR_TYPE__ int
    #define __WINT_TYPE__ unsigned int
#else
    #define __WCHAR_TYPE__ int
    #define __WINT_TYPE__ int
#endif

    #if __STDC_VERSION__ >= 201112L
    # define __STDC_NO_ATOMICS__ 1
    # define __STDC_NO_COMPLEX__ 1
    # define __STDC_NO_THREADS__ 1
    # define __STDC_UTF_16__ 1
    # define __STDC_UTF_32__ 1
    #endif

    /* Some derived integer types needed to get stdint.h to compile correctly on some platforms */
    #define __UINTPTR_TYPE__ unsigned __PTRDIFF_TYPE__
    #define __INTPTR_TYPE__ __PTRDIFF_TYPE__
    #define __INT32_TYPE__ int

#if defined __aarch64__
    /* GCC's __uint128_t appears in some Linux/OSX header files. Make it a
       synonym for long double to get the size and alignment right. */
    #define __uint128_t long double
#endif

    /* glibc defines. We do not support __USER_NAME_PREFIX__ */
    #define __REDIRECT(name, proto, alias) name proto __asm__ (#alias)
    #define __REDIRECT_NTH(name, proto, alias) name proto __asm__ (#alias) __THROW
    #define __REDIRECT_NTHNL(name, proto, alias) name proto __asm__ (#alias) __THROWNL

    /* not implemented */
    #define  __PRETTY_FUNCTION__ __FUNCTION__
    #define __has_builtin(x) 0
    #define __has_feature(x) 0
    #define __has_attribute(x) 0
    /* C23 Keywords */
    #define _Nonnull
    #define _Nullable
    #define _Nullable_result
    #define _Null_unspecified

    /* skip __builtin... with -E */
    #ifndef __TCC_PP__

    #define __builtin_offsetof(type, field) ((__SIZE_TYPE__)&((type*)0)->field)
    #define __builtin_extract_return_addr(x) x
#if !defined __linux__
    /* used by math.h */
    #define __builtin_huge_val() 1e500
    #define __builtin_huge_valf() 1e50f
    #define __builtin_huge_vall() 1e5000L
    #define __builtin_nanf(ignored_string) (0.0F/0.0F)
#endif

    /* __builtin_va_list */
#if defined __x86_64__
    /* GCC compatible definition of va_list. */

    enum __va_arg_type {
        __va_gen_reg, __va_float_reg, __va_stack
    };
    typedef struct {
        unsigned gp_offset, fp_offset;
        union {
            unsigned overflow_offset;
            char *overflow_arg_area;
        };
        char *reg_save_area;
    } __builtin_va_list[1];

    static inline void *__va_arg(__builtin_va_list ap, int arg_type,
                                 int size, int align)
    {
        size = (size + 7) & ~7;
        align = (align + 7) & ~7;
        switch ((enum __va_arg_type)arg_type) {
        case __va_gen_reg:
            if (ap->gp_offset + size <= 48) {
                ap->gp_offset += size;
                return ap->reg_save_area + ap->gp_offset - size;
            }
            goto use_overflow_area;
        case __va_float_reg:
            if (ap->fp_offset < 128 + 48) {
                ap->fp_offset += 16;
                if (size == 8)
                    return ap->reg_save_area + ap->fp_offset - 16;
                if (ap->fp_offset < 128 + 48) {
                    double *p = (double *)(ap->reg_save_area + ap->fp_offset);
                    p[-1] = p[0];
                    ap->fp_offset += 16;
                    return ap->reg_save_area + ap->fp_offset - 32;
                }
            }
            goto use_overflow_area;
        case __va_stack:
        use_overflow_area:
            ap->overflow_arg_area += size;
            ap->overflow_arg_area =
	      (char*)((long long)(ap->overflow_arg_area + align - 1) & -align);
            return ap->overflow_arg_area - size;
        default: /* should never happen */
            char *a = (char *)0; *a = 0; // abort
            return 0;
        }
    }

    #define __builtin_va_start(ap, last) \
       (*(ap) = *(__builtin_va_list)((char*)__builtin_frame_address(0) - 24))
    #define __builtin_va_arg(ap, t)   \
       (*(t *)(__va_arg(ap, __builtin_va_arg_types(t), sizeof(t), __alignof__(t))))
    #define __builtin_va_copy(dest, src) (*(dest) = *(src))

#elif defined __aarch64__
    typedef struct {
        void *__stack, *__gr_top, *__vr_top;
        int   __gr_offs, __vr_offs;
    } __builtin_va_list;
#endif
    #define __builtin_va_end(ap) (void)(ap)
    #ifndef __builtin_va_copy
    # define __builtin_va_copy(dest, src) (dest) = (src)
    #endif

    /* TCC BBUILTIN AND BOUNDS ALIASES */
    #ifdef __leading_underscore
    # define __RENAME(X) __asm__("_"X)
    #else
    # define __RENAME(X) __asm__(X)
    #endif

    #ifdef __TCC_BCHECK__
    # define __BUILTINBC(ret,name,params) ret __builtin_##name params __RENAME("__bound_"#name);
    # define __BOUND(ret,name,params) ret name params __RENAME("__bound_"#name);
    #else
    # define __BUILTINBC(ret,name,params) ret __builtin_##name params __RENAME(#name);
    # define __BOUND(ret,name,params)
    #endif

    #define __BOTH(ret,name,params) __BUILTINBC(ret,name,params)__BOUND(ret,name,params)
    #define __BUILTIN(ret,name,params) ret __builtin_##name params __RENAME(#name);

    __BOTH(void*, memcpy, (void *, const void*, __SIZE_TYPE__))
    __BOTH(void*, memmove, (void *, const void*, __SIZE_TYPE__))
    __BOTH(void*, memset, (void *, int, __SIZE_TYPE__))
    __BOTH(int, memcmp, (const void *, const void*, __SIZE_TYPE__))
    __BOTH(__SIZE_TYPE__, strlen, (const char *))
    __BOTH(char*, strcpy, (char *, const char *))
    __BOTH(char*, strncpy, (char *, const char*, __SIZE_TYPE__))
    __BOTH(int, strcmp, (const char*, const char*))
    __BOTH(int, strncmp, (const char*, const char*, __SIZE_TYPE__))
    __BOTH(char*, strcat, (char*, const char*))
    __BOTH(char*, strncat, (char*, const char*, __SIZE_TYPE__))
    __BOTH(char*, strchr, (const char*, int))
    __BOTH(char*, strrchr, (const char*, int))
    __BOTH(char*, strdup, (const char*))
#if defined __ARM_EABI__
    __BOUND(void*,__aeabi_memcpy,(void*,const void*,__SIZE_TYPE__))
    __BOUND(void*,__aeabi_memmove,(void*,const void*,__SIZE_TYPE__))
    __BOUND(void*,__aeabi_memmove4,(void*,const void*,__SIZE_TYPE__))
    __BOUND(void*,__aeabi_memmove8,(void*,const void*,__SIZE_TYPE__))
    __BOUND(void*,__aeabi_memset,(void*,int,__SIZE_TYPE__))
#endif

#if defined __linux__ || defined __APPLE__ // HAVE MALLOC_REDIR
    #define __MAYBE_REDIR __BUILTIN
#else
    #define __MAYBE_REDIR __BOTH
#endif
    __MAYBE_REDIR(void*, malloc, (__SIZE_TYPE__))
    __MAYBE_REDIR(void*, realloc, (void *, __SIZE_TYPE__))
    __MAYBE_REDIR(void*, calloc, (__SIZE_TYPE__, __SIZE_TYPE__))
    __MAYBE_REDIR(void*, memalign, (__SIZE_TYPE__, __SIZE_TYPE__))
    __MAYBE_REDIR(void, free, (void*))
    __BOTH(void*, alloca, (__SIZE_TYPE__))
    void *alloca(__SIZE_TYPE__);
    __BUILTIN(void, abort, (void))
    __BOUND(void, longjmp, ())
    __BOUND(void*, mmap, ())
    __BOUND(int, munmap, ())
    #undef __BUILTINBC
    #undef __BUILTIN
    #undef __BOUND
    #undef __BOTH
    #undef __MAYBE_REDIR
    #undef __RENAME

    #define __BUILTIN_EXTERN(name,u) 		\
        int __builtin_##name(u int);		\
        int __builtin_##name##l(u long);	\
        int __builtin_##name##ll(u long long);
    __BUILTIN_EXTERN(ffs,)
    __BUILTIN_EXTERN(clz, unsigned)
    __BUILTIN_EXTERN(ctz, unsigned)
    __BUILTIN_EXTERN(clrsb,)
    __BUILTIN_EXTERN(popcount, unsigned)
    __BUILTIN_EXTERN(parity, unsigned)
    #undef __BUILTIN_EXTERN

    #endif /* ndef __TCC_PP__ */
