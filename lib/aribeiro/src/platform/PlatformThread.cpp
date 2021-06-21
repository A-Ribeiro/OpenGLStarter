#include "PlatformThread.h"
#include "PlatformSleep.h"
#include "PlatformTime.h"

#include <stdio.h>

#if defined(OS_TARGET_win)
    #include <process.h>

#elif defined(OS_TARGET_linux)
    #include <stdlib.h>
    #include <unistd.h>
    #include <sys/syscall.h>
    #include <sys/types.h>
    #include <signal.h>
    #include <pthread.h>

    #define GetCurrentThreadId() syscall(SYS_gettid)
#elif defined(OS_TARGET_mac)
    #include <stdlib.h>
    #include <unistd.h>
    #include <sys/syscall.h>
    #include <sys/types.h>
    #include <signal.h>
    #include <pthread.h>

    uint64_t GetCurrentThreadId() {
        uint64_t tid;
        pthread_threadid_np(NULL, &tid);
        return tid;
    }
#else
    #error Thread for this platform is not implemented...
#endif

namespace aRibeiro {
    
    
    struct ThreadFinalizeStruct
    {
        THREAD_HANDLE_TYPE handle;
        ThreadRunEntryPoint *runEntryPoint;
        int32_t timeout_ms;
    };

    volatile bool ReleaseThreadIDClass_finished = false;

    class ReleaseThreadIDClass{

        PlatformMutex threadsToFinalizeLock;
        std::vector<ThreadFinalizeStruct> threadsToFinalize;
        PlatformThread releaseThread;
    
        void release(ThreadFinalizeStruct aux){
            
            //printf("ReleaseThreadIDClass::release() release thread ID\n");
            
#if defined(OS_TARGET_win)
            WaitForSingleObject(aux.handle, INFINITE);
            if (aux.handle)
                CloseHandle(aux.handle);
#else
            pthread_join(aux.handle, NULL);
#endif
            if (aux.runEntryPoint != NULL)
                delete aux.runEntryPoint;
        }
        
        void releaseThreadID_thread(){
            PlatformTime time;
            
            time.update();
            while ( !PlatformThread::isCurrentThreadInterrupted() ) {
                time.update();
                int32_t delta_ms = time.deltaTimeMicro/1000;
                threadsToFinalizeLock.lock();
                for(int i=threadsToFinalize.size()-1;i>=0;i--){
                    threadsToFinalize[i].timeout_ms -= delta_ms;
                    if (threadsToFinalize[i].timeout_ms < 0){
                        ThreadFinalizeStruct aux = threadsToFinalize[i];
                        threadsToFinalize.erase(threadsToFinalize.begin()+i);
                        release(aux);
                    }
                }
                threadsToFinalizeLock.unlock();
                PlatformSleep::sleepMillis(300);
            }
            
            threadsToFinalizeLock.lock();
            for(int i=threadsToFinalize.size()-1;i>=0;i--){
                release(threadsToFinalize[i]);
            }
            threadsToFinalize.clear();
            threadsToFinalizeLock.unlock();
            
        }
    public:
        
        ReleaseThreadIDClass():releaseThread(this, &ReleaseThreadIDClass::releaseThreadID_thread){
            releaseThread.setShouldDisposeThreadByItself(true);
            releaseThread.start();
        }
        
        virtual ~ReleaseThreadIDClass(){
            ReleaseThreadIDClass_finished = true;
            printf("~ReleaseThreadIDClass()\n");
            releaseThread.interrupt();
            releaseThread.wait();
        }
        
        void registerToReleaseThreadID( ThreadFinalizeStruct v ){
            if (ReleaseThreadIDClass_finished) {
                printf("ERROR: trying to release a thread after the ReleaseThreadIDClass has been released...\n");
                return;
            }
            threadsToFinalizeLock.lock();
            threadsToFinalize.push_back(v);
            threadsToFinalizeLock.unlock();
        }
        
        static ReleaseThreadIDClass* Instance(){
            static ReleaseThreadIDClass instance;
            return &instance;
        }
        
    };
    
    
    
    //
    // GLOBALS STATIC
    //
    //PlatformMutex PlatformThread::threadsToFinalizeLock;
    //std::vector<ThreadFinalizeStruct> PlatformThread::threadsToFinalize;
    
    
    PlatformMutex PlatformThread::openedThreadsLock;
    std::map<THREAD_ID_TYPE, PlatformThread*> PlatformThread::openedThreads;

    void PlatformThread::registerThread(PlatformThread* thread) {
        THREAD_ID_TYPE tid = GetCurrentThreadId();
        openedThreadsLock.lock();
        openedThreads[tid] = thread;
        openedThreadsLock.unlock();
    }

    void PlatformThread::unregisterThread(PlatformThread* thread) {
        THREAD_ID_TYPE tid = GetCurrentThreadId();
        std::map<THREAD_ID_TYPE, PlatformThread*>::iterator it;
        openedThreadsLock.lock();
        if (openedThreads.size() > 0) {
            it = openedThreads.find(tid);
            if (it != openedThreads.end()) {
                PlatformThread* result = it->second;
                openedThreads.erase(it);
                result->exited = true;
            }
        }
        openedThreadsLock.unlock();
    }
    
    bool PlatformThread::isCurrentThreadInterrupted() {
        PlatformThread* thread = getCurrentThread();
        if (thread != NULL)
            return thread->interrupted;
        return false;
    }

    PlatformThread* PlatformThread::getCurrentThread() {
        PlatformThread* result = NULL;
        THREAD_ID_TYPE tid = GetCurrentThreadId();
        std::map<THREAD_ID_TYPE, PlatformThread*>::iterator it;
        openedThreadsLock.lock();
        if (openedThreads.size() > 0) {
            it = openedThreads.find(tid);
            if (it != openedThreads.end())
                result = it->second;
        }
        openedThreadsLock.unlock();
        return result;
    }

