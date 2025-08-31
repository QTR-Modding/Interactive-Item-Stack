#pragma once

namespace Hooks {
    void Install();

    class GrabEventSink : public RE::BSTEventSink<RE::TESGrabReleaseEvent> {
        RE::BSEventNotifyControl ProcessEvent(const RE::TESGrabReleaseEvent* event,
                                              RE::BSTEventSource<RE::TESGrabReleaseEvent>*);

    public:
        static void Install();
    };

    class UpdateHook {
    public:
        static void Install();

    private:
        static void thunk();
        static inline REL::Relocation<decltype(thunk)> originalFunction;
    };

}