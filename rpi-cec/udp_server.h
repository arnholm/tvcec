#ifndef UDP_SERVER_H
#define UDP_SERVER_H


#include <boost/asio.hpp>

typedef boost::asio::ip::udp             boost_udp;
typedef boost::asio::ip::udp::endpoint   boost_endpoint;
typedef boost::asio::ip::udp::socket     boost_socket;
typedef boost::asio::io_service          boost_io_service;


class udp_server {
public:
   udp_server(size_t port,  size_t buffer_size = 1024);
   virtual ~udp_server();

   // check if communication socket is open
   bool is_open() const { return m_socket->is_open(); }

   // read next UDP message, this hangs until data is received
   bool read_message(std::string& message);

private:

   size_t                        m_port;         // UDP port on the receiving end
   size_t                        m_buffer_size;  // max message size in bytes

   boost_io_service              m_io_service;   // must exist with same lifetime as m_socket
   std::shared_ptr<boost_socket> m_socket;       // the socket we read UDP messages from
};

#endif // UDP_SERVER_H
