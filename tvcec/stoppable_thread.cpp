#include "stoppable_thread.h"

stoppable_thread::stoppable_thread()
: m_exit_signal()
, m_future(m_exit_signal.get_future())
{}

stoppable_thread::stoppable_thread(stoppable_thread&& obj)
: m_exit_signal(std::move(obj.m_exit_signal))
, m_future(std::move(obj.m_future))
{}

stoppable_thread& stoppable_thread::operator=(stoppable_thread&& obj)
{
   m_exit_signal = std::move(obj.m_exit_signal);
   m_future = std::move(obj.m_future);
   return *this;
}

stoppable_thread::~stoppable_thread()
{
   //dtor
}

bool stoppable_thread::stop_requested()
{
   // checks if value in future object is available
   if (m_future.wait_for(std::chrono::milliseconds(0)) == std::future_status::timeout)return false;
   return true;
}


void stoppable_thread::stop()
{
   m_exit_signal.set_value();
}
