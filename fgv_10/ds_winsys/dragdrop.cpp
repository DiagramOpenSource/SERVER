

#include <windows.h>
#include <ole2.h>
#include <stdio.h>

extern "C" void AgiDoDrag(HGLOBAL handle);

/*
class OleInit {
public:
OleInit() { OleInitialize(NULL); }
~OleInit() { OleUninitialize(); }
} OleInitGlobal;
*/

interface CDropSource : public IDropSource
{
   ULONG m_nCount;
   
   CDropSource() {
      /*OutputDebugString("In IDS's constructor\r\n");*/		
      m_nCount = 0;
   };
   
   ~CDropSource() {
      /*OutputDebugString("In IDS's destructor\r\n");*/
   } ;
   
   STDMETHODIMP QueryInterface (REFIID riid, LPVOID FAR* ppv);
   
   STDMETHODIMP_(ULONG) AddRef();
   STDMETHODIMP_(ULONG) Release();
   
   
   // *** IDropSource methods ***
   STDMETHODIMP QueryContinueDrag (BOOL fEscapePressed, DWORD grfKeyState);
   STDMETHODIMP GiveFeedback (DWORD dwEffect);
   
private:
   
};



interface CDataObject : public IDataObject
{
   unsigned long m_nCount;
   HGLOBAL m_handle;
   HGLOBAL m_handlehtml;
   CDropSource *m_pDropSource;
   
   CDataObject() { m_nCount = 0;m_pDropSource = NULL;m_handle = NULL;m_handlehtml = NULL;};
   CDataObject(CDropSource *pDropSource,HGLOBAL handle,HGLOBAL handlehtml) { m_nCount = 0;m_pDropSource = pDropSource;m_handle = handle;m_handlehtml = handlehtml; };
   
   ~CDataObject() {};
   
   STDMETHODIMP QueryInterface (REFIID riid, void * * ppvObj);
   STDMETHODIMP_(ULONG) AddRef ();
   STDMETHODIMP_(ULONG) Release ();
   // *** IDataObject methods ***
   STDMETHODIMP GetData (LPFORMATETC pFormatetc, LPSTGMEDIUM pmedium);
   STDMETHODIMP GetDataHere (LPFORMATETC pFormatetc, LPSTGMEDIUM pmedium);
   STDMETHODIMP QueryGetData (LPFORMATETC pFormatetc );
   STDMETHODIMP GetCanonicalFormatEtc (LPFORMATETC pFormatetc, LPFORMATETC pFormatetcOut);
   STDMETHODIMP SetData (LPFORMATETC pFormatetc, STGMEDIUM * pmedium, int fRelease);
   STDMETHODIMP EnumFormatEtc (unsigned long dwDirection, LPENUMFORMATETC FAR* ppenumFormatetc );
   STDMETHODIMP DAdvise (FORMATETC * pFormatetc, unsigned long advf, 
      LPADVISESINK pAdvise, unsigned long * pdwConnection);
   STDMETHODIMP DUnadvise (unsigned long dwConnection);
   STDMETHODIMP EnumDAdvise (LPENUMSTATDATA * ppenumAdvise ); 
   
};


STDMETHODIMP_(ULONG) CDataObject::AddRef()
{
   
   ++m_nCount;
   
   return m_nCount;
}


STDMETHODIMP_(ULONG) CDataObject::Release()
{
   
   
   --m_nCount;
   
   return m_nCount;
}



STDMETHODIMP CDataObject::QueryInterface (REFIID riid, void * * ppvObj)
{ 
   
   
   *ppvObj = NULL;     // must set out pointer parameters to NULL
   
   // looking for IUnknown
   if ( riid == IID_IUnknown || riid == IID_IDataObject)
   {
      AddRef();
      //m_pDropSource->AddRef();
      //*ppvObj = m_pDropSource;
      *ppvObj = this;
      return ResultFromScode(S_OK);
   }
   
   
   // looking for IDropSource
   if ( riid == IID_IDropSource)
   {
      m_pDropSource->AddRef();
      *ppvObj=m_pDropSource;
      return ResultFromScode(S_OK);
   }
   
   // Not a supported interface
   return ResultFromScode(E_NOINTERFACE);
   
}

