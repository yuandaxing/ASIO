/*
 * =====================================================================================
 *
 *       Filename:  testSocket.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2012年12月11日 09时24分39秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Yuan DaXing (http://goo.gl/VpwzO), mryuan0@gmail.com
 *   Organization:  
 *
 * =====================================================================================
 */
#include "../Socket.h"
#include <iostream>
#include <stdlib.h>
int main() {
	Socket s("127.0.0.1", 8001);
	s.listen(64);
	Socket *t =s.accept();
	std::string addr;
	int port;
	t->getPeerAddr(addr, port);
	std::cout << "addr:" << addr <<" port:" <<  port << std::endl;
	while(true)
		sleep(1);
}
