/* 
 * currently Channel do not support timeout
 * */

#ifndef  MESSAGER_CHANNEL_H__INC
#define  MESSAGER_CHANNEL_H__INC
#include <string>
#include <sys/epoll.h>
#include <tr1/functional>
class EventLoop;
class Channel {
	public:
		friend class EPoller;
		enum Options {
			kNoBlocking = 1, 
			kNoDelay    = 2, 
			kLinger 	= 4,
			kKeepAlive  = 8, 
			kReuse 		= 16
		};
		enum State {
		   kNew 		= 1, 
		   kAdded 		= 2, 
		   kDelete 		= 4
		};	   
		enum EventType {
			kRead 	= EPOLLPRI|EPOLLIN,
			kWrite 	= EPOLLOUT
		};
		Channel( );
		Channel(int fd);
		~Channel( );
		int read(char *buf, int size);
		int write(const char *buf, int size);
		bool connect(const struct sockaddr_in &);
		Channel * accept();
		bool bind(const struct sockaddr_in &);
		bool listen(int qsize);
		void setOption(int options);
		void close();
		bool Valid();
		bool readable();
		bool writeable();

		bool regEventLoop(EventLoop *el, EventType e);
		void unregEventLoop(EventLoop *el, EventType e);

		bool error();
		void doProcess();
		void setProcess(std::tr1::function<void ()> p,
				int type) {
			if(type == 1)
				readProcess_ = p;
			if(type == 2)
				writeProcess_ = p;
		}
	private:
		//no copy
		Channel(const Channel&);
		Channel & operator=(const Channel&);

		void setEvent(EventType e) {
			event_ = e;
		}

		void setWhyWakeup(int w) { whyWakeup_ = w; }
		int fd_;
		int event_;
		int whyWakeup_;
		enum State state_;
		bool Valid_;
		std::tr1::function<void ()> readProcess_, writeProcess_;
};
#endif   /* ----- #ifndef MESSAGER_CHANNEL_H__INC  ----- */
