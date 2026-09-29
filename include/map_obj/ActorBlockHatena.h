#pragma once

#include <map_obj/ActorBlockBase.h>

class ActorBlockHatena : public ActorBlockBase // vtbl: 0x100E874C
{ // derivedRuntimeInfo: 0x026A56CC
    public:
        // Address: 0x026A4FAC
        ActorBlockHatena(const ActorCreateParam& param);
        // Address: 0x026A58C8
        ~ActorBlockHatena() override { }

    protected:
        // Address: 0x026A5010
        Result create() override;

    public:
        // Address: 0x026A52EC
        void updateLiquidEffects() override;
        // Address: 0x026A5330
        void onBumpDiff() override;
        // Address: 0x026A5394
        void postBump() override;
        // Address: 0x026A5434
        u32 vf32C() override;
};
static_assert(sizeof(ActorBlockHatena) == 0x1CD0, "ActorBlockBase size mismatch");
