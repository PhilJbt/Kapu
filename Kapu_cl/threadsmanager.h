#pragma once


// Class managing threads used
// to start the timer until an action
// (sleep, hibernate or shutdown) is performed.
class ThreadsManager {
public:
    enum eType : int {
        null = -1,
        sleep,
        hibernate,
        shutdown
    };

    static ThreadsManager *instanceGet();
    void threadStart(nana::label &_label, nana::textbox &_tbHr, nana::textbox &_tbMn, nana::button &_btnCan,
        nana::button &_btnApp, nana::button &_btnSle, nana::button &_btnHib, nana::button &_btnShu
    );
    void threadStop(nana::label &_label);
    void countingStart(nana::label &_label, nana::textbox &_tbHr, nana::textbox &_tbMn, nana::button &_btnCan,
        nana::button &_btnApp, nana::button &_btnSle, nana::button &_btnHib, nana::button &_btnShu
    );
    void typeSet(eType _eType);

private:
    ~ThreadsManager();

    static ThreadsManager *m_instance;

    eType            m_eType   { eType::null };
    std::atomic_bool m_bThdRun { false };
    std::thread      m_thd;
};