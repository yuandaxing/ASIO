/*
 * =====================================================================================
 *
 *       Filename:  EPoller.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2012年09月20日 22时13分17秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Yuan DaXing (http://goo.gl/VpwzO), mryuan0@gmail.com
 *   Organization:  
 *
 * =====================================================================================
 */
#include <unistd.h>
#include <fcntl.h>
#include "EPoller.h"
#include <sys/epoll.h>
#include "Logger.h"
/* 
 * for epoll we use level trigger please see man 7 epoll to see why
 * we just use poll add del interface
 * current, it do not consider portable
 * */
using namespace threadSafe;
EPoller::EPoller(int nfd) : epfd_(::epoll_create1(0)), 
	eventList_(kNFd) { 
		if(epfd_ < 0) {
			ERR << "create epoll fd failed\n";
			exit(0);
		}
}

EPoller::~EPoller() {
	::close(epfd_);
}

int EPoller::poll(int ms, std::vector<Channel *> &v) {
	int nEvent = ::epoll_wait(epfd_, &eventList_[0],
			eventList_.size(), ms);
	if( nEvent <= 0 )
		return 0;

	for( int i = 0 ; i < nEvent ; i++ )
	{
		Channel *ch = static_cast<Channel*>(eventList_[i].data.ptr);
		ch->setWhyWakeup(eventList_[i].events);
		v.push_back(ch);
	}
	//eventList is not enough
	if( static_cast<unsigned>(nEvent) == eventList_.size() )
		eventList_.resize( 2 * nEvent );
	return nEvent;
}

bool EPoller::update(int operation, Channel &ch) {
	struct epoll_event event; 
	
	bzero(&event, sizeof(event));
	event.events = ch.event_;
	event.data.ptr = &ch;

	int fd = ch.fd_;
	if( ::epoll_ctl(epfd_, operation, fd, &event ) < 0)
	{
		if( operation == EPOLL_CTL_DEL)
		{
			ERR << "epoll could not delete\n";
		} else {
			ERR << "could not update event\n";
		}

		return false;
	}

	return true;
}
bool EPoller::add(Channel &ch, enum Channel::EventType e) {
	int operation;
	if(ch.state_ == Channel::kNew || ch.state_ == Channel::kDelete) {
		operation = EPOLL_CTL_ADD;
	} else { 
		operation = EPOLL_CTL_MOD;
	}
	ch.setEvent(e);
	ch.state_ = Channel::kAdded;
	return update(operation, ch);
}

bool EPoller::del(Channel &ch) {
	ch.state_ = Channel::kDelete;
	ch.event_ = 0;
	return update(EPOLL_CTL_DEL, ch);
}