STDMETHODIMP CDataObject::GetData (LPFORMATETC pFormatetc, LPSTGMEDIUM pmedium)
{        
   HGLOBAL handle;
   if (pFormatetc->cfFormat != CF_TEXT)
   {        
      char *buffer[2048];
      int l = GetClipboardFormatName(pFormatetc->cfFormat,(LPTSTR)buffer,sizeof(buffer));
      if (!l)
      {
         return DV_E_FORMATETC;
      }
      buffer[l] = 0;
      if (strcmp((const char *)buffer,"HTML Format"))
      {
         return DV_E_FORMATETC;
      }
      handle = m_handlehtml;
   }
   else
      handle = m_handle;
   if (!handle)
      return STG_E_MEDIUMFULL ;
   
   
   DWORD size = GlobalSize(handle);
   HGLOBAL handle2 = GlobalAlloc(GMEM_MOVEABLE | GMEM_SHARE ,size);
   LPCSTR lpClipData = (char *)GlobalLock(handle);
   if (lpClipData)
   {
      LPCSTR lpClipData2 = (char *)GlobalLock(handle2);
      if (lpClipData2)
      {
         memcpy((void *)lpClipData2,(void *)lpClipData,size);
         GlobalUnlock(handle2);
      }
      GlobalUnlock(handle);
   }
   
   pmedium->tymed = TYMED_HGLOBAL;
   pmedium->hGlobal = handle2;
   pmedium->pUnkForRelease = NULL;
   return S_OK;
}

STDMETHODIMP CDataObject::GetDataHere (LPFORMATETC pFormatetc, LPSTGMEDIUM pmedium)
{
   HGLOBAL handle;
   if (pFormatetc->cfFormat != CF_TEXT)
   {        
      char *buffer[2048];
      int l = GetClipboardFormatName(pFormatetc->cfFormat,(LPTSTR)buffer,sizeof(buffer));
      if (!l)
      {
         return DV_E_FORMATETC;
      }
      buffer[l] = 0;
      if (strcmp((const char *)buffer,"HTML Format"))
      {
         return DV_E_FORMATETC;
      }
      handle = m_handlehtml;
   }
   else
      handle = m_handle;
   if (!handle)
      return STG_E_MEDIUMFULL ;
   
   pmedium->tymed = TYMED_HGLOBAL;
   pmedium->hGlobal = handle;
   pmedium->pUnkForRelease = NULL;
   return S_OK;
}

STDMETHODIMP CDataObject::QueryGetData (LPFORMATETC pFormatetc )
{
   if (pFormatetc->cfFormat == CF_TEXT)
      return S_OK;
   
   char *buffer[2048];
   int l = GetClipboardFormatName(pFormatetc->cfFormat,(LPTSTR)buffer,sizeof(buffer));
   if (!l)
   {
      return DV_E_FORMATETC;
   }
   buffer[l] = 0;
   if (!strcmp((const char *)buffer,"HTML Format"))
   {
      return S_OK;
   }
   return DV_E_FORMATETC;
}

STDMETHODIMP CDataObject::GetCanonicalFormatEtc (LPFORMATETC pFormatetc, LPFORMATETC pFormatetcOut)
{
   
   return NOERROR;
}

STDMETHODIMP CDataObject::SetData (LPFORMATETC pFormatetc, STGMEDIUM * pmedium, int fRelease)
{
   
   return E_NOTIMPL;
}

STDMETHODIMP CDataObject::EnumFormatEtc (unsigned long dwDirection, LPENUMFORMATETC FAR* ppenumFormatetc )
{
   
   if (dwDirection == DATADIR_GET)
   {
      
      //     *ppenumFormatetc = (LPENUMFORMATETC)peformatecs;
      //		return S_OK;
      return OLE_S_USEREG;
   }
   else
      return E_NOTIMPL;
}

STDMETHODIMP CDataObject::DAdvise (FORMATETC * pFormatetc, unsigned long advf,LPADVISESINK pAdvise, unsigned long * pdwConnection)
{
   
   return NOERROR;
}

STDMETHODIMP CDataObject::DUnadvise (unsigned long dwConnection)
{
   
   return NOERROR;
}

STDMETHODIMP CDataObject::EnumDAdvise (LPENUMSTATDATA * ppenumAdvise )
{
   
   return NOERROR;
}

