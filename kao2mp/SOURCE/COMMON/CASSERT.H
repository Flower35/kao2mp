/**
 * "COMMON/CASSERT.H"
 */

/**************************************************************/
/* C compiler static assertions                               */
/**************************************************************/

#define GLUE(a,b)  a##__##b

#define STATIC_ASSERT_VERIFY(_expr,_line) \
    typedef char GLUE(compiler_verify,_line) \
    [(_expr) ? (+1) : (-1)];

#define STATIC_ASSERT(_expr) \
    STATIC_ASSERT_VERIFY(_expr, __LINE__)

/**************************************************************/
