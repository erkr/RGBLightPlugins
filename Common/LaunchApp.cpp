#include <windows.h>
#include <WinBase.h>
#include <stdio.h>
#include <tchar.h>
#include <atlstr.h>

int launchApplicationAndWait(LPWSTR commandLine)
{
	STARTUPINFO si;
	PROCESS_INFORMATION pi;

	ZeroMemory(&si, sizeof(si));
	si.cb = sizeof(si);
	ZeroMemory(&pi, sizeof(pi));

	// Start the child process. 
	if (!CreateProcess(NULL,   // No module name (use command line)
		commandLine,        // Command line
		NULL,           // Process handle not inheritable
		NULL,           // Thread handle not inheritable
		FALSE,          // Set handle inheritance to FALSE
		0,              // No creation flags
		NULL,           // Use parent's environment block
		NULL,           // Use parent's starting directory 
		&si,            // Pointer to STARTUPINFO structure
		&pi)           // Pointer to PROCESS_INFORMATION structure
		)
	{
		int lastError = GetLastError();
		if (lastError == 740)
		{
			_tprintf(L"CreateProcess failed - The requested operation requires elevation\n");
		}
		else
		{
			_tprintf(L"CreateProcess failed (%d).\n", lastError);
		}
		return lastError;
	}

	// Wait until child process exits.
	WaitForSingleObject(pi.hProcess, INFINITE);

	// Close process and thread handles. 
	CloseHandle(pi.hProcess);
	CloseHandle(pi.hThread);
	return 0;
}
