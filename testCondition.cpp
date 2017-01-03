// testCondition.cpp : Defines the entry point for the console application.
//

#include <boost/thread.hpp>
#include <boost/chrono.hpp>
#include <boost/interprocess/sync/scoped_lock.hpp>
#include <boost/interprocess/sync/interprocess_condition.hpp>

class Bank
{
public:
	Bank(int amount):
		_amount(amount)
	{
	}

	void withdraw(int amount) { 
		_amount -= amount;
	}

	int RemainingBalance()
	{
		return _amount;
	}
private:
	int _amount;
};

boost::mutex m;

void testRun(Bank* const bank, int id)
{
	for( int i = 0 ; i < 10; i++)
	{
		boost::mutex::scoped_lock l(m); // Lock on creation
		std::cout << "(" << id << ") Begins withdraw --- " ;
		bank->withdraw(5);
		std::cout << " --- (" << id << ") Ends withdraw" << std::endl;
	}
}
void TestScopedlock()
{
	Bank bank(100);

	boost::thread t1(boost::bind(testRun, &bank, 1) );
	boost::thread t2(boost::bind(testRun, &bank, 2) );
	t1.join();
	t2.join();

	std::cout << "Bank's remaining balance : " << bank.RemainingBalance() << std::endl;
}

boost::mutex io_mutex;
bool worker_is_done = false;
boost::condition_variable condition;

void workFunction()
{
    std::cout << "Waiting a little..." << std::endl;
    boost::this_thread::sleep(boost::posix_time::seconds(2));
    std::cout << "Notifying condition..." << std::endl;

    condition.notify_one();
    std::cout << "Waiting a little more - 1..." << std::endl;
    boost::this_thread::sleep(boost::posix_time::seconds(2));

    condition.notify_one();
    std::cout << "Waiting a little more - 2..." << std::endl;

    boost::this_thread::sleep(boost::posix_time::seconds(1));
}

void TestConditionWaitingOnScopedLock() 
{
    boost::thread workThread(&workFunction);
	boost::mutex::scoped_lock lock(io_mutex);

	std::cout << "Waiting " << std::endl;

	condition.wait(lock);
    std::cout << "Condition notified for first time." << std::endl;

	condition.wait(lock);
    std::cout << "Condition notified for second time." << std::endl;

    workThread.join();
    std::cout << "Thread finished." << std::endl;
}

boost::mutex m1;
boost::condition_variable c1;
void Waiting(int id)
{
	std::cout << "Waiting thread " << id << " is started" << std::endl;
	for( auto i = 0; i < 5 ;i ++)
	{
		boost::mutex::scoped_lock lk(m1);
		std::cout << "ID : " << id << " start waiting " << std::endl;
		c1.wait(lk); // Lock is unlocked
		std::cout << "ID : " << id << " is awakened" << std::endl;
	}
}

void TestMultipleThreadsWaitingOnSingleCondition()
{
	boost::thread t1(boost::bind(Waiting, 1) );
	boost::thread t2(boost::bind(Waiting, 2));
	
	for(auto i = 0 ; i < 10; i++)
	{
		boost::this_thread::sleep( boost::posix_time::millisec(100));
		std::cout << "Notify one" << std::endl;
		c1.notify_one();	
	}

	t1.join();
	t2.join();
}

int main()
{
	std::cout << "******* TestScopedlock *******" << std::endl;
	TestScopedlock();

	std::cout << "******* TestConditionWaitingOnScopedLock *******" << std::endl;
	TestConditionWaitingOnScopedLock();

	std::cout << "******* TestMultipleThreadsWaitingOnSingleCondition *******" << std::endl;
	TestMultipleThreadsWaitingOnSingleCondition();

    return 0;
}
