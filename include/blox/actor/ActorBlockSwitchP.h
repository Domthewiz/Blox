#pragma once

#include <actor/Actor.h>
#include <actor/Profile.h>
#include <collision/ActorBoxBgCollision.h>
#include <graphics/AnimModel.h>

namespace blox {
    
    class ActorBlockSwitchP : public Actor {
            SEAD_RTTI_OVERRIDE(ActorBlockSwitchP, Actor);
        public:
            static Profile* sProfile;
            static const ActorCreateInfo cCreateInfo;
        
        public:
            ActorBlockSwitchP(const ActorCreateParam& param);
            ~ActorBlockSwitchP() override = default;
            
        public:
            Result create() override;
            bool execute() override;
            bool draw() override;
            void calcMdl_();
        
        protected:
            AnimModel* mModel;
            ActorBoxBgCollision mCollider;
            bool mDotted;
    };
}