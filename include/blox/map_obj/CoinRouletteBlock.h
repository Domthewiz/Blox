#pragma once

#include <actor/Profile.h>
#include <graphics/AnimModel.h>
#include <map_obj/RouletteBlock.h>

namespace blox {

    class CoinRouletteBlock : public RouletteBlock {
        SEAD_RTTI_OVERRIDE(CoinRouletteBlock, RouletteBlock);
    public:
        enum RouletteContent
        {
            cRouletteContent_Mushroom = 0,
            cRouletteContent_Star,
            cRouletteContent_1UP,
            cRouletteContent_FireFlower,
            cRouletteContent_PropellerMushroom,
            cRouletteContent_IceFlower,
            cRouletteContent_PenguinSuit,
            cRouletteContent_SquirrelMushroom,
            cRouletteContent_MiniMushroom,
            cRouletteContent_Coin,
            cRouletteContent_Yoshi,
            cRouletteContent_BubbleChibiYoshi,
            cRouletteContent_BalloonChibiYoshi,
            cRouletteContent_GlowChibiYoshi,
            cRouletteContent_Spring,
            cRouletteContent_3UP
        };

    public:
        static Profile* sProfile;
        static const ActorCreateInfo cCreateInfo;

    public:
        CoinRouletteBlock(const ActorCreateParam& param);
        ~CoinRouletteBlock() override = default;
        
    public:
        Result create() override;
        bool execute() override;
        bool draw() override;

        void destroy() override;
        void destroy2() override;

        void spawnItemUp() override;
        void spawnItemDown() override;
        
        void vf2DC() override;
    private:
        void calcMdl_();
        
    protected:
        AnimModel* mModelCustom;
        u32 mCustomSpinCountdown;
        bool mCustomCountdownApplied;
        u8 mCustomRouletteIndex;
    };

}