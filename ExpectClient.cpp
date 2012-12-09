/*
 * =====================================================================================
 *
 *       Filename:  ExpectClient.cpp
 *
 *    Description:  i want to implement client like this
 *
 *        Version:  1.0
 *        Created:  2012年09月22日 14时44分46秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Yuan DaXing (http://goo.gl/VpwzO), mryuan0@gmail.com
 *   Organization:  
 *
 * =====================================================================================
 */

class Client {
	EventLoop el;
	Channle  ch;
	connect(const std::string &se, int port);
	onRead(channle &ch, void *arg);
	onWrite(channle &ch, void *arg);
}

int main(int argc, char *argv[])
{
	Client c;
	c.connect();
	client.loop();
}
