/// \file
#ifndef MethodPointer__H
#define MethodPointer__H

#include "Delegate.h"

/*

Example:

//////////////////////////
// returning int Example
//////////////////////////

DefineMethodPointer(MethodSumPtr, int, int a, int b) ReturnMethodCall(a,b)

int FunctionExample(int a, int b) {
    return a + b;
}

MethodSumPtr methodPtr = MethodSumPtr(&FunctionExample);

int c = 0;
if (methodPtr != NULL)
    c = methodPtr(1, 2);

//////////////////////////
// returning void Example
//////////////////////////

DefineMethodPointer(MethodSumVoidPtr, void, int a, int b) VoidMethodCall(a,b)

void FunctionExample(int a, int b) {
    ...
}

MethodSumVoidPtr methodPtr = MethodSumVoidPtr(&FunctionExample);

if (methodPtr != NULL)
    methodPtr(1, 2);

*/

namespace aRibeiro {

/// \brief Declare a Generic Method Pointer
///
/// It is needed to use two macros to declare a method pointer: #DefineMethodPointer and (#ReturnMethodCall or #VoidMethodCall).
///
/// This is a single method pointer that could be used to implement callback functions or callback methods.
///
/// Example returning int:
///
/// \code
/// #include <aribeiro/aribeiro.h>
/// using namespace aRibeiro;
///
/// DefineMethodPointer(MethodSumPtr, int, int a, int b) ReturnMethodCall(a, b)
///
/// int FunctionExample(int a, int b) {
///     return a + b;
/// }
///
/// class ExampleClass {
/// public:
///     int MethodExample(int a, int b) {
///         return a + b;
///     }
/// };
/// 
/// ExampleClass obj;
///
/// // using functor example
/// MethodSumPtr methodPtr = &FunctionExample;
/// 
/// int c = 0;
/// if (methodPtr != NULL)
///     c = methodPtr(1, 2);
///
/// // using method reference
/// methodPtr = MethodSumPtr( &obj, &ExampleClass::MethodExample );
///
/// if (methodPtr != NULL)
///     c = methodPtr(1, 2);
/// \endcode
///
///
/// Example returning void:
///
/// \code
/// #include <aribeiro/aribeiro.h>
/// using namespace aRibeiro;
///
/// DefineMethodPointer(MethodSumPtr, void, int a, int b) VoidMethodCall(a, b)
///
/// void FunctionExample(int a, int b) {
///     ...
/// }
///
/// class ExampleClass {
/// public:
///     void MethodExample(int a, int b) {
///         ...
///     }
/// };
/// 
/// ExampleClass obj;
///
/// // using functor example
/// MethodSumPtr methodPtr = &FunctionExample;
/// 
/// if (methodPtr != NULL)
///     methodPtr(1, 2);
///
/// // using method reference
/// methodPtr = MethodSumPtr( &obj, &ExampleClass::MethodExample );
///
/// if (methodPtr != NULL)
///     methodPtr(1, 2);
/// \endcode
///
/// \author Alessandro Ribeiro
///
#define DefineMethodPointer(className,returnType,...) \
    typedef returnType (aRibeiro::DelegateFriendObject::*className##Method) (__VA_ARGS__); \
    typedef returnType (*className##Function) (__VA_ARGS__); \
    class className { \
        aRibeiro::DelegateFriendObject* obj; \
        className##Method methodPtr; \
        className##Function functionPtr; \
    public: \
        className ( ){ \
            this->obj = NULL; \
            this->methodPtr = NULL; \
            this->functionPtr = NULL; \
        } \
        template <typename T> \
        className ( T* obj, returnType(T::*methodPtr) (__VA_ARGS__) ){ \
            this->obj = static_cast<aRibeiro::DelegateFriendObject*>((void*)obj); \
            this->methodPtr = (className##Method) methodPtr; \
            this->functionPtr = NULL; \
        } \
        className ( returnType (*functionPtr) (__VA_ARGS__) ){ \
            this->obj = NULL; \
            this->methodPtr = NULL; \
            this->functionPtr = functionPtr; \
        } \
        bool operator==(void* ptr) const { \
            if (ptr == NULL) \
                return this->obj == NULL && this->methodPtr == NULL && this->functionPtr == NULL; \
            return false; \
        } \
        bool operator!=(void* ptr) const { \
            if (ptr == NULL) \
                return this->obj != NULL || this->methodPtr != NULL || this->functionPtr != NULL; \
            return false; \
        } \
        returnType operator() ( __VA_ARGS__ ) const {

/// \brief Define a method pointer with some return type
///
/// It is needed to use two macros to declare a method pointer: #DefineMethodPointer and (#ReturnMethodCall or #VoidMethodCall).
///
/// This is a single method pointer that could be used to implement callback functions or callback methods.
///
/// Example returning int:
///
/// \code
/// #include <aribeiro/aribeiro.h>
/// using namespace aRibeiro;
///
/// DefineMethodPointer(MethodSumPtr, int, int a, int b) ReturnMethodCall(a, b)
///
/// int FunctionExample(int a, int b) {
///     return a + b;
/// }
///
/// class ExampleClass {
/// public:
///     int MethodExample(int a, int b) {
///         return a + b;
///     }
/// };
/// 
/// ExampleClass obj;
///
/// // using functor example
/// MethodSumPtr methodPtr = &FunctionExample;
/// 
/// int c = 0;
/// if (methodPtr != NULL)
///     c = methodPtr(1, 2);
///
/// // using method reference
/// methodPtr = MethodSumPtr( &obj, &ExampleClass::MethodExample );
///
/// if (methodPtr != NULL)
///     c = methodPtr(1, 2);
/// \endcode
///
/// \author Alessandro Ribeiro
///
#define ReturnMethodCall( ... ) \
            if (obj != NULL) \
                return (obj->*(methodPtr))(__VA_ARGS__); \
            else \
                return (functionPtr)(__VA_ARGS__); \
        } \
    };


/// \brief Define a method pointer without a return type
///
/// It is needed to use two macros to declare a method pointer: #DefineMethodPointer and (#ReturnMethodCall or #VoidMethodCall).
///
/// This is a single method pointer that could be used to implement callback functions or callback methods.
///
/// Example returning void:
///
/// \code
/// #include <aribeiro/aribeiro.h>
/// using namespace aRibeiro;
///
/// DefineMethodPointer(MethodSumPtr, void, int a, int b) VoidMethodCall(a, b)
///
/// void FunctionExample(int a, int b) {
///     ...
/// }
///
/// class ExampleClass {
/// public:
///     void MethodExample(int a, int b) {
///         ...
///     }
/// };
/// 
/// ExampleClass obj;
///
/// // using functor example
/// MethodSumPtr methodPtr = &FunctionExample;
/// 
/// if (methodPtr != NULL)
///     methodPtr(1, 2);
///
/// // using method reference
/// methodPtr = MethodSumPtr( &obj, &ExampleClass::MethodExample );
///
/// if (methodPtr != NULL)
///     methodPtr(1, 2);
/// \endcode
///
/// \author Alessandro Ribeiro
///
#define VoidMethodCall( ... ) \
            if (obj != NULL) \
                (obj->*(methodPtr))(__VA_ARGS__); \
            else \
                (functionPtr)(__VA_ARGS__); \
        } \
    };
}

#endif
