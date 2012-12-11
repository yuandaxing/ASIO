/*
 * =====================================================================================
 *
 *       Filename:  Logger.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2012年12月10日 16时36分35秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Yuan DaXing (http://goo.gl/VpwzO), mryuan0@gmail.com
 *   Organization:  
 *
 * =====================================================================================
 */
#include "Logger.h"
using namespace threadSafe;
Mutex Logger::m;
bool Logger::initFlag = false;
const char * Logger::LoggerName = NULL;
Logger::LoggerLevel Logger::level = INFO_;
FILE *Logger::fp = stdout;
const char * Logger::levelName[] = {"TRACE", "DEBUG", "INFO",
											"WARN", "ERR"};
