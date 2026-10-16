#pragma once

#include <actor/Actor.h>
#include <actor/Profile.h>

namespace blox {
    
    class ActorCoinRingJumpQuintet : public Actor {
        SEAD_RTTI_OVERRIDE(ActorCoinRingJumpQuintet, Actor);
    public:
        static Profile* sProfile;
        static const ActorCreateInfo cCreateInfo;
    
    public:
        ActorCoinRingJumpQuintet(const ActorCreateParam& param);
        ~ActorCoinRingJumpQuintet() override = default;
        
    public:
        Result create() override;
        bool execute() override;
    
    protected:
        u8 mTimer;
        sead::Vector3f coinPos1;
        sead::Vector3f coinPos2;
        sead::Vector3f coinPos3;
    };
}