#include "stdafx.h"

_Use_decl_annotations_
VOID
DestroyTracedPythonSession(
    PPTRACED_PYTHON_SESSION SessionPointer
    )
/*--

Routine Description:

    This function destroys a TRACED_PYTHON_SESSION structure that was created
    by InitializeTracedPythonSession().  This routine can be safely called
    regardless of the state of SessionPointer; that is, it will correctly
    handle a partially-initialized session that may have failed half-way
    through initialization.

Arguments:

    SessionPointer - Pointer to the address of the TRACED_PYTHON_SESSION struct
        to destroy.  This pointer is cleared by the routine.  (That is, once
        its contents is deemed to be valid, '*SessionPointer = NULL' will be
        performed.)  The rationale behind this logic is that you'd typically
        have a single, global pointer to an instance of this structure, and
        passing in the address of that pointer (versus the target contents)
        allows it to be cleared as part of destruction, ensuring it can no
        longer be dereferenced/accessed.

Returns:

    None.

--*/
{
    PTRACED_PYTHON_SESSION Session;

    //
    // Exit if SessionPointer is invalid.
    //

    if (!ARGUMENT_PRESENT(SessionPointer)) {
        return;
    }

    //
    // Dereference the pointer so we can just work with Session for the bulk
    // of the method (instead of *SessionPointer->).  (This should be wrapped
    // in a try/except ideally, however, we'd need to do some fidding to ensure
    // __C_specific_handler worked without relying on Session->Rtl, which may
    // not be available as *SessionPointer may fault.)
    //

    Session = *SessionPointer;

    //
    // Clear the originating pointer.
    //

    *SessionPointer = NULL;

    //
    // Close the trace stores.
    //

    if (Session->TraceStores) {
        Session->CloseTraceStores(Session->TraceStores);
        Session->TraceStores = NULL;
    }

    //
    // Close our threadpool.
    //

    if (Session->Threadpool) {
        CloseThreadpool(Session->Threadpool);
        DestroyThreadpoolEnvironment(&Session->ThreadpoolCallbackEnviron);
        Session->Threadpool = NULL;
    }

    //
    // XXX todo: remaining deallocation logic.
    //

    //
    // XXX todo: FreeLibrary() on all modules we loaded.
    //

}


// vim:set ts=8 sw=4 sts=4 tw=80 expandtab                                     :