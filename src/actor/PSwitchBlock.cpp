#include <actor/Actor.h>
#include <graphics/AnimModel.h>
#include <blox/Blox.h>
#include <telkin/Print.h>
#include <collision/ActorBoxBgCollision.h>
#include <collision/ActorBgCollisionMgr.h>
#include <red/util/SpriteUtil.h>
#include <map/SwitchFlagMgr.h>

namespace blox {

class PSwitchBlock : public Actor {
public:
    static Profile* cProfile;
    static const ActorCreateInfo cCreateInfo;

public:
    PSwitchBlock(const ActorCreateParam& param);
    ~PSwitchBlock() override = default;
    
    Result create() override;
    bool execute() override;
    bool draw() override;
    void updateModel();

private: 
    AnimModel* mModel;
    ActorBoxBgCollision mCollider;
    bool isDotted;
};

using ACI = ActorCreateInfo;
const ActorCreateInfo PSwitchBlock::cCreateInfo = {
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

Profile* PSwitchBlock::cProfile = blox::getRegistrar()->newProfile<PSwitchBlock>("psb")
    .resources<"block_psb">(ProfileInfo::cResType_Course)
    .drawPriority(1)
    .createInfo(&cCreateInfo)
    .build();

PSwitchBlock::PSwitchBlock(const ActorCreateParam& param)
    : Actor(param)
    , mModel(nullptr)
{ }

ActorBase::Result PSwitchBlock::create() {
    mModel = AnimModel::create("block_psb", "block_DRC", 0, 2, 2);
    mModel->playTexAnim("block_DRC");

    mModel->getTexAnim(0)->getFrameCtrl().setRate(0.0f);

    sead::Vector2f mTileSize = sead::Vector2f(1.0f,1.0f);
    
    mCollider.set(this, {
        .pos_offset         = { 0.0f, 0.0f },
        .rot_pivot_offset   = { 0.0f, 0.0f },
        .left_top_offset    = { mTileSize.x * -8.0f, mTileSize.y *  8.0f },
        .right_under_offset = { mTileSize.x * 8.0f, mTileSize.y * -8.0f },
        .angle              = mAngle.z()
    });
    
    if (!red::SpriteUtil::getNybble5(this)) {
        ActorBgCollisionMgr::instance()->entry(mCollider);
    } else {
        isDotted = true;
    }

    if (isDotted) {
        if (!SwitchFlagMgr::instance()->isPSwitch()) {
            mModel->getTexAnim(0)->getFrameCtrl().setFrame(1 + (2 * red::SpriteUtil::getNybble11(this)));
        } else {
            mModel->getTexAnim(0)->getFrameCtrl().setFrame(2 * red::SpriteUtil::getNybble11(this));
        }
    } else {
        if (SwitchFlagMgr::instance()->isPSwitch()) {
            mModel->getTexAnim(0)->getFrameCtrl().setFrame(1 + (2 * red::SpriteUtil::getNybble11(this)));
        } else {
            mModel->getTexAnim(0)->getFrameCtrl().setFrame(2 * red::SpriteUtil::getNybble11(this));
        }
    }

    return cResult_Success;
}

bool PSwitchBlock::execute() {
    mCollider.execute();
    
    if (SwitchFlagMgr::instance()->isPSwitch()) {
        if (red::SpriteUtil::getNybble5(this)) {
            mModel->getTexAnim(0)->getFrameCtrl().setFrame(2 * red::SpriteUtil::getNybble11(this));
            ActorBgCollisionMgr::instance()->entry(mCollider);
        } else {
            mModel->getTexAnim(0)->getFrameCtrl().setFrame(1 + (2 * red::SpriteUtil::getNybble11(this)));
            ActorBgCollisionMgr::instance()->release(mCollider);
        }
    } else {
        if (red::SpriteUtil::getNybble5(this)) {
            mModel->getTexAnim(0)->getFrameCtrl().setFrame(1 + (2 * red::SpriteUtil::getNybble11(this)));
            ActorBgCollisionMgr::instance()->release(mCollider);
        } else {
            mModel->getTexAnim(0)->getFrameCtrl().setFrame(2 * red::SpriteUtil::getNybble11(this));
            ActorBgCollisionMgr::instance()->entry(mCollider);
        }
    } 

    updateModel();

    return true;
}

bool PSwitchBlock::draw() {
    if (mModel != nullptr) {
        mModel->draw();
    }
    return true;
}

void PSwitchBlock::updateModel() {
    if (mModel != nullptr) {
        mModel->update(mPos, mAngle, mScale);
    }
}

}
