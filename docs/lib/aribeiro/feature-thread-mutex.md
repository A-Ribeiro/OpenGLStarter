# OpenGLStarter

[Back to HOME](../../index)

## Platform Thread and Mutex

You can find a lot of cross-platform implementation of threads and mutex. This is another implementation.

I thought the usage to be in the same Java thread fashion.

You can create a thread that points to a class method or a function.

The thread has the interrupt attribute. To be possible to send a kind of signal to the thread you want to finalize.

To use the PlatformMutex, you just need to create an object and call lock or unlock method. The base implementation is recursive.

Example:

```cpp
PlatformMutex mutex;

void thread_function() {
  printf("thread Start...\n");
  mutex.lock();
  printf("thread Critical Section Enter Success !!!\n");
  mutex.unlock();

  printf("thread waiting interrupt from another thread...\n");
  while (!PlatformThread::isCurrentThreadInterrupted()) {
    //avoid 100% CPU using by this thread
    PlatformSleep::sleepMillis(3000);
  }

  printf("thread Interrupt Detected !!!\n");
  printf("thread End...\n");
}


void main(int argc, char* argv[]) {
  PlatformThread thread(thread_function);

  printf("  MainThread: Mutex lock before thread start\n");
  mutex.lock();

  thread.start();
  PlatformSleep::sleepSec(3);

  printf("  MainThread: Mutex unlock\n");
  mutex.unlock();
	
  printf("  MainThread: Interrupt and Waiting thread To Finish!!!\n");
  thread.interrupt();
  thread.wait();
	
  printf("  MainThread: Thread Finish Detected!!!\n");
}
```
