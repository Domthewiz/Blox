#include "actor/ActorBase.h"
#include "blox/actor/BloxStringBank.h"
#include "map_obj/ActorCoinMgr.h"
#include <blox/map_obj/ActorBlockHatenaLongItemCustom.h>
#include <blox/Blox.h>
#include <actor/ProfileInfo.h>
#include <red/util/SpriteUtil.h>
#include <actor/ActorMgr.h>
#include <player/PlayerMgr.h>
#include <player/PlayerObject.h>

namespace blox {

    SEAD_RTTI_OVERRIDE_IMPL(ActorBlockHatenaLongItemCustom, ActorBlockBase);
    
    const ActorCreateInfo ActorBlockHatenaLongItemCustom::cCreateInfo = {
        .offset_x = 8, .offset_y = -16,
        .spawn_range = {
            .offset_x = 0, .offset_y = 0,
            .half_size_x = 1000000, .half_size_y = 1000000
        },
        .cull_range = { 
            .up = 0, .down = 0, .left = 0, .right = 0
        },
        .flag = ActorCreateInfo::cFlag_None
    };
    
    Profile* ActorBlockHatenaLongItemCustom::sProfile = blox::getRegistrar()->newProfile<ActorBlockHatenaLongItemCustom>("actor_block_triple_item_custom")
        .resources<"blocklong">(ProfileInfo::cResType_Course)
        .createInfo(cCreateInfo)
        .drawPriority(232)
        .build();
    
    ActorBlockHatenaLongItemCustom::ActorBlockHatenaLongItemCustom(const ActorCreateParam& param)
        : ActorBlockBase(param)
        , mBlockType()
        , mSpawnProfileID(0xFFFF)
        , mSpawned(false)
        , mPrevFrameEvent(false)
        , mStringScanAttempt(0)
        , mModel(nullptr)
        , mZInitialPosOffset(std::fmodf(mPos.x, 128.0f))
        , mUseHitModel(false)
        , mWasHitFromBelow(false)
        , mCounter(0)
        , mZPosOffset(0.0f)
    {}
    
    ActorBase::Result ActorBlockHatenaLongItemCustom::create() {
        mModel = AnimModel::create("blocklong", "block_DRC", 0, 2, 2);
        mModel->playTexAnim("block_DRC");
        mModel->playTexSrtAnim("player99");
        mModel->playTexSrtAnim("coin");
        mModel->getTexAnim(0)->getFrameCtrl().setFrame(2 * red::SpriteUtil::getNybble11(this));
        mModel->getTexAnim(0)->getFrameCtrl().setRate(0.0f);
        if (!red::SpriteUtil::getNybble9(this)) {
            mModel->getShuAnim(0)->getFrameCtrl().setRate(0.0f);
        } else {
            mModel->getShuAnim(0)->getFrameCtrl().setRate(0.3333333333333333f);
        }

        mForm = Form::cForm_Block;
        _1ab4 = 1;
        _1aec = 0;
        _1cc0 = 0;
    
        mSpawnContentAsChild = true;
    
        mType = cType_Hatena;
        mBoxBgCollision.setType(BgCollision::cType_QuestionBlock);
        mContent = cContent_Empty;
        
        if (!ActorBlockBase::init(true,true)) {
            return cResult_Failed;
        }
        
        // Movement setup
        const u8 nybble20 = red::SpriteUtil::getNybble20(this);
        if (nybble20 > cPos_KinokoLift) {
            tk::fatal("Movement type was out of bounds");
        }
        
        const ParentMovementType movementType = static_cast<ParentMovementType>(nybble20);
        u32 movementMask = mParentMovementMgr.getTypeMask(movementType);
        
        mParentMovementID = mParamEx.course.movement_id;
        mParentMovementType = static_cast<ParentMovementType>(nybble20);
        initMover();
    
        setupMovement(mPos, movementMask, movementType, mParamEx.course.movement_id);
    
        if (mType == cType_Hit) {
            changeState(StateID_HitWait);
            mModel->getTexAnim(0)->getFrameCtrl().setFrame(1 + (2 * red::SpriteUtil::getNybble11(this)));
            mUseHitModel = true;
        }
        
        registerColliderActiveInfo();
        changeState(StateID_Wait);

        mBoxBgCollision.getPoints()[0].x -= 16.0f;
        mBoxBgCollision.getPoints()[1].x += 16.0f;
        mBoxBgCollision.getPoints()[2].x += 16.0f;
        mBoxBgCollision.getPoints()[3].x -= 16.0f;
        
        execute();
        return cResult_Success;
    }

