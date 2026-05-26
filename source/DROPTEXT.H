/*-------------------------------------------------------------------------*
 * Data structures.                                                        *
 *-------------------------------------------------------------------------*/
typedef struct _CMNDATA                     /* Common data area            */
 {
  USHORT   cbData;                          /* Size of this struct         */
  HWND     hwndMyParent;                    /* Window handle of Frame      */
  HPOINTER hptrDropText;                    /* "Allow Drop" icon           */
  HPOINTER hptrNoDrop;                      /* "Prevent Drop" icon         */
  HPOINTER hptrPrevious;                    /* Normal pointer              */
  HPOINTER hptrIcon;                        /* Icon for the system menu    */
  BOOL     fPointerCaptured;                /* Flag: dragging?             */
 } CMNDATA;
typedef CMNDATA * PCMNDATA;

/*-------------------------------------------------------------------------* 
 * Constants.                                                              *
 *-------------------------------------------------------------------------*/
#define EF_TEXTLIMIT     100L             /* Max chars for the Entry field */

/*-------------------------------------------------------------------------* 
 * PM resources.                                                           *
 *-------------------------------------------------------------------------*/
#define IDDLG_DROPTEXT   100                /* Dialog Box                  */
#define IDEF_DROPTEXT    101                /* Entry Field                 */
#define IDPTR_DROPTEXT   102                /* "Allow Drop" icon           */
#define IDPTR_NODROP     103                /* "Prevent Drop" icon         */
#define IDICN_DROPTEXT   104                /* Icon for system menu        */

/*-------------------------------------------------------------------------* 
 * Private PM messages                                                     *
 *-------------------------------------------------------------------------*/
#define UM_BEGINDRAG  (WM_USER+1)           /* Begin a Drag                */
#define UM_ENDDRAG    (WM_USER+2)           /* End a Drag (i.e. Drop)      */

/*-------------------------------------------------------------------------* 
 * Prototypes.                                                             *
 *-------------------------------------------------------------------------*/
INT main( VOID );
MRESULT EXPENTRY wpDlg (HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2);
MRESULT EXPENTRY wpEF (HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2);
