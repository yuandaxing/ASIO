/*
 * =====================================================================================
 *
 *       Filename:  EPoller.h
 *
 *    Description:  to impelement EPoller
 *
 *        Version:  1.0
 *        Created:  2012年09月20日 20时58分37秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Yuan DaXing (http://goo.gl/VpwzO), mryuan0@gmail.com
 *   Organization:  
 *
 * =====================================================================================
 */

#ifndef  MESSENGER_EPOLLER_H__INC
#define  MESSENGER_EPOLLER_H__INC
#include "Channel.h"
#include <vector>
#include <sys/epoll.h>
class EPoller {
	public:
		enum {
			kNFd = 512
		};

		EPoller(int nfd = kNFd);
		~EPoller();
		int poll(int ms, std::vector<Channel *> &v);
		bool add(Channel &, enum Channel::EventType);
		bool del(Channel &);
	private:
		//no copy
		EPoller(const EPoller &);
		EPoller & operator=(const EPoller &);
		bool update(int operation, Channel &ch);
		int epfd_;
		std::vector<struct epoll_event> eventList_;
};
#endif   /* ----- #ifndef MESSENGER_EPOLLER_H__INC  ----- */
