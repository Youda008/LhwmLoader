MessageIdTypedef=DWORD

SeverityNames=(
    Success=0x0:STATUS_SEVERITY_SUCCESS
    Informational=0x1:STATUS_SEVERITY_INFORMATIONAL
    Warning=0x2:STATUS_SEVERITY_WARNING
    Error=0x3:STATUS_SEVERITY_ERROR
)

FacilityNames=(
    System=0x0:FACILITY_SYSTEM
    Runtime=0x2:FACILITY_RUNTIME
    Stubs=0x3:FACILITY_STUBS
    Io=0x4:FACILITY_IO_ERROR_CODE
)

LanguageNames=(English=0x409:MSG00409)

; // The following are message definitions.

MessageId=1
SymbolicName=SVCEVENT_STATUS_REPORT
Severity=Informational
Facility=Runtime
Language=English
Status report: %2.
.

MessageId=2
SymbolicName=SVCEVENT_INIT_SYSCALL_ERROR
Severity=Error
Facility=System
Language=English
Essential syscall failed when starting the service: %2.
.

MessageId=3
SymbolicName=SVCEVENT_CUSTOM_ERROR
Severity=Error
Facility=Runtime
Language=English
Service-specific error: %2.
.

; // A message file must end with a period on its own line
; // followed by a blank line.