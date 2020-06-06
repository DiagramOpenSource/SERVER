
#include <fgvsys.h>

#include <windows.h>

/***********************************************************************

  PortInitialize (LPTSTR lpszPortName)

***********************************************************************/

BYTE tmpl[1024];
int puntol = 0;

extern int TeclaScanner;
extern int ScannerNoBorra;
static int Bloqueado = 0,procesando = 0;
HANDLE hPortScanner = INVALID_HANDLE_VALUE;

extern unsigned int salida_caracter;

#define MAX_MESSAGES 512
BYTE *tmpx[MAX_MESSAGES];
int PuntoMsg = 0;
int PuntoRd = 0;
int TotMsg = 0;
int TotRd = 0;

WORD i_CommMessage = 0;

extern HWND principal_w;
HWND hMainWnd = NULL;
HANDLE PortInitialize (LPTSTR lpszPortName);
void PortWrite (HANDLE hPort,BYTE Byte);
BOOL PortClose (HANDLE hPort);
int LeePort(HANDLE hPort,int max,char *buf);

DWORD PortReadThread (LPVOID lpvoid);


static HANDLE hReadThread[25];
static int nibuf = 0;
static char *ibuffer[25];
static int  sbuffer[25];
static int  xbuffer[25];
static int  cbuffer[25];
static HANDLE hbuffer[25];


