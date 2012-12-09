/*
 * =====================================================================================
 *
 *       Filename:  SimplePingPong.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2012年12月08日 19时04分44秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Yuan DaXing (http://goo.gl/VpwzO), mryuan0@gmail.com
 *   Organization:  
 *
 * =====================================================================================
 */

#include "SimplePingPong.h"
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <tr1/functional>
#include <unistd.h>
#include <assert.h>
#include <fcntl.h>
void PingPongServer::ListeningHandler(Channel *ch) {
	Channel *c = ch->accept();
	c->setOption(Channel::Options::kNoBlocking);
	PingPongClient *ppc = new PingPongClient(c);
	c->setProcess(std::tr1::bind(&PingPongClient::read_handler,
			ppc, &el_, c), 1);	
	c->setProcess(std::tr1::bind(&PingPongClient::write_handler,
			ppc, &el_, c), 2);	
	clients_.push_back(ppc);
	c->regEventLoop(&el_, Channel::EventType::kRead);
}
PingPongServer::PingPongServer(const std::string &addr, int port) :
	address_(addr), port_(port) {
	struct sockaddr_in mysocket;
	mysocket.sin_family = AF_INET;
	mysocket.sin_addr.s_addr = inet_addr(addr.c_str());
	mysocket.sin_port = htons(port);

	listen_ch_.bind(mysocket);
	listen_ch_.listen(64);
	listen_ch_.setOption(Channel::Options::kNoBlocking);
	std::tr1::function<void ()> fun = std::tr1::bind(
			&PingPongServer::ListeningHandler,
			this, &listen_ch_);
	listen_ch_.setProcess(fun, 1);
	listen_ch_.regEventLoop(&el_, Channel::EventType::kRead);
	}
int main(int argc, char *argv[])
{
	int fd = epoll_create(1000);
	assert(fd > 0);
	PingPongServer server("127.0.0.1", 8000); 	
	server.server();
	return 0;
}
