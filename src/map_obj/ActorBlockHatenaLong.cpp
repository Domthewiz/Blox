#include <blox/Blox.h>
#include <telkin/Print.h>
#include <red/util/SpriteUtil.h>
#include <map_obj/ActorCoinMgr.h>
#include <player/PlayerMgr.h>
#include <player/PlayerObject.h>
#include <map/SwitchFlagMgr.h>
#include <blox/map_obj/ActorBlockHatenaLong.h>

static sead::SafeArray<BlockCoinBase::Content, 19> blockContents {
    BlockCoinBase::cContent_Empty,
    BlockCoinBase::cContent_Coin,
    BlockCoinBase::cContent_FireMushroom,
    BlockCoinBase::cContent_FireMushroom,
    BlockCoinBase::cContent_PropellerMushroom,
    BlockCoinBase::cContent_PenguinMushroom,
    BlockCoinBase::cContent_MiniMushroom,
    BlockCoinBase::cContent_Star,
    BlockCoinBase::cContent_ContinuousStar,
    BlockCoinBase::cContent_Yoshi,
    BlockCoinBase::cContent_MultiCoin,
    BlockCoinBase::cContent_LifeMushroom,
    BlockCoinBase::cContent_Vine,
    BlockCoinBase::cContent_Spring,
    BlockCoinBase::cContent_MushroomIfSmall,
    BlockCoinBase::cContent_IceFlower,
    BlockCoinBase::cContent_SquirrelMushroom,
    BlockCoinBase::cContent_LifeMoon,
    BlockCoinBase::cContent_Empty // Coin Shower
};

// TODO: Make it spawn custom contents for left and right
// TODO: Make it able to spawn switches (or make a seperate sprite)
namespace blox {

    SEAD_RTTI_OVERRIDE_IMPL(ActorBlockHatenaLong, ActorBlockBase)

