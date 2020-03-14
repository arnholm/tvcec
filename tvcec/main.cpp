#include <iostream>
#include <stdlib.h>
using namespace std;

#include <wx/defs.h>
#include <wx/platinfo.h>  // platform info
#include <wx/app.h>

#include <wx/string.h>   // wxString
#include <wx/cmdline.h>  // command line parser

#include "udp_config.h"
#include "udp_reader.h"
#include "udp_server.h"

#include <map>
using namespace std;
typedef map<wxString,wxString> CmdLineMap;    // CmdLineMap


/*
   switch 	  This is a boolean option which can be given or not, but which doesn't have any value.
              We use the word switch to distinguish such boolean options from more generic options
              like those described below. For example, -v might be a switch meaning "enable verbose mode".

   option 	  Option for us here is something which comes with a value unlike a switch.
              For example, -o:filename might be an option which allows to specify the name of the output file.

   parameter  This is a required program argument.

   More info at: http://docs.wxwidgets.org/2.8/wx_wxcmdlineparser.html#wxcmdlineparser

*/

static const wxCmdLineEntryDesc cmdLineDesc[] =
{
  //   kind            shortName     longName          description                                     parameterType          flag(s)
  { wxCMD_LINE_SWITCH, wxT_2("help"),    wxT_2("help"),    wxT_2("command line parameter help"),           wxCMD_LINE_VAL_NONE,   wxCMD_LINE_OPTION_HELP },
  { wxCMD_LINE_OPTION, wxT_2("port"),    wxT_2("port"),    wxT_2("UDP port (default=7666)"),               wxCMD_LINE_VAL_STRING, wxCMD_LINE_PARAM_OPTIONAL  },
  { wxCMD_LINE_SWITCH, wxT_2("config"),  wxT_2("config"),  wxT_2("(re-)create default configuration"),     wxCMD_LINE_VAL_NONE,   wxCMD_LINE_PARAM_OPTIONAL  },
  { wxCMD_LINE_SWITCH, wxT_2("verbose"), wxT_2("verbose"), wxT_2("echo messages to terminal"),             wxCMD_LINE_VAL_NONE,   wxCMD_LINE_PARAM_OPTIONAL  },
  { wxCMD_LINE_SWITCH, wxT_2("debug"),   wxT_2("debug"),   wxT_2("debug only, do not invoke cec-client"),  wxCMD_LINE_VAL_NONE,   wxCMD_LINE_PARAM_OPTIONAL  },
  { wxCMD_LINE_NONE,   wxT_2(""),        wxT_2(""),        wxT_2(""),                                      wxCMD_LINE_VAL_NONE,   wxCMD_LINE_PARAM_OPTIONAL  }
};


void ParserToMap(wxCmdLineParser& parser, CmdLineMap& cmdMap)
{
   size_t pcount = sizeof(cmdLineDesc)/sizeof(wxCmdLineEntryDesc) - 1;
   size_t iparam=0;
   for(size_t i=0; i<pcount; i++) {
      wxString pname = cmdLineDesc[i].longName;
      if(cmdLineDesc[i].kind == wxCMD_LINE_PARAM) {
         if(iparam < parser.GetParamCount()) {
            cmdMap.insert(make_pair(pname,parser.GetParam(iparam++)));
         }
      }
      else {
         // switch or option, mush check if present
         if(parser.Found(pname)) {
            wxString pvalue;
            if(cmdLineDesc[i].type == wxCMD_LINE_VAL_STRING) {
               parser.Found(pname,&pvalue);
            }
            else if(cmdLineDesc[i].type == wxCMD_LINE_VAL_NUMBER) {
               long lvalue=0;
               parser.Found(pname,&lvalue);
               pvalue.Printf(wxT("%i"),lvalue);
            }
            cmdMap.insert(make_pair(pname,pvalue));
         }
      }
   }
}


int main(int argc, char **argv)
{
   // initialise wxWidgets library
   wxInitializer initializer(argc,argv);

   // parse command line
   wxCmdLineParser parser(cmdLineDesc);
   parser.DisableLongOptions();
   parser.SetSwitchChars(wxT("-"));
   parser.SetCmdLine(argc,argv);
   if(parser.Parse() != 0) {
      // command line parameter error
      return 1;
   }

   // command line parser success
   // convert parameters to map
   CmdLineMap cmdMap;
   ParserToMap( parser,cmdMap);

   size_t port        = 7666;
   size_t buffer_size = 1024;


   bool create_config = cmdMap.find("config") != cmdMap.end();
   bool verbose       = cmdMap.find("verbose") != cmdMap.end();
   bool debug         = cmdMap.find("debug") != cmdMap.end();
   bool has_port      = cmdMap.find("port") != cmdMap.end();

   if(has_port) {

      // user specified the UDP port number to listen to
      wxString sport = cmdMap["port"];
      long lport=port;
      if(sport.ToLong(&lport)) {
         port = (int)lport;
      }
   }

   if(verbose) {
      cout << "tvcec listening to UDP port " << port << endl;
   }

   // read config file
   udp_config config;

   // force default config if requested
   if(create_config) {
      config.create_config();
      if(verbose) cout << "tvcec created new configuration. "<< endl;
      return 0;
   }

   // start the UDP server thread

   std::shared_ptr<udp_reader> reader = std::make_shared<udp_reader>(port,buffer_size);
   std::shared_ptr<udp_server> server = std::make_shared<udp_server>(reader);
   std::vector<std::shared_ptr<std::thread>> udp_threads;
   udp_threads.push_back(std::make_shared<std::thread>(&udp_server::run,server));

   // the UDP message queue
   auto msg_queue = server->queue();

   size_t msg_count = 0;

   bool read_udp = true;
   while(read_udp) {

      // empty the queue, get the last message only
      std::string udp_message, try_message;
      while(msg_queue->try_dequeue(try_message)) {
         msg_count++;
         udp_message = try_message;

         // special "exit" command terminates server loop
         if(udp_message =="exit") {
            server->stop();
            udp_threads[0]->join();
            exit(0);
         }
      }

      if(udp_message.length() > 0) {

         // process the message
         if(verbose) cout << endl << " udp "<< msg_count << ": " << udp_message  << std::flush;

         wxString cec_cmd;
         if(config.parse(udp_message,cec_cmd)) {
            // udp message successfully parsed

            std::string system_cmd = "echo \"" + cec_cmd.ToStdString() + "\" | cec-client -s -d 1";
            if(verbose) cout << " ==> " << system_cmd << std::endl;

            // issue the command to control the TV
            if(!debug){

               // receive the return value to avoid warning, even though the value cannot be trusted
               int retval = system(system_cmd.c_str());
            }
         }
         else {
            if(verbose) cout << " ==> Error, not understood" << std::endl;
         }
      }
      else {
         std::this_thread::sleep_for(std::chrono::milliseconds(100));
      }

   }

   server->stop();
   udp_threads[0]->join();

   return 0;
}


