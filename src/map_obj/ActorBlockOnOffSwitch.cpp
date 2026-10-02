#include <blox/Blox.h>
#include <graphics/AnimModel.h>
#include <red/util/SpriteUtil.h>
#include <map_obj/ActorCoinMgr.h>
#include <map/SwitchFlagMgr.h>
#include <audio/GameAudio.h>
#include <blox/map_obj/ActorBlockOnOffSwitch.h>

namespace blox {

    SEAD_RTTI_OVERRIDE_IMPL(ActorBlockOnOffSwitch, ActorBlockBase);

    using ACI = ActorCreateInfo;
    const ActorCreateInfo ActorBlockOnOffSwitch::cCreateInfo = {
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

    Profile* ActorBlockOnOffSwitch::sProfile = blox::getRegistrar()->newProfile<ActorBlockOnOffSwitch>("oos")
        .resources<"block_oos">(ProfileInfo::cResType_Course)
        .drawPriority(232)
        .createInfo(cCreateInfo)
        .build();

    ActorBlockOnOffSwitch::ActorBlockOnOffSwitch(const ActorCreateParam& param)
        : ActorBlockBase(param)
        , mZPosOffset(0.0f)
        , mZInitialPosOffset(std::fmodf(mPos.x, 128.0f))
        , mModel(nullptr)
    { }

    ActorBase::Result ActorBlockOnOffSwitch::create() {
        mModel = AnimModel::create("block_oos", "block_DRC", 0, 2, 2);
        mModel->playTexAnim("block_DRC");
        mModel->playTexSrtAnim("player99");
        mModel->playTexSrtAnim("coin");

        if (SwitchFlagMgr::instance()->isActivated(red::SpriteUtil::getNybbleRange(this, 1, 2) - 1)) {
            mModel->getTexAnim(0)->getFrameCtrl().setFrame(1 + (2 * red::SpriteUtil::getNybble11(this)));
        } else {
            mModel->getTexAnim(0)->getFrameCtrl().setFrame(2 * red::SpriteUtil::getNybble11(this));
        }
        mModel->getTexAnim(0)->getFrameCtrl().setRate(0.0f);
        mModel->getShuAnim(0)->getFrameCtrl().setRate(1.0f);

        mForm = Form::cForm_Block;
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

    bool ActorBlockOnOffSwitch::execute() {

        if (!ActorBlockBase::execute()) {
            return false;
        }

        if (SwitchFlagMgr::instance()->isActivated(red::SpriteUtil::getNybbleRange(this, 1, 2) - 1)) {
            mModel->getTexAnim(0)->getFrameCtrl().setFrame(1 + (2 * red::SpriteUtil::getNybble11(this)));
        } else {
            mModel->getTexAnim(0)->getFrameCtrl().setFrame(2 * red::SpriteUtil::getNybble11(this));
        }

        updateModel();

        return true;
    }

    bool ActorBlockOnOffSwitch::draw() {
        if (mModel != nullptr) {
            mModel->draw();
        }
        return true;
    }

    void ActorBlockOnOffSwitch::updateModel() {
        if (mModel != nullptr) {
            mModel->update(sead::Vector3f(mPos.x, mPos.y + 8.0f, mPos.z + mZInitialPosOffset + mZPosOffset), mAngle, sead::Vector3f(mScale.x, mScale.y, 0.01f));
        }
    }

    void ActorBlockOnOffSwitch::preSpawnItem() {
        mZPosOffset = 128.0f;
        toggleEvent();
        return ActorBlockBase::preSpawnItem();
    }

    void ActorBlockOnOffSwitch::spawnItemUp() {
        this->mBumpMode = cBumpMode_None;
        mZPosOffset = 0.0f;
        changeState(StateID_Wait);
    }

    void ActorBlockOnOffSwitch::spawnItemDown() {
        this->mBumpMode = cBumpMode_None;
        mZPosOffset = 0.0f;
        changeState(StateID_Wait);
    }

    bool ActorBlockOnOffSwitch::restoreState() {
        return true;
    }

    void ActorBlockOnOffSwitch::destroy() {
        //changeState(StateID_UpMove_Diff); // TODO
        changeState(StateID_Wait);
        toggleEvent();
    }
    void ActorBlockOnOffSwitch::destroy2() {
        //changeState(StateID_UpMove_Diff); // TODO
        changeState(StateID_Wait);
        toggleEvent();
    }

    void ActorBlockOnOffSwitch::toggleEvent() {
        GameAudio::getAudioObjMap()->startSound("SE_OBJ_STEP_ON_SWITCH", mPos);
        if (SwitchFlagMgr::instance()->isActivated(red::SpriteUtil::getNybbleRange(this, 1, 2) - 1)) {
            SwitchFlagMgr::instance()->set(red::SpriteUtil::getNybbleRange(this, 1, 2) - 1, 0, false);
        } else {
            SwitchFlagMgr::instance()->set(red::SpriteUtil::getNybbleRange(this, 1, 2) - 1, 0, true);
        }
    }

}