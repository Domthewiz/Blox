#include "actor/Actor.h"
#include "actor/ActorBase.h"
#include "collision/BgCollision.h"
#include "map_obj/ActorCoinShowerJump.h"
#include "math/seadVector.h"
#include "player/PlayerObject.h"
#include "utility/Direction.h"
#include <blox/Blox.h>
#include <graphics/AnimModel.h>
#include <red/util/SpriteUtil.h>
#include <game/CourseTask.h>
#include <map/SwitchFlagMgr.h>
#include <audio/GameAudio.h>
#include <blox/map_obj/ActorRouletteCoinJump.h>
#include <player/Yoshi.h>

namespace blox {

    SEAD_RTTI_OVERRIDE_IMPL(ActorRouletteCoinJump, ActorCoinShowerJump);

    using ACI = ActorCreateInfo;
    const ActorCreateInfo ActorRouletteCoinJump::cCreateInfo = {
        .offset_x = 0, .offset_y = 0,
        .spawn_range = {
            .offset_x = 0, .offset_y = 0,
            .half_size_x = 1000000, .half_size_y = 1000000
        },
        .cull_range = { 
            .up = 0, .down = 0, .left = 0, .right = 0
        },
        .flag = ACI::cFlag_None
    };

    Profile* ActorRouletteCoinJump::sProfile = blox::getRegistrar()->newProfile<ActorRouletteCoinJump>("roulette_coin_jump")
        .resources<"ten_coin">(ProfileInfo::cResType_Course)
        .drawPriority(232)
        .createInfo(cCreateInfo)
        .build();

    ActorRouletteCoinJump::ActorRouletteCoinJump(const ActorCreateParam& param)
        : ActorCoinShowerJump(param)
        , mModel(nullptr)
    { }

    ActorBase::Result ActorRouletteCoinJump::create() {
        mModel = AnimModel::create("ten_coin", "obj_coin_red");

        if (!ActorCoinShowerJump::create()) {
            return cResult_Failed;
        };
      
        mBoxBgCollision.getPoints()[0].x -= 8.0f;
        mBoxBgCollision.getPoints()[0].y += 16.0f;

        mBoxBgCollision.getPoints()[1].x += 8.0f;
        mBoxBgCollision.getPoints()[1].y += 16.0f;

        mBoxBgCollision.getPoints()[2].x += 8.0f;
        mBoxBgCollision.getPoints()[2].y -= 1.0f;

        mBoxBgCollision.getPoints()[3].x -= 8.0f;
        mBoxBgCollision.getPoints()[3].y -= 1.0f;
        
        // mBoxBgCollision.setType(BgCollision::cType_Urchin);
        mBoxBgCollision.setCallback(callBackFoot, callBackHead, callBackWall);

        return cResult_Success;
    }

    bool ActorRouletteCoinJump::execute() {
        if (!ActorCoinShowerJump::execute()) {
            return false;
        }

        calcMdl_();


        

        return true;
    }

    bool ActorRouletteCoinJump::draw() {
        if (mModel != nullptr) {
            mModel->draw();
        }
        return true;
    }

    void ActorRouletteCoinJump::calcMdl_() {
        if (mModel != nullptr) {
            mModel->update(mPos + sead::Vector3f(0.0f, 16.0f, 0.0f), mAngle, mScale);
        }
    }

    void ActorRouletteCoinJump::callBackFoot(BgCollision* cc_self, ActorBgCollisionCheck* cc_other) {
        ActorRouletteCoinJump* self = (ActorRouletteCoinJump*)cc_self->getOwner();
        Actor* other = cc_other->getOwner();
        s8 playerNo;
        if (other->getKind() == cActorKind_Player || other->getKind() == cActorKind_Yoshi) {
            if (other->getKind() == cActorKind_Yoshi && static_cast<Yoshi*>(other)->getPlayerRideOn() != nullptr) {
                playerNo = static_cast<PlayerBase*>(static_cast<Yoshi*>(cc_other->getOwner())->getPlayerRideOn())->getPlayerNo();
            } else {
                playerNo = static_cast<PlayerObject*>(other)->getPlayerNo();
            }
            CourseTask::instance()->addCoins(self->mPlayerNo, 9);
        }
    }

    void ActorRouletteCoinJump::callBackHead(BgCollision* cc_self, ActorBgCollisionCheck* cc_other) {
        ActorRouletteCoinJump* self = (ActorRouletteCoinJump*)cc_self->getOwner();
        Actor* other = cc_other->getOwner();
        s8 playerNo;
        if (other->getKind() == cActorKind_Player || other->getKind() == cActorKind_Yoshi) {
            if (other->getKind() == cActorKind_Yoshi && static_cast<Yoshi*>(other)->getPlayerRideOn() != nullptr) {
                playerNo = static_cast<PlayerBase*>(static_cast<Yoshi*>(cc_other->getOwner())->getPlayerRideOn())->getPlayerNo();
            } else {
                playerNo = static_cast<PlayerObject*>(other)->getPlayerNo();
            }
            CourseTask::instance()->addCoins(self->mPlayerNo, 9);
        }
    }

    void ActorRouletteCoinJump::callBackWall(BgCollision* cc_self, ActorBgCollisionCheck* cc_other, u8 direction) {
        ActorRouletteCoinJump* self = (ActorRouletteCoinJump*)cc_self->getOwner();
        Actor* other = cc_other->getOwner();
        s8 playerNo;
        if (other->getKind() == cActorKind_Player || other->getKind() == cActorKind_Yoshi) {
            if (other->getKind() == cActorKind_Yoshi && static_cast<Yoshi*>(other)->getPlayerRideOn() != nullptr) {
                playerNo = static_cast<PlayerBase*>(static_cast<Yoshi*>(cc_other->getOwner())->getPlayerRideOn())->getPlayerNo();
            } else {
                playerNo = static_cast<PlayerObject*>(other)->getPlayerNo();
            }
            CourseTask::instance()->addCoins(self->mPlayerNo, 9);
        }
    }

}