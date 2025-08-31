#include "Manager.h"

#include "DrawDebug.h"
#include "Prompt.h"
#include "Raycast.h"
#include "ScreenLog.h"

bool Manager::ProcessHoverTarget(RE::TESObjectREFR* ref) {
    if (!grabbedPtr) return false;
    auto grabbedRef = grabbedPtr.get();
    if (!grabbedRef) {
        return false;
    }

    if (hoverRefPtr) {
        if (auto hoverPtr = hoverRefPtr.get()) {
            if (auto hoverRef = hoverPtr.get()) {
                if (hoverRef == ref) {
                    return true;
                }
            }
        }
    }

    Prompt::HideAdd();
    Prompt::ShowAdd(ref);
    hoverRefPtr = ref->GetHandle();
    return true;
}

void Manager::OnHoverOut() {
    Prompt::HideAdd();
    hoverRefPtr = RE::ObjectRefHandle();
}

void Manager::OnAddToTheStack() {
    auto player = RE::PlayerCharacter::GetSingleton();
    if (!grabbedPtr || !hoverRefPtr) return;
    auto grabbedRef = grabbedPtr.get();
    auto hoverPtr = hoverRefPtr.get();
    if (!grabbedRef || !hoverPtr) {
        return;
    }
    auto targetCount = grabbedRef->extraList.GetByType<RE::ExtraCount>();
    auto sourceCount = hoverPtr->extraList.GetByType<RE::ExtraCount>();

    if (!targetCount) {
        targetCount = RE::BSExtraData::Create<RE::ExtraCount>();
        grabbedRef->AddChange(RE::TESObjectREFR::ChangeFlags::kItemExtraData);
        grabbedRef->extraList.Add(targetCount);
        targetCount->count = 1;
    }
    auto count = sourceCount ? sourceCount->count : 1;

    targetCount->count += count;
    player->PickUpObject(hoverPtr.get(), count, false, false);
    player->RemoveItem(hoverPtr->GetBaseObject(), count, RE::ITEM_REMOVE_REASON::kRemove, nullptr, nullptr);
    OnHoverOut();
}

void Manager::OnSplitStack() {
    if (!grabbedPtr) return;
    auto grabbedRef = grabbedPtr.get();
    if (!grabbedRef) {
        return;
    }
    auto sourceCount = grabbedRef->extraList.GetByType<RE::ExtraCount>();
    auto sourceOwner = grabbedRef->extraList.GetByType<RE::ExtraOwnership>();

    if (!sourceCount) {
        return;
    }

    if (sourceCount->count <= 1) {
        return;
    }

    auto count1 = std::floor(sourceCount->count / 2);
    auto count2 = sourceCount->count - count1;

    auto player = RE::PlayerCharacter::GetSingleton();
    auto handle = RE::TESDataHandler::GetSingleton()->CreateReferenceAtLocation(
        grabbedRef->GetBaseObject(), grabbedRef->GetPosition(), grabbedRef->GetAngle(), grabbedRef->GetParentCell(),
        grabbedRef->GetWorldspace(), nullptr, nullptr, RE::ObjectRefHandle(), false, true);

    if (!handle) {
        return;
    }
    auto refptr = handle.get();
    if (!refptr) {
        return;
    }
    auto ref = refptr.get();
    if (!ref) {
        return;
    }
    auto targetCount = RE::BSExtraData::Create<RE::ExtraCount>();
    ref->AddChange(RE::TESObjectREFR::ChangeFlags::kItemExtraData);
    ref->extraList.Add(targetCount);
    targetCount->count = count1;
    sourceCount->count = count2;

    if (sourceOwner) {
        auto targetOwner = ref->extraList.GetByType<RE::ExtraOwnership>();
        if (!targetOwner) {
            targetOwner = RE::BSExtraData::Create<RE::ExtraOwnership>();
            ref->AddChange(RE::TESObjectREFR::ChangeFlags::kOwnershipExtra);
            ref->extraList.Add(targetOwner);
        }
        if (targetOwner) {
            targetOwner->owner = sourceOwner->owner;
        }
    }

    if (auto d3d = ref->Load3D(false)) {
        d3d->SetCollisionLayer(RE::COL_LAYER::kClutter);
    }
}