    using ACI = ActorCreateInfo;
    const ActorCreateInfo ActorBlockHatenaLong::cCreateInfo = {
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

    Profile* ActorBlockHatenaLong::sProfile = blox::getRegistrar()->newProfile<ActorBlockHatenaLong>("tripbk")
        .resources<"blocklong">(ProfileInfo::cResType_Course)
        .drawPriority(232)
        .createInfo(cCreateInfo)
        .build();

    ActorBlockHatenaLong::ActorBlockHatenaLong(const ActorCreateParam& param)
        : ActorBlockBase(param)
        , mModel(nullptr)
        , mZInitialPosOffset(std::fmodf(mPos.x, 128.0f))
        , mUseHitModel(false)
        , mWasHitFromBelow(false)
        , mCounter(0)
        , mZPosOffset(0.0f)
    { }

    ActorBase::Result ActorBlockHatenaLong::create() {
        mModel = AnimModel::create("blocklong", "block_DRC", 0, 2, 2);
        mModel->playTexAnim("block_DRC");
        mModel->playTexSrtAnim("player99");
        mModel->playTexSrtAnim("coin");
        mModel->getTexAnim(0)->getFrameCtrl().setFrame(2 * red::SpriteUtil::getNybble11(this));
        mModel->getTexAnim(0)->getFrameCtrl().setRate(0.0f);
        if (!red::SpriteUtil::getNybble5(this)) {
            mModel->getShuAnim(0)->getFrameCtrl().setRate(0.0f);
        } else {
            mModel->getShuAnim(0)->getFrameCtrl().setRate(0.3333333333333333f);
        }

        mForm = Form::cForm_Block;
        _1ab4 = 1;
        _1aec = 0;
        _1cc0 = 0;

        mSpawnContentAsChild = true; // spawn powerup as child

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

        mContent = blockContents[red::SpriteUtil::getNybbleRange(this, 8, 9)];
        
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

    void ActorBlockHatenaLong::setupMovement(const sead::Vector3f& position, u32 movement_mask, ParentMovementType movement_type, u32 movement_id) {
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

    u32 ActorBlockHatenaLong::vf32C() {
        if (mParentMovementType != cPos_None && mParentMovementID != 0) {
            return 1;
        }
        return 0;
    }

    void ActorBlockHatenaLong::setMovementParamaters(ParentMovementType movement_type) {
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

    bool ActorBlockHatenaLong::execute() {
        //if (!ActorBlockBase::execute()) {return false;}
        // This is to replicate the 20fps of the regular ? block
        if (!red::SpriteUtil::getNybble5(this)) {
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

    bool ActorBlockHatenaLong::draw() {
        if (mModel != nullptr) {
            mModel->draw();
        }
        return true;
    }

    void ActorBlockHatenaLong::calcMdl() {
        f32 angleSin, angleCos;
        sead::Mathf::sinCosIdx(&angleSin, &angleCos, mAngle.z());

        const f32 rotatedX = -8 * angleSin;
        const f32 rotatedY = -8 * angleCos;

        if (mModel != nullptr) {
            mModel->update(sead::Vector3f(mPos.x + rotatedX, mPos.y - rotatedY, mPos.z + mZInitialPosOffset + mZPosOffset), mAngle, sead::Vector3f(mScale.x, mScale.y, 0.01f));
        }
    }
    void ActorBlockHatenaLong::spawnCoinShower() {
        // This function is overridden so it won't spawn a coin shower from the 10 coins
        // This is how it works in nsmb2, it only spawns the side coins on the last hit.
        if (!((red::SpriteUtil::getNybble10(this)) & 0x1)) {BlockCoinBase::spawnCoinShower();}
    }

    void ActorBlockHatenaLong::preSpawnItem() {
        // mWasHitFromBelow only exists because onDownMoveStart() doesn't get called when mario groundpounds the block strangely
        // luckily onUpMoveStart() gets called before this does so i made a bool
        mZPosOffset = 128.0f;

        if (mWasHitFromBelow) {
            mSpawnDirection = cDirType_Up;
            if (red::SpriteUtil::getNybble20(this) == 1) {
                mBumpMode = cBumpMode_Up;
            }
        } else {
            mSpawnDirection = cDirType_Down;
            if (red::SpriteUtil::getNybble20(this) == 1) {
                mBumpMode = cBumpMode_Down;
            }
        }

        if (((red::SpriteUtil::getNybble10(this)) & 0x1) && red::SpriteUtil::getNybbleRange(this, 8, 9) != 18) {
            spawnSideCoins();
        }

        // onBumpDiff();
        mWasHitFromBelow = false;
        return ActorBlockBase::preSpawnItem();
    }

    void ActorBlockHatenaLong::onUpMoveStart() {
        mWasHitFromBelow = true;
        return ActorBlockBase::onUpMoveStart();
    }

    void ActorBlockHatenaLong::spawnItemUp() {
        spawnItem();
        return ActorBlockBase::spawnItemUp();
    }

    void ActorBlockHatenaLong::spawnItemDown() {
        spawnItem();
        return ActorBlockBase::spawnItemDown();
    }

    void ActorBlockHatenaLong::onBumpDiff() { // Overriding this function in order to unlock the NSLU only feature of activating an event
        if (mSwitchFlag1 == 0) {
            return;
        }
        SwitchFlagMgr::instance()->set(mSwitchFlag1 - 1, 0, true, false, false, 0, SwitchFlagMgr::cFlagType_Normal);
        return;
    }

    void ActorBlockHatenaLong::spawnItem() {
        // Certain contents don't spawn normally, so i have to make exceptions for those
        mZPosOffset = 0.0f;
        bool isSmall = 0;
        u32 playerCount = PlayerMgr::instance()->getNumInGame();
        for (u32 i = 0; i < 4; i++) {
            PlayerObject * currentPlayerMode = PlayerMgr::instance()->getPlayerObject(i);
            if (currentPlayerMode == nullptr) {
                break;
            }

            if ((currentPlayerMode->getPlayerMode() == PlayerMode::cPlayerMode_Small) || (currentPlayerMode->getPlayerMode() == PlayerMode::cPlayerMode_Mini)) {
                isSmall = 1;
            }
        }

        switch (red::SpriteUtil::getNybbleRange(this, 8, 9)) {
            case 17: {
                mContent = cContent_LifeMoon;
                if (playerCount > 1) {
                    spawnMultiPowerup(mPos, 0, 1, true);
                    return;
                }
                spawnPowerup(mPos, 0, 1, true);
                return;
            }
            case 18: {
                mContent = cContent_Empty;
                BlockCoinBase::spawnCoinShower();
                return;
            }
            default: {
                return;
            }
        }
    }

    void ActorBlockHatenaLong::spawnSideCoins() {
        ActorCoinMgr::instance()->spawnItemCoin(mPos + sead::Vector3f(-16.0f, 0.0f, 0.0f), mSpawnDirection, mHitPlayerNo);
        ActorCoinMgr::instance()->spawnItemCoin(mPos + sead::Vector3f( 16.0f, 0.0f, 0.0f), mSpawnDirection, mHitPlayerNo);
    }

}