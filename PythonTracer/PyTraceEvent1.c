/*++

Copyright (c) 2016 Trent Nelson <trent@trent.me>

Module Name:

    PyTraceEvent1.c

Abstract:

    This module implements the tracing callback for the PYTHON_TRACE_EVENT1
    structure.

--*/

#include "stdafx.h"

_Use_decl_annotations_
PPYTHON_TRACE_EVENT1
AllocatePythonTraceEvent1(
    _In_ PTRACE_STORE TraceStore,
    _In_ PLARGE_INTEGER Timestamp
    )
{
    ULONG_PTR NumberOfRecords = 1;
    ULONG_PTR RecordSize = sizeof(PYTHON_TRACE_EVENT1);

    return (PPYTHON_TRACE_EVENT1)(
        TraceStore->AllocateRecordsWithTimestamp(
            TraceStore->TraceContext,
            TraceStore,
            NumberOfRecords,
            RecordSize,
            Timestamp
        )
    );
}

_Use_decl_annotations_
BOOL
PyTraceEvent1(
    PPYTHON_TRACE_CONTEXT Context,
    PPYTHON_FUNCTION Function,
    PPYTHON_EVENT_TRAITS EventTraits,
    PPYFRAMEOBJECT FrameObject,
    PPYOBJECT ArgObject
    )
{
    BOOL Success;

    PYTHON_TRACE_CONTEXT_FLAGS Flags;

    PRTL Rtl;
    PPYTHON Python;
    PTRACE_CONTEXT TraceContext;
    PTRACE_STORES TraceStores;
    PTRACE_STORE EventStore;
    PYTHON_TRACE_EVENT1 Event;
    PYTHON_TRACE_EVENT1 LastEvent;
    PPYTHON_TRACE_EVENT1 LastEventPointer;
    PPYTHON_TRACE_EVENT1 ThisEvent;
    LARGE_INTEGER Elapsed;
    LARGE_INTEGER Timestamp;
    PROCESS_MEMORY_COUNTERS_EX MemoryCounters;
    IO_COUNTERS IoCounters;
    DWORD HandleCount;
    HANDLE CurrentProcess = (HANDLE)-1;

    //
    // Initialize local variables/aliases.
    //

    Rtl = Context->Rtl;
    Python = Context->Python;
    Flags = Context->Flags;

    //
    // Load the events trace store and previous event record, if any.
    //

    TraceContext = Context->TraceContext;
    TraceStores = TraceContext->TraceStores;
    EventStore = &TraceStores->Stores[TraceStoreEventIndex];

    LastEventPointer = (PPYTHON_TRACE_EVENT1)EventStore->PrevAddress;

    if (Flags.TraceMemory) {

        Success = Rtl->K32GetProcessMemoryInfo(
            CurrentProcess,
            (PPROCESS_MEMORY_COUNTERS)&MemoryCounters,
            sizeof(MemoryCounters)
        );

        if (!Success) {
            Flags.TraceMemory = FALSE;
        }

    }

    if (Flags.TraceIoCounters) {

        Success = Rtl->GetProcessIoCounters(CurrentProcess,
                                            &IoCounters);

        if (!Success) {
            Flags.TraceIoCounters = FALSE;
        }
    }

    if (Flags.TraceHandleCount) {

        Success = Rtl->GetProcessHandleCount(CurrentProcess,
                                             &HandleCount);

        if (!Success) {
            Flags.TraceHandleCount = FALSE;
        }
    }

    SecureZeroMemory(&Event, sizeof(Event));

    //
    // Save the timestamp for this event.
    //

    TraceContextQueryPerformanceCounter(TraceContext, &Elapsed, &Timestamp);

    //
    // Fill out the function.
    //

    Event.Timestamp.QuadPart = Elapsed.QuadPart;
    Event.Function = Function;
    Event.FirstLineNumber = (USHORT)Function->FirstLineNumber;
    Event.NumberOfLines = (USHORT)Function->NumberOfLines;
    Event.NumberOfCodeLines = (USHORT)Function->NumberOfCodeLines;
    Event.ThreadId = FastGetCurrentThreadId();
    Event.CodeObjectHash = Function->CodeObjectHash;
    Event.FunctionHash = (ULONG)Function->Signature;
    Event.PathHash = Function->PathEntry.PathHash;
    Event.FullNameHash = Function->PathEntry.FullNameHash;
    Event.ModuleNameHash = Function->PathEntry.ModuleNameHash;
    Event.ClassNameHash = Function->PathEntry.ClassNameHash;
    Event.NameHash = Function->PathEntry.NameHash;

    Event.EventTraitsEx.AsLong = (ULONG)EventTraits->AsByte;
    Event.EventTraitsEx.LineNumberOrCallStackDepth = (ULONG)(
        EventTraits->IsLine ?
            Python->PyFrame_GetLineNumber(FrameObject) :
            Context->Depth
    );

    //
    // Save memory, I/O and handle counts if applicable.
    //

    if (Flags.TraceMemory) {
        Event.WorkingSetSize = MemoryCounters.WorkingSetSize;
        Event.PageFaultCount = MemoryCounters.PageFaultCount;
        Event.CommittedSize  = MemoryCounters.PrivateUsage;
    }

    if (Flags.TraceIoCounters) {
        Event.ReadTransferCount = IoCounters.ReadTransferCount;
        Event.WriteTransferCount = IoCounters.WriteTransferCount;
    }

    if (Flags.TraceHandleCount) {
        Event.HandleCount = HandleCount;
    }

    if (!LastEventPointer) {
        goto Finalize;
    }

    //
    // Take a local copy of the last event.
    //

    if (!CopyPythonTraceEvent1(&LastEvent, LastEventPointer)) {
        goto Finalize;
    }

    //
    // Calculate the elapsed time relative to the last event's timestamp and
    // then update its elapsed microsecond field.
    //

    Elapsed.QuadPart -= LastEvent.Timestamp.QuadPart;
    LastEvent.ElapsedMicroseconds = Elapsed.LowPart;

    if (LastEvent.EventTraitsEx.IsLine) {

        //
        // If the last event's line number was greater than this line
        // number, we've jumped backwards, presumably as part of a loop.
        //

        if (LastEvent.EventTraitsEx.LineNumberOrCallStackDepth >
            Event.EventTraitsEx.LineNumberOrCallStackDepth) {
            Event.EventTraitsEx.IsReverseJump = TRUE;
        }
    }

    //
    // Calculate deltas for memory, I/O and handle counts, if applicable.
    //

    if (Flags.TraceMemory) {

        //
        // Calculate memory counter deltas.
        //

        LastEvent.WorkingSetDelta = (LONG)(
            Event.WorkingSetSize -
            LastEvent.WorkingSetSize
        );

        LastEvent.PageFaultDelta = (USHORT)(
            Event.PageFaultCount -
            LastEvent.PageFaultCount
        );

        LastEvent.CommittedDelta = (LONG)(
            Event.CommittedSize -
            LastEvent.CommittedSize
        );

    }

    if (Flags.TraceIoCounters) {

        //
        // Calculate IO counter deltas.
        //

        LastEvent.ReadTransferDelta = (ULONG)(
            Event.ReadTransferCount -
            LastEvent.ReadTransferCount
        );

        LastEvent.WriteTransferDelta = (ULONG)(
            Event.WriteTransferCount -
            LastEvent.WriteTransferCount
        );

    }

    if (Flags.TraceHandleCount) {

        //
        // Calculate handle count delta.
        //

        LastEvent.HandleDelta = (SHORT)(
            Event.HandleCount -
            LastEvent.HandleDelta
        );

    }

    //
    // Copy the last event back, ignoring the return value.
    //

    if (!CopyPythonTraceEvent1(LastEventPointer, &LastEvent)) {
        NOTHING;
    }

Finalize:

    //
    // Allocate a new event record, then copy our temporary event over.
    //

    ThisEvent = AllocatePythonTraceEvent1(EventStore, &Timestamp);
    if (!ThisEvent) {
        return FALSE;
    }

    if (!CopyPythonTraceEvent1(ThisEvent, &Event)) {
        NOTHING;
    }

    return TRUE;
}

// vim:set ts=8 sw=4 sts=4 tw=80 expandtab                                     :