/*****************************************************************************************/
/*****************************************************************************************/
/*****************************************************************************************/
/*****************************************************************************************/
/*****************************************************************************************/
/*****************************************************************************************/


STDMETHODIMP_(ULONG) CDropSource::AddRef()
{
   
   // increment the interface reference count (for debugging only)
   ++m_nCount;
   
   // delegate to the document Object
   return m_nCount;
}

//**********************************************************************
//
// CDropSource::Release
//
// Purpose:
//
//      Decrements the reference count on this interface
//
// Parameters:
//
//      None
//
// Return Value:
//
//      The current reference count on this interface.
//
// Function Calls:
//      Function                    Location
//
//      CSimpleObj::Release         OBJ.CPP
//      OutputDebugString           Windows API
//
// Comments:
//
//      This function subtracts one from the ref count of the interface,
//      and calls then calls CSimpleDoc to decrement its ref.
//      count.
//
//********************************************************************

STDMETHODIMP_(ULONG) CDropSource::Release()
{
   
   // decrement the interface reference count (for debugging only)
   --m_nCount;
   
   // delegate to the document object
   return m_nCount;
}


//**********************************************************************
//
// CDropSource::QueryInterface
//
// Purpose:
//
//      Return a pointer to a requested interface
//
// Parameters:
//
//      REFIID riid         -   ID of interface to be returned
//      LPVOID FAR* ppvObj  -   Location to return the interface
//
// Return Value:
//
//      S_OK                -   Interface supported
//      E_NOINTERFACE       -   Interface NOT supported
//
// Function Calls:
//      Function                    Location
//
//      OutputDebugString           Windows API
//      CSimpleDoc::QueryInterface  DOC.CPP
//
// Comments:
//
//********************************************************************

STDMETHODIMP CDropSource::QueryInterface(REFIID riid, LPVOID FAR* ppvObj)
{    
   
/*                AddRef();				
*ppvObj = this;
return ResultFromScode(S_OK);
   */
   
   *ppvObj = NULL;     // must set out pointer parameters to NULL
   
   if ( riid == IID_IUnknown)
   {
      AddRef();				
      *ppvObj = this;
      return ResultFromScode(S_OK);
   }
   
   // looking for IDropSource
   if ( riid == IID_IDropSource)
   {
      AddRef();				
      *ppvObj = this;
      return ResultFromScode(S_OK);
   }
   
   // Not a supported interface
   return ResultFromScode(E_NOINTERFACE);
   
}


//**********************************************************************
//
// CDropSource::QueryContinueDrag
//
// Purpose:
//
//      Called to determine if a drop should take place or be canceled.
//
// Parameters:
//
//      BOOL fEscapePressed - TRUE if ESCAPE key has been pressed
//      DWORD grfKeyState   - key state
//
// Return Value:
//
//      DRAGDROP_S_CANCEL   - drag operation should be canceled
//      DRAGDROP_S_DROP     - drop operation should be performed
//      S_OK                - dragging should continue
//
//
// Function Calls:
//      Function                    Location
//
//      ResultFromScode             OLE API
//
// Comments:
//
//********************************************************************

STDMETHODIMP CDropSource::QueryContinueDrag (
                                             BOOL    fEscapePressed,
                                             DWORD   grfKeyState
                                             )
{
   if (fEscapePressed)
      return ResultFromScode(DRAGDROP_S_CANCEL);
   else if (!(grfKeyState & MK_LBUTTON))
      return ResultFromScode(DRAGDROP_S_DROP);
   else
      return NOERROR;
}


//**********************************************************************
//
// CDropSource::GiveFeedback
//
// Purpose:
//
//      Called to set cursor feedback
//
// Parameters:
//
//      DWORD dwEffect      - drop operation to give feedback for
//
// Return Value:
//
//      DRAGDROP_S_USEDEFAULTCURSORS  - tells OLE to use standard cursors
//
// Function Calls:
//      Function                    Location
//
//      ResultFromScode             OLE API
//
// Comments:
//
//********************************************************************

STDMETHODIMP CDropSource::GiveFeedback (DWORD dwEffect)
{
   return ResultFromScode(DRAGDROP_S_USEDEFAULTCURSORS);
}


