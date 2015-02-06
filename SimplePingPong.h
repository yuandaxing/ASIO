#include <string>
#include <vector>
#include <Channel.h>
#include <EventLoop.h>
#include <algorithm>
#include <iostream>
class PingPongClient {
	public:
		PingPongClient(Channel *ch) : ch_(ch) {
		}
		~PingPongClient() { delete ch_; }
		void read_handler(EventLoop *el, Channel *);
		void write_handler(EventLoop *el, Channel *);
		void err_handler(EventLoop *el, Channel *);
	private:
		std::vector<char> buffer_;
		Channel *ch_;
};

void PingPongClient::read_handler(EventLoop *el, Channel *ch) {
	unsigned pos = buffer_.size();
	//std::cout << "before read the buffer size is " << buffer_.size() << std::endl;
	unsigned rbyte = 0;
	while(rbyte != 1024) {
		buffer_.resize(pos + 1024);
		rbyte = ch->read(buffer_.data() + pos, 1024);
		if(rbyte == -1) {
			buffer_.resize(pos);
			break;
		}
		pos += rbyte;
		buffer_.resize(pos);
	}
	//std::cout << "we read:";
	//for( unsigned i = 0 ; i < buffer_.size() ; i++ )
	//{
	//	std::cout << buffer_[i];
	//}
	//std::cout << std::endl;
	//
	//std::cout << "after read the buffer size is " << buffer_.size() << std::endl;

	//currently we just remove the channle
	ch_->unregEventLoop(el, Channel::EventType::kRead);
	ch_->regEventLoop(el, Channel::EventType::kWrite);
}	

void PingPongClient::write_handler(EventLoop *el, Channel *ch) {
	//std::cout << "before write the buffer is " << buffer_.size() << std::endl;
	while(buffer_.size() > 0) {
		unsigned wbyte = ch->write(buffer_.data(), buffer_.size());
		if( wbyte > 0 )
		{
			std::copy(buffer_.begin() + wbyte, buffer_.end(),
					buffer_.begin());
			buffer_.resize(buffer_.size() - wbyte);
		} else if( wbyte == 0 )
		{
			//error handling
			break;
		} else {
			break;
		}
	}

	//std::cout << "after write the buffer is " << buffer_.size() << std::endl;

	if( buffer_.size() == 0 )
	{
		ch_->unregEventLoop(el, Channel::EventType::kWrite);
		ch_->regEventLoop(el, Channel::EventType::kRead);	
	}
}

class PingPongServer {
	public:
		PingPongServer(const std::string &address, int port);
		void ListeningHandler(Channel *);
		void server() {
			el_.loop();
		}

	private:
		std::vector<PingPongClient *> clients_;
		Channel listen_ch_;
		EventLoop el_;
		std::string address_;
		int port_;
};

