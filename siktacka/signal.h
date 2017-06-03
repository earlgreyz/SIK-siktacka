#ifndef SIK_SIKTACKA_UTILITY_H
#define SIK_SIKTACKA_UTILITY_H

#include <functional>
#include <fstream>
#include <iostream>
#include <signal.h>

/// one holder per signal type
template <int q>
struct Signal
{
    using sfx = void(int );
    typedef std::function<void(int )> fx_t;
    fx_t fx;
    static Signal holder;
    static void handler(int sn) { holder.fx(sn); }
};
template <int q> Signal<q> Signal<q>::holder;

// this is a scope
template <int q>
struct SignalScope
{
    using sfx = void(int);
    sfx *oldfx_;

    SignalScope(typename Signal<q>::fx_t fx)
    {
        Signal<q>::holder.fx = fx;
        oldfx_ = signal(q,&Signal<q>::handler);
    }
    ~SignalScope()
    {
        signal(q,oldfx_);
    }
};

#endif
