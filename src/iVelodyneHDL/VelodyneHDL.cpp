/************************************************************/
/*    NAME: Nick Rypkema                                    */
/*    ORGN: MIT                                             */
/*    FILE: VelodyneHDL.cpp                                 */
/*    DATE: 2017                                            */
/************************************************************/

#include <iterator>
#include "MBUtils.h"
#include "VelodyneHDL.h"

using namespace std;

template <typename T>
std::vector<T> operator+(const std::vector<T> &A, const std::vector<T> &B)
{
    std::vector<T> AB;
    AB.reserve( A.size() + B.size() );                // preallocate memory
    AB.insert( AB.end(), A.begin(), A.end() );        // add A;
    AB.insert( AB.end(), B.begin(), B.end() );        // add B;
    return AB;
}

template <typename T>
std::vector<T> &operator+=(std::vector<T> &A, const std::vector<T> &B)
{
    A.reserve( A.size() + B.size() );                // preallocate memory without erase original data
    A.insert( A.end(), B.begin(), B.end() );         // add B;
    return A;                                        // here A could be named AB
}

template <typename T, typename T2>
std::vector<T> &operator+=(std::vector<T> &A, const std::vector<T2> &B)
{
    A.reserve( A.size() + B.size() );                // preallocate memory without erase original data
    A.insert( A.end(), B.begin(), B.end() );         // add B;
    return A;                                        // here A could be named AB
}

//---------------------------------------------------------
// Constructor

VelodyneHDL::VelodyneHDL()
{
  m_iterations = 0;
  m_timewarp   = 1;

  m_packet = new std::string();
  m_packet_length = new unsigned int();
  m_variables.str("");
  m_variables.clear();
  m_driver.InitPacketDriver(DATA_PORT);
  m_first_packet = true;
}

//---------------------------------------------------------
// Destructor

VelodyneHDL::~VelodyneHDL()
{
}

//---------------------------------------------------------
// Procedure: OnNewMail

