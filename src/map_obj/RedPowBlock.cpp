#include "actor/ActorBase.h"
#include "heap/seadDisposer.h"
#include "map/UnitID.h"
#include <actor/Actor.h>
#include <blox/Blox.h>
#include <graphics/AnimModel.h>
#include <telkin/Print.h>
#include <red/util/SpriteUtil.h>
#include <map_obj/ActorBlockBase.h>
#include <map_obj/ActorCoinMgr.h>
#include <player/PlayerMgr.h>
#include <player/PlayerObject.h>
#include <map/SwitchFlagMgr.h>
#include <audio/GameAudio.h>
#include <map_obj/RedPowQuake.h>

namespace blox {

class RedPowBlock : public ActorBlockBase {

public:
    static Profile* sProfile;
    static const ActorCreateInfo cCreateInfo;

public:
    RedPowBlock(const ActorCreateParam& param);
    ~RedPowBlock() override = default;
    
    Result create() override;
    bool execute() override;
    bool draw() override;
    void updateModel();
    void destroy() override;
    void destroy2() override;
    void doRedPowQuake();

    void preSpawnItem() override;
    void spawnItemUp() override;
    void spawnItemDown() override;
    static void collisionCallback(ActorCollisionCheck* cc_self, ActorCollisionCheck* cc_other);
    
    f32 zPosOffset = 0.0f;

private:
    AnimModel*  mModel;
    RedPowQuake mQuake;
    bool        mExplosionActive;
    u8          mExplosionTimer;
    bool        mHitAlready;
};

using ACI = ActorCreateInfo;
const ActorCreateInfo RedPowBlock::cCreateInfo = {
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

using CC = ActorCollisionCheck;
static const ActorCollisionCheck::CollisionData cCcData = {
    .center_offset = { 0.0f, 0.0f },
    .half_size = { 72.0f, 72.0f },
    .shape_type = CC::cShapeType_Box,
    .kind = CC::cKind_Item,
    .attack = CC::cAttack_None,
    .vs_kind = CC::cTargetKind_All,
    .vs_damage = static_cast<CC::DamageFrom>(0xFFFBFFFF),
    .status = CC::cStatus_None,
    .callback = &RedPowBlock::collisionCallback
};

Profile* RedPowBlock::sProfile = blox::getRegistrar()->newProfile<RedPowBlock>("block_pow_red")
    .resources<"block_pow_red">(ProfileInfo::cResType_Course)
    .drawPriority(1)
    .createInfo(cCreateInfo)
    .build();

RedPowBlock::RedPowBlock(const ActorCreateParam& param)
    : ActorBlockBase(param)
    , mModel(nullptr)
    , mExplosionActive(false)
    , mExplosionTimer(0)
{ }

ActorBase::Result RedPowBlock::create() {
    mModel = AnimModel::create("block_pow_red", "block_pow_red");

    mForm = Form::cForm_Block;
    _1ab4 = 0;
    _1aec = 0;
    _1cc0 = 0;

    mSpawnContentAsChild = true; // spawn powerup as child

    mType = cType_Hatena;
    mBoxBgCollision.setType(BgCollision::cType_QuestionBlock);
    mContent = cContent_Empty;
    
    if (!ActorBlockBase::init(true,true)) {
        return cResult_Failed;
    }

    // registerColliderActiveInfo();
    changeState(StateID_Wait);
    mCollisionCheck.set(this, cCcData);
    
    execute();
    return cResult_Success;
}

bool RedPowBlock::execute() {

    if (!ActorBlockBase::execute()) {
        return false;
    }

    mQuake.executeQuake();

    if (mExplosionActive) {
        if (mExplosionTimer > 0) {
            mExplosionTimer--;
        } else {
            removeCollisionCheck();
            mExplosionActive = true;
        }
    }
    
    updateModel();

    return true;
}

bool RedPowBlock::draw() {
    if (mType == cType_Hit) {
        mUnitID = cUnitID_BlockUsed;
        return ActorBlockBase::draw();
    }

    if (mModel != nullptr) {
        mModel->draw();
    }
    return true;
}

void RedPowBlock::updateModel() {
    if (mModel != nullptr) {
        mModel->update(sead::Vector3f(mPos.x, mPos.y, mPos.z + std::fmodf(mPos.x, 128.0f) + zPosOffset), mAngle, sead::Vector3f(mScale.x, mScale.y, 0.01f));
    }
}

void RedPowBlock::preSpawnItem() {
    zPosOffset = 128.0f;
    ActorBlockBase::preSpawnItem();
    doRedPowQuake();
    return;
}

void RedPowBlock::spawnItemUp() {
    this->mBumpMode = cBumpMode_None;
    return ActorBlockBase::spawnItemUp();
}

void RedPowBlock::spawnItemDown() {
    this->mBumpMode = cBumpMode_None;
    return ActorBlockBase::spawnItemDown();
}

void RedPowBlock::destroy() {
    changeState(StateID_Wait);
    doRedPowQuake();
}
void RedPowBlock::destroy2() {
    changeState(StateID_Wait);
    doRedPowQuake();
}

void RedPowBlock::doRedPowQuake() {
    if (mType == cType_Hit || mQuake.isActive()) {
        return;
    }

    RumbleMgr::instance()->rumble(12);
    
    GameAudio::getAudioObjMap()->startSound("SE_OBJ_POW_BLOCK_QUAKE", mPos);
    
    sead::Vector3f effectScale(4.0f, 4.0f, 4.0f);
    sead::Vector3f effectPos(mPos.x, mPos.y + 8.0f, mPos.z);

    mQuake.initiateRedPowQuake(mPos, mHitPlayerNo);

    reviveCollisionCheck();

    mExplosionActive = true;
    mExplosionTimer = 2;
}

void RedPowBlock::collisionCallback(ActorCollisionCheck *cc_self, ActorCollisionCheck *cc_other) {
    (void)cc_self; (void)cc_other;
}

}