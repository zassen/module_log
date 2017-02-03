#include <utils/Handler.h>

using namespace mlib;
using namespace std;

status_t Handler::readyToRun(){
	
	status_t result = NO_ERROR;

	if(mMlooper != NULL) ASSERT("member Mlooper is uninited before use it");

	//mMlooper = new Mlooper();
	mMlooper = Mlooper::prepare();

	if(mMlooper < 0 ) ASSERT("create mlooper fail at handler %s", this->mName.c_str());

	result = initInThread();	

	return result;
	
}

Handler* Handler::self(){
	return this;
}

void Handler::sendMessage(const Message& msg){

	if(mMlooper == NULL) ASSERT("mlooper not create before send message at handler %s",this->mName.c_str());
	mMlooper->sendMessage(this,msg);


}

bool Handler::threadLoop(){
	bool result =0;
	mMlooper->pollOnce(10000);

	result = threadWork();
	return result;
}

void Handler::setHub(HandlerHub* const hub){
	if(hub == NULL) ASSERT("input NULL hub ptr!");
	mHub = hub;
}

Handler* Handler::getHandler(string name){
	return mHub->getHandler(name);

}

Handler* HandlerHub::getHandler(string name){

	ssize_t handlerIndex = mHub.indexOfKey(name);
	if(handlerIndex < 0){
		INFO("can not find handler %s",name.c_str());
		return NULL;
	}
	return mHub.valueAt(handlerIndex);

}

void HandlerHub::addHandler(Handler* const handler){

	if(handler == NULL){
		ASSERT("input NULL handler ptr!");
	}
	mHub.add(handler->mName, handler);
	handler->setHub(this);

}
