#pragma once
class MenuAnalyzer
{
private:
	static MenuAnalyzer* singleton;
public:
	MenuAnalyzer(void);
	~MenuAnalyzer(void);
	u_char* MenuSelector(u_char*);
	static MenuAnalyzer* GetMenuAnalyzer();
};

