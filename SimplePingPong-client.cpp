/*
 * =====================================================================================
 *
 *       Filename:  SimplePingPong-client.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2012年12月09日 10时02分06秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Yuan DaXing (http://goo.gl/VpwzO), mryuan0@gmail.com
 *   Organization:  
 *
 * =====================================================================================
 */
#include <string>
#include <vector>
#include "Channel.h"
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <iostream>
#include <string.h>
class SimpleClient {
	public:
		SimpleClient(const std::string &addr, int port) {
			struct sockaddr_in mysocket;
			mysocket.sin_family = AF_INET;
			mysocket.sin_addr.s_addr = inet_addr(addr.c_str());
			mysocket.sin_port = htons(port);
			ch_.connect(mysocket);
		}
		void send(const char *buff, int len) {
			int pos = 0;
			while(pos != len) {
				pos += ch_.write(buff, len - pos);
			}
		}
		void resev(std::vector<char> &vc, int len) {
			int pos = vc.size();
			vc.resize(pos + len);
			while(pos != vc.size()) {
				pos += ch_.read(&vc[pos], vc.size() - pos);
			}
		}

	private:
		Channel ch_;
};


int main() {
	SimpleClient sc("127.0.0.1", 8000);
	const char *str = "hello world";
	int len = ::strlen(str);
	while(true) {
		std::vector<char> buff_;
		sc.send(str, len);
		sc.resev(buff_, len);
		buff_.push_back(0);
		std::cout << buff_.data() << std::endl;
		sleep(1);
	}
}
