#pragma once

#include <actor/Profile.h>
#include <graphics/AnimModel.h>
#include <map_obj/ActorCoinShowerJump.h>

namespace blox {

    class ActorRouletteCoinJump : public ActorCoinShowerJump {
        SEAD_RTTI_OVERRIDE(ActorRouletteCoinJump, ActorCoinShowerJump);

    public:
        static Profile* sProfile;
        static const ActorCreateInfo cCreateInfo;

    public:
        ActorRouletteCoinJump(const ActorCreateParam& param);
        ~ActorRouletteCoinJump() override = default;
        
    public:
        Result create() override;
        bool execute() override;
        bool draw() override;

        static Profile* getProfile()
        {
            return sProfile;
        }

        static void callBackFoot(BgCollision* cc_self, ActorBgCollisionCheck* cc_other);
        static void callBackHead(BgCollision* cc_self, ActorBgCollisionCheck* cc_other);
        static void callBackWall(BgCollision* cc_self, ActorBgCollisionCheck* cc_other, u8 direction); 

    private:
        void calcMdl_();
        
    protected:
        AnimModel* mModel;
    };

}