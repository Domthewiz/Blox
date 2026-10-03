#include <actor/Profile.h>
#include <graphics/AnimModel.h>
#include <map_obj/ActorBlockBase.h>

namespace blox {

    class ActorBlockOnOffSwitch : public ActorBlockBase {
        SEAD_RTTI_OVERRIDE(ActorBlockOnOffSwitch, ActorBlockBase);

    public:
        static Profile* sProfile;
        static const ActorCreateInfo cCreateInfo;
    
    public:
        ActorBlockOnOffSwitch(const ActorCreateParam& param);
        ~ActorBlockOnOffSwitch() override = default;
        
    public:
        Result create() override;
        bool execute() override;
        bool draw() override;

        void calcMdl();
        bool restoreState() override;

        void destroy() override;
        void destroy2() override;
        void toggleEvent();
    
        void preSpawnItem() override;
        void spawnItemUp() override;
        void spawnItemDown() override;
        
        
    protected:
        AnimModel* mModel;
        f32 mZPosOffset;
        f32 mZInitialPosOffset;
    };
}