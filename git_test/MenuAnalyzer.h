#pragma once

typedef enum{LOGIN=1,SIGNUP=2,LOGOUT=3,LEAVE=4,SEARCH,MORE,
	WRITE_COMMENT,MODIFY_COMMENT,DELETE_COMMENT,LIKE,DISLIKE,REPORT}PROTO_TYPE;

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