HANDLE PortInitialize (LPTSTR lpszPortName)
{
  int i;
  DWORD dwError,
        dwThreadID;
  DCB PortDCB;
  COMMTIMEOUTS CommTimeouts;
  char Portname[512];
  int bauds = 9600;
  int parity = 0;
  int bytesize = 8;
  int stopbits = 1;
  int j,pp;
  HANDLE hPort = INVALID_HANDLE_VALUE;
  HANDLE hTh;
  int sizebuf = -1;

  strcpy(Portname,lpszPortName);
  j = 0;
  pp = 0;
  for (i = 0;Portname[i];i++)
  {
	  if (Portname[i] == ':')
	  {
		  Portname[i] = 0;
		  j = 1;
		  pp = i+1;
	  }
	  else if (Portname[i] == ',' || !Portname[i+1])
	  {
		  if (Portname[i] == ',')
		      Portname[i] = 0;
		  switch(j)
		  {
		  case 1:
			  bauds = atoi(Portname+pp);
			  break;
		  case 2:			  
			  switch(Portname[pp])
			  {
			  case 'O':
				  parity = 1;
				  break;
			  case 'E':
				  parity = 2;
				  break;
			  case 'M':
				  parity = 3;
				  break;
			  case 'S':
				  parity = 4;
				  break;
			  }
			  break;
		  case 3:
			  bytesize = atoi(Portname+pp);
			  break;
		  case 4:
			  stopbits = atoi(Portname+pp);
			  if (stopbits == 1)
				  stopbits = 0;
			  break;
		  case 5:
			  sizebuf = atoi(Portname+pp);
			  break;
		  }
		  j++;
		  pp = i + 1;
	  }
  }
  //strcat(Portname,":");

  // Open the serial port.
  hPort = CreateFile (Portname, // Pointer to the name of the port
                      GENERIC_READ | GENERIC_WRITE,
                                    // Access (read/write) mode
                      0,            // Share mode
                      NULL,         // Pointer to the security attribute
                      OPEN_EXISTING,// How to open the serial port
                      0,            // Port attributes
                      NULL);        // Handle to port with attribute
                                    // to copy

  // If it fails to open the port, return FALSE.
  if ( hPort == INVALID_HANDLE_VALUE ) 
  {
    // Could not open the port.
    MessageBox (hMainWnd, TEXT("No se puede abrir el SCANNER"), 
                TEXT(Portname), MB_OK);
    dwError = GetLastError ();
    return hPort;
  }  

  PortDCB.DCBlength = sizeof (DCB);     

  // Get the default port setting information.
  GetCommState (hPort, &PortDCB);

  // Change the DCB structure settings.
  PortDCB.BaudRate = bauds;              // Current baud 
  PortDCB.fBinary = TRUE;               // Binary mode; no EOF check 
  PortDCB.fParity = FALSE;               // Enable parity checking. 
  PortDCB.fOutxCtsFlow = FALSE;         // No CTS output flow control 
  PortDCB.fOutxDsrFlow = FALSE;         // No DSR output flow control 
  PortDCB.fDtrControl = DTR_CONTROL_ENABLE; 
                                        // DTR flow control type 
  PortDCB.fDsrSensitivity = FALSE;      // DSR sensitivity 
  PortDCB.fTXContinueOnXoff = TRUE;     // XOFF continues Tx 
  PortDCB.fOutX = FALSE;                // No XON/XOFF out flow control 
  PortDCB.fInX = FALSE;                 // No XON/XOFF in flow control 
  PortDCB.fErrorChar = FALSE;           // Disable error replacement. 
  PortDCB.fNull = FALSE;                // Disable null stripping. 
  PortDCB.fRtsControl = RTS_CONTROL_ENABLE;//RTS_CONTROL_ENABLE; 
                                        // RTS flow control 
  PortDCB.fAbortOnError = FALSE;        // Do not abort reads/writes on 
                                        // error.
  PortDCB.ByteSize = bytesize;                 // Number of bits/bytes, 4-8 
  PortDCB.Parity = parity;            // 0-4=no,odd,even,mark,space 
  PortDCB.StopBits = stopbits;        // 0,1,2 = 1, 1.5, 2 

  // Configure the port according to the specifications of the DCB 
  // structure.
  if (!SetCommState (hPort, &PortDCB))
  {
    // Could not create the read thread.
    MessageBox (hMainWnd, TEXT("Unable to configure the serial port"), 
                TEXT("Error"), MB_OK);
    dwError = GetLastError ();
    return INVALID_HANDLE_VALUE;
  }  

  // Retrieve the time-out parameters for all read and write operations
  // on the port. 
  GetCommTimeouts (hPort, &CommTimeouts);

  // Change the COMMTIMEOUTS structure settings.
  CommTimeouts.ReadIntervalTimeout = MAXDWORD;  
  CommTimeouts.ReadTotalTimeoutMultiplier = 0;  
  CommTimeouts.ReadTotalTimeoutConstant = 0;    
  CommTimeouts.WriteTotalTimeoutMultiplier = 10;  
  CommTimeouts.WriteTotalTimeoutConstant = 1000;    

  // Set the time-out parameters for all read and write operations
  // on the port. 
  if (!SetCommTimeouts (hPort, &CommTimeouts))
  {
    // Could not create the read thread.
    MessageBox (hMainWnd, TEXT("Unable to set the time-out parameters"), 
                TEXT("Error"), MB_OK);
    dwError = GetLastError ();
    return INVALID_HANDLE_VALUE;
  }

  // Direct the port to perform extended functions SETDTR and SETRTS.
  // SETDTR: Sends the DTR (data-terminal-ready) signal.
  // SETRTS: Sends the RTS (request-to-send) signal. 
  EscapeCommFunction (hPort, SETDTR);
  EscapeCommFunction (hPort, SETRTS);

  PuntoMsg = 0;
  PuntoRd = 0;
  TotMsg = 0;
  TotRd = 0;
  for (i = 0;i < MAX_MESSAGES;i++)
  {
	  tmpx[i] = NULL;
  }
  //PortWrite((BYTE)13);

  if (sizebuf > 0)
  {
	  for (i = 0;i < nibuf;i++)
	  {
		  if (hbuffer[i] == INVALID_HANDLE_VALUE || !ibuffer[i])
		  {
			  break;
		  }
	  }
	  if (i >= 25)
	  {
		  if (ibuffer[i])
		  {
			  free(ibuffer[i]);
			  ibuffer[i] = NULL;
		  }
		  i = 24;
	  }
	  if (i >= nibuf)
	  {
		  nibuf = i + 1;
	  }
	  hbuffer[i] = hPort;
	  ibuffer[i] = malloc(sizebuf+1);
	  sbuffer[i] = sizebuf;
	  xbuffer[i] = 0;
	  cbuffer[i] = 0;
  }

  // Create a read thread for reading data from the communication port.

  if (hTh = CreateThread (NULL, 0, (LPTHREAD_START_ROUTINE)PortReadThread,(LPVOID)hPort, 0, 
                                  &dwThreadID))
  {
	  if (sizebuf > 0)
	  {
		  hReadThread[i] = hTh;
	  }
	  else
	  {
		  CloseHandle (hTh);
	  }
  }
  else
  {
    // Could not create the read thread.
    MessageBox (hMainWnd, TEXT("Unable to create the read thread"), 
                TEXT("Error"), MB_OK);
    dwError = GetLastError ();

   if (sizebuf > 0)
   {
   	  hbuffer[i] = INVALID_HANDLE_VALUE;
	  free(ibuffer[i]);
	  sbuffer[i] = 0;
	  if ((i+1) == nibuf)
	  {
		  nibuf--;
	  }
   }

    return INVALID_HANDLE_VALUE;
  }  
  return hPort;
}


