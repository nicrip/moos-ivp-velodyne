/************************************************************/
/*    NAME: Nick Rypkema                                    */
/*    ORGN: MIT                                             */
/*    FILE: VelodyneHDLDecoder.h                            */
/*    DATE: 2017                                            */
/************************************************************/

#ifndef VelodyneHDLDecoder_HEADER
#define VelodyneHDLDecoder_HEADER

#include "PacketDriver.h"
#include "PacketDecoder.h"
#include "MOOS/libMOOS/App/MOOSApp.h"

class VelodyneHDLDecoder : public CMOOSApp
{
 public:
   VelodyneHDLDecoder();
   ~VelodyneHDLDecoder();

 protected: // Standard MOOSApp functions to overload
   bool OnNewMail(MOOSMSG_LIST &NewMail);
   bool Iterate();
   bool OnConnectToServer();
   bool OnStartUp();

 protected:
   void RegisterVariables();

 private: // Configuration variables
   std::string              m_corrections_file;
   bool                     m_intensity;
   bool                     m_laser_id;
   bool                     m_azimuth;
   bool                     m_distance;
   bool                     m_ms_from_top_of_hour;

 private: // State variables
   std::vector<double>      m_frame;
   PacketDecoder            m_decoder;
   std::string*             m_packet;
   unsigned int*            m_packet_length;
   PacketDecoder::HDLFrame  m_latest_frame;
   unsigned int             m_iterations;
   double                   m_timewarp;
   bool                     m_corrections_given;
   std::stringstream        m_variables;
};

#endif
