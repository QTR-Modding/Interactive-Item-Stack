#pragma once
#include <cmath>
#include "RayCollector.h"
struct RayOutput {
    RE::NiPoint3 normal;
    RE::NiPoint3 position;
    float hitFraction;
    const RE::hkpCdBody* body;
    RE::TESObjectREFR* hitRef;
    bool hasHit;
};

namespace RayMath{

    inline float pointDistance(const RE::NiPoint3 a, const RE::NiPoint3 b) {
        const float dx = a.x - b.x;
        const float dy = a.y - b.y;
        const float dz = a.z - b.z;
        return std::sqrt(dx * dx + dy * dy + dz * dz);
    }

    inline [[maybe_unused]] RE::MagicTarget* FindPickTarget(RE::MagicCaster* caster, RE::NiPoint3& a_targetLocation,
                                                     RE::TESObjectCELL** a_targetCell, RE::bhkPickData& a_pickData) {
        using func_t = RE::MagicTarget*(RE::MagicCaster * caster, RE::NiPoint3 & a_targetLocation,
                                        RE::TESObjectCELL * *a_targetCell, RE::bhkPickData & a_pickData);
        const REL::Relocation<func_t> func{RELOCATION_ID(33676, 34456)};
        return func(caster, a_targetLocation, a_targetCell, a_pickData);
    }
}

class RayCast {

    public:
        static RE::NiPoint3 QuaternionToEuler(const RE::NiQuaternion& q);
        static std::pair<RE::NiPoint3, RE::NiPoint3> GetCameraData();
        static RayOutput CastFromCamera(std::function<bool(RE::NiAVObject*)> const& evaluator, float raySize = 2000000000.f);
        static RayOutput CastRay(RE::NiPoint3 ray_start, RE::NiPoint3 ray_end,
                                 std::function<bool(RE::NiAVObject*)> const& evaluator, float raySize = 2000000000.f);


};