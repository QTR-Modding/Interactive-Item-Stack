#include "Hooks.h"

#include "Manager.h"
#include "Raycast.h"
#include "ScreenLog.h"
#include "DrawDebug.h"
RE::BSEventNotifyControl Hooks::CrosshairEventSink::ProcessEvent(const SKSE::CrosshairRefEvent* event,
                                                                 RE::BSTEventSource<SKSE::CrosshairRefEvent>*) {
    if (!event) {
        return RE::BSEventNotifyControl::kContinue;
    }

    if (!event->crosshairRef) {
        return RE::BSEventNotifyControl::kContinue;
    }

    auto ref = event->crosshairRef.get();

    if (!ref) {
        return RE::BSEventNotifyControl::kContinue;
    }

    // Manager::OnHover(ref);

    return RE::BSEventNotifyControl::kContinue;
}

void Hooks::CrosshairEventSink::Install() {
    SKSE::GetCrosshairRefEventSource()->AddEventSink(new CrosshairEventSink());
}
void Hooks::Install() {
    CrosshairEventSink::Install();
    GrabEventSink::Install();
    UpdateHook::Install();
}

RE::BSEventNotifyControl Hooks::GrabEventSink::ProcessEvent(const RE::TESGrabReleaseEvent* event,
                                                            RE::BSTEventSource<RE::TESGrabReleaseEvent>*) {
    if (!event) {
        return RE::BSEventNotifyControl::kContinue;
    }

    if (!event->ref) {
        return RE::BSEventNotifyControl::kContinue;
    }

    auto ref = event->ref.get();

    if (!ref) {
        return RE::BSEventNotifyControl::kContinue;
    }

    if (event->grabbed) {
        Manager::OnGrab(ref);
    } else {
        Manager::OnRelease(ref);
    }

    return RE::BSEventNotifyControl::kContinue;
}
void Hooks::GrabEventSink::Install() { RE::ScriptEventSourceHolder::GetSingleton()->AddEventSink(new GrabEventSink()); }

void Hooks::UpdateHook::Install() {
    SKSE::AllocTrampoline(14);
    auto& trampoline = SKSE::GetTrampoline();
    originalFunction =
        trampoline.write_call<5>(REL::RelocationID(35565, 36564).address() + REL::Relocate(0x748, 0xc2b), thunk);
}

void Hooks::UpdateHook::thunk() {
    Manager::OnTick();

    originalFunction();
}
