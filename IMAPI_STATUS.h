#pragma once
#include <winnt.h>
struct IMAPI_STATUS
{
	// From IDiscFormat2DataEventArgs methods
	LONG elapsedTime;
	LONG remainingTime;
	LONG totalTime;
	//https://learn.microsoft.com/en-us/windows/win32/api/imapi2/nn-imapi2-idiscformat2dataeventargs	
	

	//From IWriteEngine2EventArgs methods
	LONG freeSystemBuffer;
	LONG lastReadLba;
	LONG lastWrittenLba;
	LONG totalSystemBuffer;
	LONG usedSystemBuffer;
	LONG startLba;
	LONG sectorCount;
	//https://learn.microsoft.com/en-us/windows/win32/api/imapi2/nn-imapi2-iwriteengine2eventargs
};