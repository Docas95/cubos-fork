#include <cubos/core/ecs/reflection.hpp>
#include <cubos/core/reflection/external/glm.hpp>
#include <cubos/core/reflection/external/primitives.hpp>
#include <cubos/core/reflection/external/string.hpp>

#include <cubos/engine/assets/plugin.hpp>
#include <cubos/engine/transform/plugin.hpp>

#include "powerup.hpp"

using namespace cubos::engine;

CUBOS_REFLECT_IMPL(PowerUp)
{

    return cubos::core::ecs::TypeBuilder<PowerUp>("PowerUp")
        .withField("velocity", &PowerUp::velocity)
        .withField("killZ", &PowerUp::killZ)
        .withField("type", &PowerUp::type)
        .build();
}

void powerUpPlugin(cubos::engine::Cubos& cubos)
{
    cubos.depends(assetsPlugin);
    cubos.depends(transformPlugin);

    cubos.component<PowerUp>();

    cubos.system("move powerups")
        .call([](Commands cmds, const DeltaTime& dt, Query<Entity, PowerUp&, Position&> powerups) {
            for (auto [ent, powerup, position] : powerups)
            {
                position.vec += powerup.velocity * dt.value();
                position.vec.y = glm::abs(glm::sin(position.vec.z * 0.15F)) * 1.5F;

                if (position.vec.z < powerup.killZ)
                {
                    cmds.destroy(ent);
                }
            }
        });
}
