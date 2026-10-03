#pragma once

#include <actor/Actor.h>
#include <actor/Profile.h>
#include <collision/ActorBoxBgCollision.h>
#include <graphics/AnimModel.h>

namespace blox {
    
    class ActorBlockOnOff : public Actor {
        SEAD_RTTI_OVERRIDE(ActorBlockOnOff, Actor);
    public:
        static Profile* sProfile;
        static const ActorCreateInfo cCreateInfo;
    
    public:
        ActorBlockOnOff(const ActorCreateParam& param);
        ~ActorBlockOnOff() override = default;
        
    public:
        Result create() override;
        bool execute() override;
        bool draw() override;
        void calcMdl();
    
    protected:
        AnimModel* mModel;
        ActorBoxBgCollision mCollider;
    };
}