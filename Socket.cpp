/*
 * =====================================================================================
 *
 *       Filename:  Socket.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2012年12月10日 15时59分44秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Yuan DaXing (http://goo.gl/VpwzO), mryuan0@gmail.com
 *   Organization:  
 *
 * =====================================================================================
 */
#include "Socket.h"
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <fcntl.h>
#include <arpa/inet.h>
#include <assert.h>
#include <netinet/tcp.h>

Socket::Socket() {
	fd_ = socket(AF_INET,SOCK_STREAM 
			| SOCK_CLOEXEC, IPPROTO_TCP);
}

Socket::Socket(const std::string &addr, int port) {
	fd_ = socket(AF_INET,SOCK_STREAM 
			| SOCK_CLOEXEC, IPPROTO_TCP);
	struct sockaddr_in mysocket;
	mysocket.sin_family = AF_INET;
	mysocket.sin_addr.s_addr = inet_addr(addr.c_str());
	mysocket.sin_port = htons(port);
	int ret = ::bind(fd_, 
			(const struct sockaddr*)(&mysocket), 
			sizeof(mysocket));
	assert(ret == 0);
}
Socket::Socket(int fd) :
	fd_(fd) {
	}	

int Socket::read(char *buff, int size) {
	return ::read(fd_, buff, size);
}
int Socket::write(const char *buff, int size) {
	return ::write(fd_, buff, size);
}
bool Socket::listen(int qSize) {
	return ::listen(fd_, qSize);
}

bool Socket::connect(const std::string &addr, 
		int port) {
	struct sockaddr_in mysocket;
	mysocket.sin_family = AF_INET;
	mysocket.sin_addr.s_addr = inet_addr(addr.c_str());
	mysocket.sin_port = htons(port);
	peer_addr_ = addr;
	peer_port_ = port;
	return ::connect(fd_, 
			(const struct sockaddr*)(&mysocket),
			sizeof(mysocket)) == 0;
}

Socket* Socket::accept() {
	struct sockaddr_in addr;
	socklen_t len;
	int newfd = ::accept(fd_, (struct sockaddr *)(&addr)
			, &len);	
	if(newfd <= 0)
		return NULL;
	Socket *news = new Socket(newfd);	
	news->peer_addr_ = std::string(::inet_ntoa(addr.sin_addr));
	news->peer_port_ = ::ntohs(addr.sin_port);
	return news;
}

bool Socket::setOptions(int options) {
	int flags = 0;
	int ret = 0;
	if( options & kNoBlocking ) {
		flags = fcntl(fd_, F_GETFL, 0);
		ret += fcntl(fd_, F_SETFL, flags | O_NONBLOCK);
	}

	flags = 1;
	if( options & kKeepAlive )
		ret += ::setsockopt(fd_, SOL_SOCKET, 
				SO_KEEPALIVE, &flags, sizeof(flags));

	if( options & kNoDelay )
		ret += ::setsockopt(fd_, IPPROTO_TCP,
			   	TCP_NODELAY, &flags, sizeof(flags));

	if( options & kLinger ) {
		struct linger ling = {0, 0};
		ret += ::setsockopt(fd_, SOL_SOCKET, 
				SO_LINGER, &ling, sizeof(ling));
	}

	if( options & kReuse )
		ret += ::setsockopt(fd_, SOL_SOCKET,
				SO_REUSEADDR, &flags, sizeof(flags));

	return ret == 0;
}

bool Socket::setDefaultServerOptions() {
	return setOptions( kNoBlocking | kNoDelay |
			kLinger | kKeepAlive | kReuse );
}

