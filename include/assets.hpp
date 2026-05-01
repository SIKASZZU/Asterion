// Keep this file as simple as possible
// The purpose of this file is to serve as a map of all resources used by the game
// And to make it easy to copy resources used by the game into the bld folder
// NB: All resource paths should be from the root of the workspace

namespace Assets {
    namespace Images {
        static const char* spritesheet = "assets/spritesheet.png";
        static const char* tree = "assets/tree.png";
        static const char* tree_trunk = "assets/tree_trunk.png";
        static const char* ground_cube = "assets/ground_cube.png";
        static const char* ground_cube_sprite = "assets/ground_cube_sprite_sheet.png";
        static const char* maze_ground_cube = "assets/maze_ground_cube.png";
        static const char* maze_ground_spirte = "assets/maze_ground_cube_sprite_sheet.png";
        // static const char* snowy_ground_cube = "assets/snowy_ground_cube.png";
        static const char* error_cube = "assets/error_cube.png";
        static const char* yellow_cube = "assets/yellow_cube.png";
        static const char* blue_cube = "assets/blue_cube.png";
        static const char* ingrown_wall_cube = "assets/ingrown_wall_cube.png";
        static const char* wall_cube = "assets/wall_cube.png";
        static const char* wall_cube_ground = "assets/wall_cube_ground.png";
        static const char* wall_cube_sprite = "assets/wall_cube_sprite_sheet.png";
        static const char* vine_cube_hard = "assets/vine_cube_hard.png";
        static const char* vine_cube_medium = "assets/vine_cube_medium.png";
        static const char* vine_cube_soft = "assets/vine_cube_soft.png";
        static const char* vine_overhang_sn = "assets/vine_overhang_SN.png";
        static const char* vine_overhang_ew = "assets/vine_overhang_EW.png";
        static const char* vine_cover_n = "assets/vine_cover_N.png";
        static const char* void_cube = "assets/void_cube.png";
        static const char* void_cube_tilemap = "assets/void_cube_tilemap.png";
        static const char* grass_cover_sprite = "assets/grass_cover_sprite_sheet.png";
        static const char* grass_cover = "assets/grass_cover.png";
        static const char* grass_cover_tall = "assets/grass_cover_tall.png";
        static const char* grass_cover_short = "assets/grass_cover_short.png";
        static const char* invisible_cube = "assets/invisible_cube.png";
        static const char* wall_markings = "assets/cube_markings.png";
        static const char* maze_deco = "assets/maze_deco.png";
        static const char* campfire = "assets/campfire-Sheet.png";
    }
    namespace Player {

        //player animation
        // static const char* player_animation = "assets/player_sprite_sheet.png";

        // player-girl
        // ready idle
        static const char* player_girl_idle_down = "assets/player/GirlSampleReadyIdle/GirlSample_ReadyIdle_Down.png";
        static const char* player_girl_idle_down_left = "assets/player/GirlSampleReadyIdle/GirlSample_ReadyIdle_DownLeft.png";
        static const char* player_girl_idle_down_right = "assets/player/GirlSampleReadyIdle/GirlSample_ReadyIdle_DownRight.png";
        static const char* player_girl_idle_right = "assets/player/GirlSampleReadyIdle/GirlSample_ReadyIdle_Right.png";
        static const char* player_girl_idle_left = "assets/player/GirlSampleReadyIdle/GirlSample_ReadyIdle_Left.png";
        static const char* player_girl_idle_up = "assets/player/GirlSampleReadyIdle/GirlSample_ReadyIdle_Up.png";
        static const char* player_girl_idle_up_left = "assets/player/GirlSampleReadyIdle/GirlSample_ReadyIdle_UpLeft.png";
        static const char* player_girl_idle_up_right = "assets/player/GirlSampleReadyIdle/GirlSample_ReadyIdle_UpRight.png";

        // walk
        static const char* player_girl_walk_down = "assets/player/GirlSampleWalk/GirlSample_Walk_Down.png";
        static const char* player_girl_walk_down_left = "assets/player/GirlSampleWalk/GirlSample_Walk_DownLeft.png";
        static const char* player_girl_walk_down_right = "assets/player/GirlSampleWalk/GirlSample_Walk_DownRight.png";
        static const char* player_girl_walk_right = "assets/player/GirlSampleWalk/GirlSample_Walk_Right.png";
        static const char* player_girl_walk_left = "assets/player/GirlSampleWalk/GirlSample_Walk_Left.png";
        static const char* player_girl_walk_up = "assets/player/GirlSampleWalk/GirlSample_Walk_Up.png";
        static const char* player_girl_walk_up_left = "assets/player/GirlSampleWalk/GirlSample_Walk_UpLeft.png";
        static const char* player_girl_walk_up_right = "assets/player/GirlSampleWalk/GirlSample_Walk_UpRight.png";

