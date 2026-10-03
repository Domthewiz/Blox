#include "actor/ActorBase.h"
#include "actor/Profile.h"
#include "graphics/AnimModel.h"
#include "map_obj/ActorBlockBase.h"
#include "prim/seadRuntimeTypeInfo.h"
#include <blox/utility/ShockBlockHitter.h>

namespace blox {

    class ActorBlockShock : public ActorBlockBase {
        SEAD_RTTI_OVERRIDE(ActorBlockShock, ActorBlockBase);
    public:
        static Profile* sProfile;
        static const ActorCreateInfo cCreateInfo;

    public:
        ActorBlockShock(const ActorCreateParam& param);
        ~ActorBlockShock() override = default;
        
    public:
        Result create() override;
        bool execute() override;
        bool draw() override;

        void calcMdl();

        void destroy() override;
        void destroy2() override;
        void doRedPowQuake();
        
        void onBumpDiff() override;

        void preSpawnItem() override;
        void spawnItemUp() override;
        void spawnItemDown() override;

        static void collisionCallback(ActorCollisionCheck* cc_self, ActorCollisionCheck* cc_other);
        
        
        protected:
            f32 mZPosOffset;
            AnimModel* mModel;
            ShockBlockHitter mShock;
            bool mExplosionActive;
            u8 mExplosionTimer;
            bool mHitAlready;
            u8 mResidueRemovalTimer;
    };

}