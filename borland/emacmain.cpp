//---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop
USERES("emacmain.res");
USEFORM("bemacs.cpp", Form1);
USEUNIT("..\cppsrc\window.cpp");
USEUNIT("..\cppsrc\word.cpp");
USEUNIT("..\cppsrc\basic.cpp");
USEUNIT("..\cppsrc\bind.cpp");
USEUNIT("..\cppsrc\buffer.cpp");
USEUNIT("..\cppsrc\char.cpp");
USEUNIT("..\cppsrc\crypt.cpp");
USEUNIT("..\cppsrc\display.cpp");
USEUNIT("..\cppsrc\dolock.cpp");
USEUNIT("..\cppsrc\eval.cpp");
USEUNIT("..\cppsrc\exec.cpp");
USEUNIT("..\cppsrc\execasm.cpp");
USEUNIT("..\cppsrc\file.cpp");
USEUNIT("..\cppsrc\fileio.cpp");
USEUNIT("..\cppsrc\input.cpp");
USEUNIT("..\cppsrc\isearch.cpp");
USEUNIT("..\cppsrc\line.cpp");
USEUNIT("..\cppsrc\main.cpp");
USEUNIT("..\cppsrc\malloc.cpp");
USEUNIT("..\cppsrc\map.cpp");
USEUNIT("..\cppsrc\mouse.cpp");
USEUNIT("..\cppsrc\random.cpp");
USEUNIT("..\cppsrc\re.cpp");
USEUNIT("..\cppsrc\region.cpp");
USEUNIT("..\cppsrc\search.cpp");
USEUNIT("msdir.cpp");
USEUNIT("sem.cpp");
USEUNIT("..\src\logmsg.c");
//---------------------------------------------------------------------------
LPSTR wm_param;
void myinit(LPSTR param);
//---------------------------------------------------------------------------
WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR param, int)
{
    try
    {   myinit(param);
        Application->Initialize();
        Application->CreateForm(__classid(TForm1), &Form1);
        Application->Run();
    }
    catch (Exception &exception)
    {
        Application->ShowException(&exception);
    }
    return 0;
}
//---------------------------------------------------------------------------

void myinit(LPSTR param)

{ wm_param = param;
}

//---------------------------------------------------------------------------

