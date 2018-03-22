#pragma once
#include <chrono>
#include <map>
#include <list>
#include <mutex>
#include <thread>
#include <condition_variable>
#include <algorithm>

class Synchrophasotron
{
 
template <typename KeyType, typename ValueType, int TTL = 20/*seconds*/ >
friend class ExpirationCache;
private:
	Synchrophasotron()
	{
	}
	virtual int getTTL() const = 0; // get current object ttl , overridet in templeate
	virtual void ClearAll() = 0; // remove all elements in crrent object
	virtual int Count()const =0; // count of element in current instancieted object
	static void RemoveExpiredElements() // cleare all objects from caches() for wich the TTL was expiered
	{
		int count = 1;
		while(!endExecution())
		{
			count++;
			for(auto& i:caches())
			{
				if(count % i->getTTL() ==0)
				{
					i->ClearAll();
				}
			}
			std::this_thread::sleep_for(std::chrono::seconds(1));
		}
	}
	virtual ~Synchrophasotron()
	{
		if(caches().size() == 0)
		{
			endExecution() = true;
			removeingThread().join();
		}

	}
protected:
	static std::thread& removeingThread() {static std::thread t; return t;}
	static std::list<Synchrophasotron*>& caches() {static std::list<Synchrophasotron*> t; return t;} 
	static bool& endExecution(){static bool t = false;return t;}
};

template <typename _KeyType, typename _ValueType, int TTL/*seconds*/ >
class ExpirationCache
	:public Synchrophasotron 
{
public:
	typedef _KeyType KeyType;
	typedef _ValueType ValueType;
public:
	ExpirationCache()
	{
		// add the new created object to objects list
		caches().push_back(this);
		// if preveously the list was empty , start monitoring thread
		if (caches().size() == 1)
		{
			endExecution() = false;// loop monitring until there is a element in caches()
			removeingThread() = std::thread(&Synchrophasotron::RemoveExpiredElements);
		}

	};
	/// @brief get the value by @param key, if value does not exists throws an out_of_range exceptons
	ValueType Get(const KeyType& ky) const
	{
		std::lock_guard<std::mutex> lock(mt_);
		return map_.at(ky);
	}
	void Put(const KeyType& ky, const ValueType& val)
	{
		std::lock_guard<std::mutex> lock(mt_);
		map_[ky] = val;
	}
	void Put(const KeyType& ky, ValueType&& val)
	{
		std::lock_guard<std::mutex> lock(mt_);
		map_[ky] = std::move(val);
	}

	~ExpirationCache()
	{
		auto ret = std::find(caches().begin(),caches().end(),this);
		if(ret != caches().end())
		{
			(*ret)->ClearAll();
			caches().erase(ret);
		}

	}
	virtual int Count()const override
	{
		std::lock_guard<std::mutex> lock(mt_);
		return map_.size();
	}
private:
	virtual int getTTL() const override { return TTL; }
	virtual void ClearAll() override { std::lock_guard<std::mutex> lock(mt_); map_.clear(); }
	
	 
private:
	std::map<KeyType, ValueType> map_;
	mutable std::mutex mt_;
};
