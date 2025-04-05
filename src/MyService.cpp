//======================================================================================================================
// Project: HwMonitorService
//----------------------------------------------------------------------------------------------------------------------
// Author:      Jan Broz (Youda008)
// Description: main service functionality
//======================================================================================================================

#include "MyService.hpp"

#include "SvcCommon.hpp"
#include "EventLogMessages.h"

#include "lhwm-cpp-wrapper.h"
using SensorMap = std::invoke_result_t< decltype( LHWM::GetHardwareSensorMap ) >;

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <shlobj.h>
#include <tchar.h>

#include <ctime>
#include <thread>
#include <mutex>
#include <atomic>
#include <cstdio>
#include <string>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <memory>
using std::string;
using std::vector;
using std::unordered_map;
using std::unordered_set;
using std::unique_ptr;


//======================================================================================================================
//  global

const TCHAR * const MY_SERVICE_NAME = _T("HwMonitorService");

static HANDLE g_svcStopEvent = NULL;


//======================================================================================================================
//  logging

enum class Severity
{
	Error,
	Warning,
	Info,
	Debug,
};
static const TCHAR * const SeverityStrings [] =
{
	_T("Error"),
	_T("Warning"),
	_T("Info"),
	_T("Debug"),
};
static void log( Severity severity, const TCHAR * format, ... )
{
#ifdef DEBUGGING_PROCESS
	va_list args;
	va_start( args, format );

	TCHAR timeStr [20];
	time_t now = time(nullptr);
	_tcsftime( timeStr, 20, _T("%Y-%m-%d %H:%M:%S"), localtime(&now) );

	TCHAR finalFormat [128];
	_sntprintf( finalFormat, 127, _T("%s [%-8s] %s\n"), timeStr, SeverityStrings[ size_t(severity) ], format );
	finalFormat[126] = _T('\n');
	finalFormat[127] = _T('\0');

	_vtprintf( finalFormat, args );
	fflush( stdout );

	va_end( args );
#endif
}


//======================================================================================================================
//  main service functionality

bool MyServiceInit()
{
	// reading the hardware sensors requires admin privileges
	if (!IsUserAnAdmin())
	{
		ReportSvcEvent( SVCEVENT_CUSTOM_ERROR, _T("User is not admin, driver probably won't loaded") );
		//return false;
	}

	ReportSvcStatus( SERVICE_START_PENDING, NO_ERROR, 8000 );  // estimate of how long the initialization can take

	// This must be what loads the driver, otherwise i don't know what else does.
	auto sensorInfo = LHWM::GetHardwareSensorMap();
	if (sensorInfo.empty())
	{
		ReportSvcEvent( SVCEVENT_CUSTOM_ERROR, _T("Failed to initalize sensor monitoring") );
		return false;
	}

#ifdef DEBUGGING_PROCESS
	_tprintf( _T("Found %zu devices with sensors:\n"), sensorInfo.size() );
	for (const auto & [key, info] : sensorInfo)
	{
		_tprintf( _T("  \"%s\":\n"), key.c_str() );
	}
#endif // DEBUGGING_PROCESS

	// Create an event. The control handler function (SvcCtrlHandler)
	// signals this event when it receives the stop control code.
	g_svcStopEvent = CreateEvent( NULL, TRUE, FALSE, NULL );
	if (g_svcStopEvent == NULL)
	{
		ReportSvcEvent( SVCEVENT_CUSTOM_ERROR, _T("Failed to create stop event") );
		return false;
	}

	return true;
}

void MyServiceRun()
{
	DWORD waitResult = 0;
	do
	{
		// If the SvcCtrlHandler signals to stop the service, wake up and exit immediatelly.
		// If there is no signal, wait 2 seconds and repeat.
		waitResult = WaitForSingleObject( g_svcStopEvent, 1000 );
	}
	while (waitResult == WAIT_TIMEOUT);
}

void MyServiceStop()
{
	// signal the primary thread
	SetEvent( g_svcStopEvent );
}

void MyServiceCleanup()
{
	CloseHandle( g_svcStopEvent );
}
