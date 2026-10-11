#pragma once

#include <actor/Actor.h>
#include <actor/Profile.h>
#include <collision/ActorBoxBgCollision.h>
#include <graphics/AnimModel.h>

namespace blox {
    
    class SwitchFlagW6Link : public Actor {
        SEAD_RTTI_OVERRIDE(SwitchFlagW6Link, Actor);
    public:
        static Profile* sProfile;
        static const ActorCreateInfo cCreateInfo;
    
    public:
        SwitchFlagW6Link(const ActorCreateParam& param);
        ~SwitchFlagW6Link() override = default;
        
    public:
        Result create() override;
        bool execute() override;
    
    protected:
        bool mSet;
    };
}