void AgiDoDrag(HGLOBAL handle)
{
   char buffer[10000];
   char buff[512];
   strcpy(buff,"Version:0.9\nStartHTML=-1\nEndHTML=-1\nStartFragment:%05ld\nEndFragment:%05ld\n%s");
   strcpy(buffer,"<!--StartFragment -->\n");
   strcat(buffer,"<TABLE BORDER=1>\n");
   LPSTR lpClip = (char *)GlobalLock(handle);
   int i = strlen(buffer);
   for (LPSTR lpClip2 = lpClip;*lpClip2;lpClip2++)
   {   
      if (*lpClip2 == 10)
      {
         strcpy(buffer+i,"<TR><TD>");
         i += 8;
         for (LPSTR lpClip3 = lpClip;lpClip3 < lpClip2 && *lpClip3 != 13;lpClip3++)
         {
            if (*lpClip3 == 9)
            {
               strcpy(buffer+i,"</TD><TD>");
               i += 9;
            }
            else
            {
               buffer[i] = *lpClip3;
               i++;
               buffer[i] = 0;
            }
         }
         strcpy(buffer+i,"</TD></TR>\n");
         i = strlen(buffer);
         lpClip = lpClip2 + 1;
      }
   }
   strcat(buffer,"</TABLE>\n");
   strcat(buffer,"<!--EndFragment -->\n");
   HGLOBAL handlehtml = GlobalAlloc(GMEM_MOVEABLE | GMEM_SHARE ,strlen(buff)+strlen(buffer)-1);
   LPSTR lpClipData = (char *)GlobalLock(handlehtml);
   sprintf(lpClipData,buff,strlen(buff)-2,strlen(buff)+strlen(buffer)-2,buffer);
   GlobalUnlock(handlehtml);
   
   /*HRESULT r_ole = OleInitialize(NULL);*/
   
   CDropSource DropSource;
   CDataObject DataObject(&DropSource,handle,handlehtml);
   
   //peformatecs = new CEnumFormatEtc;
   
   //DropSource.AddRef();
   
   //peformatecs->AddRef();
   
   DWORD dwEffect = 0;
   if (::DoDragDrop ( &DataObject,&DropSource,DROPEFFECT_COPY,&dwEffect) != DRAGDROP_S_DROP )
   {
      //::MessageBox(NULL,"oops","a",MB_OK);
      
   }
   
   
   
   GlobalFree(handlehtml);
   //peformatecs->Release();
   
   //delete peformatecs;
   
   //peformatecs = NULL;
   
   /*if (r_ole ==  S_OK)
   OleUninitialize();
   */
}


#ifdef COMENTARIO

class CEnumFormatEtc : public IEnumFORMATETC
{
private:
   ULONG           m_iCur;
   ULONG           m_cRef;
   FORMATETC       formato;
   
public:
   CEnumFormatEtc();
   ~CEnumFormatEtc();
   
   //IUnknown members that delegate to m_pUnkRef.
   STDMETHODIMP  QueryInterface(REFIID riid, void * * ppv);
   STDMETHODIMP_(ULONG) AddRef(void);
   STDMETHODIMP_(ULONG) Release(void);
   
   //IEnumFORMATETC members
   STDMETHODIMP Next(ULONG, LPFORMATETC, ULONG *);
   STDMETHODIMP Skip(ULONG);
   STDMETHODIMP Reset(void);
   STDMETHODIMP Clone(IEnumFORMATETC **);
};


typedef CEnumFormatEtc *PCEnumFormatEtc;


PCEnumFormatEtc peformatecs = NULL;



/*
* CEnumFormatEtc::CEnumFormatEtc
* CEnumFormatEtc::~CEnumFormatEtc
*
* Parameters (Constructor):
*  pUnkRef         LPUNKNOWN to use for reference counting.
*  cFE             ULONG number of FORMATETCs in pFE
*  prgFE           LPFORMATETC to the array to enumerate.
*/

CEnumFormatEtc::CEnumFormatEtc()
{
   m_iCur=0;
   m_cRef=0;
   formato.cfFormat = CF_TEXT;
   formato.ptd = NULL;
   formato.dwAspect = NULL;
   formato.lindex = DVASPECT_CONTENT;
   formato.tymed = TYMED_HGLOBAL;
   return;
}


CEnumFormatEtc::~CEnumFormatEtc(void)
{
   return;
}






