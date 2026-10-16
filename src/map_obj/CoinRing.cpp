#include <map/SwitchFlagMgr.h>
#include <actor/ActorMgr.h>
#include <audio/GameAudio.h>
#include <blox/map_obj/ActorCoinRingJumpQuintet.h>
#include <effect/EffectCreateUtil.h>
#include <blox/Blox.h>
#include <blox/map_obj/CoinRing.h>

namespace blox {

    static const sead::Color4f cRingColor {
        1.0f, 3.5f, 0.0f, 1.0f
    };
    static const sead::Color4f cRingCollectColor {
        1.0f, 2.0f, 0.0f, 1.0f
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
        mEffect1.setColor(cRingColor );
        return mEffect2.createEffect(RP_Ring_Mask, &mPos, nullptr, nullptr);
        
    }

    void CoinRing::onCollect() {
        Effect getParticles;
        sead::Vector3f coinGetScale {
            1.0f, 1.5f, 1.0f
        };
        EffectCreateUtil::createEffect(RP_Coinedit_StarCoin_on, &mPos, nullptr, &coinGetScale);
        
        GameAudio::instance()->getAudioObjMap()->startSound("SE_SYS_RED_RING", mPos);
        GameAudio::instance()->getAudioObjMap()->startSound("SE_SYS_CB_TOTAL_COIN_NUM", mPos);

        // getParticles.spawn(RP_RingRedGet, &mPos, nullptr, nullptr);
        // getParticles.setColor(cRingCollectColor);
        // EffectCreateUtil::createEffect(RP_RingGet_Mask, &mPos, nullptr, nullptr);

        SwitchFlagMgr::instance()->set(mSwitchFlag0 & 0x3f - 1, 0, true);

        for (s8 i = 0; i < 4; i++) {
            if (isPlayerWithinRange(i)) {
                spawnCoinQuintet(i);
                return;
            }
        }

        tk::println("Couldn't find collected player");
    }

    void CoinRing::collectSetSwitchFlag() {
    }
    
    void CoinRing::spawnCoinQuintet(s8 no) {
        ActorCreateParam ringJumpQuintet;
        ringJumpQuintet.profile = ActorCoinRingJumpQuintet::sProfile;
        ringJumpQuintet.position = mPos;
        ringJumpQuintet.param_0 = no;
        ActorMgr::instance()->createImmediately(ringJumpQuintet);
    }

}