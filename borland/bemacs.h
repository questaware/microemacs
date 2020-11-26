//---------------------------------------------------------------------------
#ifndef bemacsH
#define bemacsH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>

#ifndef ASLEEP
#include "base.h"
#endif

//---------------------------------------------------------------------------
class TForm1 : public TForm
{
__published:	// IDE-managed Components

    void __fastcall DoKeyDown(TObject *Sender, WORD &Key,
          TShiftState Shift);
protected:
     DYNAMIC void __fastcall Paint();
     
private:	// User declarations

    TRect hrect;
    TRect eeolrect;
    TRect cursor;

    Set32 cap_set;

    AnsiString caption;
    TRect      rect;

     bool in_paint;

     int y_row;
     int y_coord;
     int next_y_coord;
     int y_step;		/* y step in pixels */
     int x_step;		/* x step in pixels */

     void __fastcall MyPaint(TObject* sdr);


public:		// User declarations
    __fastcall TForm1(TComponent* Owner);

  /*virtual void __fastcall Paint(TObject * owner);*/
     void __fastcall TForm1::PaintRow(int myrow, const short * src,
					int xstt=-1, int xlim=1000);
     void __fastcall TForm1::EEol(int myrow, int mycol);
     void __fastcall TForm1::EEop();
     void __fastcall TForm1::Scroll(int sdist);
     void __fastcall TForm1::Update(TObject* sdr);
                                                  
     short * scrsrc[200];
     TColor scrbackground[200];
     int textrow;
     int textcol;
     int noo_row;
     int noo_col;
};
//---------------------------------------------------------------------------
extern PACKAGE TForm1 *Form1;
//---------------------------------------------------------------------------
#endif
