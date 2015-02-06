#ifndef  _EVENTLOOP_H_INC
#define  _EVENTLOOP_H_INC
#include <tr1/unordered_map>
#include <EPoller.h>
#include <Channel.h>
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
