#include "Logger.h"
#include "UI.h"
#include "Hooks.h"
#include "QTRLib.h"
#include "Manager.h"
#include "Prompt.h"
void OnMessage(SKSE::MessagingInterface::Message* message) {
    QTRLib::OnMessage(message);
    if (message->type == SKSE::MessagingInterface::kDataLoaded) {
        Prompt::Install();
    }
    if (message->type == SKSE::MessagingInterface::kPreLoadGame ||
        message->type == SKSE::MessagingInterface::kNewGame) {
        Manager::Clean();
    }
}

SKSEPluginLoad(const SKSE::LoadInterface *skse) {
    SKSE::Init(skse);
    SKSE::GetMessagingInterface()->RegisterListener(OnMessage);
    SetupLog();
    logger::info("Plugin loaded");
    UI::Register();
    Hooks::Install();
    QTRLib::Install();
    return true;
}