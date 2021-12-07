#ifndef	_Simple_TC_THREAD_H_
#define _Simple_TC_THREAD_H_

#include <thread>
#include <stdexcept>
#include <string>
#include <cstring>
#include <sstream>
#include <condition_variable>
#include <mutex>
#include <memory>

namespace TC
{

using std::string;
using std::thread;
using std::exception;

	class TC_Exception : public exception
	{
	public:
		explicit TC_Exception(const string &buffer) 
		{
			_buffer = buffer; 
			_code = 0;
		}

		TC_Exception(const string &buffer, int err)
		{
			_buffer = buffer + " :" + strerror(err);
			_code = err;
		}

		virtual ~TC_Exception() throw() {}

		virtual const char* what() const throw()
		{
			return _buffer.c_str();
		}

	private:

		int     _code;
		string  _buffer;

	};


class TC_Thread
{
public:

	TC_Thread()
	{
		_running = false;
	}

	virtual ~TC_Thread()
	{
		_running = false;
	}

	void join() 
	{
		if (!_th) {
			throw TC_Exception("thread join nullptr");
		}
		if (std::this_thread::get_id() == _th->get_id()) {
			throw TC_Exception("can't be called in the same thread");
		}
		if (_th->joinable()) {
			_th->join();
		}
		else {
			throw TC_Exception("thread join error ");
		}
	}

	void detach()
	{
		if (std::this_thread::get_id() == _th->get_id()) {
			throw TC_Exception("can't be called in the same thread");
		}
		_th->detach();
	}

	static void sleep(int64_t millsecond)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(millsecond));
	}

	static void yield()
	{
		std::this_thread::yield();
	}

	bool isAlive() const
	{
		return _running;
	}

	std::thread::id id() 
	{ 
		return _th->get_id(); 
	}

    std::thread* getThread() 
	{ 
		return _th;
	}

	static size_t CURRENT_THREADID() 
	{
		std::stringstream sin;
		sin << std::this_thread::get_id();
		return std::stoull(sin.str());
	}

	void start()
	{
		if (_running)
		{
			throw TC_Exception("[TC_Thread::start] thread has start");
		}

		_th = new std::thread(threadEntry, this);

	}

protected:

	static void threadEntry(TC_Thread *pThread) 
	{
		pThread->_running = true;

		try
		{
			pThread->run();
		}
		catch (...)
		{
			pThread->_running = false;
			throw;
		}
		pThread->_running = false;
	}

    virtual void run() = 0;

protected:
    bool            _running;

    std::thread     *_th;
};


} // namespace TC

#endif //_Simple_TC_THREAD_H_

