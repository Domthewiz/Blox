#include <map/SwitchFlagMgr.h>
#include <blox/Blox.h>
#include <blox/actor/ElasticBlockSwitchFlag.h>

namespace blox {

    SEAD_RTTI_OVERRIDE_IMPL(ElasticBlockSwitchFlag, ElasticBlockEvent);

    using ACI = ActorCreateInfo;
    const ActorCreateInfo ElasticBlockSwitchFlag::cCreateInfo = {
        .offset_x = 8, .offset_y = -8,
        .spawn_range = {
            .offset_x = 0, .offset_y = 0,
            .half_size_x = 512, .half_size_y = 512
        },
        .cull_range = { 
            .up = 0, .down = 0, .left = 0, .right = 0
        },
        .flag = ACI::cFlag_None
    };

    Profile* ElasticBlockSwitchFlag::sProfile = blox::getRegistrar()->newProfile<ElasticBlockSwitchFlag>("elastic_block_onoff")
        .resources<"slide_onoff">(ProfileInfo::cResType_Course)
        .drawPriority(240)
        .createInfo(cCreateInfo)
        .build();

    ElasticBlockSwitchFlag::ElasticBlockSwitchFlag(const ActorCreateParam& param)
        : ElasticBlockEvent(param)
    { }

    void ElasticBlockSwitchFlag::initModels() {
        for (u8 i = mMaxSize; i > 0; i--) {
            mModel[i] = AnimModel::create("slide_onoff", "block_slide", 2, 1);
            mModel[i]->playTexAnim("block_slide");
            mModel[i]->getTexAnim(0)->getFrameCtrl().setRate(0.0f);
            // make it blue
            mModel[i]->getTexAnim(0)->getFrameCtrl().setFrame(static_cast<f32>(mParam0 >> 0x1C & 0x1));
        }
    }

    void ElasticBlockSwitchFlag::getSwitchState() {
        mEventActive = SwitchFlagMgr::instance()->isActivated((mSwitchFlag0 & 0x3f) - 1);
        // mEventActive = MainGame::instance()->getCSSwitchState();
    }

}
