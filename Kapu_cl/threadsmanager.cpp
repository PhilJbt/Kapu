#include "stdafx.h"


ThreadsManager *ThreadsManager::m_instance { nullptr };


// ThreadsManager class destructor
// Release the memory used by it's own instance.
// 
ThreadsManager::~ThreadsManager() {
    delete m_instance;
}

// ThreadsManager class instantiator
// Initialize a new instance if it does not exist.
// 
ThreadsManager *ThreadsManager::instanceGet() {
    if (!ThreadsManager::m_instance)
        ThreadsManager::m_instance = new ThreadsManager();
    return ThreadsManager::m_instance;
}


// Timer thread starter
// Launch the ThreadsManager::countingStart function.
// 
void ThreadsManager::threadStart(nana::label &_label, nana::textbox &_tbHr, nana::textbox &_tbMn, nana::button &_btnCan,nana::button &_btnApp, nana::button &_btnSle, nana::button &_btnHib, nana::button &_btnShu) {
    // Do not continue if no action type has been set up.
    if (m_eType == eType::null)
        return;

    // If the previous thread is still running (which should not be possible):
    // Make the thread exit from its loop by itself.
    if (m_bThdRun)
        m_bThdRun = false;
    // Wait for the thread to exit by itself.
    if (m_thd.joinable())
        m_thd.join();

    // Force the thread to stay in its loop.
    m_bThdRun = true;

    // Launch the thread.
    m_thd = std::thread(&ThreadsManager::countingStart, this,
        std::ref(_label), std::ref(_tbHr), std::ref(_tbMn),
        std::ref(_btnCan), std::ref(_btnApp), std::ref(_btnSle), std::ref(_btnHib), std::ref(_btnShu)
    );
}

// Timer thread stopper
// Stop the timer thread and reset the text of the timer.
// 
void ThreadsManager::threadStop(nana::label &_label) {
    // Do not continue if no thread is running.
    if (!m_bThdRun)
        return;

    // Make the thread exit from its loop by itself.
    m_bThdRun = false;

    // Wait for the thread to exit by itself.
    if (m_thd.joinable())
        m_thd.join();

    // Reset the counter text by a placeholder.
    _label.caption("<bold blue size=30>00:00:00</>");
}

// Timer counting text updater
// Update the text with the time remaining.
// 
void ThreadsManager::countingStart(nana::label &_label, nana::textbox &_tbHr, nana::textbox &_tbMn, nana::button &_btnCan, nana::button &_btnApp, nana::button &_btnSle, nana::button &_btnHib, nana::button &_btnShu) {
    // Cast due date string value in int
    int iDueHour(_tbHr.to_int()),
        iDueMins(_tbMn.to_int());

    // Declare and initialize the cut-off time.
    std::chrono::time_point<std::chrono::steady_clock> tpDue { std::chrono::steady_clock::now() + std::chrono::hours(iDueHour) + std::chrono::minutes(iDueMins) };
    
    // While seconds remains and the stop of the thread has not been requested.
    int iTotalSc(0);
    do {
        // Calculate how many time remains before the due date
        iTotalSc = static_cast<int>(std::chrono::duration_cast<std::chrono::seconds>(tpDue - std::chrono::steady_clock::now()).count());

        // Calculate the number of hours, minutes and seconds remaining.
        int iDeltaHr { iTotalSc / 3600 },
            iDeltaMn { (iTotalSc % 3600) / 60 },
            iDeltaSc { iTotalSc % 60 };

        // Cast int to stream with leading zeros.
        std::ostringstream ossHr, ossMn, ossSc;
        ossHr << std::setw(2) << std::setfill('0') << iDeltaHr;
        ossMn << std::setw(2) << std::setfill('0') << iDeltaMn;
        ossSc << std::setw(2) << std::setfill('0') << iDeltaSc;

        // Populate the text counter.
        _label.caption("<bold red size=30>" + ossHr.str() + ":" + ossMn.str() + ":" + ossSc.str() + "</>");

        // Relinquish its current use of processor for 1/3s.
        ::Sleep(333);
    } while (iTotalSc >= 0
        && m_bThdRun);

    // If there is no time remaining (which means that the user has not requested that the thread be terminated).
    if (m_bThdRun) {
        // Redundancy of the smooth thread exiting.
        m_bThdRun = false;

        // Disable the "Cancel" button, enable the others buttons and text boxes.
        _btnCan.enabled(false);
        _btnApp.enabled(true);
        _tbHr.enabled(true);
        _tbMn.enabled(true);
        _btnSle.enabled(true);
        _btnHib.enabled(true);
        _btnShu.enabled(true);

        // Perform the requested action.
        switch (m_eType) {
        // Shutdown
        case eType::shutdown:
            ::system("shutdown -P now");
            break;

        // Sleep or Hibernate
        case eType::sleep:
        case eType::hibernate:
            ::SetSuspendState((m_eType == eType::hibernate ? TRUE : FALSE), FALSE, TRUE);
            break;

        // Error
        default:
            throw std::runtime_error("Action to perform has no type.");
            break;
        }
    }
}

// Action type to perform setter
// Store the action to perform ("Sleep", "Hibernate" or "Shutdown").
// 
void ThreadsManager::typeSet(eType _eType) {
    m_eType = _eType;
}