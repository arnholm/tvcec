#ifndef STOPPABLE_THREAD_H
#define STOPPABLE_THREAD_H

// based on https://thispointer.com/c11-how-to-stop-or-terminate-a-thread/

#include <chrono>
#include <future>

// stoppable_thread is a convenience base class for a thread object that runs in a std::thread
// The principle is that the derived class implements the run() member to do the work,
// and it must check for stop requests by calling stop_requested() frequently
// the calling thread can request the thread to stop by calling the stop() function at any time

class stoppable_thread {
public:
   stoppable_thread();
   stoppable_thread(stoppable_thread&& obj);
   stoppable_thread& operator=(stoppable_thread&& obj);
   virtual ~stoppable_thread();

   // thread function
   void operator()() { run();}

   // run() will be called by thread function, must be implemented in derived class
	virtual void run() = 0;

   //Checks if thread is requested to stop, this is called in derived run() implementation
   bool stop_requested();

   // request the task to stop executing
   void stop();

private:
	std::promise<void> m_exit_signal;
	std::future<void>  m_future;
};

#endif // STOPPABLE_THREAD_H
