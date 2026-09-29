#include "map_obj/BlockMgr.h"
#include <telkin/Print.h>
#include <blox/Blox.h>

red::Registrar* blox::getRegistrar() {
    static red::Registrar sRegistrar("blox");
    return &sRegistrar;
}

void main() {
    tk::println("Welcome to Domthewiz' Blox!");
}

#include <telkin/Hooks.h>
#include <telkin/Telkin.h>
// tPatch8u(0x024D3A63, 0);
// tPatch8u(0x)

 
// bool LogBlockMgrInfo(BlockMgr* _this, BlockMgr::HitParam* hit_param) {
//     tk::println("HitParam:\npos) %f, %f\nfragment_type) %u\nsensor_id) %u\n_d) %u\nplayer_no) %u\nplayer_type) %i\ncoll_check %u",hit_param->position.x,hit_param->position.y,hit_param->fragment_type,hit_param->sensor_id,hit_param->_d,hit_param->player_no,hit_param->player_type,hit_param->collision_check);
//     return _this->hitBlockAt(*hit_param);
//     // return;
// }
// tBranch(0x021A6BC0, LogBlockMgrInfo, tk::BranchType::bl);
// tBranch(0x021A6D3C, LogBlockMgrInfo, tk::BranchType::bl);
// tBranch(0x021A6DEC, LogBlockMgrInfo, tk::BranchType::bl);