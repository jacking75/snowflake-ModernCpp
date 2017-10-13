#include <iostream>

#include "../include/snowflake.h"

void timeStampToHumanReadableDate(long long timeStamp, char* outBuffer)
{
	time_t now = time(&timeStamp);
	
	// Format time, "ddd yyyy-mm-dd hh:mm:ss zzz"
	auto ts = localtime(&now);
	strftime(outBuffer, 128, "%a %Y-%m-%d %H:%M:%S %Z", ts);
}

int main()
{
	Snowflake::IdWorker Sequence1;
	Sequence1.Init(2, 1);

	Snowflake::IdWorker Sequence2;
	Sequence2.Init(1, 2);

	auto seq1 = Sequence1.NextId();
	auto seq2 = Sequence2.NextId();

	char seq1HumanReadableDateBuffer[128] = { 0, };
	char seq2HumanReadableDateBuffer[128] = { 0, };

	// https://www.epochconverter.com/ 
	// 이 사이트를 통해서 타임스탬프 값(밀리세컨드)에서 현재 시간을 얻을 수 있다(단 GMT 시간 기준. 이 시간에 9시간 더해야 한국 시간이다.)
	timeStampToHumanReadableDate(Sequence1.DevSeqIdToTimeStamp(seq1), seq1HumanReadableDateBuffer);
	timeStampToHumanReadableDate(Sequence2.DevSeqIdToTimeStamp(seq2), seq2HumanReadableDateBuffer);

	printf("seq1: %uld, timeStamp: %uld, %s\r\n", seq1, Sequence1.DevSeqIdToTimeStamp(seq1), seq1HumanReadableDateBuffer);
	printf("seq2: %uld, timeStamp: %uld, %s\r\n", seq2, Sequence2.DevSeqIdToTimeStamp(seq2), seq2HumanReadableDateBuffer);

	return 0;
}