#include <PR/ultratypes.h>

#include "level_update.h"
#include "object_constants.h"
#include "randomizer_star_data.h"
#include "sm64.h"

#define RSTAR(id, course, level, act, starIndex, difficulty, seconds, kind, courseName, name) \
    { id, course, level, 1, WARP_NODE_0A, act, starIndex, difficulty, seconds, kind, courseName, name }

const struct RandomizerStar gRandomizerStars[RANDOMIZER_STAR_COUNT] = {
    RSTAR(  1, COURSE_BOB,   LEVEL_BOB,   1, STAR_INDEX_ACT_1,     2, 110, RANDOMIZER_STAR_NORMAL,            "Bob-omb Battlefield", "Big Bob-omb on the Summit"),
    RSTAR(  2, COURSE_BOB,   LEVEL_BOB,   2, STAR_INDEX_ACT_2,     2, 120, RANDOMIZER_STAR_NORMAL,            "Bob-omb Battlefield", "Footrace with Koopa the Quick"),
    RSTAR(  3, COURSE_BOB,   LEVEL_BOB,   3, STAR_INDEX_ACT_3,     2, 100, RANDOMIZER_STAR_NORMAL,            "Bob-omb Battlefield", "Shoot to the Island in the Sky"),
    RSTAR(  4, COURSE_BOB,   LEVEL_BOB,   4, STAR_INDEX_ACT_4,     2, 140, RANDOMIZER_STAR_NORMAL,            "Bob-omb Battlefield", "Find the 8 Red Coins"),
    RSTAR(  5, COURSE_BOB,   LEVEL_BOB,   5, STAR_INDEX_ACT_5,     3, 130, RANDOMIZER_STAR_NORMAL,            "Bob-omb Battlefield", "Mario Wings to the Sky"),
    RSTAR(  6, COURSE_BOB,   LEVEL_BOB,   6, STAR_INDEX_ACT_6,     2,  90, RANDOMIZER_STAR_NORMAL,            "Bob-omb Battlefield", "Behind Chain Chomp's Gate"),
    RSTAR(  7, COURSE_BOB,   LEVEL_BOB,   1, STAR_INDEX_100_COINS, 3, 300, RANDOMIZER_STAR_100_COINS,         "Bob-omb Battlefield", "100 Coins"),

    RSTAR(  8, COURSE_WF,    LEVEL_WF,    1, STAR_INDEX_ACT_1,     2,  90, RANDOMIZER_STAR_NORMAL,            "Whomp's Fortress", "Chip Off Whomp's Block"),
    RSTAR(  9, COURSE_WF,    LEVEL_WF,    2, STAR_INDEX_ACT_2,     2, 130, RANDOMIZER_STAR_NORMAL,            "Whomp's Fortress", "To the Top of the Fortress"),
    RSTAR( 10, COURSE_WF,    LEVEL_WF,    3, STAR_INDEX_ACT_3,     2,  80, RANDOMIZER_STAR_NORMAL,            "Whomp's Fortress", "Shoot into the Wild Blue"),
    RSTAR( 11, COURSE_WF,    LEVEL_WF,    4, STAR_INDEX_ACT_4,     2, 120, RANDOMIZER_STAR_NORMAL,            "Whomp's Fortress", "Red Coins on the Floating Isle"),
    RSTAR( 12, COURSE_WF,    LEVEL_WF,    5, STAR_INDEX_ACT_5,     2,  90, RANDOMIZER_STAR_NORMAL,            "Whomp's Fortress", "Fall onto the Caged Island"),
    RSTAR( 13, COURSE_WF,    LEVEL_WF,    6, STAR_INDEX_ACT_6,     1,  70, RANDOMIZER_STAR_NORMAL,            "Whomp's Fortress", "Blast Away the Wall"),
    RSTAR( 14, COURSE_WF,    LEVEL_WF,    1, STAR_INDEX_100_COINS, 3, 260, RANDOMIZER_STAR_100_COINS,         "Whomp's Fortress", "100 Coins"),

    RSTAR( 15, COURSE_JRB,   LEVEL_JRB,   1, STAR_INDEX_ACT_1,     2, 130, RANDOMIZER_STAR_NORMAL,            "Jolly Roger Bay", "Plunder in the Sunken Ship"),
    RSTAR( 16, COURSE_JRB,   LEVEL_JRB,   2, STAR_INDEX_ACT_2,     2, 100, RANDOMIZER_STAR_NORMAL,            "Jolly Roger Bay", "Can the Eel Come Out to Play?"),
    RSTAR( 17, COURSE_JRB,   LEVEL_JRB,   3, STAR_INDEX_ACT_3,     2, 120, RANDOMIZER_STAR_NORMAL,            "Jolly Roger Bay", "Treasure of the Ocean Cave"),
    RSTAR( 18, COURSE_JRB,   LEVEL_JRB,   4, STAR_INDEX_ACT_4,     3, 160, RANDOMIZER_STAR_NORMAL,            "Jolly Roger Bay", "Red Coins on the Ship Afloat"),
    RSTAR( 19, COURSE_JRB,   LEVEL_JRB,   5, STAR_INDEX_ACT_5,     2,  90, RANDOMIZER_STAR_NORMAL,            "Jolly Roger Bay", "Blast to the Stone Pillar"),
    RSTAR( 20, COURSE_JRB,   LEVEL_JRB,   6, STAR_INDEX_ACT_6,     3, 140, RANDOMIZER_STAR_NORMAL,            "Jolly Roger Bay", "Through the Jet Stream"),
    RSTAR( 21, COURSE_JRB,   LEVEL_JRB,   1, STAR_INDEX_100_COINS, 3, 320, RANDOMIZER_STAR_100_COINS,         "Jolly Roger Bay", "100 Coins"),

    RSTAR( 22, COURSE_CCM,   LEVEL_CCM,   1, STAR_INDEX_ACT_1,     1,  80, RANDOMIZER_STAR_NORMAL,            "Cool Cool Mountain", "Slip Slidin' Away"),
    RSTAR( 23, COURSE_CCM,   LEVEL_CCM,   2, STAR_INDEX_ACT_2,     2, 130, RANDOMIZER_STAR_NORMAL,            "Cool Cool Mountain", "Lil' Penguin Lost"),
    RSTAR( 24, COURSE_CCM,   LEVEL_CCM,   3, STAR_INDEX_ACT_3,     2, 100, RANDOMIZER_STAR_NORMAL,            "Cool Cool Mountain", "Big Penguin Race"),
    RSTAR( 25, COURSE_CCM,   LEVEL_CCM,   4, STAR_INDEX_ACT_4,     2, 160, RANDOMIZER_STAR_NORMAL,            "Cool Cool Mountain", "Frosty Slide for 8 Red Coins"),
    RSTAR( 26, COURSE_CCM,   LEVEL_CCM,   5, STAR_INDEX_ACT_5,     2, 120, RANDOMIZER_STAR_NORMAL,            "Cool Cool Mountain", "Snowman's Lost His Head"),
    RSTAR( 27, COURSE_CCM,   LEVEL_CCM,   6, STAR_INDEX_ACT_6,     3, 100, RANDOMIZER_STAR_NORMAL,            "Cool Cool Mountain", "Wall Kicks Will Work"),
    RSTAR( 28, COURSE_CCM,   LEVEL_CCM,   1, STAR_INDEX_100_COINS, 3, 280, RANDOMIZER_STAR_100_COINS,         "Cool Cool Mountain", "100 Coins"),

    RSTAR( 29, COURSE_BBH,   LEVEL_BBH,   1, STAR_INDEX_ACT_1,     2, 120, RANDOMIZER_STAR_NORMAL,            "Big Boo's Haunt", "Go on a Ghost Hunt"),
    RSTAR( 30, COURSE_BBH,   LEVEL_BBH,   2, STAR_INDEX_ACT_2,     2, 130, RANDOMIZER_STAR_NORMAL,            "Big Boo's Haunt", "Ride Big Boo's Merry-Go-Round"),
    RSTAR( 31, COURSE_BBH,   LEVEL_BBH,   3, STAR_INDEX_ACT_3,     2,  90, RANDOMIZER_STAR_NORMAL,            "Big Boo's Haunt", "Secret of the Haunted Books"),
    RSTAR( 32, COURSE_BBH,   LEVEL_BBH,   4, STAR_INDEX_ACT_4,     3, 170, RANDOMIZER_STAR_NORMAL,            "Big Boo's Haunt", "Seek the 8 Red Coins"),
    RSTAR( 33, COURSE_BBH,   LEVEL_BBH,   5, STAR_INDEX_ACT_5,     3, 140, RANDOMIZER_STAR_NORMAL,            "Big Boo's Haunt", "Big Boo's Balcony"),
    RSTAR( 34, COURSE_BBH,   LEVEL_BBH,   6, STAR_INDEX_ACT_6,     3, 130, RANDOMIZER_STAR_NORMAL,            "Big Boo's Haunt", "Eye to Eye in the Secret Room"),
    RSTAR( 35, COURSE_BBH,   LEVEL_BBH,   1, STAR_INDEX_100_COINS, 4, 340, RANDOMIZER_STAR_100_COINS,         "Big Boo's Haunt", "100 Coins"),

    RSTAR( 36, COURSE_HMC,   LEVEL_HMC,   1, STAR_INDEX_ACT_1,     2, 130, RANDOMIZER_STAR_NORMAL,            "Hazy Maze Cave", "Swimming Beast in the Cavern"),
    RSTAR( 37, COURSE_HMC,   LEVEL_HMC,   2, STAR_INDEX_ACT_2,     3, 160, RANDOMIZER_STAR_NORMAL,            "Hazy Maze Cave", "Elevate for 8 Red Coins"),
    RSTAR( 38, COURSE_HMC,   LEVEL_HMC,   3, STAR_INDEX_ACT_3,     3, 130, RANDOMIZER_STAR_NORMAL,            "Hazy Maze Cave", "Metal-Head Mario Can Move"),
    RSTAR( 39, COURSE_HMC,   LEVEL_HMC,   4, STAR_INDEX_ACT_4,     3, 170, RANDOMIZER_STAR_NORMAL,            "Hazy Maze Cave", "Navigating the Toxic Maze"),
    RSTAR( 40, COURSE_HMC,   LEVEL_HMC,   5, STAR_INDEX_ACT_5,     3, 140, RANDOMIZER_STAR_NORMAL,            "Hazy Maze Cave", "A-Maze-Ing Emergency Exit"),
    RSTAR( 41, COURSE_HMC,   LEVEL_HMC,   6, STAR_INDEX_ACT_6,     3, 120, RANDOMIZER_STAR_NORMAL,            "Hazy Maze Cave", "Watch for Rolling Rocks"),
    RSTAR( 42, COURSE_HMC,   LEVEL_HMC,   1, STAR_INDEX_100_COINS, 4, 360, RANDOMIZER_STAR_100_COINS,         "Hazy Maze Cave", "100 Coins"),

    RSTAR( 43, COURSE_LLL,   LEVEL_LLL,   1, STAR_INDEX_ACT_1,     2, 120, RANDOMIZER_STAR_NORMAL,            "Lethal Lava Land", "Boil the Big Bully"),
    RSTAR( 44, COURSE_LLL,   LEVEL_LLL,   2, STAR_INDEX_ACT_2,     2, 140, RANDOMIZER_STAR_NORMAL,            "Lethal Lava Land", "Bully the Bullies"),
    RSTAR( 45, COURSE_LLL,   LEVEL_LLL,   3, STAR_INDEX_ACT_3,     2, 120, RANDOMIZER_STAR_NORMAL,            "Lethal Lava Land", "8-Coin Puzzle with 15 Pieces"),
    RSTAR( 46, COURSE_LLL,   LEVEL_LLL,   4, STAR_INDEX_ACT_4,     3, 110, RANDOMIZER_STAR_NORMAL,            "Lethal Lava Land", "Red-Hot Log Rolling"),
    RSTAR( 47, COURSE_LLL,   LEVEL_LLL,   5, STAR_INDEX_ACT_5,     3, 150, RANDOMIZER_STAR_NORMAL,            "Lethal Lava Land", "Hot-Foot-It into the Volcano"),
    RSTAR( 48, COURSE_LLL,   LEVEL_LLL,   6, STAR_INDEX_ACT_6,     4, 180, RANDOMIZER_STAR_NORMAL,            "Lethal Lava Land", "Elevator Tour in the Volcano"),
    RSTAR( 49, COURSE_LLL,   LEVEL_LLL,   1, STAR_INDEX_100_COINS, 4, 320, RANDOMIZER_STAR_100_COINS,         "Lethal Lava Land", "100 Coins"),

    RSTAR( 50, COURSE_SSL,   LEVEL_SSL,   1, STAR_INDEX_ACT_1,     2, 120, RANDOMIZER_STAR_NORMAL,            "Shifting Sand Land", "In the Talons of the Big Bird"),
    RSTAR( 51, COURSE_SSL,   LEVEL_SSL,   2, STAR_INDEX_ACT_2,     2, 100, RANDOMIZER_STAR_NORMAL,            "Shifting Sand Land", "Shining Atop the Pyramid"),
    RSTAR( 52, COURSE_SSL,   LEVEL_SSL,   3, STAR_INDEX_ACT_3,     3, 160, RANDOMIZER_STAR_NORMAL,            "Shifting Sand Land", "Inside the Ancient Pyramid"),
    RSTAR( 53, COURSE_SSL,   LEVEL_SSL,   4, STAR_INDEX_ACT_4,     3, 150, RANDOMIZER_STAR_NORMAL,            "Shifting Sand Land", "Stand Tall on the Four Pillars"),
    RSTAR( 54, COURSE_SSL,   LEVEL_SSL,   5, STAR_INDEX_ACT_5,     3, 150, RANDOMIZER_STAR_NORMAL,            "Shifting Sand Land", "Free Flying for 8 Red Coins"),
    RSTAR( 55, COURSE_SSL,   LEVEL_SSL,   6, STAR_INDEX_ACT_6,     4, 170, RANDOMIZER_STAR_NORMAL,            "Shifting Sand Land", "Pyramid Puzzle"),
    RSTAR( 56, COURSE_SSL,   LEVEL_SSL,   1, STAR_INDEX_100_COINS, 4, 340, RANDOMIZER_STAR_100_COINS,         "Shifting Sand Land", "100 Coins"),

    RSTAR( 57, COURSE_DDD,   LEVEL_DDD,   1, STAR_INDEX_ACT_1,     2, 100, RANDOMIZER_STAR_NORMAL,            "Dire Dire Docks", "Board Bowser's Sub"),
    RSTAR( 58, COURSE_DDD,   LEVEL_DDD,   2, STAR_INDEX_ACT_2,     3, 120, RANDOMIZER_STAR_NORMAL,            "Dire Dire Docks", "Chests in the Current"),
    RSTAR( 59, COURSE_DDD,   LEVEL_DDD,   3, STAR_INDEX_ACT_3,     3, 160, RANDOMIZER_STAR_NORMAL,            "Dire Dire Docks", "Pole-Jumping for Red Coins"),
    RSTAR( 60, COURSE_DDD,   LEVEL_DDD,   4, STAR_INDEX_ACT_4,     3, 150, RANDOMIZER_STAR_NORMAL,            "Dire Dire Docks", "Through the Jet Stream"),
    RSTAR( 61, COURSE_DDD,   LEVEL_DDD,   5, STAR_INDEX_ACT_5,     3, 130, RANDOMIZER_STAR_NORMAL,            "Dire Dire Docks", "Manta Ray's Reward"),
    RSTAR( 62, COURSE_DDD,   LEVEL_DDD,   6, STAR_INDEX_ACT_6,     3, 160, RANDOMIZER_STAR_NORMAL,            "Dire Dire Docks", "Collect the Caps"),
    RSTAR( 63, COURSE_DDD,   LEVEL_DDD,   1, STAR_INDEX_100_COINS, 4, 360, RANDOMIZER_STAR_100_COINS,         "Dire Dire Docks", "100 Coins"),

    RSTAR( 64, COURSE_SL,    LEVEL_SL,    1, STAR_INDEX_ACT_1,     2, 110, RANDOMIZER_STAR_NORMAL,            "Snowman's Land", "Snowman's Big Head"),
    RSTAR( 65, COURSE_SL,    LEVEL_SL,    2, STAR_INDEX_ACT_2,     2, 110, RANDOMIZER_STAR_NORMAL,            "Snowman's Land", "Chill with the Bully"),
    RSTAR( 66, COURSE_SL,    LEVEL_SL,    3, STAR_INDEX_ACT_3,     2, 100, RANDOMIZER_STAR_NORMAL,            "Snowman's Land", "In the Deep Freeze"),
    RSTAR( 67, COURSE_SL,    LEVEL_SL,    4, STAR_INDEX_ACT_4,     2, 100, RANDOMIZER_STAR_NORMAL,            "Snowman's Land", "Whirl from the Freezing Pond"),
    RSTAR( 68, COURSE_SL,    LEVEL_SL,    5, STAR_INDEX_ACT_5,     3, 170, RANDOMIZER_STAR_NORMAL,            "Snowman's Land", "Shell Shreddin' for Red Coins"),
    RSTAR( 69, COURSE_SL,    LEVEL_SL,    6, STAR_INDEX_ACT_6,     3, 160, RANDOMIZER_STAR_NORMAL,            "Snowman's Land", "Into the Igloo"),
    RSTAR( 70, COURSE_SL,    LEVEL_SL,    1, STAR_INDEX_100_COINS, 4, 330, RANDOMIZER_STAR_100_COINS,         "Snowman's Land", "100 Coins"),

    RSTAR( 71, COURSE_WDW,   LEVEL_WDW,   1, STAR_INDEX_ACT_1,     2, 130, RANDOMIZER_STAR_NORMAL,            "Wet-Dry World", "Shocking Arrow Lifts"),
    RSTAR( 72, COURSE_WDW,   LEVEL_WDW,   2, STAR_INDEX_ACT_2,     2, 130, RANDOMIZER_STAR_NORMAL,            "Wet-Dry World", "Top O' the Town"),
    RSTAR( 73, COURSE_WDW,   LEVEL_WDW,   3, STAR_INDEX_ACT_3,     3, 170, RANDOMIZER_STAR_NORMAL,            "Wet-Dry World", "Secrets in the Shallows and Sky"),
    RSTAR( 74, COURSE_WDW,   LEVEL_WDW,   4, STAR_INDEX_ACT_4,     3, 140, RANDOMIZER_STAR_NORMAL,            "Wet-Dry World", "Express Elevator--Hurry Up"),
    RSTAR( 75, COURSE_WDW,   LEVEL_WDW,   5, STAR_INDEX_ACT_5,     3, 180, RANDOMIZER_STAR_NORMAL,            "Wet-Dry World", "Go to Town for Red Coins"),
    RSTAR( 76, COURSE_WDW,   LEVEL_WDW,   6, STAR_INDEX_ACT_6,     3, 160, RANDOMIZER_STAR_NORMAL,            "Wet-Dry World", "Quick Race Through Downtown"),
    RSTAR( 77, COURSE_WDW,   LEVEL_WDW,   1, STAR_INDEX_100_COINS, 4, 360, RANDOMIZER_STAR_100_COINS,         "Wet-Dry World", "100 Coins"),

    RSTAR( 78, COURSE_TTM,   LEVEL_TTM,   1, STAR_INDEX_ACT_1,     3, 150, RANDOMIZER_STAR_NORMAL,            "Tall Tall Mountain", "Scale the Mountain"),
    RSTAR( 79, COURSE_TTM,   LEVEL_TTM,   2, STAR_INDEX_ACT_2,     3, 160, RANDOMIZER_STAR_NORMAL,            "Tall Tall Mountain", "Mystery of the Monkey Cage"),
    RSTAR( 80, COURSE_TTM,   LEVEL_TTM,   3, STAR_INDEX_ACT_3,     3, 170, RANDOMIZER_STAR_NORMAL,            "Tall Tall Mountain", "Scary 'Shrooms, Red Coins"),
    RSTAR( 81, COURSE_TTM,   LEVEL_TTM,   4, STAR_INDEX_ACT_4,     3, 120, RANDOMIZER_STAR_NORMAL,            "Tall Tall Mountain", "Mysterious Mountainside"),
    RSTAR( 82, COURSE_TTM,   LEVEL_TTM,   5, STAR_INDEX_ACT_5,     3, 130, RANDOMIZER_STAR_NORMAL,            "Tall Tall Mountain", "Breathtaking View from Bridge"),
    RSTAR( 83, COURSE_TTM,   LEVEL_TTM,   6, STAR_INDEX_ACT_6,     3, 150, RANDOMIZER_STAR_NORMAL,            "Tall Tall Mountain", "Blast to the Lonely Mushroom"),
    RSTAR( 84, COURSE_TTM,   LEVEL_TTM,   1, STAR_INDEX_100_COINS, 4, 380, RANDOMIZER_STAR_100_COINS,         "Tall Tall Mountain", "100 Coins"),

    RSTAR( 85, COURSE_THI,   LEVEL_THI,   1, STAR_INDEX_ACT_1,     2, 120, RANDOMIZER_STAR_NORMAL,            "Tiny-Huge Island", "Pluck the Piranha Flower"),
    RSTAR( 86, COURSE_THI,   LEVEL_THI,   2, STAR_INDEX_ACT_2,     3, 140, RANDOMIZER_STAR_NORMAL,            "Tiny-Huge Island", "The Tip Top of the Huge Island"),
    RSTAR( 87, COURSE_THI,   LEVEL_THI,   3, STAR_INDEX_ACT_3,     3, 160, RANDOMIZER_STAR_NORMAL,            "Tiny-Huge Island", "Rematch with Koopa the Quick"),
    RSTAR( 88, COURSE_THI,   LEVEL_THI,   4, STAR_INDEX_ACT_4,     3, 160, RANDOMIZER_STAR_NORMAL,            "Tiny-Huge Island", "Five Itty Bitty Secrets"),
    RSTAR( 89, COURSE_THI,   LEVEL_THI,   5, STAR_INDEX_ACT_5,     3, 170, RANDOMIZER_STAR_NORMAL,            "Tiny-Huge Island", "Wiggler's Red Coins"),
    RSTAR( 90, COURSE_THI,   LEVEL_THI,   6, STAR_INDEX_ACT_6,     3, 150, RANDOMIZER_STAR_NORMAL,            "Tiny-Huge Island", "Make Wiggler Squirm"),
    RSTAR( 91, COURSE_THI,   LEVEL_THI,   1, STAR_INDEX_100_COINS, 4, 360, RANDOMIZER_STAR_100_COINS,         "Tiny-Huge Island", "100 Coins"),

    RSTAR( 92, COURSE_TTC,   LEVEL_TTC,   1, STAR_INDEX_ACT_1,     3, 130, RANDOMIZER_STAR_NORMAL,            "Tick Tock Clock", "Roll into the Cage"),
    RSTAR( 93, COURSE_TTC,   LEVEL_TTC,   2, STAR_INDEX_ACT_2,     3, 150, RANDOMIZER_STAR_NORMAL,            "Tick Tock Clock", "The Pit and the Pendulums"),
    RSTAR( 94, COURSE_TTC,   LEVEL_TTC,   3, STAR_INDEX_ACT_3,     3, 140, RANDOMIZER_STAR_NORMAL,            "Tick Tock Clock", "Get a Hand"),
    RSTAR( 95, COURSE_TTC,   LEVEL_TTC,   4, STAR_INDEX_ACT_4,     3, 140, RANDOMIZER_STAR_NORMAL,            "Tick Tock Clock", "Stomp on the Thwomp"),
    RSTAR( 96, COURSE_TTC,   LEVEL_TTC,   5, STAR_INDEX_ACT_5,     3, 170, RANDOMIZER_STAR_NORMAL,            "Tick Tock Clock", "Timed Jumps on Moving Bars"),
    RSTAR( 97, COURSE_TTC,   LEVEL_TTC,   6, STAR_INDEX_ACT_6,     3, 180, RANDOMIZER_STAR_NORMAL,            "Tick Tock Clock", "Stop Time for Red Coins"),
    RSTAR( 98, COURSE_TTC,   LEVEL_TTC,   1, STAR_INDEX_100_COINS, 4, 380, RANDOMIZER_STAR_100_COINS,         "Tick Tock Clock", "100 Coins"),

    RSTAR( 99, COURSE_RR,    LEVEL_RR,    1, STAR_INDEX_ACT_1,     3, 160, RANDOMIZER_STAR_NORMAL,            "Rainbow Ride", "Cruiser Crossing the Rainbow"),
    RSTAR(100, COURSE_RR,    LEVEL_RR,    2, STAR_INDEX_ACT_2,     4, 190, RANDOMIZER_STAR_NORMAL,            "Rainbow Ride", "The Big House in the Sky"),
    RSTAR(101, COURSE_RR,    LEVEL_RR,    3, STAR_INDEX_ACT_3,     3, 170, RANDOMIZER_STAR_NORMAL,            "Rainbow Ride", "Coins Amassed in a Maze"),
    RSTAR(102, COURSE_RR,    LEVEL_RR,    4, STAR_INDEX_ACT_4,     3, 150, RANDOMIZER_STAR_NORMAL,            "Rainbow Ride", "Swingin' in the Breeze"),
    RSTAR(103, COURSE_RR,    LEVEL_RR,    5, STAR_INDEX_ACT_5,     4, 170, RANDOMIZER_STAR_NORMAL,            "Rainbow Ride", "Tricky Triangles"),
    RSTAR(104, COURSE_RR,    LEVEL_RR,    6, STAR_INDEX_ACT_6,     4, 180, RANDOMIZER_STAR_NORMAL,            "Rainbow Ride", "Somewhere Over the Rainbow"),
    RSTAR(105, COURSE_RR,    LEVEL_RR,    1, STAR_INDEX_100_COINS, 5, 420, RANDOMIZER_STAR_100_COINS,         "Rainbow Ride", "100 Coins"),

    RSTAR(106, COURSE_BITDW, LEVEL_BITDW, 1, STAR_INDEX_ACT_1,     3, 160, RANDOMIZER_STAR_BOWSER_RED_COINS, "Bowser in the Dark World", "8 Red Coins"),
    RSTAR(107, COURSE_BITFS, LEVEL_BITFS, 1, STAR_INDEX_ACT_1,     4, 180, RANDOMIZER_STAR_BOWSER_RED_COINS, "Bowser in the Fire Sea", "8 Red Coins"),
    RSTAR(108, COURSE_BITS,  LEVEL_BITS,  1, STAR_INDEX_ACT_1,     4, 190, RANDOMIZER_STAR_BOWSER_RED_COINS, "Bowser in the Sky", "8 Red Coins"),
    RSTAR(109, COURSE_PSS,   LEVEL_PSS,   1, STAR_INDEX_ACT_1,     1,  50, RANDOMIZER_STAR_SECRET,           "Princess's Secret Slide", "Box Star"),
    RSTAR(110, COURSE_PSS,   LEVEL_PSS,   2, STAR_INDEX_ACT_2,     2,  45, RANDOMIZER_STAR_SECRET,           "Princess's Secret Slide", "Under 21 Seconds"),
    RSTAR(111, COURSE_COTMC, LEVEL_COTMC, 1, STAR_INDEX_ACT_1,     2, 100, RANDOMIZER_STAR_SECRET,           "Cavern of the Metal Cap", "8 Red Coins"),
    RSTAR(112, COURSE_TOTWC, LEVEL_TOTWC, 1, STAR_INDEX_ACT_1,     2, 100, RANDOMIZER_STAR_SECRET,           "Tower of the Wing Cap", "8 Red Coins"),
    RSTAR(113, COURSE_VCUTM, LEVEL_VCUTM, 1, STAR_INDEX_ACT_1,     2, 100, RANDOMIZER_STAR_SECRET,           "Vanish Cap Under the Moat", "8 Red Coins"),
    RSTAR(114, COURSE_WMOTR, LEVEL_WMOTR, 1, STAR_INDEX_ACT_1,     3, 130, RANDOMIZER_STAR_SECRET,           "Wing Mario Over the Rainbow", "8 Red Coins"),
    RSTAR(115, COURSE_SA,    LEVEL_SA,    1, STAR_INDEX_ACT_1,     2,  90, RANDOMIZER_STAR_SECRET,           "Secret Aquarium", "8 Red Coins"),
    RSTAR(116, COURSE_NONE,  LEVEL_CASTLE,1, STAR_INDEX_ACT_1,     1,  60, RANDOMIZER_STAR_TOAD,             "Castle Secret Stars", "Basement Toad"),
    RSTAR(117, COURSE_NONE,  LEVEL_CASTLE,1, STAR_INDEX_ACT_2,     1,  60, RANDOMIZER_STAR_TOAD,             "Castle Secret Stars", "Second Floor Toad"),
    RSTAR(118, COURSE_NONE,  LEVEL_CASTLE,1, STAR_INDEX_ACT_3,     1,  60, RANDOMIZER_STAR_TOAD,             "Castle Secret Stars", "Third Floor Toad"),
    RSTAR(119, COURSE_NONE,  LEVEL_CASTLE,1, STAR_INDEX_ACT_4,     2, 120, RANDOMIZER_STAR_MIPS,             "Castle Secret Stars", "MIPS 15 Star"),
    RSTAR(120, COURSE_NONE,  LEVEL_CASTLE,1, STAR_INDEX_ACT_5,     2, 130, RANDOMIZER_STAR_MIPS,             "Castle Secret Stars", "MIPS 50 Star"),
};

