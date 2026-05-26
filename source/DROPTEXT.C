/*-------------------------------------------------------------------------* 
 *                                                                         * 
 * Release 1.0 (May 7, 1993).  Copyright IBM Corp. 1993                    *
 *                                                                         * 
 * DropText:  Allow the user to type some text and then drag and drop it   *
 *            onto another window.  If the window has a textual component, *
 *            then it will be changed.                                     *
 *                                                                         * 
 *            The basic processing is:                                     *
 *                                                                         * 
 *               1) When detecting WM_BEGINDRAG in the entry field,        *
 *                  capture the mouse pointer so that we can tell          *
 *                  which window to drop the text on.                      *
 *                                                                         * 
 *               2) When detecting WM_ENDDRAG, find the window handle      *
 *                  of the window under the mouse pointer and issue        *
 *                  WinSetWindowText() to it.  Also release the mouse      *
 *                  pointer.                                               *
 *                                                                         * 
 *            Additional processing:                                       *
 *                                                                         * 
 *               1) If ESC is pressed before WM_ENDDRAG, then cancel the   *
 *                  operation.                                             *
 *                                                                         * 
 *               2) If the mouse pointer is over my own window, then       *
 *                  don't allow the drop.                                  *
 *                                                                         * 
 *               3) Since the WM_*DRAG messages are sent to the entry      *
 *                  field, we subclass the entry field and pass the        *
 *                  WM_*DRAG messages to the client window as user         *
 *                  messages.                                              *
 *                                                                         * 
 *                                                                         * 
 *  Author:  Joe DiAdamo                                                   *
 *                                                                         * 
 *  Modifications:                                                         *
 *                                                                         * 
 *                                                                         * 
 *-------------------------------------------------------------------------*/
#define INCL_WINWINDOWMGR
#define INCL_WINENTRYFIELDS
#define INCL_WINFRAMEMGR
#define INCL_WINMESSAGEMGR
#define INCL_WINDIALOGS
#define INCL_WININPUT
#define INCL_WINPOINTERS

#define INCL_DOSFILEMGR
#define INCL_DOSPROCESS

#include <os2.h>
#include <stdlib.h>
#include "droptext.h"                       /* PM Resource definitions     */

INT main()
{
   PCMNDATA pCMNData;                       /* Common data area            */
  
   HMQ      hmq;                            /* Message queue               */
   HAB      hab;                            /* Anchor block                */

   pCMNData = (PCMNDATA)malloc( sizeof( CMNDATA ) );
   pCMNData->cbData = sizeof( CMNDATA );    /* Size of data area           */
   pCMNData->fPointerCaptured = FALSE;      /* Boolean flag                */

   hab = WinInitialize( 0 );                        /* Standard PM startup */
   hmq = WinCreateMsgQueue( hab,
                            0 );

   pCMNData->hptrDropText = WinLoadPointer( HWND_DESKTOP,    /* Allow drop */
                                            NULLHANDLE,
                                            IDPTR_DROPTEXT );
   pCMNData->hptrNoDrop = WinLoadPointer( HWND_DESKTOP,    /* Prevent drop */
                                          NULLHANDLE,
                                          IDPTR_NODROP );
   pCMNData->hptrIcon = WinLoadPointer( HWND_DESKTOP, /* Icon for sys menu */
                                        NULLHANDLE,
                                        IDICN_DROPTEXT );

   WinDlgBox( HWND_DESKTOP,                 /* Just display the DLG Box    */
              HWND_DESKTOP,
              wpDlg,
              NULLHANDLE,
              IDDLG_DROPTEXT,
              (PVOID)pCMNData );

   WinDestroyPointer( pCMNData->hptrDropText );    /* Destory our pointers */
   WinDestroyPointer( pCMNData->hptrNoDrop );
   WinDestroyPointer( pCMNData->hptrIcon );
   WinDestroyMsgQueue( hmq );
   WinTerminate( hab );

   free( pCMNData );                        /* Free the common data area   */

   DosExit( EXIT_PROCESS,
            0L );
   return 0;
}


/*-------------------------------------------------------------------------* 
 * wpDlg:  Window procedure for the Dialog Box.  This is where all the     *
 *         work is done.                                                   *
 *-------------------------------------------------------------------------*/