/***********************************************************************

  PortWrite (BYTE Byte)

***********************************************************************/
void PortWrite (HANDLE hPort,BYTE Byte)
{
  DWORD dwError,
        dwNumBytesWritten;

  if (!WriteFile (hPort,              // Port handle
                  &Byte,              // Pointer to the data to write 
                  1,                  // Number of bytes to write
                  &dwNumBytesWritten, // Pointer to the number of bytes 
                                      // written
                  NULL))              // Must be NULL for Windows CE
  {
    // WriteFile failed. Report error.
    dwError = GetLastError ();
  }
}



/***********************************************************************

  PortReadThread (LPVOID lpvoid)

***********************************************************************/
DWORD PortReadThread (LPVOID lpvoid)
{
  BYTE Byte;
  DWORD dwCommModemStatus,
        dwBytesTransferred;
  HANDLE hPort = (HANDLE)lpvoid;
  int i;
  char *pbuff = NULL;
  int nbuff = 0;
  int *xbuff = NULL;
  int *semaf = NULL;  
  HANDLE *phandle = NULL;
  int escanner = 0;
  

  for (i = 0;i < nibuf;i++)
  {
	  if (hPort == hbuffer[i] && ibuffer[i])
	  {
		  pbuff = ibuffer[i];
		  nbuff = sbuffer[i];
		  xbuffer[i] = 0;
		  xbuff = &(xbuffer[i]);
		  semaf = &(cbuffer[i]);
		  phandle = &(hbuffer[i]);
		  *semaf = 0;
		  break;
	  }
  }
  if (hPort == hPortScanner)
  {
	  escanner = 1;
  }
  
  // Specify a set of events to be monitored for the port.
  SetCommMask (hPort, EV_RXCHAR /*| EV_CTS | EV_DSR | EV_RLSD | EV_RING*/);

  while ((escanner && hPortScanner != INVALID_HANDLE_VALUE) || (!escanner && phandle && *phandle != INVALID_HANDLE_VALUE))
  {
    // Wait for an event to occur for the port.
    WaitCommEvent (hPort, &dwCommModemStatus, 0);

    // Re-specify the set of events to be monitored for the port.
    SetCommMask (hPort, EV_RXCHAR /*| EV_CTS | EV_DSR | EV_RING*/);

    if (dwCommModemStatus & EV_RXCHAR) 
    {
      // Loop for waiting for the data.
      do 
      {
        // Read the data from the serial port.
        ReadFile (hPort, &Byte, 1, &dwBytesTransferred, 0);

		if ((escanner && hPortScanner == INVALID_HANDLE_VALUE) || (!escanner && (!phandle || *phandle == INVALID_HANDLE_VALUE)))
		{
			break;
		}

        // Display the data read.
        if (dwBytesTransferred == 1)
		{		   
           //ProcessChar (Byte);
			if (hPort == hPortScanner && TeclaScanner)
			{
				if (Byte == 13 || Byte == 10 /*|| puntol == 13*/)
				{
					while(Bloqueado)
					{
						Sleep(1);
					}
					procesando = 1;
					if (tmpx[PuntoMsg])
						free(tmpx[PuntoMsg]);
					tmpx[PuntoMsg] = malloc(puntol+1);
					memcpy(tmpx[PuntoMsg],tmpl,puntol);					
					tmpx[PuntoMsg][puntol] = 0;
					PuntoMsg++;
					TotMsg++;
					if (PuntoMsg >= MAX_MESSAGES)
					{
						PuntoMsg = 0;
					}
					puntol = 0;
					
					/*
					if (hMainWnd && i_CommMessage)
					{
						PostMessage(hMainWnd,WM_COMMAND,i_CommMessage,(LPARAM)PuntoMsg);
					}
					*/
					if (i_CommMessage)
					{
						/*salida_caracter = i_CommMessage;*/
						//pon_pulsado(i_CommMessage);						
						/*
						ptec(i_CommMessage);
						*/
						PostMessage(hMainWnd,WM_COMMAND,999,(LPARAM)i_CommMessage);
					}
					procesando = 0;
				}
				else
				{
					tmpl[puntol] = Byte;
					puntol++;
					tmpl[puntol] = 0;
				}
			}
			else
			{
				if (semaf && pbuff && xbuff && *xbuff < nbuff)
				{
					while(*semaf == 2)
					{
						Sleep(1);
					}
					*semaf = 1;
					pbuff[*xbuff] = Byte;
					(*xbuff)++;
					pbuff[*xbuff] = 0;
					*semaf = 0;
				}
			}
		}

      } while (dwBytesTransferred == 1);
    }
  }

  return 0;
}