#undef RSTAR

const struct RandomizerStar *randomizer_star_get(s32 starIndex) {
    if (starIndex < 0 || starIndex >= RANDOMIZER_STAR_COUNT) {
        return NULL;
    }

    return &gRandomizerStars[starIndex];
}

s32 randomizer_star_index_from_course_star(s32 courseNum, UNUSED s32 levelNum, s32 starIndex) {
    s32 i;

    for (i = 0; i < RANDOMIZER_STAR_COUNT; i++) {
        if (gRandomizerStars[i].course == courseNum && gRandomizerStars[i].starIndex == starIndex) {
            return i;
        }
    }

    return -1;
}

s32 randomizer_star_course_order(s32 starIndex) {
    const struct RandomizerStar *star = randomizer_star_get(starIndex);

    if (star == NULL) {
        return 0x7FFF;
    }

    if (star->course == COURSE_NONE) {
        return COURSE_MAX + 1;
    }

    return star->course;
}

s32 randomizer_star_count_in_course(s32 courseNum) {
    s32 i;
    s32 count = 0;

    for (i = 0; i < RANDOMIZER_STAR_COUNT; i++) {
        if (gRandomizerStars[i].course == courseNum) {
            count++;
        }
    }

    return count;
}

s32 randomizer_star_first_in_course(s32 courseNum) {
    s32 i;

    for (i = 0; i < RANDOMIZER_STAR_COUNT; i++) {
        if (gRandomizerStars[i].course == courseNum) {
            return i;
        }
    }

    return -1;
}

const char *randomizer_star_course_name(s32 courseNum) {
    s32 firstStar = randomizer_star_first_in_course(courseNum);

    if (firstStar < 0) {
        return "Unknown Course";
    }

    return gRandomizerStars[firstStar].courseName;
}
