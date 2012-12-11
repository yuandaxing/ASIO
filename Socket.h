/*
 * =====================================================================================
 *
 *       Filename:  Socket.h
 *
 *    Description:  implement the Unix socket abstraction
 *
 *        Version:  1.0
 *        Created:  2012年12月10日 10时00分55秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Yuan DaXing (http://goo.gl/VpwzO), mryuan0@gmail.com
 *   Organization:  
 *
 * =====================================================================================
 */
#ifndef  _SOCKET_H_INC
#define  _SOCKET_H_INC
#include <string>
#include "Logger.h"
using namespace threadSafe;

class Socket {
	public:
		Socket();
		Socket(const std::string &addr, int port);
		Socket(int fd);

		int read(char *buff_, int size);
		int write(const char *buff_, int size);

		bool listen(int qSize);
		bool connect(const std::string &addr, int port);
		Socket * accept();

		enum Options {
			kNoBlocking = 1, 
			kNoDelay    = 2, 
			kLinger 	= 4,
			kKeepAlive  = 8, 
			kReuse 		= 16
		};
		bool setOptions(int);
		bool setDefaultServerOptions();
		void getPeerAddr(std::string &addr, int &port) {
			addr = peer_addr_;
			port = peer_port_;
		}
		
		static Socket * serverSocket(const std::string &addr, 
				int port, int qSize = 1024) {
			Socket *s = new Socket(addr, port);
			if( s == NULL )
			{
				ERR << "could not new Socket" << std::endl;
			}
			s->setDefaultServerOptions();
			s->listen(qSize);
			return s;
		}

	private:
		int fd_;
		std::string peer_addr_;
		int peer_port_;
		bool valid_;
};
#endif   /* ----- #ifndef _SOCKET_H_INC  ----- */