bool VelodyneHDL::OnNewMail(MOOSMSG_LIST &NewMail)
{
  MOOSMSG_LIST::iterator p;

  for(p=NewMail.begin(); p!=NewMail.end(); p++) {
    CMOOSMsg &msg = *p;

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

bool VelodyneHDL::OnConnectToServer()
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

bool VelodyneHDL::Iterate()
{
  m_iterations++;

  m_driver.GetPacket(m_packet, m_packet_length);  //blocks until packet received
  if (m_write_pcap) {
    m_writer.WritePacket(reinterpret_cast<const unsigned char*>(m_packet->c_str()), m_packet->length());
  }
  if (m_first_packet) {
    Notify("VELODYNE_FIRST_PACKET_TIME", MOOSTime());
    m_first_packet = false;
  }

  if (m_bundle) {
    m_bundler.BundlePacket(m_packet, m_packet_length);
    if (m_bundler.GetLatestBundle(&m_latest_bundle, &m_latest_bundle_length)) {
      Notify("VELODYNE_PACKET_BUNDLE", (unsigned char*) m_latest_bundle.c_str(), m_latest_bundle_length);
    }
  } else {
    Notify("VELODYNE_PACKET", (unsigned char*) m_packet->c_str(), *m_packet_length);
  }

  if (m_decode) {
    m_decoder.DecodePacket(m_packet, m_packet_length);
    if (m_decoder.GetLatestFrame(&m_latest_frame)) {
      m_frame = m_latest_frame.x + m_latest_frame.y + m_latest_frame.z;
      if (m_intensity) m_frame += m_latest_frame.intensity;
      if (m_laser_id) m_frame += m_latest_frame.laser_id;
      if (m_azimuth) m_frame += m_latest_frame.azimuth;
      if (m_distance) m_frame += m_latest_frame.distance;
      if (m_ms_from_top_of_hour) m_frame += m_latest_frame.ms_from_top_of_hour;
      m_variables << "intensity=" << m_intensity << ",laser_id=" << m_laser_id << ",azimuth=" << m_azimuth << ",distance=" << m_distance << ",ms_from_top_of_hour=" << m_ms_from_top_of_hour;
      Notify("VELODYNE_FRAME_VARIABLES", m_variables.str());
      Notify("VELODYNE_FRAME", (unsigned char*) m_frame.data(), (unsigned int) (m_frame.size()*sizeof(double)));
      Notify("VELODYNE_NUM_POINTS", m_latest_frame.x.size());
      m_variables.str("");
      m_variables.clear();
      m_frame.clear();
    }
  }

  return(true);
}

//---------------------------------------------------------
// Procedure: OnStartUp()
//            happens before connection is open

bool VelodyneHDL::OnStartUp()
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

  SetIterateMode(REGULAR_ITERATE_AND_COMMS_DRIVEN_MAIL);  //must use aync (V10) comms, otherwise we can't publish packets quickly enough

  if (!m_MissionReader.GetConfigurationParam("BUNDLE", m_bundle)) {
    cerr << "BUNDLE not specified! Assuming True..." << endl;
    m_bundle = true;
  }

  if (!m_MissionReader.GetConfigurationParam("DECODE", m_decode)) {
    cerr << "DECODE not specified! Assuming True..." << endl;
    m_decode = true;
  }

  if (!m_MissionReader.GetConfigurationParam("CORRECTIONS_FILE", m_corrections_file)) {
    cerr << "CORRECTIONS_FILE not specified! Assuming none..." << endl;
    m_corrections_given = false;
  } else {
    m_corrections_given = true;
  }

  if (m_corrections_given) {
    m_decoder.SetCorrectionsFile(m_corrections_file);
  }

  if (!m_MissionReader.GetConfigurationParam("INTENSITY", m_intensity)) {
    cerr << "INTENSITY not specified! Assuming False..." << endl;
    m_intensity = false;
  }

  if (!m_MissionReader.GetConfigurationParam("LASER_ID", m_laser_id)) {
    cerr << "LASER_ID not specified! Assuming False..." << endl;
    m_laser_id = false;
  }

  if (!m_MissionReader.GetConfigurationParam("AZIMUTH", m_azimuth)) {
    cerr << "AZIMUTH not specified! Assuming False..." << endl;
    m_azimuth = false;
  }

  if (!m_MissionReader.GetConfigurationParam("DISTANCE", m_distance)) {
    cerr << "DISTANCE not specified! Assuming False..." << endl;
    m_distance = false;
  }

  if (!m_MissionReader.GetConfigurationParam("MS_FROM_TOP_OF_HOUR", m_ms_from_top_of_hour)) {
    cerr << "MS_FROM_TOP_OF_HOUR not specified! Assuming False..." << endl;
    m_ms_from_top_of_hour = false;
  }

  if (!m_MissionReader.GetConfigurationParam("WRITE_PCAP", m_write_pcap)) {
    cerr << "WRITE_PCAP not specified! Assuming False..." << endl;
    m_write_pcap = false;
  }

  if (m_write_pcap) {
    if (!m_MissionReader.GetConfigurationParam("WRITE_PATH", m_write_path)) {
      cerr << "WRITE_PATH not specified! Assuming current directory..." << endl;
      m_write_path = "./";
    }

    stringstream filepath;
    timespec tp;
    clock_gettime(CLOCK_REALTIME,&tp);
    time_t timestamp=(tp).tv_sec;
    filepath << m_write_path << timestamp << ".pcap";
    if (!m_writer.Open(filepath.str())) {
      cerr << "Could not open pcap file! Are you sure the directory exists?" << endl;
    } else {
      cout << "Successfully initialized pcap file!" << endl;
    }
  }

  RegisterVariables();
  return(true);
}

//---------------------------------------------------------
// Procedure: RegisterVariables

void VelodyneHDL::RegisterVariables()
{
  // Register("FOOBAR", 0);
}