    bool ActorBlockHatenaLongItemCustom::execute() {
        if (mSpawnProfileID == 0xFFFF) {
            if (!scanString()) {
                return true;
            }
        }

        if (!red::SpriteUtil::getNybble9(this)) {
            if (mCounter < 2) {
                mCounter ++;
                mModel->getShuAnim(0)->getFrameCtrl().setRate(0.0f);
            } else {
                mCounter = 0;
                mModel->getShuAnim(0)->getFrameCtrl().setRate(1.0f);
            }
        }

        if (!ActorBlockBase::execute()) {
            return false;
        }

        // update visuals
        if (mType == cType_Hit) {
            changeState(StateID_HitWait);
            if (!mUseHitModel) {
                mModel->getTexAnim(0)->getFrameCtrl().setFrame(1 + (2 * red::SpriteUtil::getNybble11(this)));
                mUseHitModel = true;
            }
        }
        calcMdl();

        return true;
    }

    bool ActorBlockHatenaLongItemCustom::draw() {
        if (mModel != nullptr) {
            mModel->draw();
        }
        return true;
    }

    void ActorBlockHatenaLongItemCustom::calcMdl() {
        f32 angleSin, angleCos;
        sead::Mathf::sinCosIdx(&angleSin, &angleCos, mAngle.z());

        const f32 rotatedX = -8 * angleSin;
        const f32 rotatedY = -8 * angleCos;

        if (mModel != nullptr) {
            mModel->update(sead::Vector3f(mPos.x + rotatedX, mPos.y - rotatedY, mPos.z + mZInitialPosOffset + mZPosOffset), mAngle, sead::Vector3f(mScale.x, mScale.y, 0.01f));
        }
    }
    
    void ActorBlockHatenaLongItemCustom::setupMovement(const sead::Vector3f& position, u32 movement_mask, ParentMovementType movement_type, u32 movement_id) {
        // use different link function if pivotal rotation, prevents glitches
        if (movement_type == ParentMovementType::cPos_CenterRotation) {
            ParentMovementMgr::PivotalRotationSettings pivotSettings;
            pivotSettings.position       = position;
            pivotSettings.movement_id    = movement_id;
            pivotSettings.movement_mask  = movement_mask;
            pivotSettings.pivot_center   = sead::Vector3f(0.0f, 0.0f, 0.0f);
            pivotSettings.upside_down    = red::SpriteUtil::getNybble19(this) & 0x1;
            pivotSettings.gyroscopic     = (red::SpriteUtil::getNybble19(this) >> 1) & 0x1;
            pivotSettings.tilted         = (red::SpriteUtil::getNybble19(this) >> 2) & 0x1;
            pivotSettings._21            = (red::SpriteUtil::getNybble19(this) >> 3) & 0x1;
            pivotSettings.movement_param = 1;
    
            mParentMovementMgr.linkPivotal2(pivotSettings);
        } else {
            mParentMovementMgr.link(position, movement_mask, movement_id);
        }
    
        // set type-specific data
        setMovementParamaters(movement_type);
    
        mParentMovementMgr.execute();
    }
    void ActorBlockHatenaLongItemCustom::setMovementParamaters(ParentMovementType movement_type) {
        static sead::SafeArray<f32, 16> twoWayDistanceMultiplierArr {
            1.0f, 0.1f, 0.2f, 0.3f, 0.4f, 0.5f, 0.6f, 0.7f, 0.8f, 0.9f, 1.0f, 1.1f, 1.2f, 1.3f, 1.4f, 1.5f
        };
        static sead::SafeArray<f32, 16> boltMovementSpeedArr {
            1.0f, 0.25f, 0.5f, 0.75f, 0.0f, 1.5f, 2.0f, 2.5f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f, 9.0f, 10.0f
        };
    
        switch (movement_type) {
            case cPos_Screw: {
                mParentMovementMgr.setBoltSpeed(boltMovementSpeedArr[red::SpriteUtil::getNybble18(this)]);
                mParentMovementMgr.setBoltDirection(static_cast<DirType>(red::SpriteUtil::getNybble19(this)));
                break;
            }
            case cPos_GoAndCome: {
                mParentMovementMgr.setTwoWayDistanceMultiplier(twoWayDistanceMultiplierArr[red::SpriteUtil::getNybble18(this)] + (0.01f * red::SpriteUtil::getNybble19(this)));
                break;
            }
            case cPos_ShiftingPlatform: {
                mParentMovementMgr.setRectPlatformInfo(static_cast<RectPlatformInfo>(red::SpriteUtil::getNybble19(this)));
                break;
            }
            case cPos_FloorGyration: {
                mParentMovementMgr.setFloorGyrationAngle(0x1000000 * red::SpriteUtil::getNybbleRange(this, 17, 18));
                ParentMovementMgr::MovementProperties newproperty = mParentMovementMgr.getMovementProperties();
                newproperty.hill_distance_offset = -16.0f * red::SpriteUtil::getNybble19(this);
                mParentMovementMgr.setMovementProperties(newproperty);
                break;
            }
        }
    }
    
