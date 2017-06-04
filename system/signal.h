#ifndef SIK_SIKTACKA_SIGNAL_H
#define SIK_SIKTACKA_SIGNAL_H

#include <functional>
#include <fstream>
#include <iostream>
#include <signal.h>

/// one holder per signal type
template<int Q>
struct Signal {
    using sfx = void(int);
    typedef std::function<void(int)> fx_t;
    fx_t fx;
    static Signal holder;

    static void handler(int sn) { holder.fx(sn); }
};

template<int Q> Signal<Q> Signal<Q>::holder;

// this is a scope
template<int Q>
struct SignalScope {
    using sfx = void(int);
    sfx *oldfx_;

    SignalScope(typename Signal<Q>::fx_t fx) {
        Signal<Q>::holder.fx = fx;
        oldfx_ = signal(Q, &Signal<Q>::handler);
    }

    ~SignalScope() {
        signal(Q, oldfx_);
    }
};

#endif
