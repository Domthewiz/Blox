#pragma once

#include <blox/actor/ElasticBlockEvent.h>

namespace blox {

    class ElasticBlockSwitchFlag : public ElasticBlockEvent {
        SEAD_RTTI_OVERRIDE(ElasticBlockSwitchFlag, ElasticBlockEvent);

    public:
        static Profile* sProfile;
        static const ActorCreateInfo cCreateInfo;

    public:
        ElasticBlockSwitchFlag(const ActorCreateParam& param);
        ~ElasticBlockSwitchFlag() override = default;

    public:
        void initModels() override;
        void getSwitchState() override;

    };
}