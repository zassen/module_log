
#ifndef _THREAD_H_
#define _THREAD_H_

#include <stdint.h>
#include <sys/types.h>
#include <time.h>

#if !defined(_WIN32)
# include <pthread.h>
#endif

#include <Errors.h>

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

class Thread
{
public:
    // Create a Thread object, but doesn't create or start the associated
    // thread. See the run() method.
    virtual             ~Thread();

    // Start the thread in threadLoop() which needs to be implemented.
    virtual status_t    run(    const char* name,);
                           //     int32_t priority = PRIORITY_DEFAULT,
                            //    size_t stack = 0);
    
    // Ask this object's thread to exit. This function is asynchronous, when the
    // function returns the thread might still be running. Of course, this
    // function can be called from a different thread.
    virtual void        requestExit();

    // Good place to do one-time initializations
    virtual status_t    readyToRun();
    
    // Call requestExit() and wait until this object's thread exits.
    // BE VERY CAREFUL of deadlocks. In particular, it would be silly to call
    // this function from this object's thread. Will return WOULD_BLOCK in
    // that case.
    status_t    requestExitAndWait();

    // Wait until this object's thread exits. Returns immediately if not yet running.
    // Do not call from this object's thread; will return WOULD_BLOCK in that case.
    status_t    join();

    // Indicates whether this thread is running or not.
    bool        isRunning() const;

    // Return the thread's kernel ID, same as the thread itself calling gettid(),
    // or -1 if the thread is not running.
            uint32_t       getTid() const;

protected:
    // exitPending() returns true if requestExit() has been called.
            bool        exitPending() const;
    
private:
    // Derived class must implement threadLoop(). The thread starts its life
    // here. There are two ways of using the Thread object:
    // 1) loop: if threadLoop() returns true, it will be called again if
    //          requestExit() wasn't called.
    // 2) once: if threadLoop() returns false, the thread will exit upon return.
    virtual bool        threadLoop() = 0;

private:
    Thread& operator=(const Thread&);
    static  int             _threadLoop(void* user);
    // always hold mLock when reading or writing
            uint32_t     mThread;
    mutable Mutex           mLock;
            status_t        mStatus;
    // note that all accesses of mExitPending and mRunning need to hold mLock
    volatile bool           mExitPending;
    volatile bool           mRunning;
	    Thread	    *mHoldSelf;
            uint32_t           mTid;
};



// ---------------------------------------------------------------------------
#endif // _THREAD_H_
// -------------------------------------------------------------------------
