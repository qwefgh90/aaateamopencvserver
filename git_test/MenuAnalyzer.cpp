#include "StdAfx.h"
#include "MenuAnalyzer.h"

MenuAnalyzer* MenuAnalyzer::singleton=NULL;

MenuAnalyzer::MenuAnalyzer(void)
{
}

MenuAnalyzer::~MenuAnalyzer(void)
{
}

u_char* MenuAnalyzer::MenuSelector(u_char* in_buf)
{
	u_char* out_buf;

	return out_buf;
}

MenuAnalyzer* MenuAnalyzer::GetMenuAnalyzer()
{
	if(MenuAnalyzer::singleton==NULL)
	{
		singleton = new MenuAnalyzer();
	}
	return singleton;
}