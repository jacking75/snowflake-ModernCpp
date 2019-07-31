// 만든이: 최흥배
// 오픈소스로 공개된 snowflake의 C# 버전(https://github.com/ccollie/snowflake-net)을 거의 그대로 C++로 포팅한 것이다.
//
#pragma once

#include <chrono>
#include <atomic>


namespace Snowflake
{
	enum class ERROR
	{
		NONE = 0,
		WORKER_Id_GREATER_OR_LESS_MAX_WORKER_Id = -1,
		DATACENTER_Id_GREATER_OR_LESS_0 = -2,
		INVALIDSYSTEMCLOCK_CLOCK_MOVED_BACKWARDS = -3,
	};

	class IdWorker
	{
	public:
		IdWorker() = default;
		~IdWorker() = default;

		ERROR Init(long workerId, long datacenterId, long sequence = 0L)
		{
			m_WorkerId = workerId;
			m_DatacenterId = datacenterId;
			m_Sequence = sequence;

			// sanity check for workerId
			if (workerId > MAX_WORKER_Id || workerId < 0)
			{
				//throw new ArgumentException(String.Format("worker Id can't be greater than {0} or less than 0", MaxWorkerId));
				return ERROR::WORKER_Id_GREATER_OR_LESS_MAX_WORKER_Id;
			}

			if (datacenterId > MAX_DATA_CENTER_Id || datacenterId < 0)
			{
				//throw new ArgumentException(String.Format("datacenter Id can't be greater than {0} or less than 0", MaxDatacenterId));
				return ERROR::DATACENTER_Id_GREATER_OR_LESS_0;
			}

			return ERROR::NONE;
		}


		// 새로운 시퀸스번호를 생성한다. 0 보다 작은 숫자가 나오면 에러이다.
		long long NextId()
		{
			// 락 걸기
			ENTER_LOCK();

			auto timestamp = TimeGen();

			if (timestamp >= m_LastTimestamp)
			{			
				if (m_LastTimestamp == timestamp)
				{
					m_Sequence = (m_Sequence + 1) & SEQUENCE_MASK;

					if (m_Sequence == 0)
					{
						timestamp = TilNextMillis(m_LastTimestamp);
					}
				}
				else
				{
					m_Sequence = 0;
				}
				LEAVE_LOCK(); // 락 해제

				m_LastTimestamp = timestamp;
				auto id = ((timestamp - TWEPOCH) << TIME_STAMP_LEFT_SHIFT) |
							(m_DatacenterId << DATACENTER_ID_SHIFT) |
							(m_WorkerId << WORKER_Id_SHIFT) |
							m_Sequence;

				return id;
			}
						
			LEAVE_LOCK(); // 락 해제
			return static_cast<long long>(ERROR::INVALIDSYSTEMCLOCK_CLOCK_MOVED_BACKWARDS);
		}

		// 개발자 기능. 생성한 시퀸스번호를 기준으로 생성 시간의 밀리세컨드를 반환한다.
		long long DevSeqIdToTimeStamp(long long seqId)
		{
			auto timeStamp = (seqId >> 22) + TWEPOCH;
			return timeStamp;
		}


	private:
		const long TWEPOCH = 1288834974657L;

		const int WORJER_Id_BITS = 5;
		const int DATACENTERIDBITS = 5;
		const int SEQUENCE_BITS = 12;

		const long MAX_WORKER_Id = -1L ^ (-1L << WORJER_Id_BITS);
		const long MAX_DATA_CENTER_Id = -1L ^ (-1L << DATACENTERIDBITS);

		const int WORKER_Id_SHIFT = SEQUENCE_BITS; 
		const int DATACENTER_ID_SHIFT = SEQUENCE_BITS + WORJER_Id_BITS;
		const int TIME_STAMP_LEFT_SHIFT = SEQUENCE_BITS + WORJER_Id_BITS + DATACENTERIDBITS;
		const long SEQUENCE_MASK = -1L ^ (-1L << SEQUENCE_BITS);


		long m_Sequence = 0L;
		long m_LastTimestamp = -1L;

		long m_WorkerId = 0;
		long m_DatacenterId = 0;
		//long Sequence = 0;
	

		long long TilNextMillis(long long lastTimestamp)
		{
			auto timestamp = TimeGen();

			while (timestamp <= lastTimestamp)
			{
				timestamp = TimeGen();
			}

			return timestamp;
		}

		//밀리세컨드 단위의 현재 시간
		long long TimeGen()
		{
#ifdef __USED_STEADY_TIMER__
			auto milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_timer::now().time_since_epoch());
#else
			auto milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch());
#endif
			return milliseconds.count();
		}


		// Lock
#ifndef UN_USED_LOCK
		std::atomic<int> m_AtomicLock = 0;

		void ENTER_LOCK()
		{
			for (int nloops = 0; ; nloops++)
			{
				if (std::atomic_compare_exchange_strong(&m_AtomicLock, 0, 1))
				{
					return;
				}
			}
		}

		void LEAVE_LOCK()
		{
			m_AtomicLock = 0;
		}
#else
		void ENTER_LOCK() {}
		void LEAVE_LOCK() {}
#endif
	};
}