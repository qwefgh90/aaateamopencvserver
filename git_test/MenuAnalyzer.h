#pragma once


class MenuAnalyzer
{
private:
	static MenuAnalyzer* singleton;
public:
	MenuAnalyzer(void);
	~MenuAnalyzer(void);
	Memory MenuSelector(Memory);
	static MenuAnalyzer* GetMenuAnalyzer();
};