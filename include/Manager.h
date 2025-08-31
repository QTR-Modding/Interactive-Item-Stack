#pragma once

class Manager {
    static inline RE::ObjectRefHandle grabbedPtr;
    static inline RE::ObjectRefHandle hoverRefPtr;

public:

    static void OnTick();

    static RE::TESObjectREFR* GetGrabbedRef();
    static void Clean();
    static bool ProcessHoverTarget(RE::TESObjectREFR* ref);
    static void OnHoverOut();
    static void OnAddToTheStack();
    static void OnSplitStack();
    static void OnRemove1FromStack();
    static void OnGrab(RE::TESObjectREFR* ref);
    static void OnRelease(RE::TESObjectREFR* ref);
    static int16_t GetStackCount();
    static int16_t GetStackCount(RE::TESObjectREFR* ref);
};