        // run
        static const char* player_girl_run_down = "assets/player/GirlSampleRun/GirlSample_Run_Down.png";
        static const char* player_girl_run_down_left = "assets/player/GirlSampleRun/GirlSample_Run_DownLeft.png";
        static const char* player_girl_run_down_right = "assets/player/GirlSampleRun/GirlSample_Run_DownRight.png";
        static const char* player_girl_run_right = "assets/player/GirlSampleRun/GirlSample_Run_Right.png";
        static const char* player_girl_run_left = "assets/player/GirlSampleRun/GirlSample_Run_Left.png";
        static const char* player_girl_run_up = "assets/player/GirlSampleRun/GirlSample_Run_Up.png";
        static const char* player_girl_run_up_left = "assets/player/GirlSampleRun/GirlSample_Run_UpLeft.png";
        static const char* player_girl_run_up_right = "assets/player/GirlSampleRun/GirlSample_Run_UpRight.png";

        // jump
        static const char* player_girl_jump_down = "assets/player/GirlSampleJump/GirlSample_Jump_Down.png";
        static const char* player_girl_jump_down_left = "assets/player/GirlSampleJump/GirlSample_Jump_DownLeft.png";
        static const char* player_girl_jump_down_right = "assets/player/GirlSampleJump/GirlSample_Jump_DownRight.png";
        static const char* player_girl_jump_right = "assets/player/GirlSampleJump/GirlSample_Jump_Right.png";
        static const char* player_girl_jump_left = "assets/player/GirlSampleJump/GirlSample_Jump_Left.png";
        static const char* player_girl_jump_up = "assets/player/GirlSampleJump/GirlSample_Jump_Up.png";
        static const char* player_girl_jump_up_left = "assets/player/GirlSampleJump/GirlSample_Jump_UpLeft.png";
        static const char* player_girl_jump_up_right = "assets/player/GirlSampleJump/GirlSample_Jump_UpRight.png";

        // running jump
        static const char* player_girl_running_jump_down = "assets/player/GirlSampleRunningJump/GirlSample_RunningJump_Down.png";
        static const char* player_girl_running_jump_down_left = "assets/player/GirlSampleRunningJump/GirlSample_RunningJump_DownLeft.png";
        static const char* player_girl_running_jump_down_right = "assets/player/GirlSampleRunningJump/GirlSample_RunningJump_DownRight.png";
        static const char* player_girl_running_jump_right = "assets/player/GirlSampleRunningJump/GirlSample_RunningJump_Right.png";
        static const char* player_girl_running_jump_left = "assets/player/GirlSampleRunningJump/GirlSample_RunningJump_Left.png";
        static const char* player_girl_running_jump_up = "assets/player/GirlSampleRunningJump/GirlSample_RunningJump_Up.png";
        static const char* player_girl_running_jump_up_left = "assets/player/GirlSampleRunningJump/GirlSample_RunningJump_UpLeft.png";
        static const char* player_girl_running_jump_up_right = "assets/player/GirlSampleRunningJump/GirlSample_RunningJump_UpRight.png";

        // damage
        static const char* player_girl_damage_down = "assets/player/GirlSampleDamaged/GirlSample_Damaged_Down.png";
        static const char* player_girl_damage_down_left = "assets/player/GirlSampleDamaged/GirlSample_Damaged_DownLeft.png";
        static const char* player_girl_damage_down_right = "assets/player/GirlSampleDamaged/GirlSample_Damaged_DownRight.png";
        static const char* player_girl_damage_right = "assets/player/GirlSampleDamaged/GirlSample_Damaged_Right.png";
        static const char* player_girl_damage_left = "assets/player/GirlSampleDamaged/GirlSample_Damaged_Left.png";
        static const char* player_girl_damage_up = "assets/player/GirlSampleDamaged/GirlSample_Damaged_Up.png";
        static const char* player_girl_damage_up_left = "assets/player/GirlSampleDamaged/GirlSample_Damaged_UpLeft.png";
        static const char* player_girl_damage_up_right = "assets/player/GirlSampleDamaged/GirlSample_Damaged_UpRight.png";
    }

