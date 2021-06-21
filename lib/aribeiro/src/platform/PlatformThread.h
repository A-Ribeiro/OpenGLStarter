#ifndef PLATFORM_THREAD_H
#define PLATFORM_THREAD_H

#include <aribeiro/common.h>
#include <aribeiro/PlatformMutex.h>
#include <map>

// Fix for unaligned stack with clang and GCC on Windows XP 32-bit
#if defined(OS_TARGET_win) && (defined(__clang__) || defined(__GNUC__))
    #define ALIGN_STACK __attribute__((__force_align_arg_pointer__))
#else
    #define ALIGN_STACK
#endif

#if defined(OS_TARGET_win)
    #define WIN32_LEAN_AND_MEAN
#include <WinSock2.h>
#include <WS2tcpip.h>
    #include <windows.h>
    typedef DWORD THREAD_ID_TYPE;

    typedef HANDLE THREAD_HANDLE_TYPE;

#elif defined(OS_TARGET_linux) || defined(ARIBEIRO_OS_ANDROID)
    #include <sys/syscall.h>
    #include <sys/types.h>
    typedef pid_t THREAD_ID_TYPE;

    typedef pthread_t THREAD_HANDLE_TYPE;
#elif defined(OS_TARGET_mac)
    #include <sys/types.h>
    typedef uint64_t THREAD_ID_TYPE;

    typedef pthread_t THREAD_HANDLE_TYPE;
#else
    #error Thread for this platform is not implemented...
#endif

namespace aRibeiro {

    //
    // Thread Function templates...
    //

    /// \brief Base class to hold the thread entry point reference
    ///
    /// \author Alessandro Ribeiro
    ///
    struct ThreadRunEntryPoint
    {
        virtual ~ThreadRunEntryPoint() {}
        virtual void run() = 0;
    };

    /// \brief Functor entry point specialization
    ///
    /// \author Alessandro Ribeiro
    ///
    template <typename T>
    struct ThreadRunFunctor : ThreadRunEntryPoint
    {
        ThreadRunFunctor(T functor) : m_functor(functor) {}
        virtual void run() { m_functor(); }
        T m_functor;
    };

    /// \brief Functor entry point specialization with one argument
    ///
    /// \author Alessandro Ribeiro
    ///
    template <typename F, typename A>
    struct ThreadRunFunctorWithArg : ThreadRunEntryPoint
    {
        ThreadRunFunctorWithArg(F function, A arg) : m_function(function), m_arg(arg) {}
        virtual void run() { m_function(m_arg); }
        F m_function;
        A m_arg;
    };

    /// \brief Method entry point specialization
    ///
    /// \author Alessandro Ribeiro
    ///
    template <typename C>
    struct ThreadRunMemberFunc : ThreadRunEntryPoint
    {
        ThreadRunMemberFunc(void(C::*function)(), C* object) : m_function(function), m_object(object) {}
        virtual void run() { (m_object->*m_function)(); }
        void(C::*m_function)();
        C* m_object;
    };

    /// \brief Method entry point specialization with an argument
    ///
    /// \author Alessandro Ribeiro
    ///
    template <typename C, typename A>
    struct ThreadRunMemberFuncWithArg : ThreadRunEntryPoint
    {
        ThreadRunMemberFuncWithArg(void(C::*function)(), C* object, A arg) : m_function(function), m_object(object), m_argument(arg){}
        virtual void run() { (m_object->*m_function)(m_argument); }
        void(C::*m_function)();
        C* m_object;
        A m_argument;
    };
    
    /// \brief Manager a thread in several platforms. You can use functions, structs and classes.
    ///
    /// \author Alessandro Ribeiro
    ///
    class PlatformThread
    {
    private:

        //
        // OS SPECIFIC ATTRIBUTES
        //
#if defined(OS_TARGET_win)

        ALIGN_STACK static unsigned int __stdcall entryPoint(void* userData);
        HANDLE m_thread;
        unsigned int m_threadId;

#else
        static void* entryPoint(void* userData);
        pthread_t m_thread;
        bool      m_isActive;
        
#endif

    private:
        
        PlatformMutex mutex;
        
        //static PlatformMutex threadsToFinalizeLock;
        //static std::vector<ThreadFinalizeStruct> threadsToFinalize;
        //static void disposeThread(PlatformThread* thread);
        
