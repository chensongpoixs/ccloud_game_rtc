/***********************************************************************************************
created: 		2022-01-20

author:			chensong

purpose:		assertion macros
************************************************************************************************/
#ifndef _C_ASSERTION_MACROS_H_
#define _C_ASSERTION_MACROS_H_

namespace chen {

	// If we don't have a platform-specific define for the TEXT macro, define it now.
#if !defined(TEXT) && !UE_BUILD_DOCS
#if PLATFORM_TCHAR_IS_CHAR16
#define TEXT_PASTE(x) u ## x
#else
#define TEXT_PASTE(x) L ## x
#endif
#define TEXT(x) TEXT_PASTE(x)
#endif
#define CA_ASSUME( Expr )
#ifndef UNLIKELY					/* Hints compiler that expression is unlikely to be true, allows (penalized by worse performance) expression to be true */
#if ( defined(__clang__) || defined(__GNUC__) ) && (PLATFORM_UNIX)	// effect of these on non-Linux platform has not been analyzed as of 2016-03-21
#define UNLIKELY(x)			__builtin_expect(!!(x), 0)
#else
#define UNLIKELY(x)			(x)
#endif
#endif
#if DO_CHECK
#ifndef checkCode
#define checkCode( Code )		do { Code; } while ( false );
#endif
#ifndef verify
#define verify(expr)			UE_CHECK_IMPL(expr)
#endif
#ifndef check
#define check(expr)				UE_CHECK_IMPL(expr)
#endif

	// Technically we could use just the _F version (lambda-based) for asserts
	// both with and without formatted messages. However MSVC emits extra
	// unnecessary instructions when using a lambda; hence the Exec() impl.
#define UE_CHECK_IMPL(expr) \
		{ \
			if(UNLIKELY(!(expr))) \
			{ \
				struct Impl \
				{ \
					static void FORCENOINLINE UE_DEBUG_SECTION ExecCheckImplInternal() \
					{ \
						FDebug::CheckVerifyFailed(#expr, __FILE__, __LINE__, TEXT("")); \
					} \
				}; \
				Impl::ExecCheckImplInternal(); \
				PLATFORM_BREAK(); \
				CA_ASSUME(false); \
			} \
		}

	/**
	* verifyf, checkf: Same as verify, check but with printf style additional parameters
	* Read about __VA_ARGS__ (variadic macros) on http://gcc.gnu.org/onlinedocs/gcc-3.4.4/cpp.pdf.
	*/
#ifndef verifyf
#define verifyf(expr, format,  ...)		UE_CHECK_F_IMPL(expr, format, ##__VA_ARGS__)
#endif
#ifndef checkf
#define checkf(expr, format,  ...)		UE_CHECK_F_IMPL(expr, format, ##__VA_ARGS__)
#endif

#define UE_CHECK_F_IMPL(expr, format, ...) \
		{ \
			if(UNLIKELY(!(expr))) \
			{ \
				DispatchCheckVerify([&] () FORCENOINLINE UE_DEBUG_SECTION \
				{ \
					FDebug::CheckVerifyFailed(#expr, __FILE__, __LINE__, format, ##__VA_ARGS__); \
				}); \
				PLATFORM_BREAK(); \
				CA_ASSUME(false); \
			} \
		}

	/**
	* Denotes code paths that should never be reached.
	*/
#ifndef checkNoEntry
#define checkNoEntry()       check(!"Enclosing block should never be called")
#endif

	/**
	* Denotes code paths that should not be executed more than once.
	*/
#ifndef checkNoReentry
#define checkNoReentry()     { static bool s_beenHere##__LINE__ = false;                                         \
	                               check( !"Enclosing block was called more than once" || !s_beenHere##__LINE__ );   \
								   s_beenHere##__LINE__ = true; }
#endif

	class FRecursionScopeMarker
	{
	public: 
		FRecursionScopeMarker(uint16 &InCounter) : Counter( InCounter ) { ++Counter; }
		~FRecursionScopeMarker() { --Counter; }
	private:
		uint16& Counter;
	};

	/**
	* Denotes code paths that should never be called recursively.
	*/
#ifndef checkNoRecursion
#define checkNoRecursion()  static uint16 RecursionCounter##__LINE__ = 0;                                            \
	                            check( !"Enclosing block was entered recursively" || RecursionCounter##__LINE__ == 0 );  \
	                            const FRecursionScopeMarker ScopeMarker##__LINE__( RecursionCounter##__LINE__ )
#endif

#ifndef unimplemented
#define unimplemented()		check(!"Unimplemented function called")
#endif

#else
#define checkCode(...)
#define check(expr)					{ CA_ASSUME(expr); }
#define checkf(expr, format, ...)	{ CA_ASSUME(expr); }
#define checkNoEntry()
#define checkNoReentry()
#define checkNoRecursion()
#define verify(expr)				{ if(UNLIKELY(!(expr))){ CA_ASSUME(false); } }
#define verifyf(expr, format, ...)	{ if(UNLIKELY(!(expr))){ CA_ASSUME(false); } }
#define unimplemented()				{ CA_ASSUME(false); }
#endif
}

#endif // _C_ASSERTION_MACROS_H_