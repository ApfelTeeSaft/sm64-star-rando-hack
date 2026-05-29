# Randomizer Voice Recordings

Place recorded WAV files in this directory, then run:

```
python tools/randomizer_voice_assets.py --normalize
```

By default this stages generated assets under assets/randomizer_voice so
the normal ROM build does not register the experimental voice banks.
Use --register only when testing the native audio playback integration.

Recommended source format: mono WAV, 16-bit or 24-bit PCM, 22050 Hz to 48000 Hz.
The converter trims silence, mixes to mono, resamples to 16000 Hz, writes AIFF,
and the normal SM64 build converts those samples to N64 VADPCM.

100-coin and red-coin objectives include the stage abbreviation in the filename.
This keeps every course-specific coin objective on its own recorded line.

Canonical filenames:

- `000_big_bob_omb_on_the_summit.wav` - Big Bob-omb on the Summit
- `001_footrace_with_koopa_the_quick.wav` - Footrace with Koopa the Quick
- `002_shoot_to_the_island_in_the_sky.wav` - Shoot to the Island in the Sky
- `003_bob_find_the_8_red_coins.wav` - Bob-omb Battlefield - Find the 8 Red Coins
- `004_mario_wings_to_the_sky.wav` - Mario Wings to the Sky
- `005_behind_chain_chomp_s_gate.wav` - Behind Chain Chomp's Gate
- `006_bob_one_hundred_coins.wav` - Bob-omb Battlefield - 100 Coins
- `007_chip_off_whomp_s_block.wav` - Chip Off Whomp's Block
- `008_to_the_top_of_the_fortress.wav` - To the Top of the Fortress
- `009_shoot_into_the_wild_blue.wav` - Shoot into the Wild Blue
- `010_wf_red_coins_on_the_floating_isle.wav` - Whomp's Fortress - Red Coins on the Floating Isle
- `011_fall_onto_the_caged_island.wav` - Fall onto the Caged Island
- `012_blast_away_the_wall.wav` - Blast Away the Wall
- `013_wf_one_hundred_coins.wav` - Whomp's Fortress - 100 Coins
- `014_plunder_in_the_sunken_ship.wav` - Plunder in the Sunken Ship
- `015_can_the_eel_come_out_to_play.wav` - Can the Eel Come Out to Play?
- `016_treasure_of_the_ocean_cave.wav` - Treasure of the Ocean Cave
- `017_jrb_red_coins_on_the_ship_afloat.wav` - Jolly Roger Bay - Red Coins on the Ship Afloat
- `018_blast_to_the_stone_pillar.wav` - Blast to the Stone Pillar
- `019_through_the_jet_stream.wav` - Through the Jet Stream
- `020_jrb_one_hundred_coins.wav` - Jolly Roger Bay - 100 Coins
- `021_slip_slidin_away.wav` - Slip Slidin' Away
- `022_lil_penguin_lost.wav` - Lil' Penguin Lost
- `023_big_penguin_race.wav` - Big Penguin Race
- `024_ccm_frosty_slide_for_8_red_coins.wav` - Cool Cool Mountain - Frosty Slide for 8 Red Coins
- `025_snowman_s_lost_his_head.wav` - Snowman's Lost His Head
- `026_wall_kicks_will_work.wav` - Wall Kicks Will Work
- `027_ccm_one_hundred_coins.wav` - Cool Cool Mountain - 100 Coins
- `028_go_on_a_ghost_hunt.wav` - Go on a Ghost Hunt
- `029_ride_big_boo_s_merry_go_round.wav` - Ride Big Boo's Merry-Go-Round
- `030_secret_of_the_haunted_books.wav` - Secret of the Haunted Books
- `031_bbh_seek_the_8_red_coins.wav` - Big Boo's Haunt - Seek the 8 Red Coins
- `032_big_boo_s_balcony.wav` - Big Boo's Balcony
- `033_eye_to_eye_in_the_secret_room.wav` - Eye to Eye in the Secret Room
- `034_bbh_one_hundred_coins.wav` - Big Boo's Haunt - 100 Coins
- `035_swimming_beast_in_the_cavern.wav` - Swimming Beast in the Cavern
- `036_hmc_elevate_for_8_red_coins.wav` - Hazy Maze Cave - Elevate for 8 Red Coins
- `037_metal_head_mario_can_move.wav` - Metal-Head Mario Can Move
- `038_navigating_the_toxic_maze.wav` - Navigating the Toxic Maze
- `039_a_maze_ing_emergency_exit.wav` - A-Maze-Ing Emergency Exit
- `040_watch_for_rolling_rocks.wav` - Watch for Rolling Rocks
- `041_hmc_one_hundred_coins.wav` - Hazy Maze Cave - 100 Coins
- `042_boil_the_big_bully.wav` - Boil the Big Bully
- `043_bully_the_bullies.wav` - Bully the Bullies
- `044_lll_8_coin_puzzle_with_15_pieces.wav` - Lethal Lava Land - 8-Coin Puzzle with 15 Pieces
- `045_red_hot_log_rolling.wav` - Red-Hot Log Rolling
- `046_hot_foot_it_into_the_volcano.wav` - Hot-Foot-It into the Volcano
- `047_elevator_tour_in_the_volcano.wav` - Elevator Tour in the Volcano
- `048_lll_one_hundred_coins.wav` - Lethal Lava Land - 100 Coins
- `049_in_the_talons_of_the_big_bird.wav` - In the Talons of the Big Bird
- `050_shining_atop_the_pyramid.wav` - Shining Atop the Pyramid
- `051_inside_the_ancient_pyramid.wav` - Inside the Ancient Pyramid
- `052_stand_tall_on_the_four_pillars.wav` - Stand Tall on the Four Pillars
- `053_ssl_free_flying_for_8_red_coins.wav` - Shifting Sand Land - Free Flying for 8 Red Coins
- `054_pyramid_puzzle.wav` - Pyramid Puzzle
- `055_ssl_one_hundred_coins.wav` - Shifting Sand Land - 100 Coins
- `056_board_bowser_s_sub.wav` - Board Bowser's Sub
- `057_chests_in_the_current.wav` - Chests in the Current
- `058_ddd_pole_jumping_for_red_coins.wav` - Dire Dire Docks - Pole-Jumping for Red Coins
- `059_through_the_jet_stream.wav` - Through the Jet Stream
- `060_manta_ray_s_reward.wav` - Manta Ray's Reward
- `061_collect_the_caps.wav` - Collect the Caps
- `062_ddd_one_hundred_coins.wav` - Dire Dire Docks - 100 Coins
- `063_snowman_s_big_head.wav` - Snowman's Big Head
- `064_chill_with_the_bully.wav` - Chill with the Bully
- `065_in_the_deep_freeze.wav` - In the Deep Freeze
- `066_whirl_from_the_freezing_pond.wav` - Whirl from the Freezing Pond
- `067_sl_shell_shreddin_for_red_coins.wav` - Snowman's Land - Shell Shreddin' for Red Coins
- `068_into_the_igloo.wav` - Into the Igloo
- `069_sl_one_hundred_coins.wav` - Snowman's Land - 100 Coins
- `070_shocking_arrow_lifts.wav` - Shocking Arrow Lifts
- `071_top_o_the_town.wav` - Top O' the Town
- `072_secrets_in_the_shallows_and_sky.wav` - Secrets in the Shallows and Sky
- `073_express_elevator_hurry_up.wav` - Express Elevator--Hurry Up
- `074_wdw_go_to_town_for_red_coins.wav` - Wet-Dry World - Go to Town for Red Coins
- `075_quick_race_through_downtown.wav` - Quick Race Through Downtown
- `076_wdw_one_hundred_coins.wav` - Wet-Dry World - 100 Coins
- `077_scale_the_mountain.wav` - Scale the Mountain
- `078_mystery_of_the_monkey_cage.wav` - Mystery of the Monkey Cage
- `079_ttm_scary_shrooms_red_coins.wav` - Tall Tall Mountain - Scary 'Shrooms, Red Coins
- `080_mysterious_mountainside.wav` - Mysterious Mountainside
- `081_breathtaking_view_from_bridge.wav` - Breathtaking View from Bridge
- `082_blast_to_the_lonely_mushroom.wav` - Blast to the Lonely Mushroom
- `083_ttm_one_hundred_coins.wav` - Tall Tall Mountain - 100 Coins
- `084_pluck_the_piranha_flower.wav` - Pluck the Piranha Flower
- `085_the_tip_top_of_the_huge_island.wav` - The Tip Top of the Huge Island
- `086_rematch_with_koopa_the_quick.wav` - Rematch with Koopa the Quick
- `087_five_itty_bitty_secrets.wav` - Five Itty Bitty Secrets
- `088_thi_wiggler_s_red_coins.wav` - Tiny-Huge Island - Wiggler's Red Coins
- `089_make_wiggler_squirm.wav` - Make Wiggler Squirm
- `090_thi_one_hundred_coins.wav` - Tiny-Huge Island - 100 Coins
- `091_roll_into_the_cage.wav` - Roll into the Cage
- `092_the_pit_and_the_pendulums.wav` - The Pit and the Pendulums
- `093_get_a_hand.wav` - Get a Hand
- `094_stomp_on_the_thwomp.wav` - Stomp on the Thwomp
- `095_timed_jumps_on_moving_bars.wav` - Timed Jumps on Moving Bars
- `096_ttc_stop_time_for_red_coins.wav` - Tick Tock Clock - Stop Time for Red Coins
- `097_ttc_one_hundred_coins.wav` - Tick Tock Clock - 100 Coins
- `098_cruiser_crossing_the_rainbow.wav` - Cruiser Crossing the Rainbow
- `099_the_big_house_in_the_sky.wav` - The Big House in the Sky
- `100_rr_coins_amassed_in_a_maze.wav` - Rainbow Ride - Coins Amassed in a Maze
- `101_swingin_in_the_breeze.wav` - Swingin' in the Breeze
- `102_tricky_triangles.wav` - Tricky Triangles
- `103_somewhere_over_the_rainbow.wav` - Somewhere Over the Rainbow
- `104_rr_one_hundred_coins.wav` - Rainbow Ride - 100 Coins
- `105_bitdw_8_red_coins.wav` - Bowser in the Dark World - 8 Red Coins
- `106_bitfs_8_red_coins.wav` - Bowser in the Fire Sea - 8 Red Coins
- `107_bits_8_red_coins.wav` - Bowser in the Sky - 8 Red Coins
- `108_box_star.wav` - Box Star
- `109_under_21_seconds.wav` - Under 21 Seconds
- `110_cotmc_8_red_coins.wav` - Cavern of the Metal Cap - 8 Red Coins
- `111_totwc_8_red_coins.wav` - Tower of the Wing Cap - 8 Red Coins
- `112_vcutm_8_red_coins.wav` - Vanish Cap Under the Moat - 8 Red Coins
- `113_wmotr_8_red_coins.wav` - Wing Mario Over the Rainbow - 8 Red Coins
- `114_sa_8_red_coins.wav` - Secret Aquarium - 8 Red Coins
- `115_basement_toad.wav` - Basement Toad
- `116_second_floor_toad.wav` - Second Floor Toad
- `117_third_floor_toad.wav` - Third Floor Toad
- `118_mips_15_star.wav` - MIPS 15 Star
- `119_mips_50_star.wav` - MIPS 50 Star