/*
* CEnumFormatEtc::QueryInterface
* CEnumFormatEtc::AddRef
* CEnumFormatEtc::Release
*
* Purpose:
*  IUnknown members for CEnumFormatEtc object.  For QueryInterface
*  we only return out own interfaces and not those of the data
*  object.  However, since enumerating formats only makes sense
*  when the data object is around, we insure that it stays as
*  long as we stay by calling an outer IUnknown for AddRef
*  and Release.  But since we are not controlled by the lifetime
*  of the outer object, we still keep our own reference count in
*  order to free ourselves.
*/

STDMETHODIMP CEnumFormatEtc::QueryInterface(REFIID riid, void * * ppv)
{
   *ppv=NULL;
   
   /*
   * Enumerators are separate objects, not the data object, so
   * we only need to support out IUnknown and IEnumFORMATETC
   * interfaces here with no concern for aggregation.
   */
   if (IID_IUnknown==riid || IID_IEnumFORMATETC==riid)
      *ppv=this;
   
   //AddRef any interface we'll return.
   if (NULL!=*ppv)
   {
      ((LPUNKNOWN)*ppv)->AddRef();
      return NOERROR;
   }
   
   return ResultFromScode(E_NOINTERFACE);
}


STDMETHODIMP_(ULONG) CEnumFormatEtc::AddRef(void)
{
   ++m_cRef;    
   return m_cRef;
}

STDMETHODIMP_(ULONG) CEnumFormatEtc::Release(void)
{
   ULONG       cRefT;
   
   cRefT=--m_cRef;    
   
   //if (0L==m_cRef)
   //    delete this;
   
   return cRefT;
}


/*
* CEnumFormatEtc::Next
*
* Purpose:
*  Returns the next element in the enumeration.
*
* Parameters:
*  cFE             ULONG number of FORMATETCs to return.
*  pFE             LPFORMATETC in which to store the returned
*                  structures.
*  pulFE           ULONG * in which to return how many we
*                  enumerated.
*
* Return Value:
*  HRESULT         NOERROR if successful, S_FALSE otherwise,
*/

STDMETHODIMP CEnumFormatEtc::Next(ULONG cFE, LPFORMATETC pFE
                                  , ULONG * pulFE)
{
   ULONG               cReturn=0L;
   
   if (NULL==pulFE)
   {
      if (1L!=cFE)
         return ResultFromScode(E_POINTER);
   }
   else
      *pulFE=0L;
   
   if (NULL==pFE || m_iCur >= 1)
      return ResultFromScode(S_FALSE);
   
   if (cFE > 0)
   {
      *pFE = formato;
      cReturn=1;
      m_iCur = 1;
   }
   
   if (NULL!=pulFE)
      *pulFE=cReturn;
   
   return NOERROR;
}







/*
* CEnumFormatEtc::Skip
*
* Purpose:
*  Skips the next n elements in the enumeration.
*
* Parameters:
*  cSkip           ULONG number of elements to skip.
*
* Return Value:
*  HRESULT         NOERROR if successful, S_FALSE if we could not
*                  skip the requested number.
*/

STDMETHODIMP CEnumFormatEtc::Skip(ULONG cSkip)
{    
   return ResultFromScode(S_FALSE);
}






/*
* CEnumFormatEtc::Reset
*
* Purpose:
*  Resets the current element index in the enumeration to zero.
*
* Parameters:
*  None
*
* Return Value:
*  HRESULT         NOERROR
*/

STDMETHODIMP CEnumFormatEtc::Reset(void)
{
   m_iCur=0;
   return NOERROR;
}



/*
* CEnumFormatEtc::Clone
*
* Purpose:
*  Returns another IEnumFORMATETC with the same state as ourselves.
*
* Parameters:
*  ppEnum          LPENUMFORMATETC * in which to return the
*                  new object.
*
* Return Value:
*  HRESULT         NOERROR or a general error value.
*/

STDMETHODIMP CEnumFormatEtc::Clone(LPENUMFORMATETC *ppEnum)
{
   PCEnumFormatEtc     pNew;
   
   *ppEnum=NULL;
   
   //Create the clone
   pNew=new CEnumFormatEtc();
   
   if (NULL==pNew)
      return ResultFromScode(E_OUTOFMEMORY);
   
   pNew->AddRef();
   pNew->m_iCur=m_iCur;
   
   *ppEnum=pNew;
   return NOERROR;
}

#endif