#include <map/SwitchFlagMgr.h>
#include <blox/Blox.h>
#include <red/util/SpriteUtil.h>
#include <player/PlayerObject.h>
#include <blox/map_obj/ActorBlockShock.h>

namespace blox {

    SEAD_RTTI_OVERRIDE_IMPL(ActorBlockShock, ActorBlockBase);
        
    using ACI = ActorCreateInfo;
    const ActorCreateInfo ActorBlockShock::cCreateInfo = {
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
    static const ActorCollisionCheck::CollisionData cExplosionInfo = {
        .center_offset = { 0.0f, 0.0f },
        .half_size = { 72.0f, 72.0f },
        .shape_type = CC::cShapeType_Box,
        .kind = CC::cKind_Enemy,
        .attack = CC::cAttack_None,
        .vs_kind = CC::TargetKind(
            CC::cTargetKind_Player |
            CC::cTargetKind_Enemy |
            CC::cTargetKind_ChibiYoshi |
            CC::cTargetKind_DrcTouch
            ),
        .vs_damage = CC::DamageFrom(ActorCollisionCheck::cDamageFrom_All & ~ActorCollisionCheck::cDamageFrom_Intermittent),
        .status = CC::cStatus_BurnerKill,
        .callback = &ActorBlockShock::collisionCallback
    };

    //TODO - Make it damage enemies
    void ActorBlockShock::collisionCallback(ActorCollisionCheck* cc_self, ActorCollisionCheck* cc_other) {
        Actor* other = cc_other->getOwner();
        if (cc_other->getOwner()->getKind() == cActorKind_Enemy) {
            other->setBlockHitDirection(cDirType_Up);
            other->setBlockHitTimer(8);
            other->setBlockHitFace(static_cast<ActorBlockShock*>(cc_self->getOwner())->getBlockHitFace());
        }
    }

    Profile* ActorBlockShock::sProfile = blox::getRegistrar()->newProfile<ActorBlockShock>("actor_block_shock")
        .resources<"block_pow_red">(ProfileInfo::cResType_Course)
        .drawPriority(232)
        .createInfo(cCreateInfo)
        .build();

    ActorBlockShock::ActorBlockShock(const ActorCreateParam& param)
        : ActorBlockBase(param)
        , mModel(nullptr)
        , mExplosionActive(false)
        , mExplosionTimer(0)
        , mResidueRemovalTimer(0)
        , mZPosOffset(0.0f)
    { }

    ActorBase::Result ActorBlockShock::create() {
        mModel = AnimModel::create("block_pow_red", "block_pow_red");

        mForm = Form::cForm_Block;
        _1ab4 = 0;
        _1aec = 0;
        _1cc0 = 0;

        mSpawnContentAsChild = true;

        mType = cType_Hatena;
        mBoxBgCollision.setType(BgCollision::cType_QuestionBlock);
        mContent = cContent_Empty;
        
        if (!ActorBlockBase::init(true,true)) {
            return cResult_Failed;
        }

        
        if (mParam0 >> 0x1C & 1 && mType == cType_Hit) {
            mDeleteRequestFlag = true;
        }
        changeState(StateID_Wait);
        registerColliderActiveInfo();
        
        mCollisionCheck.set(this, cExplosionInfo);
        
        execute();
        return cResult_Success;
    }

    bool ActorBlockShock::execute() {

        if (!ActorBlockBase::execute()) {
            return false;
        }

        mShock.executeQuake();

        if (mExplosionActive) {
            if (mExplosionTimer > 0) {
                mExplosionTimer--;
            } else {
                removeCollisionCheck();
                mExplosionActive = true;
            }
        }
        
        if (mResidueRemovalTimer > 6) {
            mDeleteRequestFlag = true;
        }

        if (mResidueRemovalTimer) {
            mResidueRemovalTimer++;
        }
        calcMdl_();
        
        return true;
    }
    
    bool ActorBlockShock::draw() {
        if (mType == cType_Hit) {
            if (mParam0 >> 0x1C & 1 && !mResidueRemovalTimer) {
                mResidueRemovalTimer = 1;
            }
            mUnitID = cUnitID_BlockUsed;
            return ActorBlockBase::draw();
        }

        if (mModel != nullptr) {
            mModel->draw();
        }
        return true;
    }

    void ActorBlockShock::calcMdl_() {
        if (mModel != nullptr) {
            mModel->update(sead::Vector3f(mPos.x, mPos.y, mPos.z + std::fmodf(mPos.x, 128.0f) + mZPosOffset), mAngle, sead::Vector3f(mScale.x, mScale.y, 0.01f));
        }
    }

    void ActorBlockShock::preSpawnItem() {
        mZPosOffset = 128.0f;
        ActorBlockBase::preSpawnItem();
        doShock_();
        return;
    }

    void ActorBlockShock::spawnItemUp() {
        this->mBumpMode = cBumpMode_None;
        return ActorBlockBase::spawnItemUp();
    }

    void ActorBlockShock::spawnItemDown() {
        this->mBumpMode = cBumpMode_None;
        return ActorBlockBase::spawnItemDown();
    }

    void ActorBlockShock::destroy() {
        changeState(StateID_Wait);
        doShock_();
    }
    void ActorBlockShock::destroy2() {
        changeState(StateID_Wait);
        doShock_();
    }

    void ActorBlockShock::doShock_() {
        if (mType == cType_Hit || mShock.isActive()) {
            return;
        }

        Quake::instance()->startShockAll(
            Quake::cShockType_Pow, 
            Quake::cShockFlag_ShockCamera | Quake::cShockFlag_ShockMotor, 
            0, 
            false
        );
        
        GameAudio::getAudioObjMap()->startSound("SE_OBJ_POW_BLOCK_QUAKE", mPos);
        
        sead::Vector3f effectScale(4.0f, 4.0f, 4.0f);
        sead::Vector3f effectPos(mPos.x, mPos.y + 8.0f, mPos.z);

        mShock.initiateShockBlockHitter(mPos, mHitPlayerNo);

        reviveCollisionCheck();

        mExplosionTimer = 1;
        mExplosionActive = true;
    }

    void ActorBlockShock::onBumpDiff() { // Overriding this function in order to unlock the NSLU only feature of activating an event
        if (mSwitchFlag1 == 0) {
            return;
        }
        SwitchFlagMgr::instance()->set(mSwitchFlag1 - 1, 0, true, false, false, 0, SwitchFlagMgr::cFlagType_Normal);
        return;
    }

   

}