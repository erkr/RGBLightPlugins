#pragma once
#include <Windows.h>

class EventLog
{
public:

	EventLog(void);
	virtual ~EventLog(void);

	bool Initialize(LPCWSTR log_name);
	void WriteError(LPCWSTR message);
	void WriteWarning(LPCWSTR message);
	void WriteInfo(LPCWSTR message);
private:
	HANDLE event_log=NULL;
};

