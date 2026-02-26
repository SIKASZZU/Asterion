// Keep this file as simple as possible
// The purpose of this file is to serve as a map of all resources used by the game
// And to make it easy to copy resources used by the game into the bld folder
// NB: All resource paths should be from the root of the workspace

namespace Assets {
    namespace Images {
        static const char* spritesheet = "resources/spritesheet.png";
        static const char* tree = "resources/tree.png";
        static const char* tree_trunk = "resources/tree_trunk.png";
        static const char* ground_cube = "resources/ground_cube.png";
        static const char* ground_cube_sprite = "resources/ground_cube_sprite_sheet.png";
        static const char* maze_ground_cube = "resources/maze_ground_cube.png";
        static const char* maze_ground_spirte = "resources/maze_ground_cube_sprite_sheet.png";
        // static const char* snowy_ground_cube = "resources/snowy_ground_cube.png";
        static const char* error_cube = "resources/error_cube.png";
        static const char* yellow_cube = "resources/yellow_cube.png";
        static const char* blue_cube = "resources/blue_cube.png";
        static const char* ingrown_wall_cube = "resources/ingrown_wall_cube.png";
        static const char* wall_cube = "resources/wall_cube.png";
        static const char* wall_cube_ground = "resources/wall_cube_ground.png";
        static const char* wall_cube_sprite = "resources/wall_cube_sprite_sheet.png";
        static const char* vine_cube_hard = "resources/vine_cube_hard.png";
        static const char* vine_cube_medium = "resources/vine_cube_medium.png";
        static const char* vine_cube_soft = "resources/vine_cube_soft.png";

        static const char* vine_overhang_sn = "resources/vine_overhang_SN.png";
        static const char* vine_overhang_ew = "resources/vine_overhang_EW.png";
        static const char* vine_cover_n = "resources/vine_cover_N.png";
        static const char* void_cube = "resources/void_cube.png";
        static const char* void_cube_tilemap = "resources/void_cube_tilemap.png";
        static const char* grass_cover_sprite = "resources/grass_cover_sprite_sheet.png";
        static const char* grass_cover = "resources/grass_cover.png";
        static const char* grass_cover_tall = "resources/grass_cover_tall.png";
        static const char* grass_cover_short = "resources/grass_cover_short.png";
        static const char* invisible_cube = "resources/invisible_cube.png";
        static const char* wall_markings = "resources/cube_markings.png";
        static const char* maze_deco = "resources/maze_deco.png";

    }
    namespace Player {

        //player animation
        static const char* player_animation = "resources/player_sprite_sheet.png";

        // player-girl
        // walk
        static const char* player_girl_walk_down = "resources/player/GirlSampleWalk/GirlSample_Walk_Down.png";
        static const char* player_girl_walk_down_left = "resources/player/GirlSampleWalk/GirlSample_Walk_DownLeft.png";
        static const char* player_girl_walk_down_right = "resources/player/GirlSampleWalk/GirlSample_Walk_DownRight.png";
        static const char* player_girl_walk_right = "resources/player/GirlSampleWalk/GirlSample_Walk_Right.png";
        static const char* player_girl_walk_left = "resources/player/GirlSampleWalk/GirlSample_Walk_Left.png";
        static const char* player_girl_walk_up = "resources/player/GirlSampleWalk/GirlSample_Walk_Up.png";
        static const char* player_girl_walk_up_left = "resources/player/GirlSampleWalk/GirlSample_Walk_UpLeft.png";
        static const char* player_girl_walk_up_right = "resources/player/GirlSampleWalk/GirlSample_Walk_UpRight.png";

        // run
        static const char* player_girl_run_down = "resources/player/GirlSampleRun/GirlSample_Run_Down.png";
        static const char* player_girl_run_down_left = "resources/player/GirlSampleRun/GirlSample_Run_DownLeft.png";
        static const char* player_girl_run_down_right = "resources/player/GirlSampleRun/GirlSample_Run_DownRight.png";
        static const char* player_girl_run_right = "resources/player/GirlSampleRun/GirlSample_Run_Right.png";
        static const char* player_girl_run_left = "resources/player/GirlSampleRun/GirlSample_Run_Left.png";
        static const char* player_girl_run_up = "resources/player/GirlSampleRun/GirlSample_Run_Up.png";
        static const char* player_girl_run_up_left = "resources/player/GirlSampleRun/GirlSample_Run_UpLeft.png";
        static const char* player_girl_run_up_right = "resources/player/GirlSampleRun/GirlSample_Run_UpRight.png";
    }

    namespace Spider {
        // run

        static const char* spider_run45_animation = "resources/spider/Run/Run_Body_045.png";
        static const char* spider_run135_animation = "resources/spider/Run/Run_Body_135.png";
        static const char* spider_run225_animation = "resources/spider/Run/Run_Body_225.png";
        static const char* spider_run315_animation = "resources/spider/Run/Run_Body_315.png";
        static const char* spider_run90_animation = "resources/spider/Run/Run_Body_090.png";
        static const char* spider_run180_animation = "resources/spider/Run/Run_Body_180.png";
        static const char* spider_run270_animation = "resources/spider/Run/Run_Body_270.png";
        static const char* spider_run0_animation = "resources/spider/Run/Run_Body_0.png";
        // walk

        static const char* spider_walk45_animation = "resources/spider/Walk/Walk_Body_045.png";
        static const char* spider_walk135_animation = "resources/spider/Walk/Walk_Body_135.png";
        static const char* spider_walk225_animation = "resources/spider/Walk/Walk_Body_225.png";
        static const char* spider_walk315_animation = "resources/spider/Walk/Walk_Body_315.png";
        static const char* spider_walk90_animation = "resources/spider/Walk/Walk_Body_090.png";
        static const char* spider_walk180_animation = "resources/spider/Walk/Walk_Body_180.png";
        static const char* spider_walk270_animation = "resources/spider/Walk/Walk_Body_270.png";
        static const char* spider_walk0_animation = "resources/spider/Walk/Walk_Body_0.png";
        // idle

        static const char* spider_idle45_animation = "resources/spider/Idle_Nervous/Idle_Nervous_Body_045.png";
        static const char* spider_idle135_animation = "resources/spider/Idle_Nervous/Idle_Nervous_Body_135.png";
        static const char* spider_idle225_animation = "resources/spider/Idle_Nervous/Idle_Nervous_Body_225.png";
        static const char* spider_idle315_animation = "resources/spider/Idle_Nervous/Idle_Nervous_Body_315.png";
        static const char* spider_idle90_animation = "resources/spider/Idle_Nervous/Idle_Nervous_Body_090.png";
        static const char* spider_idle180_animation = "resources/spider/Idle_Nervous/Idle_Nervous_Body_180.png";
        static const char* spider_idle270_animation = "resources/spider/Idle_Nervous/Idle_Nervous_Body_270.png";
        static const char* spider_idle0_animation = "resources/spider/Idle_Nervous/Idle_Nervous_Body_0.png";

    }
}
