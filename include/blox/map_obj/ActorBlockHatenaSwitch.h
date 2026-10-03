#pragma once

#include <graphics/AnimModel.h>
#include <map_obj/ActorBlockSwitch.h>

namespace blox {

    class ActorBlockHatenaSwitch : public ActorBlockSwitch {
        SEAD_RTTI_OVERRIDE(ActorBlockHatenaSwitch, ActorBlockSwitch)
    public:
        static Profile* sProfile;
        static const ActorCreateInfo cCreateInfo;
        
    public:
        ActorBlockHatenaSwitch(const ActorCreateParam& param);
        ~ActorBlockHatenaSwitch() override = default;
        
    public:
        Result create() override;

        void spawnItemUp() override;
        void spawnItemDown() override;
        
    };
}