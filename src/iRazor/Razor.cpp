/************************************************************/
/*    NAME: Nick Rypkema                                              */
/*    ORGN: MIT                                             */
/*    FILE: Razor.cpp                                        */
/*    DATE:                                                 */
/************************************************************/

#include <math.h>
#include <iterator>
#include "MBUtils.h"
#include "Razor.h"

using namespace std;

//---------------------------------------------------------
// Constructor

Razor::Razor()
{
  m_iterations = 0;
  m_timewarp   = 1;

  m_baud = 115200;
  m_raw = true;
  m_quaternion = true;
  m_euler = true;
}

//---------------------------------------------------------
// Destructor

Razor::~Razor()
{
}

//---------------------------------------------------------
// Procedure: OnNewMail

bool Razor::OnNewMail(MOOSMSG_LIST &NewMail)
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

bool Razor::OnConnectToServer()
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

bool Razor::Iterate()
{
  m_iterations++;

  std::string line;
  char c;

  while (m_reader->read_char(c) && c != '\n') {
		line += c;
	}

  std::vector<std::string> line_values = parseString(line, ",");

  Notify("RAZOR_TIME", atof(line_values[0].c_str()));

  if (m_raw) {
    Notify("RAZOR_ACCEL_X", atof(line_values[1].c_str()));
    Notify("RAZOR_ACCEL_Y", atof(line_values[2].c_str()));
    Notify("RAZOR_ACCEL_Z", atof(line_values[3].c_str()));
    Notify("RAZOR_GYRO_X", atof(line_values[4].c_str()));
    Notify("RAZOR_GYRO_Y", atof(line_values[5].c_str()));
    Notify("RAZOR_GYRO_Z", atof(line_values[6].c_str()));
    Notify("RAZOR_MAG_X", atof(line_values[7].c_str()));
    Notify("RAZOR_MAG_Y", atof(line_values[8].c_str()));
    Notify("RAZOR_MAG_Z", atof(line_values[9].c_str()));
  }

  double w, x, y, z;
  w = atof(line_values[10].c_str());
  x = atof(line_values[11].c_str());
  y = atof(line_values[12].c_str());
  z = atof(line_values[13].c_str());
  if (m_quaternion) {
    Notify("RAZOR_W", w);
    Notify("RAZOR_X", x);
    Notify("RAZOR_Y", y);
    Notify("RAZOR_Z", z);
  }

  if (m_euler) {
    PublishEulerian(w, x, y, z);
  }

  return(true);
}

void Razor::PublishEulerian(double w, double x, double y, double z)
{
  double ysqr = y * y;

  // roll (x-axis rotation)
  double t0 = +2.0 * (w * x + y * z);
  double t1 = +1.0 - 2.0 * (x * x + ysqr);
  double roll = atan2(t0, t1);

  // pitch (y-axis rotation)
  double t2 = +2.0 * (w * y - z * x);
  t2 = t2 > 1.0 ? 1.0 : t2;
  t2 = t2 < -1.0 ? -1.0 : t2;
  double pitch = asin(t2);

  // yaw (z-axis rotation)
  double t3 = +2.0 * (w * z + x * y);
  double t4 = +1.0 - 2.0 * (ysqr + z * z);
  double yaw = atan2(t3, t4);

  Notify("RAZOR_ROLL", roll*180/PI);
  Notify("RAZOR_PITCH", pitch*180/PI);
  Notify("RAZOR_YAW", yaw*180/PI);
}

//---------------------------------------------------------
// Procedure: OnStartUp()
//            happens before connection is open

bool Razor::OnStartUp()
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

  SetIterateMode(REGULAR_ITERATE_AND_COMMS_DRIVEN_MAIL);

  if (!m_MissionReader.GetConfigurationParam("PORT", m_port)) {
    cerr << "PORT must be specified! Qutting..." << endl;
    return(false);
  }

  if (!m_MissionReader.GetConfigurationParam("BAUD", m_baud)) {
    cerr << "BAUD not specified! Assuming 115200..." << endl;
  }

  if (!m_MissionReader.GetConfigurationParam("RAW", m_raw)) {
    cerr << "RAW not specified! Assuming true..." << endl;
  }

  if (!m_MissionReader.GetConfigurationParam("QUATERNION", m_quaternion)) {
    cerr << "QUATERNION not specified! Assuming true..." << endl;
  }

  if (!m_MissionReader.GetConfigurationParam("EULER", m_euler)) {
    cerr << "EULER not specified! Assuming true..." << endl;
  }

  m_serial = boost::shared_ptr<boost::asio::serial_port>(new boost::asio::serial_port(m_io, m_port));
  m_serial->set_option(boost::asio::serial_port_base::baud_rate(m_baud));
  m_reader = new blocking_reader(*m_serial, 500);

  RegisterVariables();
  return(true);
}

//---------------------------------------------------------
// Procedure: RegisterVariables

void Razor::RegisterVariables()
{
  // Register("FOOBAR", 0);
}

