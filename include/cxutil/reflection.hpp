#ifndef CXUTIL_REFLECTION_HPP
#define CXUTIL_REFLECTION_HPP

#ifdef CXL_REFLECTED_CODE_GENERATOR
#define CXL_REFLECTABLE __attribute__((annotate("CXL_REFLECTABLE")))
#define CXL_HIDDEN __attribute__((annotate("CXL_HIDDEN")))
#define CXL_KEY(x) __attribute__((annotate("CXL_SERIALIZATION_KEY=" x)))
#else
#define CXL_REFLECTABLE
#define CXL_HIDDEN
#define CXL_KEY(x)
#endif

#endif	// !defined(CXUTIL_REFLECTION_HPP)
