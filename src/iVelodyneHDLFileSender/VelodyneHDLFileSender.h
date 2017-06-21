/************************************************************/
/*    NAME: Nick Rypkema                                    */
/*    ORGN: MIT                                             */
/*    FILE: VelodyneHDLFileSender.h                         */
/*    DATE: 2017                                            */
/************************************************************/

#ifndef VelodyneHDLFileSender_HEADER
#define VelodyneHDLFileSender_HEADER

#include "PacketFileReader.h"
#include <string>
#include <cstdlib>
#include <iostream>
#include <boost/thread/thread.hpp>
#include <boost/asio.hpp>
#include "MOOS/libMOOS/MOOSLib.h"

class VelodyneHDLFileSender : public CMOOSApp
{
 public:
   VelodyneHDLFileSender();
   ~VelodyneHDLFileSender();

 protected: // Standard MOOSApp functions to overload
   bool OnNewMail(MOOSMSG_LIST &NewMail);
   bool Iterate();
   bool OnConnectToServer();
   bool OnStartUp();

 protected:
   void RegisterVariables();

 private: // Configuration variables
   std::string                                        m_pcap_file;
   bool                                               m_trigger;
   std::string                                        m_trigger_db;
   double                                             m_trigger_time;
   int                                                m_microsec_sleep;

 private: // State variables
   boost::asio::io_service                            m_io_service;
   boost::shared_ptr<boost::asio::ip::udp::endpoint>  m_destination_endpoint;
   boost::shared_ptr<boost::asio::ip::udp::socket>    m_socket;
   unsigned int                                       m_iterations;
   double                                             m_timewarp;
   vtkPacketFileReader                                m_packet_reader;
   unsigned long                                      m_packet_counter;
   bool                                               m_stream;
   double                                             m_prevSystemTime;
   double                                             m_prevPacketTime;
};

#endif
