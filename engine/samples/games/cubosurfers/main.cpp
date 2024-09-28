#include <cubos/engine/assets/plugin.hpp>
#include <cubos/engine/collisions/colliding_with.hpp>
#include <cubos/engine/defaults/plugin.hpp>
#include <cubos/engine/input/plugin.hpp>
#include <cubos/engine/render/lights/environment.hpp>
#include <cubos/engine/render/voxels/palette.hpp>
#include <cubos/engine/scene/plugin.hpp>
#include <cubos/engine/settings/plugin.hpp>
#include <cubos/engine/voxels/plugin.hpp>

#include "obstacle.hpp"
#include "player.hpp"
#include "spawner.hpp"
#include "powerup.hpp"

using namespace cubos::engine;

static const Asset<Scene> SceneAsset = AnyAsset("ee5bb451-05b7-430f-a641-a746f7009eef");
static const Asset<VoxelPalette> PaletteAsset = AnyAsset("101da567-3d23-46ae-a391-c10ec00e8718");
static const Asset<InputBindings> InputBindingsAsset = AnyAsset("b20900a4-20ee-4caa-8830-14585050bead");

int main()
{
    Cubos cubos{};

    cubos.plugin(defaultsPlugin);
    cubos.plugin(spawnerPlugin);
    cubos.plugin(obstaclePlugin);
    cubos.plugin(playerPlugin);
    cubos.plugin(powerUpPlugin);

    cubos.startupSystem("configure settings").tagged(settingsTag).call([](Settings& settings) {
        settings.setString("assets.io.path", SAMPLE_ASSETS_FOLDER);
    });

    cubos.startupSystem("set the palette, environment, input bindings and spawn the scene")
        .tagged(assetsTag)
        .call([](Commands commands, const Assets& assets, RenderPalette& palette, Input& input,
                 RenderEnvironment& environment) {
            palette.asset = PaletteAsset;
            environment.ambient = {0.1F, 0.1F, 0.1F};
            environment.skyGradient[0] = {0.2F, 0.4F, 0.8F};
            environment.skyGradient[1] = {0.6F, 0.6F, 0.8F};
            input.bind(*assets.read(InputBindingsAsset));
            commands.spawn(assets.read(SceneAsset)->blueprint);
        });

    cubos.system("restart the game on input")
        .call([](Commands cmds, const Assets& assets, const Input& input, Query<Entity> all) {
            if (input.justPressed("restart"))
            {
                for (auto [ent] : all)
                {
                    cmds.destroy(ent);
                }

                cmds.spawn(assets.read(SceneAsset)->blueprint);
            }
        });

    cubos.system("detect player vs obstacle collisions")
        .call([](Commands cmds, const Assets& assets, Query<Entity> all, Query<Player&, const CollidingWith&, const Obstacle&, Entity> collisions) {
            for (auto [player, collidingWith, obstacle, ent] : collisions)
            {
                CUBOS_INFO("Player collided with an obstacle!");
                
                if (player.shield)
                {
                    CUBOS_INFO("Shield destroyed!");
                    player.shield = 0;
                    cmds.destroy(ent);
                    break;
                }

                // Destroy all entities
                for (auto [ent] : all)
                {
                    cmds.destroy(ent);
                }
                // Respawn all entities
                cmds.spawn(assets.read(SceneAsset)->blueprint);
            }
        });

    cubos.system("detect player vs powerups collisions")
        .call([](Commands cmds, Query<Player&, const CollidingWith&, const PowerUp&, Entity> collisions) {
            for (auto [player, collidingWith, powerup, ent] : collisions)
            {
                CUBOS_INFO("Player collided with a powerup");

                if (powerup.type == "shield")
                {
                    CUBOS_INFO("Shield received");
                    player.shield = 1;
                }
                else if (powerup.type == "jetpack")
                {
                    CUBOS_INFO("Jetpack received");
                    player.jetpack = 1;
                }

                cmds.destroy(ent);
            }
        });

    cubos.system("speed up obstacles and powerups")
        .call([](Query<Obstacle&> obstacles, Query<PowerUp&> powerups, Query<const Spawner&> spawners) {
            float spawned = 0;    
            for (auto [spawner] : spawners)
            {
                spawned = (float) spawner.entitiesSpawned;
            }

            for (auto [obstacle] : obstacles)
            {
                obstacle.velocity.z = -100.0f - spawned * 10.0f;
            }

            for (auto [powerup] : powerups)
            {
                powerup.velocity.z = -100.0f - spawned * 10.0f;
            }
        });
    
    cubos.run();
}