    //
    // GLOBALS - NON STATIC
    //
    void PlatformThread::interrupt() {
        interrupted = true;
    }

    PlatformThread::~PlatformThread() {
        
        interrupt();
        wait();
        
        if (shouldReleaseThreadID_byItself){
            printf("PlatformThread::~PlatformThread() shouldReleaseThreadID_byItself = true\n");
            PlatformSleep::sleepMillis(10);
            while (waitCalls>0)
                PlatformSleep::sleepMillis(10);
            
#if defined(OS_TARGET_win)
            if (m_thread)
            {
                // A thread cannot wait for itself!
                if (m_threadId != GetCurrentThreadId())
                    WaitForSingleObject(m_thread, INFINITE);
                
                //CLOSE THREAD
                if (m_thread)
                    CloseHandle(m_thread);
                m_thread = NULL;
            }
        
#else
        
            if (m_isActive)
            {
                // A thread cannot wait for itself!
                if (pthread_equal(pthread_self(), m_thread) == 0)
                    pthread_join(m_thread, NULL);
                
                //m_thread = NULL;
            }
        
#endif
        
            if (runEntryPoint != NULL)
                delete runEntryPoint;
            runEntryPoint = NULL;
        } else {
            
            ThreadFinalizeStruct aux;
            aux.handle = m_thread;
            aux.runEntryPoint = runEntryPoint;
            aux.timeout_ms = 1000;
            
            
#if defined(OS_TARGET_win)
            if (m_thread)
#else
            if (m_isActive)
#endif
                ReleaseThreadIDClass::Instance()->registerToReleaseThreadID(aux);
            else {
                if (runEntryPoint != NULL)
                    delete runEntryPoint;
                runEntryPoint = NULL;
            }
            
        }
        
    }

    void PlatformThread::start() {
        
        mutex.lock();
        if (started) {
            printf("thread already started...\n");
            mutex.unlock();
            return;
        }
        started = true;
        mutex.unlock();
        
        // create OS thread
#if defined(OS_TARGET_win)
        m_thread = reinterpret_cast<HANDLE>(_beginthreadex(NULL, 0, &PlatformThread::entryPoint, this, 0, &m_threadId));

        if (!m_thread)
        {
            printf("Erro ao abrir thread...\n");
        }

#else
        pthread_attr_t attrs;
        pthread_attr_init(&attrs);
        pthread_attr_setdetachstate(&attrs, PTHREAD_CREATE_JOINABLE);
        
        m_isActive = pthread_create(&m_thread, &attrs, &PlatformThread::entryPoint, this) == 0;
        if (!m_isActive)
        {
            printf("Erro ao abrir thread...\n");
        }
#endif

    }
    
    /*
    void PlatformThread::disposeThread(PlatformThread* t) {
        ThreadFinalizeStruct structFinalize;
        structFinalize.handle = t->m_thread;
        structFinalize.runEntryPoint = t->runEntryPoint;
        structFinalize.timeout = 1000;
        threadsToFinalizeLock.lock();
        threadsToFinalize.push_back(structFinalize);
        threadsToFinalizeLock.unlock();
    }*/
    
    

    void PlatformThread::wait() {

        //self thread test
#if defined(OS_TARGET_win)
        if (!m_thread)
            return;
        if (m_threadId == GetCurrentThreadId())
            return;
#else
        if (!m_isActive)
            return;
        if (pthread_equal(pthread_self(), m_thread) != 0)
            return;
#endif

        waitCalls++;
        while (isAlive())
            PlatformSleep::sleepMillis(1);
        waitCalls--;
    }
    
    bool PlatformThread::isAlive() {
#if defined(OS_TARGET_win)
        return !exited && m_thread != NULL;
#else
        return !exited && m_isActive;
#endif
    }
    
    bool PlatformThread::isStarted(){
        return started;
    }
    
    void PlatformThread::setShouldDisposeThreadByItself(bool v) {
        shouldReleaseThreadID_byItself = v;
    }

    void PlatformThread::terminate() {
        /*
        if (m_impl)
        {
            m_impl->terminate();
            delete m_impl;
            m_impl = NULL;
        }*/

#if defined(OS_TARGET_win)
        if (m_thread)
            TerminateThread(m_thread, 0);
        //CLOSE THREAD
        if (m_thread)
            CloseHandle(m_thread);
        m_thread = NULL;
#else
        if (m_isActive)
        {
#ifndef OS_ANDROID
            pthread_cancel(m_thread);
#else
            // See http://stackoverflow.com/questions/4610086/pthread-cancel-al
            pthread_kill(m_thread, SIGUSR1);
#endif
        }
#endif
    }

    //void PlatformThread::run()
    //{
    //    runEntryPoint->run();
    //}

#if defined(OS_TARGET_win)

    unsigned int __stdcall PlatformThread::entryPoint(void* userData)
    {
        // The Thread instance is stored in the user data
        PlatformThread* owner = static_cast<PlatformThread*>(userData);
        registerThread(owner);
        // Forward to the owner
        owner->runEntryPoint->run();
        unregisterThread(owner);
        // Optional, but it is cleaner
        _endthreadex(0);
        return 0;
    }

#else
    void* PlatformThread::entryPoint(void* userData)
    {
        // The Thread instance is stored in the user data
        PlatformThread* owner = static_cast<PlatformThread*>(userData);

#ifndef OS_ANDROID
        // Tell the thread to handle cancel requests immediately
        pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);
#endif
        registerThread(owner);
        // Forward to the owner
        owner->runEntryPoint->run();
        unregisterThread(owner);
        return NULL;
    }
#endif
    

}
