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

namespace blox {

class HatenaMimic : public ActorBlockBase {

public:
    static Profile* cProfile;
    static const ActorCreateInfo cCreateInfo;

public:
    HatenaMimic(const ActorCreateParam& param);
    ~HatenaMimic() override = default;
    
    Result create() override;
    bool execute() override;
    bool draw() override;

    void spawnItemUp() override;
    void spawnItemDown() override;
    void spawnItem();
    void spawnCoins();
    void updateModel();

    void onUpMoveStart() override;

    void spawnCoinShower() override;
    void preSpawnItem() override;

    s32 hitPlayerID = 0;
    bool isBumpFromBelow = false;
    bool gotBonkedModel = false;
    f32 zPosOffset = 0.0f;

private:
    AnimModel* mModel;
    ParentMovementMgr mMovementMgr;
};

using ACI = ActorCreateInfo;
const ActorCreateInfo HatenaMimic::cCreateInfo = {
    .offset_x = 0, .offset_y = 0,
    .spawn_range = {
        .offset_x = 0, .offset_y = 0,
        .half_size_x = 1000000, .half_size_y = 1000000
    },
    .cull_range = { 
        .up = 0, .down = 0, .left = 0, .right = 0
    },
    .flag = 0
};

Profile* HatenaMimic::cProfile = blox::getRegistrar()->newProfile<HatenaMimic>("htnamimic")
    .resources<"blockhtna">(ProfileInfo::cResType_Course)
    .drawPriority(1)
    .createInfo(&cCreateInfo)
    .build();

HatenaMimic::HatenaMimic(const ActorCreateParam& param)
    : ActorBlockBase(param)
    , mModel(nullptr)
{ }

ActorBase::Result HatenaMimic::create() {
    mModel = AnimModel::create("blockhtna", "block_DRC", 0, 2, 2);
    mModel->playTexAnim("block_DRC");
    mModel->playTexSrtAnim("player99");
    mModel->playTexSrtAnim("coin");
    mModel->getTexAnim(0)->getFrameCtrl().setFrame(2 * red::SpriteUtil::getNybble11(this));
    mModel->getTexAnim(0)->getFrameCtrl().setRate(0.0f);

    _1c68 = 1;
    _1ab4 = 0;
    _1aec = 0;
    _1cc0 = 0;

    _1ace = 1; // spawn powerup as child

    mType = cType_Hatena;
    mBoxBgCollision.setType(BgCollision::cType_QuestionBlock);
    mContent = cContent_Empty;
    
    if (!ActorBlockBase::init(true,true)) {
        return cResult_Failed;
    }

    registerColliderActiveInfo();
    changeState(StateID_Wait);

    switch (red::SpriteUtil::getNybbleRange(this, 8, 9)) {
        default:
            mContent = cContent_Empty;
            break;
        case 1:
            mContent = cContent_Coin;
            break;
        case 2:
        case 3:
            mContent = cContent_FireMushroom;
            break;
        case 4:
            mContent = cContent_PropellerMushroom;
            break;
        case 5:
            mContent = cContent_PenguinMushroom;
            break;
        case 6:
            mContent = cContent_MiniMushroom;
            break;
        case 7:
            mContent = cContent_Star;
            break;
        case 8:
            mContent = cContent_ContinuousStar;
            break;
        case 9:
            mContent = cContent_Yoshi;
            break;
        case 10:
            mContent = cContent_MultiCoin;
            break;
        case 11:
            mContent = cContent_LifeMushroom;
            break;
        case 12:
            mContent = cContent_Vine;
            break;
        case 13:
            mContent = cContent_Spring;
            break;
        case 14:
            mContent = cContent_MushroomIfSmall;
            break;
        case 15:
            mContent = cContent_IceMushroom;
            break;
        case 16:
            mContent = cContent_SquirrelMushroom;
            break;
        case 17:
            mContent = cContent_LifeMoon;
            break;
        case 18:
            mContent = cContent_Empty;
            break;
    }
    
    if (mType == cType_Hit) {
        changeState(StateID_HitWait);
        mModel->getTexAnim(0)->getFrameCtrl().setFrame(1 + (2 * red::SpriteUtil::getNybble11(this)));
        this->gotBonkedModel = true;
    }
    
    const u8 movementType = red::SpriteUtil::getNybble20(this);
    if (movementType > ParentMovementType::cPos_KinokoLift) {
        tk::fatal("Invalid movement type");
    }
    u32 movementMask = mMovementMgr.getTypeMask(static_cast<ParentMovementType>(movementType));
    mMovementMgr.link(mPos, movementMask, mParamEx.course.movement_id);

    initMover();

    execute();
    return cResult_Success;
}

bool HatenaMimic::execute() {

    // TODO: Fix this janky collider nonsense
    if (red::SpriteUtil::getNybble20(this) != 0) {
        mMovementMgr.execute();
        mPos.x = mMovementMgr.getPosition().x;
        mAngle.z() = mMovementMgr.getAngle();
        mBoxBgCollision.execute();
    }
    if (!ActorBlockBase::execute()) {return false;}
    if (red::SpriteUtil::getNybble20(this) != 0) {
        mPos.y = mMovementMgr.getPosition().y + _1a7c;
    }

    // update visuals
    if (mType == cType_Hit) {
        changeState(StateID_HitWait);
        if (!gotBonkedModel) {
            mModel->getTexAnim(0)->getFrameCtrl().setFrame(1 + (2 * red::SpriteUtil::getNybble11(this)));
            this->gotBonkedModel = true;
        }
    }

    updateModel();

    return true;
}

bool HatenaMimic::draw() {
    if (mModel != nullptr) {
        mModel->draw();
    }
    return true;
}

void HatenaMimic::updateModel() {
    f32 angleSin, angleCos;
    sead::Mathf::sinCosIdx(&angleSin, &angleCos, mAngle.z());

    const f32 rotatedX = -8 * angleSin;
    const f32 rotatedY = -8 * angleCos;

    if (mModel != nullptr) {
        mModel->update(sead::Vector3f(mPos.x + rotatedX, mPos.y - rotatedY, mPos.z + std::fmodf(mPos.x, 128.0f) + zPosOffset), mAngle, sead::Vector3f(mScale.x, mScale.y, 0.01f));
    }
}
void HatenaMimic::spawnCoinShower() {
    // This function is overridden so it won't spawn a coin shower from the 10 coins
    // This is how it works in nsmb2, it only spawns the side coins on the last hit.
    if (!((red::SpriteUtil::getNybble10(this)) & 0x1)) {BlockCoinBase::spawnCoinShower();}
}

void HatenaMimic::preSpawnItem() {
    // isBumpFromBelow only exists because onDownMoveStart() doesn't get called when mario groundpounds the block strangely
    // luckily onUpMoveStart() gets called before this does so i made a bool
    zPosOffset = 128.0f;
    if (this->isBumpFromBelow) {
        this->mSpawnDirection = cDirType_Up;
        if (red::SpriteUtil::getNybble20(this) == 1) {
            mVSpawnType = cVSpawnType_MoveUp;
        }
    } else {
        this->mSpawnDirection = cDirType_Down;
        if (red::SpriteUtil::getNybble20(this) == 1) {
            mVSpawnType = cVSpawnType_MoveDown;
        }
    }
    spawnCoins();

    this->isBumpFromBelow = false;
    return ActorBlockBase::preSpawnItem();
}

void HatenaMimic::onUpMoveStart() {
    this->isBumpFromBelow = true;
    return ActorBlockBase::onUpMoveStart();
}

void HatenaMimic::spawnItemUp() {
    spawnItem();
    return ActorBlockBase::spawnItemUp();
}

void HatenaMimic::spawnItemDown() {
    spawnItem();
    return ActorBlockBase::spawnItemDown();
}

void HatenaMimic::spawnItem() {
    // Certain contents don't spawn normally, so i have to make exceptions for those
    zPosOffset = 0.0f;
    bool isSmall = 0;
    u32 playerCount = PlayerMgr::instance()->getNumInGame();
    for (u32 i = 0; i < 4; i++) {
        PlayerObject * currentPlayerMode = PlayerMgr::instance()->getPlayerObject(i);
        if (currentPlayerMode != nullptr) {
            if ((currentPlayerMode->getPlayerMode() == PlayerMode::cPlayerMode_Small) || (currentPlayerMode->getPlayerMode() == PlayerMode::cPlayerMode_Mini)) {
                isSmall = 1;
            }
        }
    }
    switch (red::SpriteUtil::getNybbleRange(this, 8, 9)) {
        default:
            return;
        case 17:
            mContent = cContent_LifeMoon;
            if (playerCount > 1) {
                this->spawnMultiPowerup(mPos, 0, 1, true);
                return;
            }
            this->spawnPowerup(mPos, 0, 1, true);
            return;
        case 18:
            mContent = cContent_Empty;
            BlockCoinBase::spawnCoinShower();
            return;
    }
}

void HatenaMimic::spawnCoins() {
    // Activate the event
    if (red::SpriteUtil::getNybbleRange(this, 1, 2) != 0)
        SwitchFlagMgr::instance()->set(red::SpriteUtil::getNybbleRange(this, 1, 2) - 1, 0, true);
}

}