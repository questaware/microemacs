#ifndef REDIRECT_H_INCLUDED__
#define REDIRECT_H_INCLUDED__

class CRedirect
{
public:

	//----------------------------------------------------------------------
	//	constructor
	//----------------------------------------------------------------------
	CRedirect();

	//----------------------------------------------------------------------
	//	destructor
	//----------------------------------------------------------------------
	virtual ~CRedirect();

	//----------------------------------------------------------------------
	//	public member functions
	//----------------------------------------------------------------------
	virtual Cc  Create(LPCTSTR szCommand, int pipesz = 0, bool conn_stdin= 0);
	virtual int Read(int * offset, TCHAR * PipeData, int bufsz);
		int Write(TCHAR * PipeData, int stt, int lim);
	        Cc  CloseWrite();
	        Cc  Close();
	virtual	void Stop();

protected:
	//----------------------------------------------------------------------
	//	member functions
	//----------------------------------------------------------------------
	void  AppendText(LPCTSTR Text);
	void  PeekAndPump();
	void  SetSleepInterval(DWORD dwMilliseconds);

	//----------------------------------------------------------------------
	//	member data
	//----------------------------------------------------------------------
	bool		m_bStopped;
	DWORD		m_dwSleepMilliseconds;
	LPCTSTR		m_szCommand;
private:
	HANDLE		    DownReadHandle;
	HANDLE		    DownWriteHandle;
	HANDLE		    UpReadHandle;
	HANDLE		    UpWriteHandle;
	PROCESS_INFORMATION ProcessInfo;
	TCHAR               m_szCurrentDirectory[132];

	bool stopping;

	int next_offset;
	int next_end;

	TCHAR schar;
};

#endif	// REDIRECT_H_INCLUDED__