    void ActorBlockHatenaLongItemCustom::spawnItemUp()
    {
        spawnPowerUp(false);
        return ActorBlockBase::spawnItemUp();
    }
    
    void ActorBlockHatenaLongItemCustom::spawnItemDown()
    {
        spawnPowerUp(true);
        return ActorBlockBase::spawnItemDown();
    }

    void ActorBlockHatenaLongItemCustom::preSpawnItem() {
        mZPosOffset = 128.0f;

        if ((red::SpriteUtil::getNybble10(this)) & 0x1) {
            spawnSideCoins();
        }

        // onBumpDiff();
        mWasHitFromBelow = false;
        return ActorBlockBase::preSpawnItem();
    }
    
    void ActorBlockHatenaLongItemCustom::spawnPowerUp(bool down)
    {
        ActorCreateParam item;
        
        item.profile = red::ProfileEx::get(mSpawnProfileID);
        
        // check if we spawn multi powerups
        u32 playerCount = PlayerMgr::instance()->getNumInGame();
        
        // check each player's state, and if they do be small
        u8 smallPlayers = 0;
        for (u32 i = 0; i < 4; i++) {
            PlayerObject * currentPlayerMode = PlayerMgr::instance()->getPlayerObject(i);
            if (currentPlayerMode != nullptr) {
                if ((currentPlayerMode->getPlayerMode() == PlayerMode::cPlayerMode_Small) || (currentPlayerMode->getPlayerMode() == PlayerMode::cPlayerMode_Mini)) {
                    smallPlayers += (1 << i);
                }
            }
        }

        // nybble 6
        if (mParam0 >> 0x18 & 1) {
            smallPlayers = 0;
        }
    
        // one player only logic
        if (playerCount == 1) {
        
            if (smallPlayers) {
                item.profile = red::ProfileEx::get(591); // Super Mushroom Profile
            }
    
            item.position = mPos;
        
            if (down) {
                item.param_0 = 0x000C3000;
            } else {
                item.param_0 = 0x00082000;
            }
        
            ActorMgr::instance()->createImmediately(item);
            return;
        }
    
        for (u8 i = 0; i < playerCount; i++) {
            
            if (smallPlayers && !i) { } else if ((smallPlayers >> i) & 0x1) {
                item.profile = red::ProfileEx::get(591); // Super Mushroom Profile
            }
            
            item.position = mPos;
    
            if (down) {
                item.param_0 = 0x050C3000 + ((i - 1) << 0x10);
            } else {
                item.param_0 = 0x04082000 + ((i - 1) << 0x10);
            }
        
            ActorMgr::instance()->createImmediately(item);
        }
    }
    
    bool ActorBlockHatenaLongItemCustom::scanString() {
        // Setting: String Bank ID
        const u8 stringBankID = red::SpriteUtil::getNybbleRange(this, 7, 8);
        
        mStringScanAttempt++;
        if (mStringScanAttempt > 5) {
            tk::fatal("ActorSpawnerEx: Failed to find String Bank with ID %i after 5 attempts...", stringBankID);
            return false;
        }
        
        ActorMgr* actorMgr = ActorMgr::instance();
        
        for (auto it = actorMgr->getActorBegin(); it != actorMgr->getActorEnd(); it++) {
            if (StringBank* bank = sead::DynamicCast<StringBank>(*it)) {
                if (bank->getBankID() == stringBankID && bank->getType() == StringBank::Type::Primary) {
                    mSpawnProfileID = red::ProfileEx::get(bank->getString())->getID();
                    return true;
                }
            }
        }
        
        return false;
    }

    void ActorBlockHatenaLongItemCustom::spawnSideCoins() {
        ActorCoinMgr::instance()->spawnItemCoin(mPos + sead::Vector3f(-16.0f, 0.0f, 0.0f), mSpawnDirection, mHitPlayerNo);
        ActorCoinMgr::instance()->spawnItemCoin(mPos + sead::Vector3f( 16.0f, 0.0f, 0.0f), mSpawnDirection, mHitPlayerNo);
    }
}