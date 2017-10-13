#pragma once

#include <chrono>


namespace Snowflake
{
	class IdWorker
	{
	public:
		IdWorker() = default;
		~IdWorker() = default;

		// TODO: bool 이 아닌 enum 으로 바꾸기
		bool Init(long workerId, long datacenterId, long sequence = 0L)
		{
			WorkerId = workerId;
			DatacenterId = datacenterId;
			_sequence = sequence;

			// sanity check for workerId
			if (workerId > MaxWorkerId || workerId < 0)
			{
				//throw new ArgumentException(String.Format("worker Id can't be greater than {0} or less than 0", MaxWorkerId));
				return false;
			}

			if (datacenterId > MaxDatacenterId || datacenterId < 0)
			{
				//throw new ArgumentException(String.Format("datacenter Id can't be greater than {0} or less than 0", MaxDatacenterId));
				return false;
			}
		}


		//TODO: 엄청나게 큰 숫자를 에러번호로 하기. 에러번호 define 하기
		long long NextId()
		{
			//TODO: 락 걸기
			//lock(_lock)
			//{
			auto timestamp = TimeGen();

			if (timestamp < _lastTimestamp)
			{
				//throw new InvalidSystemClock(String.Format("Clock moved backwards.  Refusing to generate id for {0} milliseconds", _lastTimestamp - timestamp));
				return 0;
			}

			if (_lastTimestamp == timestamp)
			{
				_sequence = (_sequence + 1) & SequenceMask;
				if (_sequence == 0)
				{
					timestamp = TilNextMillis(_lastTimestamp);
				}
			}
			else {
				_sequence = 0;
			}

			_lastTimestamp = timestamp;
			auto id = ((timestamp - Twepoch) << TimestampLeftShift) | (DatacenterId << DatacenterIdShift) | (WorkerId << WorkerIdShift) | _sequence;

			return id;
			//}
		}


		long long DevSeqIdToTimeStamp(long long seqId)
		{
			auto timeStamp = (seqId >> 22) + Twepoch;
			return timeStamp;
		}


	private:
		const long Twepoch = 1288834974657L;

		const int WorkerIdBits = 5;
		const int DatacenterIdBits = 5;
		const int SequenceBits = 12;
		const long MaxWorkerId = -1L ^ (-1L << WorkerIdBits);
		const long MaxDatacenterId = -1L ^ (-1L << DatacenterIdBits);

		const int WorkerIdShift = SequenceBits;
		const int DatacenterIdShift = SequenceBits + WorkerIdBits;
		const int TimestampLeftShift = SequenceBits + WorkerIdBits + DatacenterIdBits;
		const long SequenceMask = -1L ^ (-1L << SequenceBits);

		long _sequence = 0L;
		long _lastTimestamp = -1L;


		long WorkerId = 0;
		long DatacenterId = 0;
		long Sequence = 0;
	

		long long TilNextMillis(long long lastTimestamp)
		{
			auto timestamp = TimeGen();

			while (timestamp <= lastTimestamp)
			{
				timestamp = TimeGen();
			}

			return timestamp;
		}

		long long TimeGen()
		{
			auto milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch());
			return milliseconds.count();
		}
	};
}