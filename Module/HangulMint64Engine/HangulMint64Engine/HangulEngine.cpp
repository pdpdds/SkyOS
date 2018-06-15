#include "HangulEngine.h"
#include "HangulInput.h"
#include "memory.h"
#include "string.h"
#include "2DGraphics.h"

#define KEY_BACKSPACE 8
#define KEY_LALT 0x85

BUFFERMANAGER stBufferManager;

HangulEngine::HangulEngine()
{
	m_bHangulMode = false;
	memset(&stBufferManager, 0, sizeof(BUFFERMANAGER));
}

void HangulEngine::Reset()
{
	memset(&stBufferManager, 0, sizeof(BUFFERMANAGER));
}

HangulEngine::~HangulEngine()
{
}

bool HangulEngine::SwitchMode()
{
	if (m_bHangulMode == false)
		m_bHangulMode = true;
	else
		m_bHangulMode = false;

	return m_bHangulMode;
}

bool HangulEngine::InputAscii(unsigned char letter)
{
	switch (letter)
	{
		//------------------------------------------------------------------
		// 한/영 모드 전환 처리
		//------------------------------------------------------------------
	case KEY_LALT:
		// 한글 입력 모드 중에 Alt 키가 눌러지면 한글 조합을 종료
		if (m_bHangulMode == TRUE)
		{
			// 키 입력 버퍼를 초기화
			stBufferManager.iInputBufferLength = 0;

			if ((stBufferManager.vcOutputBufferForProcessing[0] != '\0') &&
				(stBufferManager.iOutputBufferLength + 2 < MAXOUTPUTLENGTH))
			{
				// 조합 중인 한글을 윈도우 화면에 출력하는 버퍼로 복사
				memcpy(stBufferManager.vcOutputBuffer +
					stBufferManager.iOutputBufferLength,
					stBufferManager.vcOutputBufferForProcessing, 2);
				stBufferManager.iOutputBufferLength += 2;

				// 조합 중인 한글을 저장하는 버퍼를 초기화
				stBufferManager.vcOutputBufferForProcessing[0] = '\0';
			}
		}
		// 영문 입력 모드 중에 Alt 키가 눌러지면 한글 조합용 버퍼를 초기화
		else
		{
			stBufferManager.iInputBufferLength = 0;
			stBufferManager.vcOutputBufferForComplete[0] = '\0';
			stBufferManager.vcOutputBufferForProcessing[0] = '\0';
		}
		m_bHangulMode = true - m_bHangulMode;
		break;

		//------------------------------------------------------------------
		// 백스페이스 키 처리
		//------------------------------------------------------------------
	case KEY_BACKSPACE:
		// 한글을 조합하는 중이면 입력 버퍼의 내용을 삭제하고 남은
		// 키 입력 버퍼의 내용으로 한글을 조합
		if ((m_bHangulMode == true) &&
			(stBufferManager.iInputBufferLength > 0))
		{
			// 키 입력 버퍼의 내용을 하나 제거하고 한글을 다시 조합
			stBufferManager.iInputBufferLength--;
			ComposeHangul(stBufferManager.vcInputBuffer,
				&stBufferManager.iInputBufferLength,
				stBufferManager.vcOutputBufferForProcessing,
				stBufferManager.vcOutputBufferForComplete);
		}
		// 한글 조합 중이 아니면 윈도우 화면에 출력하는 버퍼의 내용을 삭제
		else
		{
			if (stBufferManager.iOutputBufferLength > 0)
			{
				// 화면 출력 버퍼에 들어있는 내용이 2바이트 이상이고 버퍼에
				// 저장된 값에 최상위 비트가 켜져 있으면 한글로 간주하고
				// 마지막 2바이트를 모두 삭제
				if ((stBufferManager.iOutputBufferLength >= 2) &&
					(stBufferManager.vcOutputBuffer[
						stBufferManager.iOutputBufferLength - 1] & 0x80))
				{
					stBufferManager.iOutputBufferLength -= 2;
					memset(stBufferManager.vcOutputBuffer +
						stBufferManager.iOutputBufferLength, 0, 2);
				}
						// 한글이 아니면 마지막 1바이트만 삭제
				else
				{
					stBufferManager.iOutputBufferLength--;
					stBufferManager.vcOutputBuffer[
						stBufferManager.iOutputBufferLength] = '\0';
				}
			}
		}
		break;

		//------------------------------------------------------------------
		// 나머지 키들은 현재 입력 모드에 따라 한글을 조합하거나
		// 윈도우 화면에 출력하는 버퍼로 직접 삽입
		//------------------------------------------------------------------
	default:
		// 특수 키들은 모두 무시
		if (letter & 0x80)
		{
			break;
		}

		// 한글 조합 중이면 한글 조합 처리(한글을 화면 출력 버퍼에 저장할
		// 공간이 충분한지도 확인)
		if ((m_bHangulMode == TRUE) &&
			(stBufferManager.iOutputBufferLength + 2 <= MAXOUTPUTLENGTH))
		{
			// 자/모음이 시프트와 조합되는 경우를 대비하여 쌍자음이나
			// 쌍모음을 제외한 나머지는 소문자로 변환
			ConvertJaumMoumToLowerCharactor((BYTE*)&letter);

			// 입력 버퍼에 키 입력 값을 채우고 데이터의 길이를 증가
			stBufferManager.vcInputBuffer[
				stBufferManager.iInputBufferLength] = letter;
			stBufferManager.iInputBufferLength++;

			// 한글 조합에 필요한 버퍼를 넘겨줘서 한글을 조합
			if (ComposeHangul(stBufferManager.vcInputBuffer,
				&stBufferManager.iInputBufferLength,
				stBufferManager.vcOutputBufferForProcessing,
				stBufferManager.vcOutputBufferForComplete) == TRUE)
			{
				// 조합이 완료된 버퍼에 값이 있는가 확인하여 윈도우 화면에
				// 출력할 버퍼로 복사
				if (stBufferManager.vcOutputBufferForComplete[0] != '\0')
				{
					memcpy(stBufferManager.vcOutputBuffer +
						stBufferManager.iOutputBufferLength,
						stBufferManager.vcOutputBufferForComplete, 2);
					stBufferManager.iOutputBufferLength += 2;

					// 조합된 한글을 복사한 뒤에 현재 조합 중인 한글을 출력할
					// 공간이 없다면 키 입력 버퍼와 조합 중인 한글 버퍼를 모두 초기화
					if (stBufferManager.iOutputBufferLength + 2 > MAXOUTPUTLENGTH)
					{
						stBufferManager.iInputBufferLength = 0;
						stBufferManager.vcOutputBufferForProcessing[0] = '\0';
					}
				}
			}
			// 조합에 실패하면 입력 버퍼에 마지막으로 입력된 값이 한글 자/모음이
			// 아니므로 한글 조합이 완료된 버퍼의 값과 입력 버퍼에 있는 값을 모두
			// 출력 버퍼로 복사
			else
			{
				// 조합이 완료된 버퍼에 값이 있는가 확인하여 윈도우 화면에
				// 출력할 버퍼로 복사
				if (stBufferManager.vcOutputBufferForComplete[0] != '\0')
				{
					// 완성된 한글을 출력 버퍼로 복사
					memcpy(stBufferManager.vcOutputBuffer +
						stBufferManager.iOutputBufferLength,
						stBufferManager.vcOutputBufferForComplete, 2);
					stBufferManager.iOutputBufferLength += 2;
				}

				// 윈도우 화면에 출력하는 버퍼의 공간이 충분하면 키 입력 버퍼에
				// 마지막으로 입력된 한글 자/모가 아닌 값을 복사
				if (stBufferManager.iOutputBufferLength < MAXOUTPUTLENGTH)
				{
					stBufferManager.vcOutputBuffer[
						stBufferManager.iOutputBufferLength] =
						stBufferManager.vcInputBuffer[0];
						stBufferManager.iOutputBufferLength++;
				}

				// 키 입력 버퍼를 비움
				stBufferManager.iInputBufferLength = 0;
			}
		}
		// 한글 입력 모드가 아닌 경우
		else if ((m_bHangulMode == false) &&
			(stBufferManager.iOutputBufferLength + 1 <= MAXOUTPUTLENGTH))
		{
			// 키 입력을 그대로 윈도우 화면에 출력하는 버퍼로 저장
			stBufferManager.vcOutputBuffer[
				stBufferManager.iOutputBufferLength] = letter;
			stBufferManager.iOutputBufferLength++;
		}
		break;
	}

	return true;
}