void Manager::OnRemove1FromStack() {
    if (!grabbedPtr) return;
    auto grabbedRef = grabbedPtr.get();
    if (!grabbedRef) {
        return;
    }
    auto sourceCount = grabbedRef->extraList.GetByType<RE::ExtraCount>();
    auto sourceOwner = grabbedRef->extraList.GetByType<RE::ExtraOwnership>();

    if (!sourceCount) {
        return;
    }

    if (sourceCount->count <= 1) {
        return;
    }

    auto player = RE::PlayerCharacter::GetSingleton();
    auto handle = RE::TESDataHandler::GetSingleton()->CreateReferenceAtLocation(
        grabbedRef->GetBaseObject(), grabbedRef->GetPosition(), grabbedRef->GetAngle(), grabbedRef->GetParentCell(),
        grabbedRef->GetWorldspace(), nullptr, nullptr, RE::ObjectRefHandle(), false, true);

    if (!handle) {
        return;
    }
    auto refptr = handle.get();
    if (!refptr) {
        return;
    }
    auto ref = refptr.get();
    if (!ref) {
        return;
    }

    sourceCount->count--;

    if (sourceOwner) {
        auto targetOwner = ref->extraList.GetByType<RE::ExtraOwnership>();
        if (!targetOwner) {
            targetOwner = RE::BSExtraData::Create<RE::ExtraOwnership>();
            ref->AddChange(RE::TESObjectREFR::ChangeFlags::kOwnershipExtra);
            ref->extraList.Add(targetOwner);
        }
        if (targetOwner) {
            targetOwner->owner = sourceOwner->owner;
        }
    }

    if (auto d3d = ref->Load3D(false)) {
        d3d->SetCollisionLayer(RE::COL_LAYER::kClutter);
    }
}

void Manager::OnGrab(RE::TESObjectREFR* ref) {
    grabbedPtr = ref->GetHandle();
    Prompt::ShowSplit();
}

void Manager::OnRelease(RE::TESObjectREFR* ref) { 
    Clean(); 
}

int16_t Manager::GetStackCount() {
    if (!grabbedPtr) return 0;
    auto grabbedRef = grabbedPtr.get();
    if (!grabbedRef) {
        return 0;
    }

    return GetStackCount(grabbedRef.get());

}
int16_t Manager::GetStackCount(RE::TESObjectREFR* ref) {

    if (!ref) {
        return 0;
    }

    auto sourceCount = ref->extraList.GetByType<RE::ExtraCount>();

    if (!sourceCount) {
        return 1;
    }

    return sourceCount->count;
}
void Manager::OnTick() {
    //QTRLib::DrawDebug::Clean();
    if (auto ref = Manager::GetGrabbedRef()) {
        auto callback = [ref](RE::NiAVObject* obj) {
            if (obj) {
                if (auto data = obj->GetUserData()) {

                    auto player = RE::PlayerCharacter::GetSingleton();

                    if (data->IsPlayerRef()) {
                        return false;
                    }
                    if (data->GetBaseObject() != ref->GetBaseObject()) {
                        return false;
                    }

                    if (player->WouldBeStealing(data) != player->WouldBeStealing(ref)) {
                        return false;
                    }

                    if (data != ref) {
                        return true;
                    }
                }
            }
            return false;
        };

        auto center = ref->GetPosition();
        float radius = 20.0f;
        int samples = 100;

        std::optional<RayOutput> best = std::nullopt;
        
        for (int i = 0; i < samples; i++) {
            float offset = 2.0f / samples;
            float y = i * offset - 1.0f + (offset / 2.0f);
            float r = sqrt(1.0f - y * y);
            float phi = i * glm::pi<float>() * (3.0f - sqrt(5.0f));
            float x = cos(phi) * r;
            float z = sin(phi) * r;
            RE::NiPoint3 spherePoint = center + RE::NiPoint3(x, y, z) * radius;

            auto out = RayCast::CastRay(center, spherePoint, callback, radius);

            if (out.hitRef) {
                if (!best || out.hitFraction < best->hitFraction) {
                    best = out;
                }
                //QTRLib::DrawDebug::DrawLine(center, spherePoint, glm::uvec4(0, 255, 0, 255));
            } else {
                //QTRLib::DrawDebug::DrawLine(center, spherePoint, glm::uvec4(255, 0, 0, 255));
            }

        }

        if (best) {
            if (!ProcessHoverTarget(best->hitRef)) {
                OnHoverOut();
            }
        } else {
            OnHoverOut();
        }
    

    }
}
RE::TESObjectREFR* Manager::GetGrabbedRef() {
    if (!grabbedPtr) return nullptr;
    auto grabbedRef = grabbedPtr.get();
    if (!grabbedRef) {
        return nullptr;
    }
    auto ref = grabbedRef.get();
    if (!ref) {
        return nullptr;
    }
    return ref;
}
void Manager::Clean() {
    grabbedPtr = RE::ObjectRefHandle();
    hoverRefPtr = RE::ObjectRefHandle();
    Prompt::HideAdd();
    Prompt::HideSplit();
}
