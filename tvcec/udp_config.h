#ifndef UDP_CONFIG_H
#define UDP_CONFIG_H

#include <map>
#include <wx/config.h>
#include <wx/string.h>

class udp_config {
public:
   typedef std::map<wxString, wxString>  udp_map;   // <udp_cmd,cec_cmd>

   udp_config();
   virtual ~udp_config();

   // read configuration file
   size_t read_config();

   // generate default configuratiuon
   size_t create_config();

   // parse message and return CEC command
   // this waits until a message is received
   bool parse(const wxString& udp_cmd, wxString& cec_cmd);

private:
   udp_map    m_map;
   wxConfig*  m_conf;
};

#endif // UDP_CONFIG_H
