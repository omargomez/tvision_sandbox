//
//  main.cpp
//  tv_template_test
//
//  Created by Omar Eduardo Gomez Padilla on 14/01/21.
//

#define Uses_TKeys
#define Uses_TApplication
#define Uses_TEvent
#define Uses_TRect
#define Uses_TDialog
#define Uses_TStaticText
#define Uses_TButton
#define Uses_TMenuBar
#define Uses_TSubMenu
#define Uses_TMenuItem
#define Uses_TStatusLine
#define Uses_TStatusItem
#define Uses_TStatusDef
#define Uses_TDeskTop
#include <tvision/tv.h>

const int GreetThemCmd = 100;

class TTemplateApp : public TApplication
{

public:

    TTemplateApp();

    virtual void handleEvent( TEvent& event );
    static TMenuBar *initMenuBar( TRect );
    static TStatusLine *initStatusLine( TRect );

private:

    void greetingBox();
};

TTemplateApp::TTemplateApp() :
    TProgInit( &TTemplateApp::initStatusLine,
               &TTemplateApp::initMenuBar,
               &TTemplateApp::initDeskTop
             )
{
}

void TTemplateApp::greetingBox()
{
    TDialog *d = new TDialog(TRect( 25, 5, 55, 16 ), "Hello, World!" );

    d->insert( new TStaticText( TRect( 3, 5, 15, 6 ), "Como andas?" ) );
    d->insert( new TButton( TRect( 16, 2, 28, 4 ), "Terrific", cmCancel, bfNormal ) );
    d->insert( new TButton( TRect( 16, 4, 28, 6 ), "Ok", cmCancel, bfNormal ) );
    d->insert( new TButton( TRect( 16, 6, 28, 8 ), "Lousy", cmCancel, bfNormal ) );
    d->insert( new TButton( TRect( 16, 8, 28, 10 ), "Cancel", cmCancel, bfNormal ) );

    deskTop->execView( d );
    destroy(d);
}

void TTemplateApp::handleEvent( TEvent& event )
{
    TApplication::handleEvent( event );
    if( event.what == evCommand )
        {
        switch( event.message.command )
            {
            case GreetThemCmd:
                greetingBox();
                clearEvent( event );
                break;
            default:
                break;
            }
        }
}

TMenuBar *TTemplateApp::initMenuBar( TRect r )
{

    r.b.y = r.a.y+1;

    return new TMenuBar( r,
      *new TSubMenu( "~H~ello", kbAltH ) +
        *new TMenuItem( "~G~reeting...", GreetThemCmd, kbAltG ) +
         newLine() +
        *new TMenuItem( "E~x~it", cmQuit, cmQuit, hcNoContext, "Alt-X" )
        );

}

TStatusLine *TTemplateApp::initStatusLine( TRect r )
{
    r.a.y = r.b.y-1;
    return new TStatusLine( r,
        *new TStatusDef( 0, 0xFFFF ) +
            *new TStatusItem( "~Alt-X~ Exit", kbAltX, cmQuit ) +
            *new TStatusItem( 0, kbF10, cmMenu )
            );
}

int main()
{
    TTemplateApp helloWorld;
    helloWorld.run();
    return 0;
}
