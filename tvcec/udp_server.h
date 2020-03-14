#ifndef UDP_SERVER_H
#define UDP_SERVER_H

#include "stoppable_thread.h"
#include "udp_reader.h"
#include "safe_queue.h"
#include <memory>
#include <string>

class udp_server : public stoppable_thread {
public:
   udp_server(std::shared_ptr<udp_reader> udp_reader);
   virtual ~udp_server();

   void run();

   // return access to the message queue
   std::shared_ptr<safe_queue<std::string>> queue() const { return m_msg_queue; }

private:
   std::shared_ptr<udp_reader>               m_udp_reader;  // udp reader
   std::shared_ptr<safe_queue<std::string>>  m_msg_queue;   // UDP messages end up here
};

#endif // UDP_SERVER_H
