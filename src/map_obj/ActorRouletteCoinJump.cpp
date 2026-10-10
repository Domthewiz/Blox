#include <blox/Blox.h>
#include <game/CourseTask.h>
#include <blox/map_obj/ActorRouletteCoinJump.h>

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
        self->callbackGeneral(self);
    }

    void ActorRouletteCoinJump::callBackHead(BgCollision* cc_self, ActorBgCollisionCheck* cc_other) {
        ActorRouletteCoinJump* self = (ActorRouletteCoinJump*)cc_self->getOwner();
        self->callbackGeneral(self);
    }

    void ActorRouletteCoinJump::callBackWall(BgCollision* cc_self, ActorBgCollisionCheck* cc_other, u8 direction) {
        ActorRouletteCoinJump* self = (ActorRouletteCoinJump*)cc_self->getOwner();
        self->callbackGeneral(self);
    }

    void ActorRouletteCoinJump::callbackGeneral(ActorRouletteCoinJump* _this) {
        if (_this->mCoinCollectPlayerNo != -1) {
            CourseTask::instance()->addCoins(_this->mCoinCollectPlayerNo, 9);
        }
    }

}
