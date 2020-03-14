#include "udp_server.h"

udp_server::udp_server(std::shared_ptr<udp_reader> udp_reader)
: m_udp_reader(udp_reader)
, m_msg_queue(std::make_shared<safe_queue<std::string>>())
{}

udp_server::~udp_server()
{}

void udp_server::run()
{
   while(!stop_requested()) {

      // read UDP message from socket, or time out if nothing received
      // typically the timeout is 1 sec (depends on setting in udp_reader)

      std::string udp_cmd;
      if(m_udp_reader->read_message(udp_cmd)) {

         // message received, put it on the queue
         m_msg_queue->enqueue(udp_cmd);
      }

      std::this_thread::sleep_for(std::chrono::milliseconds(100));
   }
}
