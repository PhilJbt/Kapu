/* Kapu
** Philippe Jaubert - 2022
** https://github.com/PhilJbt/Kapu
** 
** Application to put in standby, in hibernation,
** or shut down a Windows workstation after a certain time.
*/


#include "stdafx.h"


// Entry point
// 
// 
int WINAPI WinMain(HINSTANCE hinstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    // Define the window.
    nana::form fm (nana::API::make_center(600, 400), nana::appear::decorate<nana::appear::minimize>());

    // Set up the executable icon.
    nana::API::window_icon_default(nana::paint::image("Kapu_cl.exe"));
    nana::API::window_icon(fm.handle(), nana::paint::image("Kapu_cl.exe"), nana::paint::image("Kapu_cl.exe"));

    // Set up the window title
    fm.caption(L"Paku");

    // Define the text input boxes to set up the timer.
    nana::textbox tbHr { fm },
                  tbMn { fm };
    tbHr.caption("1");
    tbMn.caption("30");
    tbHr.text_align(nana::align::center);
    tbMn.text_align(nana::align::center);
    tbHr.borderless(true);
    tbMn.borderless(true);
    tbHr.bgcolor(nana::color(212, 208, 200));
    tbMn.bgcolor(nana::color(212, 208, 200));
    tbHr.fgcolor(nana::color(0, 0, 0));
    tbMn.fgcolor(nana::color(0, 0, 0));
    tbHr.multi_lines(false);
    tbMn.multi_lines(false);

    // Define the timer label and populate it with a placeholder.
    nana::label txtCount { fm, "<bold blue size=30>00:00:00</>" };
    txtCount.format(true);

    // Define the buttons.
    nana::button btnExi { fm, "Exit" },
                 btnCan { fm, "Cancel" },
                 btnApp { fm, "Apply" },
                 btnSle { fm, "Sleep" },
                 btnHib { fm, "Hibernate" },
                 btnShu { fm, "Shutdown" };

    // Disable "Cancel" and "Apply" buttons, since they
    // should not be usable before set up the timing.
    btnCan.enabled(false);
    btnApp.enabled(false);

    // Set the background color of the "Sleep",
    // "Hibernate" and "Shutdown" buttons.
    btnSle.bgcolor(nana::color(150, 150, 150));
    btnHib.bgcolor(nana::color(150, 150, 150));
    btnShu.bgcolor(nana::color(150, 150, 150));

    // 1. Bind the callback function,
    // 2. stopping the timer thread and closing the application
    // 3. to the "Exit" button.
    btnExi.events().click([&fm, &txtCount] {
        THREADSMANAGER->threadStop(txtCount);
        fm.close();
    });

    // 1. Bind the callback function,
    // 2. stopping the timer thread, disabling the "Cancel" button
    //    and allowing the user to set up a new timer
    // 3. to the "Cancel" button.
    btnCan.events().click([&txtCount, &tbHr, &tbMn, &btnCan, &btnApp, &btnSle, &btnHib, &btnShu] {
        THREADSMANAGER->threadStop(txtCount);
        btnCan.enabled(false);
        btnApp.enabled(true);
        tbHr.enabled(true);
        tbMn.enabled(true);
        btnSle.enabled(true);
        btnHib.enabled(true);
        btnShu.enabled(true);
    });

    // 1. Bind the callback function,
    // 2. starting the timer thread, enabling the "Cancel" button
    //    and disallowing the user to modify the timer
    // 3. to the "Apply" button.
    btnApp.events().click([&txtCount, &tbHr, &tbMn, &btnCan, &btnApp, &btnSle, &btnHib, &btnShu] {
        THREADSMANAGER->threadStart(txtCount, tbHr, tbMn, btnCan, btnApp, btnSle, btnHib, btnShu);
        btnCan.enabled(true);
        btnApp.enabled(false);
        tbHr.enabled(false);
        tbMn.enabled(false);
        btnSle.enabled(false);
        btnHib.enabled(false);
        btnShu.enabled(false);
    });

    // 1. Bind the callback function,
    // 2. store the type of action to perform, highlight the "Sleep" button
    //    and darken the "Hibernate" and the "Shutdown" buttons
    // 3. to the "Sleep" button.
    btnSle.events().click([&fm, &btnApp, &btnSle, &btnHib, &btnShu] {
        THREADSMANAGER->typeSet(ThreadsManager::eType::sleep);
        btnSle.bgcolor(nana::color(255, 255, 255));
        btnHib.bgcolor(nana::color(150, 150, 150));
        btnShu.bgcolor(nana::color(150, 150, 150));
        if (!btnApp.enabled())
            btnApp.enabled(true);
    });

    // 1. Bind the callback function,
    // 2. store the type of action to perform, highlight the "Hibernate" button
    //    and darken the "Sleep" and the "Shutdown" buttons
    // 3. to the "Hibernate" button.
    btnHib.events().click([&fm, &btnApp, &btnSle, &btnHib, &btnShu] {
        THREADSMANAGER->typeSet(ThreadsManager::eType::hibernate);
        btnSle.bgcolor(nana::color(150, 150, 150));
        btnHib.bgcolor(nana::color(255, 255, 255));
        btnShu.bgcolor(nana::color(150, 150, 150));
        if (!btnApp.enabled())
            btnApp.enabled(true);
        });

    // 1. Bind the callback function,
    // 2. store the type of action to perform, highlight the "Shutdown" button
    //    and darken the "Sleep" and the "Hibernate" buttons
    // 3. to the "Shutdown" button.
    btnShu.events().click([&fm, &btnApp, &btnSle, &btnHib, &btnShu] {
        THREADSMANAGER->typeSet(ThreadsManager::eType::shutdown);
        btnSle.bgcolor(nana::color(150, 150, 150));
        btnHib.bgcolor(nana::color(150, 150, 150));
        btnShu.bgcolor(nana::color(255, 255, 255));
        if (!btnApp.enabled())
            btnApp.enabled(true);
    });

    // Set up the layout management to the form.
    fm.div(
        "vert \
        <>\
        <<><weight=170 txtCount><>>\
        <<><weight=360 <margin=10 btnSle><margin=10 btnHib><margin=10 btnShu>><>>\
        <<><weight=300 margin=10 <tbHr><tbMn>><>>\
        <weight=30<><btnExi><btnCan><btnApp><>>\
        <>\
    ");

    // Replace keywords from the form by serialized objects.
    fm["txtCount"] << txtCount;
    fm["btnSle"]   << btnSle;
    fm["btnHib"]   << btnHib;
    fm["btnShu"]   << btnShu;
    fm["tbHr"]     << tbHr;
    fm["tbMn"]     << tbMn;
    fm["btnExi"]   << btnExi;
    fm["btnCan"]   << btnCan;
    fm["btnApp"]   << btnApp;

    // Calculate and organise the place taken 
    // by each element.
    fm.collocate();

    // Show the form into the Nana window.
    fm.show();

    // Start to event loop process,
    // it blocks until the form is closed.
    nana::exec();

    // Close the application
    return 0;
}