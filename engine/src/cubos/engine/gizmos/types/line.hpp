#pragma once

#include <glm/gtc/matrix_transform.hpp>

#include <cubos/engine/gizmos/gizmos.hpp>

#include "../renderer.hpp"

namespace cubos::engine
{
    /// @brief A gizmo that is a line connecting two points
    class LineGizmo : public cubos::engine::Gizmos::Gizmo
    {
        glm::vec3 mPointA;
        glm::vec3 mPointB;

    public:
        /// @brief LineGizmo constructor
        /// @param id identifier of the gizmo
        /// @param from point at one of the ends of the line
        /// @param to point at the other end of the line
        /// @param color color for the gizmo to be drawn in
        /// @param lifespan time the gizmo will remain on screen, in seconds
        LineGizmo(const std::string& id, glm::vec3 from, glm::vec3 to, const glm::vec3& color, float lifespan)
            : cubos::engine::Gizmos::Gizmo(id, color, lifespan)
            , mPointA(from)
            , mPointB(to)
        {
        }

        /// @brief draws the gizmo to screen
        /// @param renderer the GizmosRenderer in use
        void draw(cubos::engine::GizmosRenderer& renderer, const glm::mat<4, 4, float, glm::packed_highp>& mvp) override
        {
            auto* verts = static_cast<glm::vec3*>(renderer.linePrimitive.vb->map());
            verts[0] = {mPointA[0], mPointA[1], mPointA[2]};
            verts[1] = {mPointB[0], mPointB[1], mPointB[2]};

            renderer.linePrimitive.vb->unmap();

            renderer.renderDevice->setVertexArray(renderer.linePrimitive.va);

            auto mvpBuffer =
                renderer.renderDevice->createConstantBuffer(sizeof(glm::mat4), &mvp, cubos::core::gl::Usage::Dynamic);
            renderer.pipeline->getBindingPoint("MVP")->bind(mvpBuffer);

            renderer.pipeline->getBindingPoint("objColor")->setConstant(mColor);
            renderer.renderDevice->setRasterState(
                renderer.renderDevice->createRasterState(cubos::core::gl::RasterStateDesc{}));
            renderer.renderDevice->drawLines(0, 2);
        }
    };
} // namespace cubos::engine
