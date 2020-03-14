#include "udp_server.h"

udp_server::udp_server(size_t port,  size_t buffer_size)
: m_port(port)
, m_buffer_size(buffer_size)
{
   // we need the "io_service" when creating the socket
   // create and open socket at designated port
   unsigned short uport =  static_cast<unsigned short>(m_port);
   m_socket = std::make_shared<boost_socket>(m_io_service, boost_endpoint(boost_udp::v4(),uport));
}

udp_server::~udp_server()
{}

bool udp_server::read_message(std::string& message)
{
   // receive buffer
   std::vector<char> recv_buf(m_buffer_size);

   // the sender endpoint can be anything
   boost_endpoint  sender_endpoint;
   boost::system::error_code error;

   // blocking read
   size_t received_length = m_socket->receive_from( boost::asio::buffer(recv_buf), sender_endpoint,0,error);
   if(received_length > 0) {

      // truncate the buffer to actual received size
      recv_buf.resize(received_length);

      // turn it into a string and assign to output variable
      message = std::string(recv_buf.begin(),recv_buf.end());
      return true;
   }

   // if we end up here, something failed
   return false;
}
