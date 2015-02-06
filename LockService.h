#ifndef  _LOCKSERVICE_H_INC
#define  _LOCKSERVICE_H_INC
#include <pthread.h>
#include <time.h>
#include <boost/utility.hpp>

namespace threadSafe{
// init and destroy are used at the same time
class Mutex: private boost::noncopyable {
	private:
		pthread_mutex_t mutex;
	public:
		Mutex() {
			pthread_mutex_init(&mutex, NULL);   
		};
		~Mutex() { pthread_mutex_destroy(&mutex); }
		
		void lock() { pthread_mutex_lock(&mutex); }

		void unlock() { pthread_mutex_unlock(&mutex) ; }
		pthread_mutex_t & getMutex() { return mutex; }
};

class Condition: private boost::noncopyable {
	private:
		Mutex &m;
		pthread_cond_t cond;
	public:
		explicit Condition(Mutex &mutex): m(mutex) {
		   		pthread_cond_init(&cond, NULL);	
			}
		~Condition() { pthread_cond_destroy(&cond); }

		void wait() { pthread_cond_wait(&cond, &(m.getMutex())); } 
		void waitTimeOut(const struct timespec &abstime) { 
			pthread_cond_timedwait(&cond, &(m.getMutex()), &abstime);
		}

		void notify() { pthread_cond_signal(&cond); }
		void notifyAll() { pthread_cond_broadcast(&cond); }

};

class MutexGuard: private boost::noncopyable {
	private:
		Mutex &m;
	public:
		explicit MutexGuard(Mutex &mutex): m(mutex) { m.lock(); }
		~MutexGuard() { m.unlock(); }
};

}
#endif   /* ----- #ifndef _LOCKSERVICE_H_INC  ----- */
