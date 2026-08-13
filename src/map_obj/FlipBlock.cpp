#include <actor/Actor.h>
#include <blox/Blox.h>
#include <graphics/AnimModel.h>
#include <telkin/Print.h>
#include <red/util/SpriteUtil.h>
#include <map_obj/ActorBlockBase.h>
#include <player/PlayerMgr.h>
#include <player/PlayerObject.h>
#include <map/SwitchFlagMgr.h>
#include <enemy/Enemy.h>
#include <collision/ActorBgCollisionMgr.h>

namespace blox {

class FlipBlock : public ActorBlockBase {

public:
    static Profile* cProfile;
    static const ActorCreateInfo cCreateInfo;

public:
    FlipBlock(const ActorCreateParam& param);
    ~FlipBlock() override = default;
    
    Result create() override;
    bool execute() override;
    bool draw() override;
    void updateModel();
    bool isBlockActive() override;
    void destroy() override;
    void destroy2() override;
    bool playerOverlaps();

    void preSpawnItem() override;
    void spawnItemUp() override;
    void spawnItemDown() override;

    DECLARE_STATE_ID(FlipBlock, Flipping)

private:
    AnimModel* mModel;
    void initializeFootSensor_();
    void setBoxBgCollisionOfs_();
    s32 mFlipsRemaining;
    bool flipInstantly;
};

using CC = ActorCollisionCheck;
static const CC::CollisionData cCcData = {
    .center_offset = { 0.0f, 8.0f },
    .half_size = { 8.0f, 8.0f },
    .shape_type = CC::cShapeType_Box,
    .kind = CC::cKind_Enemy,
    .attack = CC::cAttack_None,
    .vs_kind = CC::cTargetKind_None,
    .vs_damage = CC::cDamageFrom_None,
    .status = CC::cStatus_None,
    .callback = &Enemy::normal_collcheck
};

CREATE_STATE_ID(FlipBlock, Flipping)

using ACI = ActorCreateInfo;
const ActorCreateInfo FlipBlock::cCreateInfo = {
    .offset_x = 8, .offset_y = -8,
    .spawn_range = {
        .offset_x = 0, .offset_y = 0,
        .half_size_x = 0x100, .half_size_y = 0x100
    },
    .cull_range = { 
        .up = 0, .down = 0, .left = 0, .right = 0
    },
    .flag = 0
};

Profile* FlipBlock::cProfile = blox::getRegistrar()->newProfile<FlipBlock>("flip")
    .resources<"block_flipp">(ProfileInfo::cResType_Course)
    .drawPriority(1)
    .createInfo(&cCreateInfo)
    .build();

FlipBlock::FlipBlock(const ActorCreateParam& param)
    : ActorBlockBase(param)
    , mModel(nullptr)
{ }

void FlipBlock::initializeFootSensor_()
{
    mFootSensor.p1 = -(8 - 1);
    mFootSensor.p2 = (8 - 1);
    mFootSensor.center_offset = -8;
}

void FlipBlock::setBoxBgCollisionOfs_()
{
    setBoxBgCollisionOfs(-8, 16, 8, 0);
}

ActorBase::Result FlipBlock::create() {
    mModel = AnimModel::create("block_flipp", "block_slide", 0, 0, 0);

    flipInstantly = red::SpriteUtil::getNybble5(this);

    _1c68 = 1;
    _1ab4 = 0;
    _1aec = 0;
    _1cc0 = 0;

    mType = cType_Hatena;
    mBoxBgCollision.setType(BgCollision::cType_QuestionBlock);
    mContent = cContent_Empty;
    
    if (!ActorBlockBase::init(true,true)) {
        return cResult_Failed;
    }

    registerColliderActiveInfo();
    changeState(StateID_Wait);

    mCollisionCheck.set(this, cCcData);
    reviveCollisionCheck();
    
    execute();
    return cResult_Success;
}

bool FlipBlock::execute() {

    if (!ActorBlockBase::execute()) {
        return false;
    }

    updateModel();

    return true;
}

bool FlipBlock::draw() {
    if (mModel != nullptr) {
        mModel->draw();
    }
    return true;
}

void FlipBlock::updateModel() {
    if (mModel != nullptr) {
        mModel->update(sead::Vector3f(mPos.x, mPos.y + 8.0f, mPos.z), mAngle, sead::Vector3f(mScale.x,mScale.y,0.5));
    }
}

void FlipBlock::preSpawnItem() {
    if (flipInstantly) {
        changeState(StateID_Flipping);
    }
    SwitchFlagMgr::instance()->set(red::SpriteUtil::getNybbleRange(this, 1, 2) - 1, 0, true);
    return ActorBlockBase::preSpawnItem();
}

void FlipBlock::spawnItemUp() {
    this->mVSpawnType = 0;
    if (!flipInstantly) {
        changeState(StateID_Flipping);
    }
}

void FlipBlock::spawnItemDown() {
    this->mVSpawnType = 0;
    if (!flipInstantly) {
        changeState(StateID_Flipping);
    }
}

bool FlipBlock::isBlockActive() {
    return true;
}

void FlipBlock::destroy() {
    //changeState(StateID_UpMove_Diff); // TODO
    changeState(StateID_Flipping);
    SwitchFlagMgr::instance()->set(red::SpriteUtil::getNybbleRange(this, 1, 2) - 1, 0, true);
}
void FlipBlock::destroy2() {
    //changeState(StateID_UpMove_Diff); // TODO
    changeState(StateID_Flipping);
    SwitchFlagMgr::instance()->set(red::SpriteUtil::getNybbleRange(this, 1, 2) - 1, 0, true);
}

// TODO: make this code less abood-ness
void FlipBlock::initializeState_Flipping()
{
    mFlipsRemaining = 7;
    ActorBgCollisionMgr::instance()->release(mBoxBgCollision);
}

void FlipBlock::executeState_Flipping()
{
    if (mSpawnDirection == cDirType_Down) // Down
        mAngle.x() += 0x8000000;

    else
        mAngle.x() -= 0x8000000;

    if (mAngle.x() == 0 && --mFlipsRemaining <= 0 && !playerOverlaps())
    {
        // Add the collider back and literally "reset" the actor
        init(true, true);
        changeState(StateID_Wait);
        mContent = cContent_Empty;
    }
}

void FlipBlock::finalizeState_Flipping()
{
    mVSpawnType = cVSpawnType_None;
    mAngle.x() = 0;

    setBoxBgCollisionOfs_();
}

bool FlipBlock::playerOverlaps()
{
    for (s32 i = 0; i < 4; i++)
    {
        if (PlayerMgr::instance()->isPlayerActive(i))
        {
            const PlayerObject* player = PlayerMgr::instance()->getPlayerObject(i);
            if (player != nullptr && mCollisionCheck.isOverlap(player->getCollisionCheck()))
                return true;
        }
    }
    return false;
}

}