#pragma once

#include <functional>

namespace libcommon {

struct ScopeGuard {
        ScopeGuard( const std::function<void()>& onInit, const std::function<void()>& onExit ) : m_OnInit( onInit ), m_OnExit( onExit ) {
            m_OnInit();
        }
        ~ScopeGuard() {
            m_OnExit();
        }
    private:
        const std::function<void()> m_OnInit;
        const std::function<void()> m_OnExit;
};

}
