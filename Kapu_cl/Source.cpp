#include <iostream>
#include <windows.h>
#include <PowrProf.h>

#pragma comment(lib, "PowrProf.lib")

#include <thread>
#include <chrono>

#include <nana/gui.hpp>
#include <nana/gui/widgets/label.hpp>
#include <nana/gui/widgets/button.hpp>
#include <nana/gui/widgets/checkbox.hpp>
#include <nana/gui/widgets/textbox.hpp>

using namespace nana;


struct sSingleton {
public:
    bool threadStart(label &_label, textbox &_tbHr, textbox &_tbMn, button &_btnCan, button &_btnApp, button &_btnSle, button &_btnHib, button &_btnShu) {
        if (m_eType == eType::null)
            return false;

        if (m_bThdRun) 
            m_bThdRun = false;
        if (m_thd.joinable())
            m_thd.join();
        m_bThdRun = true;

        m_thd = std::thread(&sSingleton::countingStart, this,
            std::ref(_label), std::ref(_tbHr), std::ref(_tbMn), 
            std::ref(_btnCan), std::ref(_btnApp), std::ref(_btnSle), std::ref(_btnHib), std::ref(_btnShu));

        return true;
    }

    void threadStop(label &_label) {
        if (m_bThdRun) {
            m_bThdRun = false;

            if (m_thd.joinable())
                m_thd.join();

            _label.caption("<bold blue size=30>00:00:00</>");
        }
    }

    void countingStart(label &_label, textbox &_tbHr, textbox &_tbMn,
        button &_btnCan, button &_btnApp, button &_btnSle, button &_btnHib, button &_btnShu) {
        std::chrono::time_point<std::chrono::steady_clock> tpDue    { std::chrono::steady_clock::now() + std::chrono::hours(_tbHr.to_int()) + std::chrono::minutes(_tbMn.to_int()) };
        int                                                iTotalSc { 0 };

        while (m_bThdRun
            && ((iTotalSc = static_cast<int>(std::chrono::duration_cast<std::chrono::seconds>(tpDue - std::chrono::steady_clock::now()).count())) >= 0)) {
            int iDeltaHr { iTotalSc / 3600 },
                iDeltaMn { (iTotalSc % 3600) / 60 },
                iDeltaSc { iTotalSc % 60 };

            std::ostringstream ossHr, ossMn, ossSc;
            ossHr << std::setw(2) << std::setfill('0') << iDeltaHr;
            ossMn << std::setw(2) << std::setfill('0') << iDeltaMn;
            ossSc << std::setw(2) << std::setfill('0') << iDeltaSc;

            _label.caption("<bold red size=30>" + ossHr.str() + ":" + ossMn.str() + ":" + ossSc.str() + "</>");

            ::Sleep(333);
        }

        if (m_bThdRun) {
            _btnCan.enabled(false);
            _btnApp.enabled(true);
            _tbHr.enabled(true);
            _tbMn.enabled(true);
            _btnSle.enabled(true);
            _btnHib.enabled(true);
            _btnShu.enabled(true);

            if (m_eType == eType::shutdown)
                ::system("shutdown -P now");
            else if(m_eType != eType::null)
                ::SetSuspendState((m_eType == eType::hibernate ? TRUE : FALSE), FALSE, TRUE);
        }
    }

    enum eType : int {
        null = -1,
        sleep,
        hibernate,
        shutdown
    };

    void typeSet(eType _eType) {
        m_eType = _eType;
    }
    
    eType            m_eType   { eType::null };
    std::atomic_bool m_bThdRun { false };
    std::thread      m_thd;


    static sSingleton *instanceGet() {
        if (!m_instance)
            m_instance = new sSingleton();
        return m_instance;
    }

    ~sSingleton() {
        delete m_instance;
    }

    static sSingleton *m_instance;
};
sSingleton *sSingleton::m_instance { nullptr };

#define SINGLETON sSingleton::instanceGet()

