#pragma once

enum enumDumpDetectionLevel
{
	DL_DEFAULT, //일반적인 핸들러(그냥 넣었음. 덤프 안남는 상황이 발생할 수 있다)
	DL_MY_HANDLER, //다른 핸들러를 모두 무효화 시키고 자신이 등록한 핸들러만 작동하도록 한다.
	DL_MY_HANDLER_STACKOVERFLOW, //스택오버플로우 핸들러와 내 핸들러만을 예외 필터에 등록하고 나머지는 작동이 되지 않도록 한다.
	DL_MAX,
};

enum enumExceptionHandler
{	
	EH_MINIDUMP = 0, //미니덤프 클래스
	EH_BUGTRAP, //버그트랩
	EH_BREAKPAD, //구글 브레이크 패드
	EH_USER_DEFINED, //로그 및 닥터 왓슨
	EH_BUGTRAP_MIX, //버그트랩과 사용자 정의 핸들러 Mix
	EH_MAX,
};