#ifndef _Simple_TC_SINGLETON_H__
#define _Simple_TC_SINGLETON_H__


#include <cassert>
#include <cstdlib>
#include <mutex>

namespace TC
{



template<typename T>
class CreateUsingNew
{
public:
    static T* create() 
    { 
        return new T; 
    }

    static void destroy(T *t) 
    { 
        delete t; 
    }
};

template<typename T>
class CreateUsingNew1
{
public:
    static T* create() 
    { 
        return new T; 
    }

    static void destroy(T *t) 
    { 
        delete t; 
    }
};

template<typename T>
class CreateStatic
{
public:
    union MaxAlign 
    { 
        char t_[sizeof(T)]; 
        long double longDouble_; 
    }; 

    static T* create() 
    { 
        static MaxAlign t; 
        return new(&t) T; 
    }

    static void destroy(T *t) 
    {
        t->~T();
    }
};


template<typename T>
class DefaultLifetime
{
public:
    static void deadReference()
    {
        throw std::logic_error("singleton object has dead.");
    }

    static void scheduleDestruction(T*, void (*pFun)())
    {
        std::atexit(pFun);
    }
};

template<typename T>
class PhoneixLifetime
{
public:
    static void deadReference()
    {
        _bDestroyedOnce = true;
    }

    static void scheduleDestruction(T*, void (*pFun)())
    {
        if(!_bDestroyedOnce)
            std::atexit(pFun);
    }
private:
    static bool _bDestroyedOnce; 
};
template <class T> 
bool PhoneixLifetime<T>::_bDestroyedOnce = false; 

template <typename T> 
struct NoDestroyLifetime 
{ 
    static void scheduleDestruction(T*, void (*)()) 
    {
    } 

    static void deadReference() 
    {
    } 
}; 










    
template
<
    typename T,
    template<typename> class CreatePolicy   = CreateUsingNew,
    template<typename> class LifetimePolicy = DefaultLifetime
>
class TC_Singleton 
{
public:
    typedef T  instance_type;
    typedef volatile T volatile_type;
 
    typedef CreatePolicy<T> TCreatePolicy;

    static T *getInstance()
    {

        if(!_pInstance)
        {
			if (_tl == NULL)
				_tl = new std::mutex();

            std::lock_guard<std::mutex> lock(*_tl);
            if(!_pInstance)
            {
                if(_destroyed)
                {
                    LifetimePolicy<T>::deadReference();
                    _destroyed = false;
                }
                _pInstance = CreatePolicy<T>::create();
                LifetimePolicy<T>::scheduleDestruction((T*)_pInstance, &destroySingleton);
            }
        }
        
        return (T*)_pInstance;
    }
    
protected:

    static void destroySingleton()
    {
        assert(!_destroyed);
        CreatePolicy<T>::destroy((T*)_pInstance);
        _pInstance = NULL;
        _destroyed = true;
    }
protected:

    static std::mutex		*_tl;
    static volatile T*      _pInstance;
    static bool             _destroyed;

protected:
    TC_Singleton(){}
    virtual ~TC_Singleton(){}; 
    TC_Singleton (const TC_Singleton &); 
    TC_Singleton &operator=(const TC_Singleton &);
};

template <class T, template<class> class CreatePolicy, template<class> class LifetimePolicy> 
std::mutex * TC_Singleton<T, CreatePolicy, LifetimePolicy>::_tl = new std::mutex();

template <class T, template<class> class CreatePolicy, template<class> class LifetimePolicy> 
bool TC_Singleton<T, CreatePolicy, LifetimePolicy>::_destroyed = false; 

template <class T, template<class> class CreatePolicy, template<class> class LifetimePolicy> 
volatile T* TC_Singleton<T, CreatePolicy, LifetimePolicy>::_pInstance = NULL; 

} //namespace TC

#endif //_Simple_TC_SINGLETON_H__