int LeePort(HANDLE hPort,int max,char *buf)
{
  int i;
  char *pbuff = NULL;
  int nbuff = 0;
  int *xbuff = NULL;
  int *semaf = NULL;  
  int r = 0;

  Sleep(10); 
  for (i = 0;i < nibuf;i++)
  {
	  if (hPort == hbuffer[i] && ibuffer[i])
	  {
		  pbuff = ibuffer[i];
		  nbuff = sbuffer[i];		  
		  xbuff = &(xbuffer[i]);
		  semaf = &(cbuffer[i]);		  
		  break;
	  }
  }  
  if (semaf && pbuff && xbuff && *xbuff < nbuff)
  {
		while(*semaf == 1)
		{
			Sleep(1);
		}
		*semaf = 2;
		i = (*xbuff);
		if (i)
		{
			if (i > max)
				i = max;
			r = i;
			i++;
			memcpy(buf,pbuff,i);
			buf[i] = 0;		
			if (r < (*xbuff))
			{
				i = (*xbuff) - r;
				memcpy(pbuff,pbuff+r,i);
				pbuff[i] = 0;
				(*xbuff) = i;
			}
			else
			{
				*xbuff = 0;
			}
		}
		*semaf = 0;
  }
  return r;
}


/***********************************************************************

  PortClose (HANDLE hCommPort)

***********************************************************************/
BOOL PortClose (HANDLE hPort)
{
  DWORD dwError;  
  int i;
  HANDLE hTh = INVALID_HANDLE_VALUE;
  

  if (hPort != INVALID_HANDLE_VALUE)
  {
    // Close the communication port.
	for (i = 0;i < nibuf;i++)
	{
		  if (hPort == hbuffer[i] && ibuffer[i])
		  {
			  hTh = hReadThread[i];
			  hReadThread[i] = INVALID_HANDLE_VALUE;
   			  hbuffer[i] = INVALID_HANDLE_VALUE;
			  if (ibuffer[i])
			  {
				free(ibuffer[i]);
			  }
			  sbuffer[i] = 0;
			  if ((i+1) == nibuf)
			  {
				  nibuf--;
			  }
			  break;
		  }
	}

	if (hTh != INVALID_HANDLE_VALUE)
	{
		TerminateThread(hTh,0);
		CloseHandle (hTh);
	}

    if (!CloseHandle (hPort))
    {
      dwError = GetLastError ();
      return FALSE;
    }
    else
    {	  
	  if (hPort == hPortScanner)
	  {
		  hMainWnd = NULL;
		  hPort = INVALID_HANDLE_VALUE;
		  for (i = 0;i < MAX_MESSAGES;i++)
		  {
			  if (tmpx[i])
				  free(tmpx[i]);
			  tmpx[i] = NULL;
		  }
		  PuntoMsg = 0;
		  PuntoRd = 0;
		  TotMsg = 0;
		  TotRd = 0;
		  hPortScanner = INVALID_HANDLE_VALUE;
	  }
      return TRUE;
    }
  }

  return FALSE;
}

int ScannerGet()
{
	if (hPortScanner == INVALID_HANDLE_VALUE)
		return 0;
	hMainWnd = principal_w;
	i_CommMessage = TeclaScanner;
	return 0;
}

int ScannerUnGet()
{
	if (hPortScanner == INVALID_HANDLE_VALUE)
		return 0;
	i_CommMessage = 0;
	hMainWnd = NULL;
	return 0;
}

int DameScanner(char *qo)
{
	if (hPortScanner == INVALID_HANDLE_VALUE)
		return 0;
	while(procesando)
	{
		Sleep(1);
	}
	Bloqueado = 1;
    if (TotMsg == TotRd)
	{
		Bloqueado = 0;
		return 0;
	}
	if (PuntoRd == PuntoMsg || !tmpx[PuntoRd])
	{
		PuntoRd = PuntoMsg;
		TotRd = TotMsg;
		Bloqueado = 0;
		return 0;
	}
	if (ScannerNoBorra)
	{
		quitab(qo);
		strcat(qo,tmpx[PuntoRd]);
	}
	else
	{
		strcpy(qo,tmpx[PuntoRd]);
	}
	free(tmpx[PuntoRd]);
	tmpx[PuntoRd] = NULL;
	PuntoRd++;
	if (PuntoRd >= MAX_MESSAGES)
	{
		PuntoRd = 0;
	}
	Bloqueado = 0;
	return strlen(qo);
}
 
