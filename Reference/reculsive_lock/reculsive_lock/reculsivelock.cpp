// reculsivelock.cpp : 콘솔 응용 프로그램에 대한 진입점을 정의합니다.
//

#include "stdafx.h"
#include <boost/thread/recursive_mutex.hpp>

int main()
{
	boost::recursive_mutex smapleLock;
	smapleLock.lock();
	smapleLock.lock();
	smapleLock.unlock();
	smapleLock.unlock();

    return 0;
}

/*boost::mutex smapleLock;
smapleLock.lock();
smapleLock.lock();
smapleLock.unlock();
smapleLock.unlock();*/