MRESULT wpDlg(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2)
{
  MRESULT  mResult = (MRESULT)FALSE;          /* Function result parameter */
  PCMNDATA pCMNData;                        /* Pointer to common data area */
  POINTL   ptlPointer;           /* (x,y) coordinates of the mouse pointer */
  HWND     hwndTarget,           /* Window handle of the target window ... */
           hwndTargetParent;                /* ... and its parent          */
  CHAR     szDropText[EF_TEXTLIMIT + 1];    /* Text supplied by the user   */
  struct   _CHARMSG *pCharMsg;              /* Structure used by WM_CHAR   */

  switch (msg)
   {
    case WM_MOUSEMOVE :                     /* User Moved the Mouse        */
      pCMNData = (PCMNDATA) WinQueryWindowPtr( hwnd,   /* -> our data area */
                                               QWL_USER );
      if (pCMNData->fPointerCaptured)       /* Currently "dragging"        */
       {
        WinQueryPointerPos( HWND_DESKTOP,   /* Where's the mouse?          */
                            &ptlPointer );
        hwndTarget = WinWindowFromPoint( HWND_DESKTOP,  /* Get HWND of ... */
                                         &ptlPointer,   /* window under... */
                                         TRUE );      /* the mouse pointer */
        hwndTargetParent = WinQueryWindow( hwndTarget,   /* and its parent */
                                           QW_PARENT );

        if ( hwndTarget == HWND_DESKTOP ||  /* The  desktop                */
             hwndTargetParent == hwnd   ||  /* Or one of my children       */
             hwndTargetParent == pCMNData->hwndMyParent ||/* Or one of my siblings */
             hwndTarget == hwnd         ||  /* Or my own window            */
             hwndTarget == NULLHANDLE)      /* Or invalid handle found     */
          WinSetPointer( HWND_DESKTOP,      /* Don't allow the drop        */
                         pCMNData->hptrNoDrop );   
        else
          WinSetPointer( HWND_DESKTOP,      /* Allow the drop              */
                         pCMNData->hptrDropText );
       }
      else
        mResult = WinDefDlgProc(hwnd, msg, mp1, mp2);    /* Pass it on ... */
      break;

    case UM_BEGINDRAG :                     /* Begining a Drag operation   */
      pCMNData = (PCMNDATA) WinQueryWindowPtr( hwnd,
                                               QWL_USER );
      pCMNData->hptrPrevious = WinQueryPointer( HWND_DESKTOP );
      WinSetCapture( HWND_DESKTOP,     /* Capture the mouse so we know ... */
                     hwnd );                /* where to drop               */
      WinSetPointer( HWND_DESKTOP,          /* Assume can't drop yet       */
                     pCMNData->hptrNoDrop );
      pCMNData->fPointerCaptured = TRUE;    /* Set captured flag           */
      break;

    case UM_ENDDRAG :                       /* End the drag (i.e. Drop)    */
      pCMNData = (PCMNDATA) WinQueryWindowPtr( hwnd,
                                               QWL_USER );
      if (pCMNData->fPointerCaptured)       /* User could have pressed ESC */
       {
        WinQueryPointerPos( HWND_DESKTOP,   /* Where's the mouse pointer?  */
                            &ptlPointer );
        hwndTarget = WinWindowFromPoint( HWND_DESKTOP,  /* Get HWND of ... */
                                         &ptlPointer,/* window under the ... */
                                         TRUE );          /* mouse pointer */
        hwndTargetParent = WinQueryWindow( hwndTarget,   /* and its parent */
                                           QW_PARENT );                                 
                                                                                        
        if ( hwndTarget != HWND_DESKTOP &&  /* Not the desktop             */           
             hwndTarget != NULLHANDLE   &&  /* Valid handle found          */           
             hwndTargetParent != hwnd   &&  /* Not one of my children      */           
             hwndTargetParent != pCMNData->hwndMyParent &&/* Not one of my siblings */
             hwndTarget != hwnd )           /* Not my own window           */           
         {                                  /* Perform the drop            */
          WinQueryWindowText( WinWindowFromID( hwnd,   /* Get text from EF */
                                               IDEF_DROPTEXT),                          
                              sizeof( szDropText ),                                     
                              szDropText );                                             
          WinSetWindowText( hwndTarget,     /* Drop it!                    */
                            szDropText );                                               
         }                                                                              
        else                               /* Can't drop on our own window */
          WinAlarm( HWND_DESKTOP,                                                       
                    WA_ERROR );                                                         

        WinSetCapture( HWND_DESKTOP,        /* Release the mouse           */
                       NULLHANDLE );                                                    
        WinSetPointer( HWND_DESKTOP,        /* Restore the pointer         */
                       pCMNData->hptrPrevious );                                        
        pCMNData->fPointerCaptured = FALSE; /* Reset the captured flag     */
       }
      break;

    case WM_CHAR :                    /* User pressed a key, check for ESC */
      pCMNData = (PCMNDATA) WinQueryWindowPtr( hwnd,
                                               QWL_USER );
      pCharMsg = CHARMSG(&msg);
      if (!(pCharMsg->fs & KC_KEYUP) &&     /* Only want down_stroke       */
           (pCharMsg->vkey == VK_ESC) )     /* Pressed Esc                 */
       {
        if (pCMNData->fPointerCaptured)   /* Currently dragging, cancel it */
         {
          WinSetCapture( HWND_DESKTOP,      /* Release the mouse           */
                         NULLHANDLE );
          WinSetPointer( HWND_DESKTOP,      /* Restore the pointer         */
                         pCMNData->hptrPrevious );
          pCMNData->fPointerCaptured = FALSE;   /* Reset the captured flag */
         }
       }
      else
        mResult = WinDefDlgProc(hwnd, msg, mp1, mp2);        /* Pass it on */
      break;

    case WM_INITDLG :                       /* Perform Startup             */
      pCMNData = (PCMNDATA) mp2;            /* Pointer to common data area */
      pCMNData->hwndMyParent = WinQueryWindow( hwnd,/* Save my parent's HWND */
                                               QW_PARENT );

      WinSendMsg( hwnd,                /* Use our ICON for the system menu */
                  WM_SETICON,
                  MPFROMHWND(pCMNData->hptrIcon),
                  MPFROMLONG(0L) );
      WinSetWindowPtr( hwnd,          /* Save the pointer to our data area */
                       QWL_USER,
                       (PVOID) pCMNData );
      WinSendMsg( WinWindowFromID( hwnd,  /* Limit length of text that ... */
                                   IDEF_DROPTEXT ),        /* can be typed */
                  EM_SETTEXTLIMIT,
                  MPFROMLONG( EF_TEXTLIMIT ),
                  MPFROMLONG( 0L ) );
                
      WinSubclassWindow( WinWindowFromID( hwnd,             /* Subclass EF */
                                          IDEF_DROPTEXT ),
                         (PFNWP)wpEF );
      break;

    default :
      mResult = WinDefDlgProc(hwnd, msg, mp1, mp2);
      break;
   }
  return mResult;
}


