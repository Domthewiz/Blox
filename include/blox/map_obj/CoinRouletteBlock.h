#pragma once

#include <actor/Profile.h>
#include <graphics/AnimModel.h>
#include <map_obj/RouletteBlock.h>

namespace blox {

    class CoinRouletteBlock : public RouletteBlock {
        SEAD_RTTI_OVERRIDE(CoinRouletteBlock, RouletteBlock);
    public:
        enum CoinRouletteContent
        {
            cCoinRouletteContent_5 = 0,
            cCoinRouletteContent_10,
            cCoinRouletteContent_30,
            cCoinRouletteContent_50,
            cCoinRouletteContent_Num, // TODO: Add 100.
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

        void preSpawnItem() override;
        void onUpMoveStart() override;
        
        void vf2DC() override;
        void spawnTenCoin(u8 no, bool down);
        
    private:
        void calcMdl_();
        
    protected:
        AnimModel* mModelCustom;
        u32        mCustomSpinCountdown;
        bool       mCustomCountdownApplied;
        u8         mCustomRouletteIndex;
        bool       mWasHitFromBelow;
    };

}