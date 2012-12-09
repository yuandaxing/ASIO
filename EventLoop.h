/*
 * =====================================================================================
 *
 *       Filename:  EventLoop.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2012年09月22日 14时28分15秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Yuan DaXing (http://goo.gl/VpwzO), mryuan0@gmail.com
 *   Organization:  
 *
 * =====================================================================================
 */

#ifndef  _EVENTLOOP_H_INC
#define  _EVENTLOOP_H_INC
#include <tr1/unordered_map>
#include "EPoller.h"
#include "Channel.h"
class EventLoop {
	public:
		EventLoop();
		~EventLoop();
		void loop();
		void stop();
		bool regChannel(int, Channel *, Channel::EventType e);
		void unregChannel(int, Channel *, Channel::EventType e);
	private:
		std::tr1::unordered_map<int, Channel *> ch_map_;
		EPoller ep_;
		bool run_;
		std::vector<Channel *> vc_;
};
#endif   /* ----- #ifndef _EVENTLOOP_H_INC  ----- */
