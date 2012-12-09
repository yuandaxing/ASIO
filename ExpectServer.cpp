/*
 * =====================================================================================
 *
 *       Filename:  ExpectServer.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2012年09月22日 14时28分45秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Yuan DaXing (http://goo.gl/VpwzO), mryuan0@gmail.com
 *   Organization:  
 *
 * =====================================================================================
 */
class Server {
	Channel Acceptor;
	EventLoop el;
	std::unoredered_map<std::string, scoped_ptr<Channel *> > clients;


	Server(const std::string &str, int port);
	void onAccept(Channel *ch);
	void onRead(Channel *ch, void *arg);
	void onWrite(Channel *ch, void *arg);
	loop();
}

int main(int argc, char *argv[])
{
	Server server;
	server.loop();
}


