/*++

Copyright (c) 2017 Trent Nelson <trent@trent.me>

Module Name:

    DebugEnginePrivate.h

Abstract:

    This is the private header file for the DebugEngine module.  It defines
    function typedefs and declares functions for all major (i.e. not local to
    the individual module/file) routines available for use by individual modules
    within this component, as well as defining supporting structures and enums.

--*/

#pragma once

#include "stdafx.h"

#ifdef __cplusplus
extern "C" {
#endif

#define DEFINE_GUID_EX(name, l, w1, w2, b1, b2, b3, b4, b5, b6, b7, b8) \
    const GUID DECLSPEC_SELECTANY name                                  \
        = { l, w1, w2, { b1, b2,  b3,  b4,  b5,  b6,  b7,  b8 } }

//
// Private function typedefs.
//

typedef
_Check_return_
_Success_(return != 0)
_Requires_exclusive_lock_held_(DebugEngine->Lock)
BOOL
(INITIALIZE_DEBUG_ENGINE)(
    _In_ PRTL Rtl,
    _In_ struct _DEBUG_ENGINE *DebugEngine
    );
typedef INITIALIZE_DEBUG_ENGINE *PINITIALIZE_DEBUG_ENGINE;

typedef
_Check_return_
_Success_(return != 0)
BOOL
(CREATE_DEBUG_INTERFACES)(
    _In_ PDEBUG_ENGINE_SESSION Session
    );
typedef CREATE_DEBUG_INTERFACES *PCREATE_DEBUG_INTERFACES;

typedef
_Check_return_
_Success_(return != 0)
BOOL
(DEBUG_ENGINE_DISASSEMBLE_ADDRESS)(
    _In_ PDEBUG_ENGINE_SESSION Session,
    _In_ ULONG64 Offset,
    _In_ PSTRING Buffer
    );
typedef DEBUG_ENGINE_DISASSEMBLE_ADDRESS *PDEBUG_ENGINE_DISASSEMBLE_ADDRESS;

typedef
_Check_return_
_Success_(return != 0)
BOOL
(INITIALIZE_CALLBACKS)(
    _In_ PDEBUG_ENGINE_SESSION Session
    );
typedef INITIALIZE_CALLBACKS *PINITIALIZE_CALLBACKS;

//
// DEBUG_ENGINE_OUTPUT related function typedefs and structures.
//

typedef
HRESULT
(STDAPICALLTYPE DEBUG_ENGINE_OUTPUT_CALLBACK)(
    _In_ struct _DEBUG_ENGINE *DebugEngine,
    _In_ DEBUG_OUTPUT_TYPE OutputType,
    _In_ DEBUG_OUTPUT_CALLBACK_FLAGS OutputFlags,
    _In_ ULONG64 Arg,
    _In_opt_ PCWSTR Text
    );
typedef DEBUG_ENGINE_OUTPUT_CALLBACK *PDEBUG_ENGINE_OUTPUT_CALLBACK;

typedef union _DEBUG_ENGINE_ENUM_SYMBOLS_OUTPUT_CALLBACK_CONTEXT_FLAGS {
    LONG AsLong;
    ULONG AsULong;
    struct _Struct_size_bytes_(sizeof(ULONG)) {
        ULONG WideCharacter:1;
    };
} DEBUG_ENGINE_ENUM_SYMBOLS_OUTPUT_CALLBACK_CONTEXT_FLAGS;

typedef
struct _Struct_size_bytes_(SizeOfStruct)
_DEBUG_ENGINE_ENUM_SYMBOLS_OUTPUT_CALLBACK_CONTEXT {

    //
    // Size of structure, in bytes.
    //

    _Field_range_(
        == ,
        sizeof(struct _DEBUG_ENGINE_ENUM_SYMBOLS_OUTPUT_CALLBACK_CONTEXT)
    ) ULONG SizeOfStruct;

    //
    // Flags.
    //

    DEBUG_ENGINE_ENUM_SYMBOLS_OUTPUT_CALLBACK_CONTEXT_FLAGS Flags;

    PDEBUG_ENGINE_OUTPUT_CALLBACK OutputCallback;
    PVOID CallerContext;
    PDEBUG_ENGINE_ENUM_SYMBOLS_CALLBACK CallerCallback;
    PALLOCATOR Allocator;
    PRTL_PATH ModulePath;
    union {
        PSTRING Command;
        PUNICODE_STRING CommandWide;
    };
} DEBUG_ENGINE_ENUM_SYMBOLS_OUTPUT_CALLBACK_CONTEXT;
typedef DEBUG_ENGINE_ENUM_SYMBOLS_OUTPUT_CALLBACK_CONTEXT
      *PDEBUG_ENGINE_ENUM_SYMBOLS_OUTPUT_CALLBACK_CONTEXT;

typedef
HRESULT
(STDAPICALLTYPE DEBUG_ENGINE_ENUM_SYMBOLS_OUTPUT_CALLBACK)(
    _In_ struct _DEBUG_ENGINE *DebugEngine,
    _In_ DEBUG_OUTPUT_TYPE OutputType,
    _In_ DEBUG_OUTPUT_CALLBACK_FLAGS OutputFlags,
    _In_ ULONG64 Arg,
    _In_ PCWSTR Text
    );

//
// DisassembleFunction-related structures and function pointer typedefs.
//

typedef struct _DEBUG_ENGINE_DISASSEMBLE_FUNCTION_OUTPUT_CALLBACK_CONTEXT {
    PDEBUG_ENGINE_OUTPUT_CALLBACK OutputCallback;
    PVOID CallerContext;
    PVOID CallerCallback;
    PALLOCATOR Allocator;
    PRTL_PATH ModulePath;
    PUNICODE_STRING Command;
} DEBUG_ENGINE_DISASSEMBLE_FUNCTION_OUTPUT_CALLBACK_CONTEXT;
typedef DEBUG_ENGINE_DISASSEMBLE_FUNCTION_OUTPUT_CALLBACK_CONTEXT
      *PDEBUG_ENGINE_DISASSEMBLE_FUNCTION_OUTPUT_CALLBACK_CONTEXT;

typedef union _DEBUG_ENGINE_OUTPUT_CALLBACK_CONTEXT {
    PDEBUG_ENGINE_ENUM_SYMBOLS_OUTPUT_CALLBACK_CONTEXT EnumSymbols;
    PDEBUG_ENGINE_DISASSEMBLE_FUNCTION_OUTPUT_CALLBACK_CONTEXT
        DisassembleFunction;
} DEBUG_ENGINE_OUTPUT_CALLBACK_CONTEXT;

//
// DEBUG_ENGINE-related function typedefs and structures.
//

typedef union _DEBUG_ENGINE_FLAGS {
    LONG AsLong;
    ULONG AsULong;
    struct _Struct_size_bytes_(sizeof(ULONG)) {
        ULONG Unused:1;
    };
} DEBUG_ENGINE_FLAGS;
C_ASSERT(sizeof(DEBUG_ENGINE_FLAGS) == sizeof(ULONG));

typedef struct _Struct_size_bytes_(SizeOfStruct) _DEBUG_ENGINE {

    //
    // Size of the structure, in bytes.
    //

    _Field_range_(==, sizeof(struct _DEBUG_ENGINE)) ULONG SizeOfStruct;

    //
    // Flags.
    //

    DEBUG_ENGINE_FLAGS Flags;

    //
    // IIDs, interfaces and vtables.
    //

    PCGUID IID_Client;
    PIDEBUGCLIENT IClient;
    PDEBUGCLIENT Client;

    PCGUID IID_Control;
    PIDEBUGCONTROL IControl;
    PDEBUGCONTROL Control;

    PCGUID IID_Symbols;
    PIDEBUGSYMBOLS ISymbols;
    PDEBUGSYMBOLS Symbols;

    PCGUID IID_SymbolGroup;
    PIDEBUGSYMBOLGROUP ISymbolGroup;
    PDEBUGSYMBOLGROUP SymbolGroup;

    PCGUID IID_Advanced;
    PIDEBUGADVANCED IAdvanced;
    PDEBUGADVANCED Advanced;

    PCGUID IID_DataSpaces;
    PIDEBUGDATASPACES IDataSpaces;
    PDEBUGDATASPACES DataSpaces;

    //
    // Client/Control state.
    //

    ULONG SessionStatus;

    DEBUG_EXECUTION_STATUS ExecutionStatus;

    ULONG EngineState;
    ULONGLONG EngineStateArg;
    DEBUG_CHANGE_ENGINE_STATE ChangeEngineState;

    ULONG DebuggeeState;
    ULONGLONG DebuggeeStateArg;

    ULONG SymbolState;
    ULONGLONG SymbolStateArg;

    ULONG ActualProcessorType;
    struct {
        ULONG Error;
        ULONG Level;
    } SystemError;

    //
    // Callback-related fields.
    //

    volatile LONG EventCallbacksRefCount;
    volatile LONG InputCallbacksRefCount;
    volatile LONG OutputCallbacksRefCount;
    volatile LONG OutputCallbacks2RefCount;

    SRWLOCK Lock;

    _Guarded_by_(Lock)
    struct {

        DEBUG_ENGINE_OUTPUT_CALLBACK_CONTEXT OutputCallbackContext;

        DEBUG_EVENT_CALLBACKS_INTEREST_MASK EventCallbacksInterestMask;
        DEBUG_OUTPUT_CALLBACKS_INTEREST_MASK OutputCallbacksInterestMask;
        DEBUG_OUTPUT_CALLBACKS2_INTEREST_MASK OutputCallbacks2InterestMask;
        ULONG Unused1;

        DEBUGEVENTCALLBACKS EventCallbacks;
        IDEBUGEVENTCALLBACKS IEventCallbacks;

        DEBUGINPUTCALLBACKS InputCallbacks;
        IDEBUGINPUTCALLBACKS IInputCallbacks;

        DEBUGOUTPUTCALLBACKS OutputCallbacks;
        IDEBUGOUTPUTCALLBACKS IOutputCallbacks;

        DEBUGOUTPUTCALLBACKS2 OutputCallbacks2;
        IDEBUGOUTPUTCALLBACKS2 IOutputCallbacks2;

        GUID IID_CurrentEventCallbacks;
        GUID IID_CurrentInputCallbacks;
        GUID IID_CurrentOutputCallbacks;
    };

} DEBUG_ENGINE, *PDEBUG_ENGINE, **PPDEBUG_ENGINE;

#define AcquireDebugEngineLock(Engine) \
    AcquireSRWLockExclusive(&(Engine)->Lock)

#define ReleaseDebugEngineLock(Engine) \
    ReleaseSRWLockExclusive(&(Engine)->Lock)

//
// Event callbacks.
//

#define DEBUG_CALLBACK_PROLOGUE(Name)                             \
    PDEBUG_ENGINE Engine;                                         \
    Engine = CONTAINING_RECORD(This->lpVtbl, DEBUG_ENGINE, Name);

#define DEBUG_ADD_REF(Name) InterlockedIncrement(&Engine->##Name##RefCount)

#define DEBUG_RELEASE(Name) InterlockedIncrement(&Engine->##Name##RefCount)

#define DEBUG_QUERY_INTERFACE(Name, Upper)                       \
    if (InlineIsEqualGUID(InterfaceId, &IID_IUNKNOWN) ||         \
        InlineIsEqualGUID(InterfaceId, &IID_IDEBUG_##Upper##)) { \
        InterlockedIncrement(&Engine->##Name##RefCount);         \
        *Interface = &Engine->I##Name##;                         \
        return S_OK;                                             \
    }                                                            \
    *Interface = NULL

typedef
_Check_return_
_Success_(return != 0)
BOOL
(DEBUG_ENGINE_SET_EVENT_CALLBACKS)(
    _In_ PDEBUG_ENGINE Engine,
    _In_ PDEBUGEVENTCALLBACKS EventCallbacks,
    _In_ PCGUID InterfaceId,
    _In_ DEBUG_EVENT_CALLBACKS_INTEREST_MASK InterestMask
    );
typedef DEBUG_ENGINE_SET_EVENT_CALLBACKS *PDEBUG_ENGINE_SET_EVENT_CALLBACKS;

typedef
_Check_return_
_Success_(return != 0)
BOOL
(DEBUG_ENGINE_SET_INPUT_CALLBACKS)(
    _In_ PDEBUG_ENGINE Engine,
    _In_ PDEBUGINPUTCALLBACKS InputCallbacks,
    _In_ PCGUID InterfaceId
    );
typedef DEBUG_ENGINE_SET_INPUT_CALLBACKS *PDEBUG_ENGINE_SET_INPUT_CALLBACKS;

typedef
_Check_return_
_Success_(return != 0)
BOOL
(DEBUG_ENGINE_SET_OUTPUT_CALLBACKS)(
    _In_ PDEBUG_ENGINE Engine,
    _In_ PDEBUGOUTPUTCALLBACKS OutputCallbacks,
    _In_ PCGUID InterfaceId,
    _In_ DEBUG_OUTPUT_CALLBACKS_INTEREST_MASK InterestMask
    );
typedef DEBUG_ENGINE_SET_OUTPUT_CALLBACKS *PDEBUG_ENGINE_SET_OUTPUT_CALLBACKS;

typedef
_Check_return_
_Success_(return != 0)
BOOL
(DEBUG_ENGINE_SET_OUTPUT_CALLBACKS2)(
    _In_ PDEBUG_ENGINE Engine,
    _In_ PDEBUGOUTPUTCALLBACKS2 OutputCallbacks2,
    _In_ PCGUID InterfaceId,
    _In_ DEBUG_OUTPUT_CALLBACKS2_INTEREST_MASK InterestMask
    );
typedef DEBUG_ENGINE_SET_OUTPUT_CALLBACKS2
      *PDEBUG_ENGINE_SET_OUTPUT_CALLBACKS2;

//
// Inline functions for copying callback vtables to the engine.
//

_Requires_exclusive_lock_held_(Engine->Lock)
FORCEINLINE
VOID
CopyIDebugEventCallbacks(
    _In_ PDEBUG_ENGINE Engine,
    _Out_writes_bytes_(sizeof(DEBUGEVENTCALLBACKS)) PDEBUGEVENTCALLBACKS Dest,
    _In_ PCDEBUGEVENTCALLBACKS Source
    )
{
    UNREFERENCED_PARAMETER(Engine);
    __movsq((PDWORD64)Dest,
            (PDWORD64)&Source,
            QUADWORD_SIZEOF(DEBUGEVENTCALLBACKS));
}

_Requires_exclusive_lock_held_(Engine->Lock)
FORCEINLINE
VOID
CopyIDebugInputCallbacks(
    _In_ PDEBUG_ENGINE Engine,
    _Out_writes_bytes_(sizeof(DEBUGINPUTCALLBACKS)) PDEBUGINPUTCALLBACKS Dest,
    _In_ PCDEBUGINPUTCALLBACKS Source
    )
{
    UNREFERENCED_PARAMETER(Engine);
    __movsq((PDWORD64)Dest,
            (PDWORD64)&Source,
            QUADWORD_SIZEOF(DEBUGINPUTCALLBACKS));
}

_Requires_exclusive_lock_held_(Engine->Lock)
FORCEINLINE
VOID
CopyIDebugOutputCallbacks(
    _In_ PDEBUG_ENGINE Engine,
    _Out_writes_bytes_(sizeof(DEBUGOUTPUTCALLBACKS))
        PDEBUGOUTPUTCALLBACKS Dest,
    _In_ PCDEBUGOUTPUTCALLBACKS Source
    )
{
    UNREFERENCED_PARAMETER(Engine);
    __movsq((PDWORD64)Dest,
            (PDWORD64)Source,
            QUADWORD_SIZEOF(DEBUGOUTPUTCALLBACKS));
}

_Requires_exclusive_lock_held_(Engine->Lock)
FORCEINLINE
VOID
CopyIDebugOutputCallbacks2(
    _In_ PDEBUG_ENGINE Engine,
    _Out_writes_bytes_(sizeof(DEBUGOUTPUTCALLBACKS2))
        PDEBUGOUTPUTCALLBACKS2 Dest,
    _In_ PCDEBUGOUTPUTCALLBACKS2 Source
    )
{
    UNREFERENCED_PARAMETER(Engine);
    __movsq((PDWORD64)Dest,
            (PDWORD64)Source,
            QUADWORD_SIZEOF(DEBUGOUTPUTCALLBACKS2));
}

//
// Inline function for copying GUIDs.
//

FORCEINLINE
VOID
CopyInterfaceId(
    _Out_writes_bytes_(sizeof(GUID)) PGUID Dest,
    _In_ PCGUID Source
    )
{
    __movsq((PDWORD64)Dest,
            (PDWORD64)Source,
            QUADWORD_SIZEOF(GUID));
}

//
// Private function declarations.
//

#pragma component(browser, off)

INITIALIZE_CALLBACKS InitializeCallbacks;
CREATE_DEBUG_INTERFACES CreateDebugInterfaces;
INITIALIZE_DEBUG_ENGINE InitializeDebugEngine;

DEBUG_ENGINE_ENUM_SYMBOLS DebugEngineEnumSymbols;
DEBUG_ENGINE_ENUM_SYMBOLS_OUTPUT_CALLBACK DebugEngineEnumSymbolsOutputCallback;

DEBUG_ENGINE_DISASSEMBLE_FUNCTION DebugEngineDisassembleFunction;

//
// IDebugEventCallbacks
//

DEBUG_EVENT_QUERY_INTERFACE DebugEventQueryInterface;
DEBUG_EVENT_ADD_REF DebugEventAddRef;
DEBUG_EVENT_RELEASE DebugEventRelease;
DEBUG_EVENT_GET_INTEREST_MASK_CALLBACK DebugEventGetInterestMaskCallback;
DEBUG_EVENT_BREAKPOINT_CALLBACK DebugEventBreakpointCallback;
DEBUG_EVENT_EXCEPTION_CALLBACK DebugEventExceptionCallback;
DEBUG_EVENT_CREATE_THREAD_CALLBACK DebugEventCreateThreadCallback;
DEBUG_EVENT_EXIT_THREAD_CALLBACK DebugEventExitThreadCallback;
DEBUG_EVENT_CREATE_PROCESS_CALLBACK DebugEventCreateProcessCallback;
DEBUG_EVENT_EXIT_PROCESS_CALLBACK DebugEventExitProcessCallback;
DEBUG_EVENT_LOAD_MODULE_CALLBACK DebugEventLoadModuleCallback;
DEBUG_EVENT_UNLOAD_MODULE_CALLBACK DebugEventUnloadModuleCallback;
DEBUG_EVENT_SYSTEM_ERROR_CALLBACK DebugEventSystemErrorCallback;
DEBUG_EVENT_SESSION_STATUS_CALLBACK DebugEventSessionStatusCallback;
DEBUG_EVENT_CHANGE_DEBUGGEE_STATE_CALLBACK DebugEventChangeDebuggeeStateCallback;
DEBUG_EVENT_CHANGE_ENGINE_STATE_CALLBACK DebugEventChangeEngineStateCallback;
DEBUG_EVENT_CHANGE_SYMBOL_STATE_CALLBACK DebugEventChangeSymbolStateCallback;

//
// IDebugInputCallbacks
//

DEBUG_INPUT_QUERY_INTERFACE DebugInputQueryInterface;
DEBUG_INPUT_ADD_REF DebugInputAddRef;
DEBUG_INPUT_RELEASE DebugInputRelease;
DEBUG_INPUT_START_INPUT_CALLBACK DebugInputStartInputCallback;
DEBUG_INPUT_END_INPUT_CALLBACK DebugInputEndInputCallback;

//
// IDebugOutputCallbacks
//

DEBUG_OUTPUT_CALLBACKS_QUERY_INTERFACE DebugOutputCallbacksQueryInterface;
DEBUG_OUTPUT_CALLBACKS_ADD_REF DebugOutputCallbacksAddRef;
DEBUG_OUTPUT_CALLBACKS_RELEASE DebugOutputCallbacksRelease;
DEBUG_OUTPUT_CALLBACKS_OUTPUT DebugOutputCallbacksOutput;

//
// IDebugOutputCallbacks2
//

DEBUG_OUTPUT_CALLBACKS2_QUERY_INTERFACE DebugOutputCallbacks2QueryInterface;
DEBUG_OUTPUT_CALLBACKS2_ADD_REF DebugOutputCallbacks2AddRef;
DEBUG_OUTPUT_CALLBACKS2_RELEASE DebugOutputCallbacks2Release;
DEBUG_OUTPUT_CALLBACKS2_OUTPUT DebugOutputCallbacks2Output;
DEBUG_OUTPUT_CALLBACKS2_GET_INTEREST_MASK DebugOutputCallbacks2GetInterestMask;
DEBUG_OUTPUT_CALLBACKS2_OUTPUT2 DebugOutputCallbacks2Output2;

#pragma component(browser, on)

#ifdef __cplusplus
} // extern "C"
#endif

// vim:set ts=8 sw=4 sts=4 tw=80 expandtab                                     :