/*-------------------------------------------------------------------------* 
 * wpEF:  Subclass the entry field so that we can process the WM_*DRAG     *
 *        messages in the client window procedure.                         *
 *-------------------------------------------------------------------------*/
MRESULT wpEF(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2)
{
 MRESULT mr = (MRESULT) FALSE;              /* Return value                */
 CLASSINFO EFClassInfo;              /* Class information for Entry Fields */

 switch (msg)
  {
   case WM_BEGINDRAG :      /* Starting a drag ... pass it on to our owner */
     WinPostMsg( WinQueryWindow( hwnd,      /* HWND of our owner           */
                                 QW_OWNER ),
                 UM_BEGINDRAG,              /* Our "user defined" message  */
                 mp1,
                 mp2 );
     break;

   case WM_ENDDRAG :        /* Ending the drag ... pass it on to our owner */
     WinPostMsg( WinQueryWindow( hwnd,      /* HWND of our owner           */
                                 QW_OWNER ),
                 UM_ENDDRAG,                /* Our "user defined" message  */
                 mp1,
                 mp2 );
     break;

   default :       /* All other messages go to the default WinProc for EFs */
     WinQueryClassInfo( WinQueryAnchorBlock( hwnd),
                        WC_ENTRYFIELD,
                        &EFClassInfo );
     mr = EFClassInfo.pfnWindowProc( hwnd,  /* Default WinProc for EFs     */
                                     msg,
                                     mp1,
                                     mp2);
     break;
  }

 return mr;
}
