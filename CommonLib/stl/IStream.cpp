
/* Engesn (Derived) Operating System
 * Copyright (c) 2002, 2003 Stephen von Takach
 * All Rights Reserved.
 * 
 * Permission to use, copy, modify and distribute this software
 * is hereby granted, provided that both the copyright notice and 
 * this permission notice appear in all copies of the software, 
 * derivative works or modified versions. 
*/

#include "IStream.h"
#include "KeyboardController.h"

namespace std
{
	IStream cin;

	unsigned int IStream::getString()
	{						//Acts like a console, prints char's as they are typed
		char input = '\0';	// and returns on 'Enter'. Text in IStream buffer.
		unsigned int control = 0;

		while(1)
		{
			input = KeyboardController::GetInput();

			if(input == '\r')
			{
				if(control >= (BUFFSIZE - 2))
				{
					control++;
				}
				buffer[control] = '\0';
				return control;
			}
			else if((input == 8) && (control > 0))
			{
				control--;
				cout << input;
			}
			else if(input >= ' ')
			{
				buffer[control] = input;
				if(control < (BUFFSIZE - 2))
				{
					control++;
				}
				else
				{
					cout << (char)8;
				}
				cout << input;
			}
		}
	}

	void IStream::getWord(char *str1, char *p1)
	{
		//Obtains first word in a string

		if(*str1 != '\0')
		{
			while(*str1 != ' ')
			{
				if(*str1 == '\0')
				{ 
					*p1 = '\0';
					break;
				}
				*p1 = *str1;
				*p1++;
				*str1++;
			}
		}
		else
		{
			*p1 = '\0';
		}
	}

	IStream& IStream::operator >> (char *cp)
	{
		getString();			//Allows input
		getWord(buffer, cp);	//Obtains first word
		cout << '\n';			//Moves cursor down a line
		return *this;
	}

	IStream& IStream::operator >> (char &c)
	{
		getString();			//Allows input
		c = buffer[0];			//Obtains first char
		cout << '\n';			//Moves cursor down a line
		return *this;
	}

	IStream& IStream::operator >> (unsigned char *ucp)
	{
		getString();			//Allows input
		getWord(buffer, (char *)ucp);	//Obtains first word
		cout << '\n';			//Moves cursor down a line
		return *this;
	}

	IStream& IStream::operator >> (unsigned char &uc)
	{
		getString();			//Allows input
		uc = (unsigned char)buffer[0];	//Obtains first char
		cout << '\n';			//Moves cursor down a line
		return *this;
	}
};