#ifdef _DEBUG
int main() {
#else
int WINAPI WinMain(HINSTANCE hinstance, HINSTANCE hPrevInstance,
    LPSTR lpCmdLine, int nCmdShow) {
#endif
    // Define a form.
    form fm (API::make_center(600, 400), appear::decorate<appear::minimize>());
    fm.caption(L"Paku");

    // Define text boxes
    textbox tbHr { fm },
            tbMn { fm };
    tbHr.caption("1");
    tbMn.caption("30");
    tbHr.text_align(align::center);
    tbMn.text_align(align::center);
    tbHr.borderless(true);
    tbMn.borderless(true);
    tbHr.bgcolor(color(212, 208, 200));
    tbMn.bgcolor(color(212, 208, 200));
    tbHr.fgcolor(color(0, 0, 0));
    tbMn.fgcolor(color(0, 0, 0));
    tbHr.multi_lines(false);
    tbMn.multi_lines(false);

    // Define a label and display a text.
    label txtCount { fm, "<bold blue size=30>00:00:00</>" };
    txtCount.format(true);

    // Define a button and answer the click event.
    button btnExi { fm, "Exit" },
           btnCan { fm, "Cancel" },
           btnApp { fm, "Apply" },
           btnSle { fm, "Sleep" },
           btnHib { fm, "Hibernate" },
           btnShu { fm, "Shutdown" };

    btnCan.enabled(false);
    btnApp.enabled(false);

    btnSle.bgcolor(color(150, 150, 150));
    btnHib.bgcolor(color(150, 150, 150));
    btnShu.bgcolor(color(150, 150, 150));

    btnExi.events().click([&fm, &txtCount] {
        SINGLETON->threadStop(txtCount);
        fm.close();
    });

    btnCan.events().click([&txtCount, &tbHr, &tbMn, &btnCan, &btnApp, &btnSle, &btnHib, &btnShu] {
        SINGLETON->threadStop(txtCount);
        btnCan.enabled(false);
        btnApp.enabled(true);
        tbHr.enabled(true);
        tbMn.enabled(true);
        btnSle.enabled(true);
        btnHib.enabled(true);
        btnShu.enabled(true);
    });

    btnApp.events().click([&txtCount, &tbHr, &tbMn, &btnCan, &btnApp, &btnSle, &btnHib, &btnShu] {
        if (SINGLETON->threadStart(txtCount, tbHr, tbMn,
            btnCan, btnApp, btnSle, btnHib, btnShu)) {
            btnCan.enabled(true);
            btnApp.enabled(false);
            tbHr.enabled(false);
            tbMn.enabled(false);
            btnSle.enabled(false);
            btnHib.enabled(false);
            btnShu.enabled(false);
        }
    });

    btnSle.events().click([&fm, &btnApp, &btnSle, &btnHib, &btnShu] {
        SINGLETON->typeSet(sSingleton::eType::sleep);
        btnSle.bgcolor(color(255, 255, 255));
        btnHib.bgcolor(color(150, 150, 150));
        btnShu.bgcolor(color(150, 150, 150));
        if (!btnApp.enabled())
            btnApp.enabled(true);
    });

    btnHib.events().click([&fm, &btnApp, &btnSle, &btnHib, &btnShu] {
        SINGLETON->typeSet(sSingleton::eType::hibernate);
        btnSle.bgcolor(color(150, 150, 150));
        btnHib.bgcolor(color(255, 255, 255));
        btnShu.bgcolor(color(150, 150, 150));
        if (!btnApp.enabled())
            btnApp.enabled(true);
        });

    btnShu.events().click([&fm, &btnApp, &btnSle, &btnHib, &btnShu] {
        SINGLETON->typeSet(sSingleton::eType::shutdown);
        btnSle.bgcolor(color(150, 150, 150));
        btnHib.bgcolor(color(150, 150, 150));
        btnShu.bgcolor(color(255, 255, 255));
        if (!btnApp.enabled())
            btnApp.enabled(true);
    });

    // Layout management
    fm.div(
"vert \
<>\
<<><weight=170 txtCount><>>\
<<><weight=360 <margin=10 btnSle><margin=10 btnHib><margin=10 btnShu>><>>\
<<><weight=300 margin=10 <tbHr><tbMn>><>>\
<weight=30<><btnExi><btnCan><btnApp><>>\
<>\
");
    fm["txtCount"] << txtCount;
    fm["btnSle"]   << btnSle;
    fm["btnHib"]   << btnHib;
    fm["btnShu"]   << btnShu;
    fm["tbHr"]     << tbHr;
    fm["tbMn"]     << tbMn;
    fm["btnExi"]   << btnExi;
    fm["btnCan"]   << btnCan;
    fm["btnApp"]   << btnApp;

    fm.collocate();
    fm.show();

    // Start to event loop process, it blocks until the form is closed.
    exec();

    return 0;
}