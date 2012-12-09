/*
 * =====================================================================================
 *
 *       Filename:  Logger.h
 *
 *    Description:  first we need to init Logger, the Logger filename, the 
 *    Logger level, then we can just use it like, DEBUG << "test", INFO << "Test"
 *    I use fprintf function to ensure thread safe, and others use write raw fucntion.
 *
 *        Version:  1.0
 *        Created:  2012年05月21日 07时32分20秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Yuan DaXing (http://goo.gl/VpwzO), mryuan0@gmail.com
 *   Organization:  
 *
 * =====================================================================================
 */
#ifndef  _LOGGER_H__INC
#define  _LOGGER_H__INC

#include "LockService.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <sstream>
#include <errno.h>
namespace threadSafe{
class Logger{
	public:
		enum LoggerLevel{
			TRACE_ = 0,
			DEBUG_, 
			INFO_ ,
			WARN_,
			ERR_
		};

		static void init(enum LoggerLevel le = INFO_, 
				const char *name = NULL) {
			MutexGuard mg(m);

			if(initFlag)
				return ;
			initFlag = true;
			LoggerName = name;
			level = le;
			if(LoggerName != NULL) {
				fp = fopen(LoggerName, "a+");
				if(fp == NULL) {
					fprintf(stderr, "cannot create logger file\n");
					::exit(0);
				}
				::atexit(closeFile);
			} else {
				fp = stdout;
			}
		}

		static void flush() {
			if(fp != NULL)
				fflush(fp);
		}

		Logger(LoggerLevel level, const char *fileName_, const char *funName_, int lineNo_):
			cur(level), fileName(fileName_), funName(funName_), lineNo(lineNo_), errNum(errno) {
				char tbuf[30];
				struct tm ti;
				time_t t;

				time(&t);
				localtime_r(&t, &ti);
				strftime(tbuf, sizeof(tbuf), "%x %X ", &ti);
				logPiece << tbuf;

				//	sprintf(buf, " (%s) %s %s %d", levelName[cur], fileName, funName, lineNo);

				logPiece << "(" << levelName[cur] << ") ";
				if(fileName != NULL)
					logPiece << fileName << " ";
				if(funName != NULL)
					logPiece << funName << " ";
				logPiece << lineNo << " ";
				/*-----------------------------------------------------------------------------
				 *i need to implement a thread safe error detection, and  the errno is shared 
				 *between thread
				 *-----------------------------------------------------------------------------*/
				if(errNum != 0) {
					char buf[100];
					strerror_r(errNum, buf, sizeof(buf));
					logPiece << buf << " ";
				}
			}

		~Logger() {
			if(cur >= level) {
				fprintf(fp, "%s", logPiece.str().c_str()); 
			}
		}

		std::ostringstream & getStream() {
			return logPiece;
		}

	private:
		std::ostringstream logPiece;
		LoggerLevel cur;
		const char *fileName;
		const char *funName;
		int lineNo;
		int errNum;
		/*-----------------------------------------------------------------------------
		 *  the block below implement the Logger static part
		 *-----------------------------------------------------------------------------*/
	private:
		static void closeFile(void) {
			if(LoggerName != NULL)
				fclose(fp);
		}

	private:
		static Mutex m;
		static bool initFlag;
		static const char *LoggerName;
		static LoggerLevel level;
		static FILE *fp;
		static const char *levelName[]; 
};

Mutex Logger::m;
bool Logger::initFlag = false;
const char * Logger::LoggerName = NULL;
Logger::LoggerLevel Logger::level = INFO_;
FILE *Logger::fp = stdout;
const char * Logger::levelName[] = {"TRACE", "DEBUG", "INFO",
											"WARN", "ERR"};
#define TRACE 	threadSafe::Logger(Logger::LoggerLevel::TRACE_, __FILE__, __func__, __LINE__).getStream()
#define DEBUG 	threadSafe::Logger(Logger::LoggerLevel::DEBUG_, __FILE__, __func__, __LINE__).getStream()
#define INFO 	threadSafe::Logger(Logger::LoggerLevel::INFO_, __FILE__, __func__, __LINE__).getStream()
#define WARN 	threadSafe::Logger(Logger::LoggerLevel::WARN_, __FILE__, __func__, __LINE__).getStream()
#define ERR 	threadSafe::Logger(Logger::LoggerLevel::ERR_, __FILE__, __func__, __LINE__).getStream()
}
#endif   /* ----- #ifndef _LOGGER_H__INC  ----- */
