/************************************************************/
/*    NAME: Nick Rypkema                                              */
/*    ORGN: MIT                                             */
/*    FILE: Razor.h                                          */
/*    DATE:                                                 */
/************************************************************/

#ifndef Razor_HEADER
#define Razor_HEADER

#include <boost/asio.hpp>
#include "blocking_reader.h"
#include "MOOS/libMOOS/MOOSLib.h"

class Razor : public CMOOSApp
{
 public:
   Razor();
   ~Razor();

 protected:
   bool OnNewMail(MOOSMSG_LIST &NewMail);
   bool Iterate();
   bool OnConnectToServer();
   bool OnStartUp();
   void RegisterVariables();
   void PublishEulerian(double w, double x, double y, double z);

 private: // Configuration variables
   std::string                                 m_port;
   unsigned int                                m_baud;
   bool                                        m_raw;
   bool                                        m_quaternion;
   bool                                        m_euler;

 private: // State variables
   boost::asio::io_service                     m_io;
   boost::shared_ptr<boost::asio::serial_port> m_serial;
   blocking_reader*                            m_reader;
   unsigned int                                m_iterations;
   double                                      m_timewarp;
};

#endif