        static PlatformMutex openedThreadsLock;
        static std::map<THREAD_ID_TYPE, PlatformThread*> openedThreads;
        static void registerThread(PlatformThread* thread);
        static void unregisterThread(PlatformThread* thread);
        
        volatile bool interrupted;
        volatile bool exited;
        volatile bool started;
        volatile bool shouldReleaseThreadID_byItself;
        
        volatile int waitCalls;

        ThreadRunEntryPoint *runEntryPoint;

    public:

        /// \brief Check if the current thread is interrupted
        ///
        /// A thread can be interrupted by calling interrupt() method from the parent thread.
        ///
        /// Example:
        ///
        /// \code
        /// #include <aribeiro/aribeiro.h>
        /// using namespace aRibeiro;
        ///
        /// void thread_example() {
        ///     // the thread or main thread could call #interrupt() to change this state
        ///     while (!PlatformThread::isCurrentThreadInterrupted()) {
        ///         ...
        ///         //avoid 100% CPU using by this thread
        ///         PlatformSleep::sleepMillis(1);
        ///     }
        /// }
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \return true if the thread is interrupted
        ///
        static bool isCurrentThreadInterrupted();

        /// \brief Get the current Thread
        ///
        /// Get the current thread. If the thread is not spawned by the PlatformThread, it will return null.
        ///
        /// \author Alessandro Ribeiro
        /// \return A reference to the current thread spawned.
        ///
        static PlatformThread* getCurrentThread();

        virtual ~PlatformThread();

        /// \brief Set a flag to the thread indicating that it is interrupted.
        ///
        /// Example:
        ///
        /// \code
        /// #include <aribeiro/aribeiro.h>
        /// using namespace aRibeiro;
        ///
        /// void thread_example() {
        ///     // the thread or main thread could call #PlatformThread::interrupt() to change this state
        ///     while (!PlatformThread::isCurrentThreadInterrupted()) {
        ///         ...
        ///         //avoid 100% CPU using by this thread
        ///         PlatformSleep::sleepMillis(1);
        ///     }
        /// }
        ///
        /// PlatformThread thread( &thread_example );
        /// // start the thread
        /// thread.start();
        /// ...
        /// // inside the thread implementation, it is necessary to check the #PlatformThread::isCurrentThreadInterrupted()
        /// thread.interrupt();
        /// // wait thread execution finish
        /// thread.wait();
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        ///
        void interrupt();

        /// \brief Spawn the thread. It can be called just once.
        ///
        /// \brief Set a flag to the thread indicating that it is interrupted.
        ///
        /// Example:
        ///
        /// \code
        /// #include <aribeiro/aribeiro.h>
        /// using namespace aRibeiro;
        ///
        /// void thread_example() {
        ///     // the thread or main thread could call #PlatformThread::interrupt() to change this state
        ///     while (!PlatformThread::isCurrentThreadInterrupted()) {
        ///         ...
        ///         //avoid 100% CPU using by this thread
        ///         PlatformSleep::sleepMillis(1);
        ///     }
        /// }
        ///
        /// PlatformThread thread( &thread_example );
        /// // start the thread
        /// thread.start();
        /// ...
        /// // inside the thread implementation, it is necessary to check the #PlatformThread::isCurrentThreadInterrupted()
        /// thread.interrupt();
        /// // wait thread execution finish
        /// thread.wait();
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        ///
        void start();

        /// \brief Wait the current thread to terminate their execution scope. Can be called once.
        ///
        /// Example:
        ///
        /// \code
        /// #include <aribeiro/aribeiro.h>
        /// using namespace aRibeiro;
        ///
        /// void thread_example() {
        ///     // the thread or main thread could call #PlatformThread::interrupt() to change this state
        ///     while (!PlatformThread::isCurrentThreadInterrupted()) {
        ///         ...
        ///         //avoid 100% CPU using by this thread
        ///         PlatformSleep::sleepMillis(1);
        ///     }
        /// }
        ///
        /// PlatformThread thread( &thread_example );
        /// // start the thread
        /// thread.start();
        /// ...
        /// // inside the thread implementation, it is necessary to check the #PlatformThread::isCurrentThreadInterrupted()
        /// thread.interrupt();
        /// // wait thread execution finish
        /// thread.wait();
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        ///
        void wait();
        bool isAlive();
        bool isStarted();
        void setShouldDisposeThreadByItself(bool v);

