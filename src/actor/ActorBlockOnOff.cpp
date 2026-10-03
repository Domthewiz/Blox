#include <blox/Blox.h>
#include <collision/ActorBgCollisionMgr.h>
#include <red/util/SpriteUtil.h>
#include <map/SwitchFlagMgr.h>
#include <blox/actor/ActorBlockOnOff.h>

namespace blox {
    SEAD_RTTI_OVERRIDE_IMPL(ActorBlockOnOff, Actor);

    using ACI = ActorCreateInfo;
    const ActorCreateInfo ActorBlockOnOff::cCreateInfo = {
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

    Profile* ActorBlockOnOff::sProfile = blox::getRegistrar()->newProfile<ActorBlockOnOff>("oob")
        .resources<"block_oob">(ProfileInfo::cResType_Course)
        .drawPriority(232)
        .createInfo(cCreateInfo)
        .build();

    ActorBlockOnOff::ActorBlockOnOff(const ActorCreateParam& param)
        : Actor(param)
        , mModel(nullptr)
    { }

    ActorBase::Result ActorBlockOnOff::create() {
        mModel = AnimModel::create("block_oob", "block_DRC", 0, 2, 2);
        mModel->playTexAnim("block_DRC");
        mModel->playTexSrtAnim("player99");
        mModel->playTexSrtAnim("coin");

        if (SwitchFlagMgr::instance()->isActivated(red::SpriteUtil::getNybbleRange(this, 1, 2) - 1)) {
            mModel->getTexAnim(0)->getFrameCtrl().setFrame(1 + (2 * red::SpriteUtil::getNybble11(this)));
        } else {
            mModel->getTexAnim(0)->getFrameCtrl().setFrame(2 * red::SpriteUtil::getNybble11(this));
        }
        mModel->getShuAnim(0)->getFrameCtrl().setFrame(red::SpriteUtil::getNybble5(this));

        mModel->getTexAnim(0)->getFrameCtrl().setRate(0.0f);
        mModel->getShuAnim(0)->getFrameCtrl().setRate(0.0f);

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
        }

        execute();
        
        return cResult_Success;
    }

    bool ActorBlockOnOff::execute() {
        mCollider.execute();
        
        if (SwitchFlagMgr::instance()->isActivated(red::SpriteUtil::getNybbleRange(this, 1, 2) - 1)) {
            mModel->getTexAnim(0)->getFrameCtrl().setFrame(1 + (2 * red::SpriteUtil::getNybble11(this)));
            if (red::SpriteUtil::getNybble5(this)) {
                ActorBgCollisionMgr::instance()->entry(mCollider);
            } else {
                ActorBgCollisionMgr::instance()->release(mCollider);
            }
        } else {
            mModel->getTexAnim(0)->getFrameCtrl().setFrame(2 * red::SpriteUtil::getNybble11(this));
            if (red::SpriteUtil::getNybble5(this)) {
                ActorBgCollisionMgr::instance()->release(mCollider);
            } else {
                ActorBgCollisionMgr::instance()->entry(mCollider);
            }
        } 

        calcMdl();

        return true;
    }

    bool ActorBlockOnOff::draw() {
        if (mModel != nullptr) {
            mModel->draw();
        }
        return true;
    }

    void ActorBlockOnOff::calcMdl() {
        if (mModel != nullptr) {
            mModel->update(mPos, mAngle, mScale);
        }
    }

}
