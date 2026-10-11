#pragma once

#include <actor/Profile.h>
#include <map_obj/RingBase.h>

namespace blox {

    class CoinRing : public RingBase {
        SEAD_RTTI_OVERRIDE(CoinRing, RingBase);

    public:
        static Profile* sProfile;
        static const ActorCreateInfo cCreateInfo;

    public:
        CoinRing(const ActorCreateParam& param);
        ~CoinRing() override = default;
        
    public:
        void loadActorRes() override;
        void playTexAnim() override;
        bool createParticles() override;
        void collectSoundEffect() override;
        void collectSetSwitchFlag() override;

        void spawnCoinQuintet();
    protected:
        
    };
}