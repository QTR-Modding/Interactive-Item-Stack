#include "Prompt.h"
#include "InputConfig.h"
#include "SkyPromptAPI.h"
#include "Translations.h"
#include <ScreenLog.h>
#include "Manager.h"

using promptList = std::vector<SkyPromptAPI::Prompt>;
using buttonList = std::vector<std::pair<RE::INPUT_DEVICE, SkyPromptAPI::ButtonID>>;

#define PROMPT_BUTTON_1 1
#define PROMPT_BUTTON_2 2
#define PROMPT_BUTTON_3 3
#define PROMPT_BUTTON_4 4

SkyPromptAPI::ClientID clientID1 = 0;
SkyPromptAPI::ClientID clientID = 0;

struct ButtonSetAdd {
    buttonList button1;
    promptList prompts = {};
};

class AddSink final : public SkyPromptAPI::PromptSink {
public:
    RE::TESObjectREFR* lastRef = nullptr;
    void Show(RE::TESObjectREFR* ref) { 
        if (!ref) {
            return;
        }
        lastRef = ref;

        for (auto& item : prompts) {
            item.refid = ref->GetFormID();
        }
        auto count = Manager::GetStackCount(ref);
        stackCount = std::format("{} ({})", stackCountText, count);
        prompts[0].text = stackCount;
        SkyPromptAPI::SendPrompt(this, clientID); 
    }
    std::string stackCount;
    const char* stackCountText;
    void Hide() { SkyPromptAPI::RemovePrompt(this, clientID); }
    promptList prompts;
    buttonList button1;
    AddSink() {
        stackCountText = Translations::Get("AddToStackMenu.Add");
        button1 = InputConfig::Get("AddToStackMenu.Add");
        prompts = {SkyPromptAPI::Prompt("missing stack count", PROMPT_BUTTON_1, 0, SkyPromptAPI::PromptType::kHold, 0,
                                        button1, 0xFFFFFFFF)};
    };

    static inline ButtonSetAdd set;

    void ProcessEvent(const SkyPromptAPI::PromptEvent event) override;

    std::span<const SkyPromptAPI::Prompt> GetPrompts() override { return prompts; }
};

class SplitSink final : public SkyPromptAPI::PromptSink {
public:
    void Show() {

        auto count = Manager::GetStackCount();
        stackCount = std::format("{} {}", stackCountText, count);
        all[2].text = stackCount;
        onlyRemove[1].text = stackCount;
        none[0].text = stackCount;
        SkyPromptAPI::SendPrompt(this, clientID);

    }
    std::string stackCount;
    void Hide() { SkyPromptAPI::RemovePrompt(this, clientID); }
    promptList all;
    promptList onlyRemove;
    promptList none;
    buttonList button1;
    buttonList button2;
    buttonList button3;
    SkyPromptAPI::Prompt removeOne;
    SkyPromptAPI::Prompt splitInHalf;
    SkyPromptAPI::Prompt displayCount;
    const char* stackCountText;
    SplitSink() {
        stackCountText = Translations::Get("RemoveFromStackMenu.StackCount");
        button1 = InputConfig::Get("RemoveFromStackMenu.RemoveOne");
        button2 = InputConfig::Get("RemoveFromStackMenu.SplitInHalf");
        button3 = {
            {RE::INPUT_DEVICE::kKeyboard, SkyPromptAPI::kSkyrim},
            {RE::INPUT_DEVICE::kGamepad, SkyPromptAPI::kSkyrim},
        };
        removeOne = SkyPromptAPI::Prompt(Translations::Get("RemoveFromStackMenu.RemoveOne"), PROMPT_BUTTON_2, 0, SkyPromptAPI::PromptType::kHold, 0, button1, 0xFFFFFFFF);
        splitInHalf = SkyPromptAPI::Prompt(Translations::Get("RemoveFromStackMenu.SplitInHalf"), PROMPT_BUTTON_3, 0, SkyPromptAPI::PromptType::kHold, 0, button2, 0xFFFFFFFF);
        displayCount = SkyPromptAPI::Prompt("missing stack count", PROMPT_BUTTON_4, 0, SkyPromptAPI::PromptType::kSinglePress, 0, button3, 0xFFFFFFFF);
        all = {removeOne, splitInHalf, displayCount};
        onlyRemove = {removeOne, displayCount};
        none = {displayCount};
    };

    static inline ButtonSetAdd set;

    void ProcessEvent(const SkyPromptAPI::PromptEvent event) override;

    std::span<const SkyPromptAPI::Prompt> GetPrompts() override { 
        auto count = Manager::GetStackCount();
        if (count > 2) {
            return all;
        }
        if (count > 1) {
            return onlyRemove;
        }
        return none;
    }
};
AddSink* addSink = nullptr;
SplitSink* splitSink = nullptr;
void AddSink::ProcessEvent(const SkyPromptAPI::PromptEvent event) {

    if (event.type == SkyPromptAPI::PromptEventType::kTimeout) {
        if (lastRef) {
            Show(lastRef);
        }
        return;
    }


    if (event.type == SkyPromptAPI::PromptEventType::kAccepted) {
        switch (event.prompt.eventID) {
            case PROMPT_BUTTON_1:
                Manager::OnAddToTheStack();
                splitSink->Hide();
                splitSink->Show();
                return;
            default:
                break;
        }
    }
}

void SplitSink::ProcessEvent(const SkyPromptAPI::PromptEvent event) {

    if (event.type == SkyPromptAPI::PromptEventType::kTimeout) {
        Show();
        return;
    }

    if (event.type == SkyPromptAPI::PromptEventType::kDeclined ||
        event.type == SkyPromptAPI::PromptEventType::kRemovedByMod) {
        Hide();
        return;
    }


    if (event.type == SkyPromptAPI::PromptEventType::kAccepted) {
        switch (event.prompt.eventID) {
            case PROMPT_BUTTON_2:
                Manager::OnRemove1FromStack();
                Hide();
                Show();
                return;
            case PROMPT_BUTTON_3:
                Manager::OnSplitStack();
                Hide();
                Show();
                return;
            default:
                break;
        }

    }
}



void Prompt::Install() {
    clientID = SkyPromptAPI::RequestClientID();
    addSink = new AddSink();
    splitSink = new SplitSink();
}

void Prompt::ShowAdd(RE::TESObjectREFR* ref) {

    if (!addSink) return;
    addSink->Show(ref);
}
void Prompt::HideAdd() {
    if (!addSink) return;
    addSink->Hide();
}

void Prompt::ShowSplit() { splitSink->Show(); }

void Prompt::HideSplit() { splitSink->Hide(); }

