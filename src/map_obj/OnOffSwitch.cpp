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

class OnOffSwitch : public ActorBlockBase {

public:
    static Profile* cProfile;
    static const ActorCreateInfo cCreateInfo;

public:
    OnOffSwitch(const ActorCreateParam& param);
    ~OnOffSwitch() override = default;
    
    Result create() override;
    bool execute() override;
    bool draw() override;
    void updateModel();
    bool isBlockActive() override;
    void destroy() override;
    void destroy2() override;
    void toggleEvent();

    void preSpawnItem() override;
    void spawnItemUp() override;
    void spawnItemDown() override;
    
    f32 zPosOffset = 0.0f;

private:
    AnimModel* mTripModel;
};

using ACI = ActorCreateInfo;
const ActorCreateInfo OnOffSwitch::cCreateInfo = {
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

Profile* OnOffSwitch::cProfile = blox::getRegistrar()->newProfile<OnOffSwitch>("oos")
    .resources<"block_oos">(ProfileInfo::cResType_Course)
    .drawPriority(1)
    .createInfo(&cCreateInfo)
    .build();

OnOffSwitch::OnOffSwitch(const ActorCreateParam& param)
    : ActorBlockBase(param)
    , mTripModel(nullptr)
{ }

ActorBase::Result OnOffSwitch::create() {
    mTripModel = AnimModel::create("block_oos", "block_DRC", 0, 2, 2);
    mTripModel->playTexAnim("block_DRC");
    mTripModel->playTexSrtAnim("player99");
    mTripModel->playTexSrtAnim("coin");

    if (SwitchFlagMgr::instance()->isActivated(red::SpriteUtil::getNybbleRange(this, 1, 2) - 1)) {
        mTripModel->getTexAnim(0)->getFrameCtrl().setFrame(1 + (2 * red::SpriteUtil::getNybble11(this)));
    } else {
        mTripModel->getTexAnim(0)->getFrameCtrl().setFrame(2 * red::SpriteUtil::getNybble11(this));
    }
    mTripModel->getTexAnim(0)->getFrameCtrl().setRate(0.0f);
    mTripModel->getShuAnim(0)->getFrameCtrl().setRate(1.0f);

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
    
    execute();
    return cResult_Success;
}

bool OnOffSwitch::execute() {

    if (!ActorBlockBase::execute()) {
        return false;
    }

    if (SwitchFlagMgr::instance()->isActivated(red::SpriteUtil::getNybbleRange(this, 1, 2) - 1)) {
        mTripModel->getTexAnim(0)->getFrameCtrl().setFrame(1 + (2 * red::SpriteUtil::getNybble11(this)));
    } else {
        mTripModel->getTexAnim(0)->getFrameCtrl().setFrame(2 * red::SpriteUtil::getNybble11(this));
    }

    updateModel();

    return true;
}

bool OnOffSwitch::draw() {
    if (mTripModel != nullptr) {
        mTripModel->draw();
    }
    return true;
}

void OnOffSwitch::updateModel() {
    if (mTripModel != nullptr) {
        mTripModel->update(sead::Vector3f(mPos.x, mPos.y + 8.0f, mPos.z + std::fmodf(mPos.x, 128.0f) + zPosOffset), mAngle, sead::Vector3f(mScale.x, mScale.y, 0.01f));
    }
}

void OnOffSwitch::preSpawnItem() {
    zPosOffset = 128.0f;
    toggleEvent();
    return ActorBlockBase::preSpawnItem();
}

void OnOffSwitch::spawnItemUp() {
    zPosOffset = 0.0f;
    changeState(StateID_Wait);
}

void OnOffSwitch::spawnItemDown() {
    zPosOffset = 0.0f;
    changeState(StateID_Wait);
}

bool OnOffSwitch::isBlockActive() {
    return true;
}

void OnOffSwitch::destroy() {
    //changeState(StateID_UpMove); // TODO
    changeState(StateID_Wait);
    toggleEvent();
}
void OnOffSwitch::destroy2() {
    //changeState(StateID_UpMove); // TODO
    changeState(StateID_Wait);
    toggleEvent();
}

void OnOffSwitch::toggleEvent() {
    if (SwitchFlagMgr::instance()->isActivated(red::SpriteUtil::getNybbleRange(this, 1, 2) - 1)) {
        SwitchFlagMgr::instance()->set(red::SpriteUtil::getNybbleRange(this, 1, 2) - 1, 0, false);
    } else {
        SwitchFlagMgr::instance()->set(red::SpriteUtil::getNybbleRange(this, 1, 2) - 1, 0, true);
    }
}

}