//------------------------------------------------------------------
// 화면 출력 버퍼에 있는 문자열과 조합 중인 한글 문자 스트링을 얻어낸다	
//------------------------------------------------------------------
int HangulEngine::GetString(char* buffer)
{		
	int length = stBufferManager.iOutputBufferLength;

	if (length != 0)
	{
		strcpy(buffer, stBufferManager.vcOutputBuffer);
	}

	// 현재 조합 중인 한글이 있다면 화면 출력 버퍼의 내용이 출력된
	// 다음 위치에 조합중인 한글을 출력
	if (stBufferManager.vcOutputBufferForProcessing[0] != '\0')
	{
		buffer[length] = stBufferManager.vcOutputBufferForProcessing[0];
		buffer[length + 1] = stBufferManager.vcOutputBufferForProcessing[1];

		length += 2;
	}

	return length;	
}

int HangulEngine::DrawText(int iX1, int iY1, int iX2, int iY2, DWORD* pstMemoryAddress, int iX, int iY,
	DWORD stTextColor, DWORD stBackgroundColor, const char* pcString, int iLength)
{
	RECT memoryArea;
	memoryArea.iX1 = iX1;
	memoryArea.iY1 = iY1;
	memoryArea.iX2 = iX2;
	memoryArea.iY2 = iY2;

	return kInternalDrawText(&memoryArea, pstMemoryAddress, iX, iY, stTextColor, stBackgroundColor, pcString, iLength);
}
