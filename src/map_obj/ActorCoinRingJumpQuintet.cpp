#include <effect/EffectCreateUtil.h>
#include <game/CourseTask.h>
#include <map_obj/ActorCoinMgr.h>
#include <blox/map_obj/ActorCoinRingJumpQuintet.h>
#include <blox/Blox.h>

namespace blox {
    SEAD_RTTI_OVERRIDE_IMPL(ActorCoinRingJumpQuintet, Actor);

    using ACI = ActorCreateInfo;
    const ActorCreateInfo ActorCoinRingJumpQuintet::cCreateInfo = {
        .offset_x = 0, .offset_y = 0,
        .spawn_range = {
            .offset_x = 0, .offset_y = 0,
            .half_size_x = 1000000, .half_size_y = 1000000
        },
        .cull_range = { 
            .up = 0, .down = 0, .left = 0, .right = 0
        },
        .flag = ACI::cFlag_IgnoreSpawnRange
    };

    Profile* ActorCoinRingJumpQuintet::sProfile = blox::getRegistrar()->newProfile<ActorCoinRingJumpQuintet>("actor_coin_ring_jump_quintet")
        .createInfo(cCreateInfo)
        .build();

    ActorCoinRingJumpQuintet::ActorCoinRingJumpQuintet(const ActorCreateParam& param)
        : Actor(param)
        , mTimer(0)
    { }

    ActorBase::Result ActorCoinRingJumpQuintet::create() {
        coinPos1 = mPos + sead::Vector3f{
            0.0f, 16.0f, 3000.0f
        };
        
        coinPos2 = mPos + sead::Vector3f{
            -16.0f, -8.0f, 3000.0f
        };
        
        coinPos3 = mPos + sead::Vector3f{
            16.0f, -8.0f, 3000.0f
        };
        mTimer = 0;
        return cResult_Success;
    }

    bool ActorCoinRingJumpQuintet::execute() {
        s8 no = mParam0 & 0xFF;

        if (mTimer < 50) {
            mTimer++;
        } else {
            EffectCreateUtil::createEffect(RP_CoinNormalGet, &coinPos3, nullptr, nullptr);
            CourseTask::instance()->addCoins(no, 1);
            mDeleteRequestFlag = true;
        }
        if (mTimer == 10) {
            ActorCoinMgr::instance()->spawnItemCoin(coinPos1 - sead::Vector3f(0.0f, 24.0f, 0.0f), cDirType_Up, no);
        }
        if (mTimer == 20) {
            ActorCoinMgr::instance()->spawnItemCoin(coinPos2 - sead::Vector3f(0.0f, 24.0f, 0.0f), cDirType_Up, no);
        }
        if (mTimer == 30) {
            ActorCoinMgr::instance()->spawnItemCoin(coinPos3 - sead::Vector3f(0.0f, 24.0f, 0.0f), cDirType_Up, no);
            EffectCreateUtil::createEffect(RP_CoinNormalGet, &coinPos1, nullptr, nullptr);
        }
        if (mTimer == 40) {
            CourseTask::instance()->addCoins(no, 1);
            EffectCreateUtil::createEffect(RP_CoinNormalGet, &coinPos2, nullptr, nullptr);
        }
        return true;
    }

}
