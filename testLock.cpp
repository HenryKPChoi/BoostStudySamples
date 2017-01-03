// TestLock.cpp : Defines the entry point for the console application.
//

//#include "stdafx.h"
#include <boost/thread.hpp>
#include <boost/thread/recursive_mutex.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <queue>

boost::recursive_mutex m;
std::queue<std::string> q;

#define TARGET_PRODUCT_COUNT 4

void producer()
{
	for( int i = 0 ; i < TARGET_PRODUCT_COUNT; i++)
	{
		boost::recursive_mutex::scoped_lock lk(m);
		boost::recursive_mutex::scoped_lock lk_again(m);
		
		q.push("a");
	}
}

void consumer()
{
	int consumedCount = 0;
	do{
		boost::recursive_mutex::scoped_lock lk(m);
		boost::recursive_mutex::scoped_lock lk_again(m); // lock is re-entrant

		if( q.size() > 0 )
		{
			q.pop();
			consumedCount++;
		}
	}while(consumedCount < TARGET_PRODUCT_COUNT);

	std::cout << "Consumer done. [" << consumedCount << "]" << std::endl;
}

void TestRecursiveMutex()
{
	boost::thread p(producer);
	boost::thread c(consumer);

	p.join();
	c.join();
}

std::vector<std::string> products;

void run()
{
	while (1)
	{
		boost::this_thread::sleep( boost::posix_time::seconds(1));
		std::cout << "Producing " << std::endl;
		products.push_back("A Product");
	}
	std::cout << "Normal exit from run()" << std::endl;
}

void TestInterruptionPoint()
{
	boost::thread t(run);
	
	while (1)
	{
		boost::this_thread::sleep( boost::posix_time::seconds(1));
		if(products.size() > TARGET_PRODUCT_COUNT )
		{
			std::cout << "Goal achieved" << std::endl;
			t.interrupt();
			break;
		}
	}
	t.join();
}

int main(int argc, char* argv[])
{
	TestRecursiveMutex();
	TestInterruptionPoint();
	return 0;
}

