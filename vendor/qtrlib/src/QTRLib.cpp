#include "QTRLib.h"
#include "DrawDebug.h"
#include "Graphics.h"
#include "ScreenLog.h"
#include "InputConfig.h"
#include "Translations.h"
void QTRLib::OnMessage(SKSE::MessagingInterface::Message* message) {
    #ifndef NDEBUG
    QTRLib::DrawDebug::OnMessage(message);
    #endif
}

void QTRLib::Install() {
    
    #ifndef NDEBUG
    Graphics::Install();
    ScreenLog::Install();
    #endif 
    InputConfig::Install();
    Translations::Install();
}
