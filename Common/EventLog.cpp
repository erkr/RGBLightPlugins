#include "EventLog.h"
#include <atlstr.h>
#include <atlsafe.h>
#include <filesystem>
#include <iostream>

using namespace std; 
namespace filesys = std::filesystem; // searchFile()  requires language standard C++17

std::wstring searchFile(const std::wstring& rootPath, const std::wstring& fileName)
{
	// Check if given path exists and points to a directory
	if (filesys::exists(rootPath) && filesys::is_directory(rootPath))
	{
		// Create a Recursive Directory Iterator object and points to the starting of directory
		filesys::recursive_directory_iterator iter(rootPath);
		// Create a Recursive Directory Iterator object pointing to end.
		filesys::recursive_directory_iterator end;
		// Iterate till end
		while (iter != end)
		{
			// Check if current entry matches the file searched for
			if (iter->path().filename() == fileName)
			{
				//std::cout << "Found: " << iter->path().string() << '\n';
				return iter->path().wstring();
			}
			error_code ec;
			// Increment the iterator to point to next entry in recursive iteration
			iter.increment(ec);
			if (ec) {
				//std::cerr << "Error While Accessing : " << iter->path().string() << " :: " << ec.message() << '\n';
			}
		}
	}
	else
	{
		//std::wcerr << "Cant open directory: " << rootPath << '\n';
	}
	return std::wstring();
}


EventLog::EventLog(void)
{
	event_log = NULL;
}


EventLog::~EventLog()
{
	if (event_log != NULL)
		DeregisterEventSource(event_log);
}

bool EventLog::Initialize(LPCWSTR log_name)
{
	if (event_log != NULL)
		return false;

	CString strReg = L"SYSTEM\\CurrentControlSet\\Services\\Eventlog\\Application\\";
	strReg += log_name;
	// create/open the registry event source key
	DWORD dwResult = 0;
	HKEY hKey = NULL;
	if (ERROR_SUCCESS != RegCreateKeyExW(HKEY_LOCAL_MACHINE, strReg, 0, NULL, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &hKey, &dwResult))
	{
		return false;
	}
	// create/set the value EventMessageFile
	WCHAR searchRoot[MAX_PATH];
	GetEnvironmentVariable(L"windir", searchRoot, MAX_PATH);
	wcscat_s(searchRoot, MAX_PATH, L"\\Microsoft.NET");
	std::wstring szPathName = searchFile(searchRoot, L"EventLogMessages.dll"); // this standard .NET dll is used as event log message file
	if (ERROR_SUCCESS != RegSetValueExW(hKey, L"EventMessageFile", 0, REG_SZ, (BYTE *)szPathName.c_str(), (DWORD)(wcslen(szPathName.c_str()) + 1) * sizeof(WCHAR)) )
	{
		return false;
	}
	// register the event source
	event_log = RegisterEventSource(NULL, log_name);
	return (event_log != NULL);
}



void EventLog::WriteError(LPCWSTR message)
{
	if (event_log != NULL)
		ReportEvent(event_log, EVENTLOG_ERROR_TYPE, 0, 0, NULL, 1, 0, &message, NULL);
}

void EventLog::WriteWarning(LPCWSTR message)
{
	if (event_log != NULL)
		ReportEvent(event_log, EVENTLOG_WARNING_TYPE, 0, 0, NULL, 1, 0, &message, NULL);
}

void EventLog::WriteInfo(LPCWSTR message)
{
	if (event_log != NULL)
		ReportEvent(event_log, EVENTLOG_INFORMATION_TYPE, 0, 0, NULL, 1, 0, &message, NULL);
}
 