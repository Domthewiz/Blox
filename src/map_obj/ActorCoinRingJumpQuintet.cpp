#include "audio/GameAudio.h"
#include "game/CourseTask.h"
#include "map_obj/ActorCoinMgr.h"
#include "player/PlayerMgr.h"
#include <system/MainGame.h>
#include <blox/map_obj/ActorCoinRingJumpQuintet.h>
#include <blox/Blox.h>
#include <red/util/SpriteUtil.h>
#include <map/SwitchFlagMgr.h>

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
        mTimer = 0;
        return cResult_Success;
    }

    bool ActorCoinRingJumpQuintet::execute() {
        if (mTimer < 15) {
            mTimer++;
        } else {
            GameAudio::instance()->getAudioObjMap()->startSound("SE_SYS_CB_RESULT_COIN_FINISH", mPos);
            ActorCoinMgr::instance()->spawnItemCoin(mPos + sead::Vector3f(0.0f, -8.0f, 2000.0f), cDirType_Up, -1);
            s8 noInGame = PlayerMgr::instance()->getNumInGame();
            for (s8 i = noInGame; i > 0; i--) {
                CourseTask::instance()->addCoins(i, 5);
            }
            mDeleteRequestFlag = true;
        }
        if (mTimer == 3) {
            ActorCoinMgr::instance()->spawnItemCoin(mPos + sead::Vector3f(-14.0f, -20.0f, 2000.0f), cDirType_Up, -1);
        }
        if (mTimer == 6) {
            ActorCoinMgr::instance()->spawnItemCoin(sead::Vector3f::zero, cDirType_Up, -1);
        }
        if (mTimer == 9) {
            ActorCoinMgr::instance()->spawnItemCoin(mPos + sead::Vector3f(14.0f, -20.0f, 2000.0f), cDirType_Up, -1);
        }
        if (mTimer == 12) {
            ActorCoinMgr::instance()->spawnItemCoin(sead::Vector3f::zero, cDirType_Up, -1);
        }
        return true;
    }

}
