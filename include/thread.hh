
#ifndef inc_thread_hh
#define inc_thread_hh

#include <functional>
#include <3ds.h>


template <typename ... Ts>
class thread
{
public:
	void _die()
	{
		this->alive = false;
	}

	/* exit the current thread */
	static void exit()
	{
		threadExit(0);
	}

	/* create a new thread */
	thread(std::function<void(Ts...)> cb, Ts& ... args)
	{
		ThreadFuncParams *params = new ThreadFuncParams;
		params->func = [&]() -> void { cb(args...); };
		params->self = this;

		s32 prio = 0;
		svcGetThreadPriority(&prio, CUR_THREAD_HANDLE);

		this->threadobj = threadCreate(&thread::_entrypoint, params, 64 * 1024, prio - 1,
			-2, false);
		this->alive = true;
	}

	~thread()
	{
		this->join();
		threadFree(this->threadobj);
	}

	/* wait for the thread to finish */
	void join()
	{
		if(this->alive) threadJoin(this->threadobj, U64_MAX);
	}

	/* returns if the thread is done */
	bool finished()
	{
		return !this->alive;
	}


private:
	typedef struct ThreadFuncParams
	{
		std::function<void()> func;
		thread *self;
	} ThreadFuncParams;

	static void _entrypoint(void *arg)
	{
		ThreadFuncParams *params = (ThreadFuncParams *) arg;
		params->func();
		params->self->_die();
		delete params;
	}

	bool alive = false;
	Thread threadobj;


};

#endif