        /// \brief Force the target thread to terminate. It does not garantee the execution scope to terminate.
        ///
        /// \warning It is dangerous to call this method.
        ///
        /// Example:
        ///
        /// \code
        /// #include <aribeiro/aribeiro.h>
        /// using namespace aRibeiro;
        ///
        /// void thread_example() {
        ///     ...
        /// }
        ///
        /// PlatformThread thread( &thread_example );
        /// // start the thread
        /// thread.start();
        /// ...
        /// thread.terminate();
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        ///
        void terminate();

        /// \brief Constructor (function)
        ///
        /// Creates a thread associated with a function as its run point.
        ///
        /// example:
        ///
        /// \code
        /// #include <aribeiro/aribeiro.h>
        /// using namespace aRibeiro;
        ///
        /// void thread_function()
        /// {
        ///     ...
        /// }
        ///
        /// PlatformThread thread( &thread_function );
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        ///
        template <typename F>
        PlatformThread(F function) {
            runEntryPoint = new ThreadRunFunctor<F>(function);
            interrupted = false;
            exited = false;
            started = false;
            shouldReleaseThreadID_byItself = false;
            waitCalls = 0;
#if defined(OS_TARGET_win)
            m_thread = NULL;
#else
            m_isActive = false;
#endif
        }

        /// \brief Constructor (function, argument)
        ///
        /// Creates a thread associated with a function as its run point.
        /// Can pass an argument to the thread.
        ///
        /// example:
        ///
        /// \code
        /// #include <aribeiro/aribeiro.h>
        /// using namespace aRibeiro;
        ///
        /// void threadFunc(int argument) {
        ///     ...
        /// }
        ///
        /// PlatformThread thread(&threadFunc, 5);
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        ///
        template <typename F, typename A>
        PlatformThread(F function, A argument) {
            runEntryPoint = new ThreadRunFunctorWithArg<F, A>(function, argument);
            interrupted = false;
            exited = false;
            started = false;
            shouldReleaseThreadID_byItself = false;
            waitCalls = 0;
#if defined(OS_TARGET_win)
            m_thread = NULL;
#else
            m_isActive = false;
#endif
        }

        /// \brief Constructor (method, object)
        ///
        /// Creates a thread associated with a method as its run point.
        /// Need to pass the object as the second parameter
        ///
        /// example:
        ///
        /// \code
        /// #include <aribeiro/aribeiro.h>
        /// using namespace aRibeiro;
        ///
        /// class Task {
        /// public:
        ///     void run()
        ///     {
        ///         ...
        ///     }
        /// };
        ///
        /// Task task_instance;
        ///
        /// PlatformThread thread(&task_instance, &Task::run);
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        ///
        template <typename C>
        PlatformThread(C* object, void(C::*function)()) {
            runEntryPoint = new ThreadRunMemberFunc<C>(function, object);
            interrupted = false;
            exited = false;
            started = false;
            shouldReleaseThreadID_byItself = false;
            waitCalls = 0;
#if defined(OS_TARGET_win)
            m_thread = NULL;
#else
            m_isActive = false;
#endif
        }

        /// \brief Constructor (method, object, argument)
        ///
        /// Creates a thread associated with a method as its run point.
        /// Need to pass the object as the second parameter
        ///
        /// example:
        ///
        /// \code
        /// #include <aribeiro/aribeiro.h>
        /// using namespace aRibeiro;
        ///
        /// class Task {
        /// public:
        ///     void run(int arg)
        ///     {
        ///         ...
        ///     }
        /// };
        ///
        /// Task task_instance;
        ///
        /// PlatformThread thread(&task_instance, &Task::run, 5);
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        ///
        template <typename C, typename A>
        PlatformThread(C* object, void(C::*function)(), A argument) {
            runEntryPoint = new ThreadRunMemberFuncWithArg<C,A>(function, object, argument);
            interrupted = false;
            exited = false;
            started = false;
            shouldReleaseThreadID_byItself = false;
            waitCalls = 0;
#if defined(OS_TARGET_win)
            m_thread = NULL;
#else
            m_isActive = false;
#endif
        }
    };

}



#endif
