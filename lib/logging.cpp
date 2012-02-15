
#include"os/system.h"
#include"logging.h"

#define FILENAME_SYSLOG "syslog"
#define DEEPSOLVER_LOGGER "deepsolver"

static std::string configLogFileName;
static int configLogLevel=LOG_WARNING;

/**\brief Generates string representation of current system time*/
static std::string getCurrentTime()
{
  std::string s;
  time_t t;
  time(&t);
  s=ctime(&t);
  std::string ss;
  for(std::string::size_type i = 0;i < s.length();i++)
    if (s[i] != '\r' && s[i] != '\n')
      ss += s[i];
  return ss;
}

/**\brief Saves the log message in log file or sends it to syslog
 *
 * \param [in] level The error level of the message
 * \param [in] line The error text
 */
static void saveLogLine(int level, const char* line)
{
  assert(line != NULL);
  if (configLogFileName.empty())
    return;
  if (configLogFileName == FILENAME_SYSLOG)
    {
      syslog(level, "%s", line);
      return;
    }
  std::ofstream f(configLogFileName.c_str(), std::ios_base::out | std::ios_base::app);
  if (!f)
    return;
  f << getCurrentTime() << ':';
  if (level <= LOG_CRIT)
    f << "FATAL:";  else
  if (level <= LOG_ERR)
    f << "ERROR:"; else
  if (level <= LOG_WARNING)
    f << "WARNING:"; else
  if (level <= LOG_INFO)
    f << "INFO:"; else
    f << "TRACE:";
  f << line << std::endl;
}

/**\brief Processes constructed log message
 *
 * \param [in] level The error level of the message
 * \param [in] line The message text
 */
static void logLine(int level, const char* line)
{
  if (level > configLogLevel)
    return;
  saveLogLine(level, line);
}

void logMsg(int level, const char* format, ...)
{
  if (!format)
    return;
  va_list args;
  va_start(args, format);
  char buf[4096];
  vsnprintf(buf, sizeof(buf), format, args);
  buf[sizeof(buf)-1]='\0';
  va_end(args);
  std::string str(buf);
  //FIXME:  removeNewLineChars(str);
  logLine(level, str.c_str());
}

void initLogging(const std::string& logFileName, int logLevel)
{
  configLogFileName = logFileName;
  configLogLevel = logLevel;
  if (configLogFileName == FILENAME_SYSLOG)
    openlog(DEEPSOLVER_LOGGER, LOG_PID, LOG_DAEMON);
}
