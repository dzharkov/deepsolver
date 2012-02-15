
#ifndef DEEPSOLVER_LOGGING_H
#define DEEPSOLVER_LOGGING_H

/**\brief Prepares log output
 *
 * This function makes all operations required to prepare logging
 * output. It can be called only once. The logging level can be specified
 * in syslog constants as they used in Linux applications. The name of a
 * file to save log messages in can be "syslog" and it means to translate
 * all messages to usual syslog mechanism. Also it can be empty and in
 * this case messages will not be saved at all.
 * 
 * \param [in] logFileName The name of a file to save logging information in
 * \param [in] logLevel The level to filter logging information
 */
void initLogging(const std::string& logFileName, int logLevel); 

/**\brief Processes log message
 *
 * This is the main function to make single log message. The message text
 * can be specified as printf() formatted string with consequent
 * parameters. Log level value can be provided as syslog level constants.
 *
 * \param [in] level The error level for this message
 * \param [in] format The message text
 */
void logMsg(int level, const char* format,... );

#endif //DEEPSOLVER_LOGGING_H;
