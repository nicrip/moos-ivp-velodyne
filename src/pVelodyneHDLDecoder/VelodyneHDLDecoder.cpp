/************************************************************/
/*    NAME: Nick Rypkema                                    */
/*    ORGN: MIT                                             */
/*    FILE: VelodyneHDLDecoder.cpp                          */
/*    DATE: 2017                                            */
/************************************************************/

#include <iterator>
#include "MBUtils.h"
#include "VelodyneHDLDecoder.h"

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

VelodyneHDLDecoder::VelodyneHDLDecoder()
{
  m_iterations = 0;
  m_timewarp   = 1;

  m_packet = new std::string();
  m_packet_length = new unsigned int();
  m_latest_bundle = new std::string();
  m_latest_bundle_length = new unsigned int();
  m_variables.str("");
  m_variables.clear();
}

//---------------------------------------------------------
// Destructor

VelodyneHDLDecoder::~VelodyneHDLDecoder()
{
}

//---------------------------------------------------------
// Procedure: OnNewMail

bool VelodyneHDLDecoder::OnNewMail(MOOSMSG_LIST &NewMail)
{
  MOOSMSG_LIST::iterator p;

  for(p=NewMail.begin(); p!=NewMail.end(); p++) {
    CMOOSMsg &msg = *p;

    if(p->IsName("VELODYNE_PACKET")) {
      m_packet = new string((char*)p->GetBinaryData(), p->GetBinaryDataSize());
      *m_packet_length = p->GetBinaryDataSize();

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
      delete m_packet;
    }

    if(p->IsName("VELODYNE_PACKET_BUNDLE")) {
      m_latest_bundle = new string((char*)p->GetBinaryData(), p->GetBinaryDataSize());
      *m_latest_bundle_length = p->GetBinaryDataSize();

      m_bundle_decoder.DecodeBundle(m_latest_bundle, m_latest_bundle_length);
      if (m_bundle_decoder.GetLatestFrame(&m_latest_frame_b)) {
        m_frame = m_latest_frame_b.x + m_latest_frame_b.y + m_latest_frame_b.z;
        if (m_intensity) m_frame += m_latest_frame_b.intensity;
        if (m_laser_id) m_frame += m_latest_frame_b.laser_id;
        if (m_azimuth) m_frame += m_latest_frame_b.azimuth;
        if (m_distance) m_frame += m_latest_frame_b.distance;
        if (m_ms_from_top_of_hour) m_frame += m_latest_frame_b.ms_from_top_of_hour;
        m_variables << "intensity=" << m_intensity << ",laser_id=" << m_laser_id << ",azimuth=" << m_azimuth << ",distance=" << m_distance << ",ms_from_top_of_hour=" << m_ms_from_top_of_hour;
        Notify("VELODYNE_FRAME_VARIABLES", m_variables.str());
        Notify("VELODYNE_FRAME", (unsigned char*) m_frame.data(), (unsigned int) (m_frame.size()*sizeof(double)));
        Notify("VELODYNE_NUM_POINTS", m_latest_frame_b.x.size());
        m_variables.str("");
        m_variables.clear();
        m_frame.clear();
      }
      delete m_latest_bundle;
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

bool VelodyneHDLDecoder::OnConnectToServer()
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

bool VelodyneHDLDecoder::Iterate()
{
  m_iterations++;
  return(true);
}

//---------------------------------------------------------
// Procedure: OnStartUp()
//            happens before connection is open

bool VelodyneHDLDecoder::OnStartUp()
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

  SetIterateMode(REGULAR_ITERATE_AND_COMMS_DRIVEN_MAIL);  //must use aync (V10) comms, otherwise we can't read packets quickly enough

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

  RegisterVariables();
  return(true);
}

//---------------------------------------------------------
// Procedure: RegisterVariables

void VelodyneHDLDecoder::RegisterVariables()
{
  // Register("FOOBAR", 0);
  Register("VELODYNE_PACKET", 0);
  Register("VELODYNE_PACKET_BUNDLE", 0);
}

