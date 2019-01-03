/*
 * blogcxx :: https://www.blogcxx.de
 */

#pragma once

#include <cassert>
#include <future>
#include <map>
#include <set>
#include <type_traits>

#include "Log/Log.h"

template <typename KeyT, typename RetValT>
class AsyncWorker
{
public:
	AsyncWorker(size_t max_threads);

	using FuncT = std::function<RetValT(void)>;

	using DoneT =
		typename std::conditional<std::is_void<RetValT>::value, std::set<KeyT>,
								  std::map<KeyT, RetValT>>::type;

	bool Add(KeyT key, FuncT callable);
	DoneT GetResults();

private:
	KeyT WaitForOne(std::chrono::milliseconds ms) const;

	using ChooserT =
		typename std::conditional<std::is_void<RetValT>::value, std::true_type,
								  std::false_type>::type;

	void MarkDone(KeyT completed_key, std::true_type);
	void MarkDone(KeyT completed_key, std::false_type);

	size_t m_max_threads;

	std::map<KeyT, std::function<RetValT()>> m_work_to_do;
	std::map<KeyT, std::future<RetValT>> m_running;

	DoneT m_done;
};

template <typename KeyT, typename RetValT>
AsyncWorker<KeyT, RetValT>::AsyncWorker(size_t max_threads)
	: m_max_threads(max_threads)
{
}

template <typename KeyT, typename RetValT>
bool AsyncWorker<KeyT, RetValT>::Add(
	KeyT key, typename AsyncWorker<KeyT, RetValT>::FuncT callable)
{
	auto pos = m_work_to_do.find(key);
	if (pos != m_work_to_do.end())
	{
		THROW_FATAL("AsyncWorker<>: Key '%1%' already as work registered", key);
		return false;
	}

	m_work_to_do[key] = std::move(callable);
	return true;
}

template <typename KeyT, typename RetValT>
typename AsyncWorker<KeyT, RetValT>::DoneT
AsyncWorker<KeyT, RetValT>::GetResults()
{
	const size_t all_work_count = m_work_to_do.size();

	// add the first max_threads to the work queue
	auto fillup_running_queue = [this]() {
		while (m_running.size() < m_max_threads && m_work_to_do.size() > 0)
		{
			auto &current = *m_work_to_do.begin();
			std::future<RetValT> result_future =
				std::async(std::launch::async, current.second);

			m_running.emplace(current.first, std::move(result_future));
			m_work_to_do.erase(current.first);
		}
	};

	// now shuffle the work until all is done
	fillup_running_queue();
	while (m_done.size() < all_work_count)
	{
		// wait for the one to complete and get move the result to the result
		KeyT completed_key = WaitForOne(std::chrono::milliseconds(10));

		MarkDone(completed_key, ChooserT());

		m_running.erase(completed_key);

		fillup_running_queue();
	}

	// last check
	assert(m_done.size() == all_work_count);
	assert(m_running.size() == 0);
	assert(m_work_to_do.size() == 0);

	return m_done;
}

template <typename KeyT, typename RetValT>
void AsyncWorker<KeyT, RetValT>::MarkDone(KeyT completed_key, std::true_type)
{
	m_done.emplace(completed_key);
}

template <typename KeyT, typename RetValT>
void AsyncWorker<KeyT, RetValT>::MarkDone(KeyT completed_key, std::false_type)
{
	m_done.emplace(completed_key, m_running[completed_key].get());
}

template <typename KeyT, typename RetValT>
KeyT AsyncWorker<KeyT, RetValT>::WaitForOne(std::chrono::milliseconds ms) const
{
	std::future_status status;
	while (true)
	{
		bool at_least_one_valid = false;
		for (const auto &c : m_running)
		{
			if (!c.second.valid())
			{
				continue;
			}
			at_least_one_valid = true;

			// get the status of this one
			status = c.second.wait_for(std::chrono::milliseconds(0));
			if (status == std::future_status::ready)
			{
				return c.first;
			}
		}

		if (!at_least_one_valid)
		{
			THROW_FATAL("AsyncWorker<>::WaitForOne() No future valid.");
		}
		std::this_thread::sleep_for(ms);
	}

	// never reached
	THROW_FATAL("AsyncWorker<>::WaitForOne() while loop left.");
}
