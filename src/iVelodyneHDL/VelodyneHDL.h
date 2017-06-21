/************************************************************/
/*    NAME: Nick Rypkema                                    */
/*    ORGN: MIT                                             */
/*    FILE: VelodyneHDL.h                                   */
/*    DATE: 2017                                            */
/************************************************************/

#ifndef VelodyneHDL_HEADER
#define VelodyneHDL_HEADER

#include "PacketDriver.h"
#include "PacketDecoder.h"
#include "PacketFileWriter.h"
#include "MOOS/libMOOS/App/MOOSApp.h"

class VelodyneHDL : public CMOOSApp
{
 public:
   VelodyneHDL();
   ~VelodyneHDL();

 protected:
   bool OnNewMail(MOOSMSG_LIST &NewMail);
   bool Iterate();
   bool OnConnectToServer();
   bool OnStartUp();
   void RegisterVariables();

 private: // Configuration variables
   bool                     m_decode;
   std::string              m_corrections_file;
   bool                     m_intensity;
   bool                     m_laser_id;
   bool                     m_azimuth;
   bool                     m_distance;
   bool                     m_ms_from_top_of_hour;
   bool                     m_write_pcap;
   std::string              m_write_path;

 private: // State variables
   std::vector<double>      m_frame;
   PacketDriver             m_driver;
   PacketDecoder            m_decoder;
   vtkPacketFileWriter      m_writer;
   std::string*             m_packet;
   unsigned int*            m_packet_length;
   PacketDecoder::HDLFrame  m_latest_frame;
   unsigned int             m_iterations;
   double                   m_timewarp;
   bool                     m_corrections_given;
   std::stringstream        m_variables;
   bool                     m_first_packet;
};

#endif
