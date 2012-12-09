/*
 * =====================================================================================
 *
 *       Filename:  EventLoop.c
 *
 *    Description: implement eventloop 
 *
 *        Version:  1.0
 *        Created:  2012年12月07日 22时06分34秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Yuan DaXing (http://goo.gl/VpwzO), mryuan0@gmail.com
 *   Organization:  
 *
 * =====================================================================================
 */

#include "EventLoop.h"

EventLoop::EventLoop () : run_(true) { }
EventLoop::~EventLoop() { }

void EventLoop::loop () 
{
	while(run_) {
		vc_.clear();
		int nEvent = ep_.poll(1000, vc_);
		for( int i = 0 ; i < nEvent ; i++ )
		{
			vc_[i]->doProcess();
		}
	}
}

void EventLoop::stop()
{
	run_ = false;
}

bool EventLoop::regChannel(int fd, Channel *ch, Channel::EventType e)
{
	ch_map_[fd] = ch;
	return ep_.add(*ch, e);
}

void EventLoop::unregChannel(int fd, Channel *ch, Channel::EventType e) {
	auto iter = ch_map_.find(fd);
	if( iter != ch_map_.end() )
	{
		ch_map_.erase(iter);
	}
	ep_.del(*ch);
}
