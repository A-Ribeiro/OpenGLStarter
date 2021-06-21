/// \file
#ifndef Delegate__H
#define Delegate__H

//#include "Object.h"
//#include <aribeiro/aribeiro.h>
//#include <aribeiro/common.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <vector>

/*
delegate declaration:

BEGIN_DECLARE_DELEGATE(ClassName, <Parameters List Method Sign> ) CALL_PATTERN (<Parameters List Method Call>) END_DECLARE_DELEGATE;

Example:

BEGIN_DECLARE_DELEGATE(ViewportDelegate, Viewport* viewport ) CALL_PATTERN (viewport) END_DECLARE_DELEGATE;

ViewportDelegate OnViewport;

//on code
OnViewport.add(&func);
OnViewport(viewport);


*/

namespace aRibeiro {

    class DelegateFriendObject {};

/// \brief Declare a Delegate Class
///
/// It is needed to use three macros to declare a delegate: #BEGIN_DECLARE_DELEGATE, #CALL_PATTERN, #END_DECLARE_DELEGATE.
///
/// The delegate in this framework work as a list of functors or methods that can be called with a call signature.
///
/// How to use them:
///
/// \code
/// #include <aribeiro/aribeiro.h>
/// using namespace aRibeiro;
///
/// // Define a class to hold the delegation information
/// // The name of the class will be ViewportDelegate, 
/// // it will handle the call signature `void call(Viewport* viewport)`.
/// BEGIN_DECLARE_DELEGATE(ViewportDelegate, Viewport* viewport ) CALL_PATTERN (viewport) END_DECLARE_DELEGATE;
///
/// // after that, you can declare an instance of the delegate class
/// ViewportDelegate OnViewport;
///
/// class ExampleClass {
/// public:
///     void method(Viewport* viewport) {
///         ...
///     }
/// };
///
/// void func(Viewport* viewport) {
///     ...
/// }
///
/// ExampleClass obj_instance;
/// // Adding references
/// OnViewport.add(&func);
/// OnViewport.add(&obj_instance, &ExampleClass::method);
///
/// // call all functors or methods added to this delegate
/// OnViewport(viewport);
///
/// // it is possible to remove a previous added reference afterwards
///
/// // Removing references
/// OnViewport.remove(&func);
/// OnViewport.remove(&obj_instance, &ExampleClass::method);
/// \endcode
///
/// \author Alessandro Ribeiro
///
#define BEGIN_DECLARE_DELEGATE(className, ... ) \
    typedef void (aRibeiro::DelegateFriendObject::*className##Method) (__VA_ARGS__); \
    typedef void (*className##Function) (__VA_ARGS__); \
    class className{ \
        struct DelegateEntry{ \
            aRibeiro::DelegateFriendObject* mObjPtr; \
            className##Method mMethodPtr; \
            className##Function mFunctionPtr; \
            int functionID; \
            DelegateEntry(){ mObjPtr = NULL; mMethodPtr = NULL; mFunctionPtr = NULL; functionID = 0;} \
            DelegateEntry(aRibeiro::DelegateFriendObject* objptr, className##Method methodPtr){ mObjPtr = objptr; mMethodPtr = methodPtr; mFunctionPtr = NULL;functionID = 1;} \
            DelegateEntry(className##Function functionPtr){ mObjPtr = NULL; mMethodPtr = NULL; mFunctionPtr = functionPtr;functionID = 2;} \
            bool operator==(const DelegateEntry& v)const{ \
                if ( (functionID == 1) && mObjPtr != NULL && v.mObjPtr != NULL) \
                    return mObjPtr == v.mObjPtr && mMethodPtr == v.mMethodPtr;  \
                else if ( (functionID == 2) && mFunctionPtr != NULL && v.mFunctionPtr != NULL) \
                    return mFunctionPtr == v.mFunctionPtr; \
                else \
                    return false; \
            } \
        }; \
        std::vector<DelegateEntry> mDelegateEntry; \
        typedef std::vector<DelegateEntry>::iterator iteratorType; \
    public: \
        void add(aRibeiro::DelegateFriendObject* objPtr, className##Method methodPtr){ \
            DelegateEntry entry (objPtr,methodPtr); \
            for (size_t i=0;i<mDelegateEntry.size();i++) \
                if (mDelegateEntry[i] == entry) \
                    return; \
            mDelegateEntry.push_back(entry); \
        } \
        size_t size(){ \
            return mDelegateEntry.size(); \
        } \
        template <typename __T__> \
        void add(void* objPtr, void (__T__::*methodPtr) (__VA_ARGS__) ){ \
            add(static_cast<aRibeiro::DelegateFriendObject*>(objPtr), (className##Method) methodPtr) ; \
        } \
        void add(className##Function functionPtr){ \
            DelegateEntry entry (functionPtr); \
            for (size_t i=0;i<mDelegateEntry.size();i++) \
                if (mDelegateEntry[i] == entry) \
                    return; \
            mDelegateEntry.push_back(entry); \
        } \
        void remove(aRibeiro::DelegateFriendObject* objPtr, className##Method methodPtr){ \
            DelegateEntry entry (objPtr,methodPtr ); \
            iteratorType it = mDelegateEntry.begin(); \
            for (; it != mDelegateEntry.end(); it++) \
                if ((*it) == entry){ \
                    mDelegateEntry.erase(it); \
                    return; \
                } \
        } \
        template <typename __T__> \
        void remove(void* objPtr, void (__T__::*methodPtr) (__VA_ARGS__) ){ \
            remove( static_cast<aRibeiro::DelegateFriendObject*>(objPtr), (className##Method) methodPtr); \
        } \
        void remove(className##Function functionPtr){ \
            DelegateEntry entry (functionPtr ); \
            iteratorType it = mDelegateEntry.begin(); \
            for (; it != mDelegateEntry.end(); it++) \
                if ((*it) == entry){ \
                    mDelegateEntry.erase(it); \
                    return; \
                } \
        } \
        void removeAll(){ \
            mDelegateEntry.clear(); \
        } \
        void operator()(__VA_ARGS__){ 


#define BEGIN_DECLARE_DELEGATE_INSIDE_TEMPLATE(className, ... ) \
    typedef void (aRibeiro::DelegateFriendObject::*className##Method) (__VA_ARGS__); \
    typedef void (*className##Function) (__VA_ARGS__); \
    class className{ \
        struct DelegateEntry{ \
            aRibeiro::DelegateFriendObject* mObjPtr; \
            className##Method mMethodPtr; \
            className##Function mFunctionPtr; \
            int functionID; \
            DelegateEntry(){ mObjPtr = NULL; mMethodPtr = NULL; mFunctionPtr = NULL; functionID = 0;} \
            DelegateEntry(aRibeiro::DelegateFriendObject* objptr, className##Method methodPtr){ mObjPtr = objptr; mMethodPtr = methodPtr; mFunctionPtr = NULL;functionID = 1;} \
            DelegateEntry(className##Function functionPtr){ mObjPtr = NULL; mMethodPtr = NULL; mFunctionPtr = functionPtr;functionID = 2;} \
            bool operator==(const DelegateEntry& v)const{ \
                if ( (functionID == 1) && mObjPtr != NULL && v.mObjPtr != NULL) \
                    return mObjPtr == v.mObjPtr && mMethodPtr == v.mMethodPtr;  \
                else if ( (functionID == 2) && mFunctionPtr != NULL && v.mFunctionPtr != NULL) \
                    return mFunctionPtr == v.mFunctionPtr; \
                else \
                    return false; \
            } \
        }; \
        std::vector<DelegateEntry> mDelegateEntry; \
        typedef typename std::vector<DelegateEntry>::iterator iteratorType; \
    public: \
        void add(aRibeiro::DelegateFriendObject* objPtr, className##Method methodPtr){ \
            DelegateEntry entry (objPtr,methodPtr); \
            for (size_t i=0;i<mDelegateEntry.size();i++) \
                if (mDelegateEntry[i] == entry) \
                    return; \
            mDelegateEntry.push_back(entry); \
        } \
        size_t size(){ \
            return mDelegateEntry.size(); \
        } \
        template <typename __T__> \
        void add(void* objPtr, void (__T__::*methodPtr) (__VA_ARGS__) ){ \
            add(static_cast<aRibeiro::DelegateFriendObject*>(objPtr), (className##Method) methodPtr) ; \
        } \
        void add(className##Function functionPtr){ \
            DelegateEntry entry (functionPtr); \
            for (size_t i=0;i<mDelegateEntry.size();i++) \
                if (mDelegateEntry[i] == entry) \
                    return; \
            mDelegateEntry.push_back(entry); \
        } \
        void remove(aRibeiro::DelegateFriendObject* objPtr, className##Method methodPtr){ \
            DelegateEntry entry (objPtr,methodPtr ); \
            iteratorType it = mDelegateEntry.begin(); \
            for (; it != mDelegateEntry.end(); it++) \
                if ((*it) == entry){ \
                    mDelegateEntry.erase(it); \
                    return; \
                } \
        } \
        template <typename __T__> \
        void remove(void* objPtr, void (__T__::*methodPtr) (__VA_ARGS__) ){ \
            remove( static_cast<aRibeiro::DelegateFriendObject*>(objPtr), (className##Method) methodPtr); \
        } \
        void remove(className##Function functionPtr){ \
            DelegateEntry entry (functionPtr ); \
            iteratorType it = mDelegateEntry.begin(); \
            for (; it != mDelegateEntry.end(); it++) \
                if ((*it) == entry){ \
                    mDelegateEntry.erase(it); \
                    return; \
                } \
        } \
        void removeAll(){ \
            mDelegateEntry.clear(); \
        } \
        void operator()(__VA_ARGS__){ 

/// \brief Declare a Delegate Class
///
/// It is needed to use three macros to declare a delegate: #BEGIN_DECLARE_DELEGATE, #CALL_PATTERN, #END_DECLARE_DELEGATE.
///
/// The delegate in this framework work as a list of functors or methods that can be called with a call signature.
///
/// How to use them:
///
/// \code
/// // Define a class to hold the delegation information
/// // The name of the class will be ViewportDelegate, 
/// // it will handle the call signature `void call(Viewport* viewport)`.
/// BEGIN_DECLARE_DELEGATE(ViewportDelegate, Viewport* viewport ) CALL_PATTERN (viewport) END_DECLARE_DELEGATE;
///
/// // after that, you can declare an instance of the delegate class
/// ViewportDelegate OnViewport;
///
/// class ExampleClass {
/// public:
///     void method(Viewport* viewport) {
///         ...
///     }
/// };
///
/// void func(Viewport* viewport) {
///     ...
/// }
///
/// ExampleClass obj_instance;
/// // Adding references
/// OnViewport.add(&func);
/// OnViewport.add(&obj_instance, &ExampleClass::method);
///
/// // call all functors or methods added to this delegate
/// OnViewport(viewport);
///
/// // it is possible to remove a previous added reference afterwards
///
/// // Removing references
/// OnViewport.remove(&func);
/// OnViewport.remove(&obj_instance, &ExampleClass::method);
/// \endcode
///
/// \author Alessandro Ribeiro
///
#define CALL_PATTERN( ... ) \
            for (size_t i=0;i<mDelegateEntry.size();i++) \
                if (mDelegateEntry[i].functionID == 1) \
                    (mDelegateEntry[i].mObjPtr->*(mDelegateEntry[i].mMethodPtr))(__VA_ARGS__); \
                else if (mDelegateEntry[i].functionID == 2) \
                    (mDelegateEntry[i].mFunctionPtr)(__VA_ARGS__);


/// \brief Declare a Delegate Class
///
/// It is needed to use three macros to declare a delegate: #BEGIN_DECLARE_DELEGATE, #CALL_PATTERN, #END_DECLARE_DELEGATE.
///
/// The delegate in this framework work as a list of functors or methods that can be called with a call signature.
///
/// How to use them:
///
/// \code
/// // Define a class to hold the delegation information
/// // The name of the class will be ViewportDelegate, 
/// // it will handle the call signature `void call(Viewport* viewport)`.
/// BEGIN_DECLARE_DELEGATE(ViewportDelegate, Viewport* viewport ) CALL_PATTERN (viewport) END_DECLARE_DELEGATE;
///
/// // after that, you can declare an instance of the delegate class
/// ViewportDelegate OnViewport;
///
/// class ExampleClass {
/// public:
///     void method(Viewport* viewport) {
///         ...
///     }
/// };
///
/// void func(Viewport* viewport) {
///     ...
/// }
///
/// ExampleClass obj_instance;
/// // Adding references
/// OnViewport.add(&func);
/// OnViewport.add(&obj_instance, &ExampleClass::method);
///
/// // call all functors or methods added to this delegate
/// OnViewport(viewport);
///
/// // it is possible to remove a previous added reference afterwards
///
/// // Removing references
/// OnViewport.remove(&func);
/// OnViewport.remove(&obj_instance, &ExampleClass::method);
/// \endcode
///
/// \author Alessandro Ribeiro
///
#define END_DECLARE_DELEGATE ; \
        } \
    }


}


#endif
