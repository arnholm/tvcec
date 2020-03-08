#include "udp_config.h"
#include <vector>
#include <wx/tokenzr.h>

udp_config::udp_config()
: m_conf(std::make_shared<wxConfig>(wxT("rpicec"),wxT("rpicec")))
{
   read_config();
}

udp_config::~udp_config()
{}

// read configuration
size_t udp_config::read_config()
{
   m_map.clear();

   size_t ncmd = m_conf->GetNumberOfEntries();
   if(ncmd == 0) ncmd = create_config();

   m_conf->SetPath("/cec");

   // first enumerate all entries
   wxString name;
   long dummy;
   std::vector<wxString> names;
   bool bCont = m_conf->GetFirstEntry(name, dummy);
   while ( bCont ) {
      names.push_back(name);
      bCont = m_conf->GetNextEntry(name, dummy);
   }

   // then insert in map
   for(auto& name : names) {
      wxString value;
      m_conf->Read(name,&value,"");
      m_map[name] = value;
   }
}

// generate default configuratiuon
size_t udp_config::create_config()
{
   m_conf->DeleteAll();
   m_conf->SetPath("/cec");
   m_conf->Write("cec-on","on %s");
   m_conf->Write("cec-standby","standby %s");
   m_conf->Write("cec-volup","volup");
   m_conf->Write("cec-voldown","voldown");
   m_conf->Write("cec-mute","mute");
   m_conf->Write("cec-hdmi","txn 4f:82:%s0:00");

   return m_conf->GetNumberOfEntries();
}

bool udp_config::parse(const wxString& udp_cmd, wxString& cec_cmd)
{
   cec_cmd = "";

   std::vector<wxString> tokens;
   wxStringTokenizer tokenizer(udp_cmd, " ");
   while ( tokenizer.HasMoreTokens() )  {
      tokens.push_back(tokenizer.GetNextToken());
   }

   if(tokens.size() > 0) {
      auto i = m_map.find(tokens[0]);
      if(i != m_map.end()) {
         const wxString& cec = i->second;
         if( (cec.Find("%s") != wxNOT_FOUND) && tokens.size()>1 ) {
            cec_cmd = wxString::Format(cec,tokens[1]);
         }
         else {
            cec_cmd = cec;
         }
         return true;
      }
   }
   return false;
}
