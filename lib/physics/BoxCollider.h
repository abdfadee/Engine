#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/norm.hpp>
#include <vector>

class BoxCollider {
public:
    struct CollisionInfo {
        bool isColliding = false;
        float penetrationDepth = 0.0f;
        glm::vec3 normal;       // Collision normal (points from this to other)
        glm::vec3 contactPoint; // World-space contact point
    };

    BoxCollider(const glm::vec3& extents)
        : m_extents(extents), m_transform(1.0f) {
        UpdateVertices();
    }

    void SetTransform(const glm::mat4& transform) {
        m_transform = transform;
        UpdateVertices();
    }

    bool CollidesWith(const BoxCollider& other) const {
        // Test 15 potential separating axes
        for (int i = 0; i < 3; i++) {
            if (!OverlapsOnAxis(other, GetAxis(i))) return false;
        }
        for (int i = 0; i < 3; i++) {
            if (!OverlapsOnAxis(other, other.GetAxis(i))) return false;
        }
        for (int i = 0; i < 3; i++) {
            for (int j = 0; j < 3; j++) {
                glm::vec3 axis = glm::cross(GetAxis(i), other.GetAxis(j));
                if (glm::length2(axis) < 0.001f) continue;
                axis = glm::normalize(axis);
                if (!OverlapsOnAxis(other, axis)) return false;
            }
        }
        return true;
    }

    CollisionInfo GetCollisionInfo(const BoxCollider& other) const {
        CollisionInfo info;
        float minPenetration = std::numeric_limits<float>::max();
        glm::vec3 minAxis;

        if (!TestAllAxes(other, info, minPenetration, minAxis)) {
            info.isColliding = false;
            return info;
        }

        info.isColliding = true;
        info.normal = minAxis;
        info.penetrationDepth = minPenetration;
        info.contactPoint = CalculateContactPoint(other);

        return info;
    }

private:
    glm::vec3 m_extents;
    glm::mat4 m_transform;
    std::vector<glm::vec3> m_vertices;

    void UpdateVertices() {
        m_vertices.clear();
        for (float x : { -1.0f, 1.0f }) {
            for (float y : { -1.0f, 1.0f }) {
                for (float z : { -1.0f, 1.0f }) {
                    glm::vec4 vertex = m_transform * glm::vec4(
                        x * m_extents.x,
                        y * m_extents.y,
                        z * m_extents.z,
                        1.0f
                    );
                    m_vertices.push_back(glm::vec3(vertex));
                }
            }
        }
    }

    glm::vec3 GetAxis(int index) const {
        return glm::normalize(glm::vec3(m_transform[index]));
    }

    bool OverlapsOnAxis(const BoxCollider& other, const glm::vec3& axis) const {
        float minA, maxA, minB, maxB;
        ProjectOntoAxis(axis, minA, maxA);
        other.ProjectOntoAxis(axis, minB, maxB);
        return (maxA >= minB) && (maxB >= minA);
    }

    void ProjectOntoAxis(const glm::vec3& axis, float& min, float& max) const {
        min = std::numeric_limits<float>::max();
        max = std::numeric_limits<float>::lowest();
        for (const auto& vertex : m_vertices) {
            float projection = glm::dot(vertex, axis);
            min = std::min(min, projection);
            max = std::max(max, projection);
        }
    }

    bool TestAllAxes(const BoxCollider& other, CollisionInfo& info,
        float& minPenetration, glm::vec3& minAxis) const {
        for (int i = 0; i < 3; i++) {
            if (!TestPenetration(other, GetAxis(i), minPenetration, minAxis))
                return false;
        }
        for (int i = 0; i < 3; i++) {
            if (!TestPenetration(other, other.GetAxis(i), minPenetration, minAxis))
                return false;
        }
        for (int i = 0; i < 3; i++) {
            for (int j = 0; j < 3; j++) {
                glm::vec3 axis = glm::cross(GetAxis(i), other.GetAxis(j));
                if (glm::length2(axis) < 0.001f) continue;
                axis = glm::normalize(axis);
                if (!TestPenetration(other, axis, minPenetration, minAxis))
                    return false;
            }
        }
        return true;
    }

    bool TestPenetration(const BoxCollider& other, const glm::vec3& axis,
        float& minPenetration, glm::vec3& minAxis) const {
        float minA, maxA, minB, maxB;
        ProjectOntoAxis(axis, minA, maxA);
        other.ProjectOntoAxis(axis, minB, maxB);

        if (maxA < minB || maxB < minA) return false;

        float penetration = std::min(maxA, maxB) - std::max(minA, minB);
        if (penetration < minPenetration) {
            minPenetration = penetration;
            minAxis = axis;

            // Ensure normal points from this to other
            glm::vec3 centerA = glm::vec3(m_transform[3]);
            glm::vec3 centerB = glm::vec3(other.m_transform[3]);
            if (glm::dot(centerB - centerA, minAxis) < 0.0f)
                minAxis = -minAxis;
        }
        return true;
    }

    glm::vec3 CalculateContactPoint(const BoxCollider& other) const {
        std::vector<glm::vec3> contacts;
        for (const auto& vertex : m_vertices) {
            if (other.ContainsPoint(vertex))
                contacts.push_back(vertex);
        }
        for (const auto& vertex : other.m_vertices) {
            if (ContainsPoint(vertex))
                contacts.push_back(vertex);
        }
        if (contacts.empty()) {
            return (glm::vec3(m_transform[3]) + glm::vec3(other.m_transform[3])) * 0.5f;
        }
        glm::vec3 sum(0.0f);
        for (const auto& point : contacts)
            sum += point;
        return sum / (float)contacts.size();
    }

    bool ContainsPoint(const glm::vec3& point) const {
        glm::mat4 invTransform = glm::inverse(m_transform);
        glm::vec3 localPoint = glm::vec3(invTransform * glm::vec4(point, 1.0f));
        return (fabs(localPoint.x) <= m_extents.x + 0.001f) &&
            (fabs(localPoint.y) <= m_extents.y + 0.001f) &&
            (fabs(localPoint.z) <= m_extents.z + 0.001f);
    }
};