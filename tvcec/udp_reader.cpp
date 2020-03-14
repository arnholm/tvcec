#include "udp_reader.h"

udp_reader::udp_reader(size_t port,  size_t buffer_size, const int timeout_ms )
: m_port(port)
, m_buffer_size(buffer_size)
{
   // we need the "io_service" when creating the socket
   // create and open socket at designated port
   unsigned short uport =  static_cast<unsigned short>(m_port);
   m_socket = std::make_shared<boost_socket>(m_io_service, boost_endpoint(boost_udp::v4(),uport));


   // set milliseconds timeout on the UDP socket
   // This does not work under Raspian, it throws an exception
  //  m_socket->set_option(boost::asio::detail::socket_option::integer<SOL_SOCKET, SO_RCVTIMEO>{ timeout_ms });
}

udp_reader::~udp_reader()
{}

bool udp_reader::read_message(std::string& message)
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

   // if we end up here, it is most likely doe to socket timeout
   return false;
}
