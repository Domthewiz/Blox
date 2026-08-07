#include <actor/Actor.h>
#include <blox/Blox.h>
#include <telkin/Print.h>
#include <graphics/JointBlendModel.h>
#include <red/util/SpriteUtil.h>
#include <map_obj/ActorBlockBase.h>
#include <map_obj/ActorCoinMgr.h>
#include <player/PlayerMgr.h>
#include <player/PlayerObject.h>
#include <map/SwitchFlagMgr.h>

namespace blox {

class TripBl : public ActorBlockBase {

public:
    static Profile* cProfile;
    static const ActorCreateInfo cCreateInfo;

public:
    TripBl(const ActorCreateParam& param);
    ~TripBl() override = default;
    
    Result create() override;
    bool execute() override;
    bool draw() override;

    void spawnItemUp() override;
    void spawnItemDown() override;
    void spawnItem();
    void spawnCoins();
    void updateModel();

    void onUpMoveStart() override;

    void spawnSideCoins();
    void spawnCoinShower() override;
    void preSpawnItem() override;

    s32 hitPlayerID = 0;
    u8 frameCounter = 0;
    bool isBumpFromBelow = false;
    bool gotBonkedModel = false;

private:
    JointBlendModel* mTripModel;
};

using ACI = ActorCreateInfo;
const ActorCreateInfo TripBl::cCreateInfo = {
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

Profile* TripBl::cProfile = blox::getRegistrar()->newProfile<TripBl>("tripbk")
    .resources<"blocklong">(ProfileInfo::cResType_Course)
    .drawPriority(1)
    .createInfo(&cCreateInfo)
    .build();

TripBl::TripBl(const ActorCreateParam& param)
    : ActorBlockBase(param)
    , mTripModel(nullptr)
{ }

ActorBase::Result TripBl::create() {

    mTripModel = JointBlendModel::create("blocklong", "block_DRC", 0, 2, 2);
    mTripModel->playTexAnim("block_DRC");
    mTripModel->playTexSrtAnim("player99");
    mTripModel->playTexSrtAnim("coin");
    mTripModel->getTexAnim(0)->getFrameCtrl().setFrame(2 * red::SpriteUtil::getNybble11(this));
    mTripModel->getTexAnim(0)->getFrameCtrl().setRate(0.0f);
    mTripModel->getShuAnim(0)->getFrameCtrl().setRate(0.0f);
    //mTripModel->getShuAnim(0)->getFrameCtrl().setRate(0.33333333333f);

    _1c68 = 1;
    _1ab4 = 0;
    _1aec = 0;
    _1cc0 = 0;
    
    mCollisionMask.setDirect(0x01);

    mType = cType_Hatena;
    mBoxBgCollision.setType(BgCollision::cType_QuestionBlock);

    mContent = cContent_Empty;

    ActorBlockBase::init(true,true);

    registerColliderActiveInfo();

    mBoxBgCollision.setFlag(0x00000018);
    mBoxBgCollision.setCallback(
        &BlockCoinBase::callbackFoot,
        &BlockCoinBase::callbackHead,
        &BlockCoinBase::callBackWall
    );
    mBoxBgCollision.setDrcTouchCallback(&mDrcTouchCallback);

    changeState(StateID_Wait);

    mBoxBgCollision.getPoints()[0].x -= 16.0f;
    mBoxBgCollision.getPoints()[1].x += 16.0f;
    mBoxBgCollision.getPoints()[2].x += 16.0f;
    mBoxBgCollision.getPoints()[3].x -= 16.0f;

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
        mTripModel->getTexAnim(0)->getFrameCtrl().setFrame(1 + (2 * red::SpriteUtil::getNybble11(this)));
        this->gotBonkedModel = true;
    }
    
    execute();
    return cResult_Success;
}

bool TripBl::execute() {

    // This is to replicate the 20fps of the regular ? block
    
    if (frameCounter < 2) {
        this->frameCounter ++;
        mTripModel->getShuAnim(0)->getFrameCtrl().setRate(0.0f);
    } else {
        this->frameCounter = 0;
        mTripModel->getShuAnim(0)->getFrameCtrl().setRate(1.0f);
    }


    if (!ActorBlockBase::execute()) {
        return false;
    }

    if (mType == cType_Hit) {
        changeState(StateID_HitWait);
        if (!gotBonkedModel) {
            mTripModel->getTexAnim(0)->getFrameCtrl().setFrame(1 + (2 * red::SpriteUtil::getNybble11(this)));
            this->gotBonkedModel = true;
        }
    }

    updateModel();

    return true;
}

bool TripBl::draw() {
    mTripModel->draw();
    return true;
}

void TripBl::updateModel() {
    mTripModel->update(sead::Vector3f(mPos.x, mPos.y + 8.0f, mPos.z + std::fmodf(mPos.x, 128.0f)), mAngle, sead::Vector3f(mScale.x, mScale.y, 0.0f));
}
void TripBl::spawnCoinShower() {
    // This function is overridden so it won't spawn a coin shower from the 10 coins
    // This is how it works in nsmb2, it only spawns the side coins on the last hit.
    tk::println("Coin Shower Cancelled");
}

void TripBl::preSpawnItem() {
    // isBumpFromBelow only exists because onDownMoveStart() doesn't get called when mario groundpounds the block strangely
    // luckily onUpMoveStart() gets called before this does so i made a bool
    if (this->isBumpFromBelow) {
        this->mSpawnDirection = cDirType_Up;
    } else {
        this->mSpawnDirection = cDirType_Down;
    }
    if (((red::SpriteUtil::getNybble10(this)) & 0x1) && red::SpriteUtil::getNybbleRange(this, 8, 9) != 18) {this->spawnSideCoins();}
    spawnCoins();

    this->isBumpFromBelow = false;
    return ActorBlockBase::preSpawnItem();
}

void TripBl::onUpMoveStart() {
    this->isBumpFromBelow = true;
    return ActorBlockBase::onUpMoveStart();
}

void TripBl::spawnItemUp() {
    spawnItem();
    return ActorBlockBase::spawnItemUp();
}

void TripBl::spawnItemDown() {
    spawnItem();
    return ActorBlockBase::spawnItemDown();
}

void TripBl::spawnItem() {
    // Certain contents don't spawn normally, so i have to make exceptions for those
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

void TripBl::spawnCoins() {
    // Activate the event
    SwitchFlagMgr::instance()->set(red::SpriteUtil::getNybbleRange(this, 1, 2) - 1, 0, true);

    // Coins get spawned immediately when the block is hit
    switch (red::SpriteUtil::getNybbleRange(this, 8, 9)) {
        default:
            return;
        case 1:
            mContent = cContent_Coin;
            ActorCoinMgr::instance()->spawnItemCoin(mPos, this->mSpawnDirection, this->hitPlayerID);
            return;
    }
}

void TripBl::spawnSideCoins() {
    ActorCoinMgr::instance()->spawnItemCoin(mPos + sead::Vector3f(-16.0f, 0.0f, 0.0f), this->mSpawnDirection, this->hitPlayerID);
    ActorCoinMgr::instance()->spawnItemCoin(mPos + sead::Vector3f( 16.0f, 0.0f, 0.0f), this->mSpawnDirection, this->hitPlayerID);
}

}