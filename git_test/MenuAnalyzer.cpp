#include "StdAfx.h"
#include "MenuAnalyzer.h"

MenuAnalyzer* MenuAnalyzer::singleton=NULL;

MenuAnalyzer::MenuAnalyzer(void)
{
}

MenuAnalyzer::~MenuAnalyzer(void)
{
}
//param
//@data received from mobile 
//return
//@buffer and size to send to mobile
Memory MenuAnalyzer::MenuSelector(Memory in_buf)
{
	Memory memory;
	u_char* out_buf;

	return memory;
}

MenuAnalyzer* MenuAnalyzer::GetMenuAnalyzer()
{
	if(MenuAnalyzer::singleton==NULL)
	{
		singleton = new MenuAnalyzer();
	}
	return singleton;
}