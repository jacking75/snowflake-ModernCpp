# 한글
  
## snowflake-ModernCpp
- C++11을 지원하는 snowflake.  
- C++11을 지원하는 모든 컴파일러에서 사용할 수 있다.
- 크로스 플랫폼 지원.
- snowflake의 C# 버전(https://github.com/ccollie/snowflake-net)을 기준으로 C++로 컨버팅 한 것이다. 
  
  
### snowflake ?  
- Twitter에서 만든 오픈 소스 라이브러리로 주 목적은 분산 서버에서 일련 번호를 생성하는 것이다.
- 자세한 설명은 본인의 문서(https://www.slideshare.net/jacking/twitter-snowflake)를 보기바란다.  
  
  
### 사용법 
include 디렉토리에 있는 snowflake.h 파일만 추가하면 된다.      
test 디렉토리에는 snowflake-ModernCpp을 이용한 Visual Studio로 만든 콘솔 애플리케이션이 있다.  
멀티스레드에서 사용할 수 있다.  
만약 멀티스레드에서 사용하지 않는다면 **UN_USED_LOCK**를 선언하는 것이 좋다.  
   
#### 시간이 역행하지 않는 타이머 사용하기   
snowflake를 사용할 때 가장 조심해야 할 부분은 사용 도중 OS의 시간이 역행하지 않게 하는 것이다.    
시간 동기화나 사용자의 행동에 의해 시간이 역행 될 수있는데 이런 경우 올바른 일련 번호를 생성할 수 없다.  
만약 위와 같이 시간이 역행 되는 것을 막고 싶다면 snowflake를 include 하기 전에 '__USED_STEADY_TIMER__'을 선언한다.  
```
#define __USED_STEADY_TIMER__ 1
#include "snowflake.h"
```
     
	 
#### Sample Program  
```
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
	// 워커Id와 데이터센터Id가 서로 각각 다른 2개를 만들어서 일련 번호를 생성한다.
	Snowflake::IdWorker Sequence1;
	Sequence1.Init(2, 1);

	Snowflake::IdWorker Sequence2;
	Sequence2.Init(1, 2);

	auto seq1 = Sequence1.NextId(); // 일련 번호를 생성한다.
	auto seq2 = Sequence2.NextId();

	char seq1HumanReadableDateBuffer[128] = { 0, };
	char seq2HumanReadableDateBuffer[128] = { 0, };

	// 타임스탬프 값을 이용하여 시간 문자열을 만들어본다.
	// Sequence1.DevSeqIdToTimeStamp 함수를 사용하면 일련번호에서 타임스탬프 값만 추출한다.
	// https://www.epochconverter.com/  이 사이트를 통해서 타임스탬프 값(밀리세컨드)에서 현재 시간을 얻을 수 있다(단 GMT 시간 기준. 이 시간에 9시간 더해야 한국 시간이다.)
	timeStampToHumanReadableDate(Sequence1.DevSeqIdToTimeStamp(seq1), seq1HumanReadableDateBuffer);
	timeStampToHumanReadableDate(Sequence2.DevSeqIdToTimeStamp(seq2), seq2HumanReadableDateBuffer);

	printf("seq1: %uld, timeStamp: %uld, %s\r\n", seq1, Sequence1.DevSeqIdToTimeStamp(seq1), seq1HumanReadableDateBuffer);
	printf("seq2: %uld, timeStamp: %uld, %s\r\n", seq2, Sequence2.DevSeqIdToTimeStamp(seq2), seq2HumanReadableDateBuffer);

	return 0;
}
```
  
  
# English  
## snowflake-ModernCpp
- Snowflake with C ++ 11 support.
- Can be used by any compiler that supports C ++ 11.
- Cross-platform support.
- Converted to C ++ based on the C # version of snowflake (https://github.com/ccollie/snowflake-net).
    
## snowflake ?  
- An open source library created on Twitter. The main purpose is to generate a serial number on a distributed server.
    
## How to use
- Just add the snowflake.h file in the include directory.
- The test directory contains a console application created with Visual Studio using snowflake-ModernCpp.
- Can be used in multi-threaded.
     - If you are not using it in multi-threading, you should declare **UN_USED_LOCK**  
  
