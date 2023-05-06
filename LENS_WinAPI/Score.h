#pragma once

class Score {
public:
	int iMil, iCent, iDec, iUnit;
	int iMaxScore;

	void createiMaxS();
	int createNumber();
	void scoreCmp();
	void getMaxScore();
	void saveMaxScore();
	void saveTxt();
};

