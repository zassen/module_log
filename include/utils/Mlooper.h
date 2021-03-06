#ifndef UTILS_MLOOPER_H
#define UTILS_MLOOPER_H

#include <Debug.h>
#include <utils/KeyedVector.h>
#include <utils/Timers.h>
#include <string>
#include <sys/epoll.h>
#include <utils/Mutex.h>
using namespace std;
namespace mlib{


typedef int (*Mlooper_callbackFunc)(int fd, int events, void *data);

struct Message{

	Message():mWhat(0){}
	Message(int what):mWhat(what) {}
	Message(string str):mStr(str){}
	void setData(const void *data,int len){
		char *pData = NULL;
		pData = (char *)data;
		memset(&mData,0,128);
		if(data != NULL){

			memcpy(&mData,pData,len);
			mDataSize = len;
		}
	}
	void setSource(string name){
		mSource = name;
	}
	void setWhat(int what){
		mWhat = what;
	}
	int mWhat;
	int argc[3];
	string mStr;
	char mData[128];
	int mDataSize;
	string mSource;

	

};

typedef void *hm(const Message &message);

class MessageHandler  {
protected:
    virtual ~MessageHandler() { }

public:
    /**
     * Handles a message.
     */
    virtual void handleMessage(const Message &message) = 0;
};

class MlooperEventCallback {
	protected:
		virtual ~MlooperEventCallback(){}
	public:
		virtual int handleEvent(int fd, int ident, int events, void* data) = 0;
};
   
class MlooperTimeoutHandler{
	public:
		virtual void timeoutHandler(){
			TRACE("Mlooper timeout handler");
		};
		virtual ~MlooperTimeoutHandler(){}

};
/*
int MessageTest(const Message &msg)
{


	DEBUG("msg.what=%d",msg.mWhat);

	

}
*/

class Mlooper{

	protected:
		virtual ~Mlooper();
	public:
		enum{
			POLL_WAKE = -1,

			POLL_CALLBACK = -2,

			POLL_TIMEOUT = -3,

			POLL_ERROR = -4,
		};
		enum{
			EVENT_INPUT = 1 << 0 ,
			EVENT_OUTPUT = 1 << 1 ,
			EVENT_ERROR = 1 << 2 ,
			EVENT_HANGUP = 1 << 3 ,
			EVENT_INVALID = 1 << 4 , 
		};
		Mlooper();
		int pollOnce(int timeoutMillis, int* outFd, int* outEvents, void** outData);
		int timeoutHandlerOnOff(bool value){
			mEnableTimeoutHandler = value;
		}

		void wake();
		int removeFd(int fd);
		bool isIdling() const;

		void setName(string name);
		static Mlooper* prepare();

		void sendMessage(MessageHandler* const &handler, const Message& message); 

		void sendMessageAtTime(nsecs_t uptime, MessageHandler* const &handler, const Message& message);
		static void bindThread(const Mlooper* mlooper);

		static Mlooper* getMlooperFromThread();

		int addFd(int fd, int ident, int events,  MlooperEventCallback* const &evnetCallback, void* data);
		int pollOnce(int timeoutMillis);
		void setTimeoutHandler(MlooperTimeoutHandler *handler){
			mTimeout = handler;
		}
	private:
		string mOwner;
		bool mEnableTimeoutHandler;
		MlooperTimeoutHandler *mTimeout;
		struct Request{

			int fd;
			int ident;
			MlooperEventCallback* eventCallback;
			void* data;
		};

		struct Response{
			int events;
			Request request;

		};
		
		struct MessageEnvelope{

			MessageEnvelope():uptime(0){}
			MessageEnvelope(nsecs_t uptime, MessageHandler* const handler, const Message& message):uptime(uptime),handler(handler),message(message){}
			nsecs_t uptime;
			MessageHandler* handler;
			Message message;
		};

		bool mSendingMessage;

		volatile bool mIdling;

		int mEpollFd;

		int mWakeReadPipeFd;

	        int mWakeWritePipeFd;	
		
		Mutex mLock;

		size_t mResponseIndex;

		Vector<Response> mResponses;

		KeyedVector<int,Request> mRequests;

		Vector<MessageEnvelope> mMessageEnvelopes;

		int pollInner(int timeoutMillis);

		void awoken();

		void pushResponse(int events, const Request &request);

		static void initTLSKey();

		static void freeTLS(void *mlooper);
		
		nsecs_t mNextMessageUptime;


};












}

#endif

