/************************************************************/
/*    NAME: Nick Rypkema                                    */
/*    ORGN: MIT                                             */
/*    FILE: VelodyneHDLFileSender.cpp                       */
/*    DATE: 2017                                            */
/************************************************************/

#include <iterator>
#include "MBUtils.h"
#include "PacketDriver.h"
#include "PacketDecoder.h"
#include "VelodyneHDLFileSender.h"

using namespace std;

//---------------------------------------------------------
// Constructor

VelodyneHDLFileSender::VelodyneHDLFileSender()
{
  m_stream = false;
  m_microsec_sleep = 450;
  m_prevSystemTime = 0;
  m_prevPacketTime = 0;

  m_iterations = 0;
  m_timewarp   = 1;
}

//---------------------------------------------------------
// Destructor

VelodyneHDLFileSender::~VelodyneHDLFileSender()
{
}

//---------------------------------------------------------
// Procedure: OnNewMail

bool VelodyneHDLFileSender::OnNewMail(MOOSMSG_LIST &NewMail)
{
  MOOSMSG_LIST::iterator p;

  for(p=NewMail.begin(); p!=NewMail.end(); p++) {
    CMOOSMsg &msg = *p;

    if((p->IsName("DB_TIME")) && (msg.GetSource() == m_trigger_db)) {
      cout << fixed << msg.GetDouble() << " vs. trigger time: " << m_trigger_time << endl;
      if(msg.GetDouble() >= m_trigger_time) {
        m_stream = true;
      }
    }

#if 0 // Keep these around just for template
    string key   = msg.GetKey();
    string comm  = msg.GetCommunity();
    double dval  = msg.GetDouble();
    string sval  = msg.GetString();
    string msrc  = msg.GetSource();
    double mtime = msg.GetTime();
    bool   mdbl  = msg.IsDouble();
    bool   mstr  = msg.IsString();
#endif
   }

   return(true);
}

//---------------------------------------------------------
// Procedure: OnConnectToServer

bool VelodyneHDLFileSender::OnConnectToServer()
{
   // register for variables here
   // possibly look at the mission file?
   // m_MissionReader.GetConfigurationParam("Name", <string>);
   // m_Comms.Register("VARNAME", 0);

   RegisterVariables();
   return(true);
}

//---------------------------------------------------------
// Procedure: Iterate()
//            happens AppTick times per second

bool VelodyneHDLFileSender::Iterate()
{
  m_iterations++;

  if(m_stream) {
    if(m_packet_reader.IsOpen()) {
      const unsigned char* data = 0;
      unsigned int dataLength = 0;
      double timeSinceStart = 0;
      double systemTime = 0;
      double packetTime = 0;
      if(!m_packet_reader.NextPacket(data, dataLength, timeSinceStart)) {
        cout << "End of pcap file. Closing..." << endl;
        m_packet_reader.Close();
        return(true);
      }

      if (dataLength != 1206) {
        return(true);
      }

      size_t bytesSent = m_socket->send_to(boost::asio::buffer(data, dataLength), *m_destination_endpoint);

      if((++m_packet_counter % 500) == 0) {
        cout << "Total sent packets: " << m_packet_counter << endl;
      }

      unsigned char* data2 = const_cast<unsigned char*>(data);
      HDLDataPacket* dataPacket = reinterpret_cast<HDLDataPacket *>(data2);
      packetTime = (dataPacket->gpsTimestamp*1e-6);

      timespec tp;
      clock_gettime(CLOCK_REALTIME,&tp);
      long long timestamp_s = (tp).tv_sec*1e9;
      long long timestamp = (tp).tv_nsec + timestamp_s;
      systemTime = ((double)timestamp)*1e-9;

      if (m_prevPacketTime != 0 && m_prevSystemTime != 0) {
        double packetDiff = packetTime-m_prevPacketTime;
        double systemDiff = systemTime-m_prevSystemTime;
        m_microsec_sleep = m_microsec_sleep - (systemDiff-packetDiff)*1e6;
      }

      m_prevSystemTime = systemTime;
      m_prevPacketTime = packetTime;

      boost::this_thread::sleep(boost::posix_time::microseconds(m_microsec_sleep));
    }
  }

  return(true);
}

//---------------------------------------------------------
// Procedure: OnStartUp()
//            happens before connection is open

bool VelodyneHDLFileSender::OnStartUp()
{
  list<string> sParams;
  m_MissionReader.EnableVerbatimQuoting(false);
  if(m_MissionReader.GetConfiguration(GetAppName(), sParams)) {
    list<string>::iterator p;
    for(p=sParams.begin(); p!=sParams.end(); p++) {
      string original_line = *p;
      string param = stripBlankEnds(toupper(biteString(*p, '=')));
      string value = stripBlankEnds(*p);

      if(param == "FOO") {
        //handled
      }
      else if(param == "BAR") {
        //handled
      }
    }
  }

  m_timewarp = GetMOOSTimeWarp();

  if (!m_MissionReader.GetConfigurationParam("PCAP_FILE", m_pcap_file)) {
    cerr << "PCAP_FILE not specified! Cannot run without one..." << endl;
    return(false);
  }

  if (!m_MissionReader.GetConfigurationParam("TRIGGER", m_trigger)) {
    cerr << "TRIGGER not specified! Assuming false..." << endl;
    m_trigger = false;
  }

  if (m_trigger) {
    if (!m_MissionReader.GetConfigurationParam("TRIGGER_DB", m_trigger_db)) {
      cerr << "TRIGGER_DB not specified! Assuming MOOSDB_#1..." << endl;
      m_trigger_db = "MOOSDB_#1";
    }

    if (!m_MissionReader.GetConfigurationParam("TRIGGER_TIME", m_trigger_time)) {
      cerr << "TRIGGER_TIME not specified! Assuming 0..." << endl;
      m_trigger_time = 0;
    }
  } else {
    m_stream = true;
  }

  m_packet_reader.Open(m_pcap_file);
  if (!m_packet_reader.IsOpen()) {
    cerr << "Failed to open pcap file: " << m_pcap_file << ". Exiting..." << endl;
    return(false);
  }

  try {
    std::string destinationIp = "127.0.0.1";
    int dataPort = 2368;

    m_destination_endpoint = boost::shared_ptr<boost::asio::ip::udp::endpoint>(new boost::asio::ip::udp::endpoint(boost::asio::ip::address_v4::from_string(destinationIp), dataPort));
    m_socket = boost::shared_ptr<boost::asio::ip::udp::socket>(new boost::asio::ip::udp::socket(m_io_service));
    m_socket->open(m_destination_endpoint->protocol());

    m_packet_counter = 0;
  } catch(std::exception & e) {
    cerr << "Exception trying to open socket: " << e.what() << ". Exiting..." << endl;
    return(false);
  }

  RegisterVariables();
  return(true);
}

//---------------------------------------------------------
// Procedure: RegisterVariables

void VelodyneHDLFileSender::RegisterVariables()
{
  // Register("FOOBAR", 0);
  Register("DB_TIME", 0);
}

