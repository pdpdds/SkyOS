#pragma once
#include "stdint.h"
#include "KeyBoardIO.h"


typedef struct tag_KeyBoardState
{
	char _scancode;
	bool _numlock; 
	bool _scrolllock;
	bool _capslock;
	bool _shift;
	bool _alt;
	bool _ctrl;

	int _error;
	//Check if Basic Assurance Test(BAT) failed
	bool _batFailed;
	//Check if diagnostics failed
	bool _diagnosticsFailed;
	//Check if system should resend last command
	bool _resendCmd;
	bool _disable;

}KeyBoardState;

namespace KeyBoard
{
	void		Install(int);
	void		ResetSystem();

	void		Disable();
	void		Enable();
	bool		IsDisabled();

	bool		GetScrollLock();
	bool		GetNumLock();
	bool		GetCapsLock();

	bool		GetAlt();
	bool		GetCtrl();
	bool		GetShift();

	void		SetLeds(bool num, bool caps, bool scroll);

	uint8_t		GetLastScanCode();
	KEYCODE		GetLastKeyCode();
	void		DiscardLastKeyCode();	

	char		ConvertKeyToAscii(KEYCODE);

	bool		SelfTest();
	uint8_t		ReadStatus();
	void		SendCommand(uint8_t);


	uint8_t		ReadEncodeBuffer();
	void		SendEncodeCommand(uint8_t);
	

	void ProcessKeyBoardInterrupt();

}