    namespace Spider {
        // run
        static const char* spider_run45_animation = "assets/spider/Run/Run_Body_045.png";
        static const char* spider_run135_animation = "assets/spider/Run/Run_Body_135.png";
        static const char* spider_run225_animation = "assets/spider/Run/Run_Body_225.png";
        static const char* spider_run315_animation = "assets/spider/Run/Run_Body_315.png";
        static const char* spider_run90_animation = "assets/spider/Run/Run_Body_090.png";
        static const char* spider_run180_animation = "assets/spider/Run/Run_Body_180.png";
        static const char* spider_run270_animation = "assets/spider/Run/Run_Body_270.png";
        static const char* spider_run0_animation = "assets/spider/Run/Run_Body_0.png";
        // walk
        static const char* spider_walk45_animation = "assets/spider/Walk/Walk_Body_045.png";
        static const char* spider_walk135_animation = "assets/spider/Walk/Walk_Body_135.png";
        static const char* spider_walk225_animation = "assets/spider/Walk/Walk_Body_225.png";
        static const char* spider_walk315_animation = "assets/spider/Walk/Walk_Body_315.png";
        static const char* spider_walk90_animation = "assets/spider/Walk/Walk_Body_090.png";
        static const char* spider_walk180_animation = "assets/spider/Walk/Walk_Body_180.png";
        static const char* spider_walk270_animation = "assets/spider/Walk/Walk_Body_270.png";
        static const char* spider_walk0_animation = "assets/spider/Walk/Walk_Body_0.png";
        // idle
        static const char* spider_idle45_animation = "assets/spider/Idle_Nervous/Idle_Nervous_Body_045.png";
        static const char* spider_idle135_animation = "assets/spider/Idle_Nervous/Idle_Nervous_Body_135.png";
        static const char* spider_idle225_animation = "assets/spider/Idle_Nervous/Idle_Nervous_Body_225.png";
        static const char* spider_idle315_animation = "assets/spider/Idle_Nervous/Idle_Nervous_Body_315.png";
        static const char* spider_idle90_animation = "assets/spider/Idle_Nervous/Idle_Nervous_Body_090.png";
        static const char* spider_idle180_animation = "assets/spider/Idle_Nervous/Idle_Nervous_Body_180.png";
        static const char* spider_idle270_animation = "assets/spider/Idle_Nervous/Idle_Nervous_Body_270.png";
        static const char* spider_idle0_animation = "assets/spider/Idle_Nervous/Idle_Nervous_Body_0.png";
        // raise
        static const char* spider_raise45_animation = "assets/spider/Raise/Raise_Body_045.png";
        static const char* spider_raise135_animation = "assets/spider/Raise/Raise_Body_135.png";
        static const char* spider_raise225_animation = "assets/spider/Raise/Raise_Body_225.png";
        static const char* spider_raise315_animation = "assets/spider/Raise/Raise_Body_315.png";
        static const char* spider_raise90_animation = "assets/spider/Raise/Raise_Body_090.png";
        static const char* spider_raise180_animation = "assets/spider/Raise/Raise_Body_180.png";
        static const char* spider_raise270_animation = "assets/spider/Raise/Raise_Body_270.png";
        static const char* spider_raise0_animation = "assets/spider/Raise/Raise_Body_0.png";
        // attack 1
        static const char* spider_attack1_45_animation = "assets/spider/Attack1/Attack1_Body_045.png";
        static const char* spider_attack1_135_animation = "assets/spider/Attack1/Attack1_Body_135.png";
        static const char* spider_attack1_225_animation = "assets/spider/Attack1/Attack1_Body_225.png";
        static const char* spider_attack1_315_animation = "assets/spider/Attack1/Attack1_Body_315.png";
        static const char* spider_attack1_90_animation = "assets/spider/Attack1/Attack1_Body_090.png";
        static const char* spider_attack1_180_animation = "assets/spider/Attack1/Attack1_Body_180.png";
        static const char* spider_attack1_270_animation = "assets/spider/Attack1/Attack1_Body_270.png";
        static const char* spider_attack1_0_animation = "assets/spider/Attack1/Attack1_Body_0.png";
        // attack 2
        static const char* spider_attack2_45_animation = "assets/spider/Attack2/Attack2_Body_045.png";
        static const char* spider_attack2_135_animation = "assets/spider/Attack2/Attack2_Body_135.png";
        static const char* spider_attack2_225_animation = "assets/spider/Attack2/Attack2_Body_225.png";
        static const char* spider_attack2_315_animation = "assets/spider/Attack2/Attack2_Body_315.png";
        static const char* spider_attack2_90_animation = "assets/spider/Attack2/Attack2_Body_090.png";
        static const char* spider_attack2_180_animation = "assets/spider/Attack2/Attack2_Body_180.png";
        static const char* spider_attack2_270_animation = "assets/spider/Attack2/Attack2_Body_270.png";
        static const char* spider_attack2_0_animation = "assets/spider/Attack2/Attack2_Body_0.png";
    }
}
