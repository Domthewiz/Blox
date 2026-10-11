#include "effect/EffectID.h"
#include <actor/ActorCreateParam.h>
#include <actor/ActorMgr.h>
#include <audio/GameAudio.h>
#include <blox/map_obj/ActorCoinRingJumpQuintet.h>
#include <effect/EffectCreateUtil.h>
#include <blox/Blox.h>
#include <blox/map_obj/CoinRing.h>

namespace blox {

    static const sead::Color4f cRingColor {
        1.5f, 1.0f, 0.0f, 1.0f
    };

    static sead::SafeArray<f32, 12> cCoinRouletteSpinArray1 { 
        0.0f, 1.0f, 2.0f, 0.0f, 1.0f, 2.0f, 0.0f, 1.0f, 2.0f, 0.0f, 3.0f, 1.0f
    };
    
    using ACI = ActorCreateInfo;
    const ActorCreateInfo CoinRing::cCreateInfo = {
        .offset_x = 8, .offset_y = -16,
        .spawn_range = {
            .offset_x = 0, .offset_y = 0,
            .half_size_x = 0x100, .half_size_y = 0x100
        },
        .cull_range = { 
            .up = 0, .down = 0, .left = 0, .right = 0
        },
        .flag = ACI::cFlag_None
    };

    SEAD_RTTI_OVERRIDE_IMPL(CoinRing, RingBase);

    Profile* CoinRing::sProfile = blox::getRegistrar()->newProfile<CoinRing>("coin_ring")
        .resources<"coin_ring">(ProfileInfo::cResType_Course)
        .drawPriority(232)
        .createInfo(cCreateInfo)
        .build();

    CoinRing::CoinRing(const ActorCreateParam& param)
        : RingBase(param)
    { }

    void CoinRing::loadActorRes() {
        mModel = AnimModel::create("coin_ring", "green_ring", 1,0, 1);
    }

    void CoinRing::playTexAnim() {
        FrameCtrl pMVar2;
        
        mModel->playSklAnim("green_ring");
        mModel->playTexSrtAnim("green_ring");
        pMVar2 = mModel->getShuAnim(0)->getFrameCtrl();
        pMVar2.setFrame(0.0f);
        pMVar2.setPlayMode(FrameCtrl::cMode_Repeat);
        pMVar2.setRate(1.0f);
    }

    bool CoinRing::createParticles() {
        mEffect1.createEffect(RP_RingRed, &mPos, nullptr, nullptr);
        mEffect1.setColor(cRingColor);
        return mEffect2.createEffect(RP_Ring_Mask, &mPos, nullptr, nullptr);

    }

    void CoinRing::collectSoundEffect() {
        Effect getParticles;

        GameAudio::instance()->getAudioObjMap()->startSound("SE_SYS_GOAL_RING", mPos);
        // getParticles.spawn(RP_Coinedit_StarCoin_on, &mPos, nullptr, nullptr);
        getParticles.spawn(RP_RingRedGet, &mPos, nullptr, nullptr);
        getParticles.setColor(cRingColor);
        EffectCreateUtil::createEffect(RP_RingGet_Mask, &mPos, nullptr, nullptr);

        spawnCoinQuintet();
    }

    void CoinRing::collectSetSwitchFlag() {
    }
    
    void CoinRing::spawnCoinQuintet() {
        ActorCreateParam ringJumpQuintet;
        ringJumpQuintet.profile = ActorCoinRingJumpQuintet::sProfile;
        ringJumpQuintet.position = mPos;
        ActorMgr::instance()->createImmediately(ringJumpQuintet);
    }

}