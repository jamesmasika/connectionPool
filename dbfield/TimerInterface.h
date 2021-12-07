#pragma once
#ifndef TimerInterface_h__
#define TimerInterface_h__


#include <thread>
#include <atomic>
#include <mutex>
#include <condition_variable>
#include <functional>


class CTimerInterface 
{
public:
	CTimerInterface() :expired_(true), try_to_expire_(false)
	{
	}

	~CTimerInterface()
	{
		Expire();
	}

	template<class F, class... Args>
	void StartTimer(int interval, F &&f, Args &&... args)
	{
		if (!expired_.load())
			return;
		
		expired_.store(false);

		using ftype = typename std::result_of<F(Args...)>::type;

		std::function<ftype()> task = std::bind(std::forward<F>(f), std::forward<Args>(args)...);
	
		std::thread([this, interval, task](){
			while (!try_to_expire_.load()){
				std::this_thread::sleep_for(std::chrono::milliseconds(interval));
				task();		
			}

			expired_ = true;		

		}).detach();
	}

	void Expire(){
		if (expired_.load()){
			return;
		}

		if (try_to_expire_.load())
			return;
		
		try_to_expire_.store(true);
		while (!expired_.load())
			std::this_thread::sleep_for(std::chrono::milliseconds(50));
	}

private:
	std::atomic<bool> expired_;
	std::atomic<bool> try_to_expire_;
};


#endif // TimerInterface_h__
