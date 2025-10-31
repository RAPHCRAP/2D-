#include<iostream>
#include <vector>
#include <memory>
#include <cassert>
#include <cmath>
#include <SFML/Graphics.hpp>
#include "CDS.h"

using namespace sf;
using namespace std;

const float PI = 3.14159265358979323846f;



// ==================== UTILITY FUNCTIONS ====================
namespace MathUtils {

    void display3DVector(Vector3f v)
    {
        cout << "(" << v.x << ", " << v.y << ", " << v.z << ")   ";
    }

    void display2DVector(Vector2f v)
    {
        cout << "(" << v.x << ", " << v.y << ")   ";
    }


    Vector3f cross(const Vector3f& a, const Vector3f& b) {
        return {
            a.y * b.z - a.z * b.y,
            a.z * b.x - a.x * b.z,
            a.x * b.y - a.y * b.x
        };
    }

    float dot(const Vector2f& a, const Vector2f& b) {
        return a.x * b.x + a.y * b.y;
    }

    float dot(const Vector3f& a, const Vector3f& b) {
        return a.x * b.x + a.y * b.y + a.z * b.z;
    }


    float length(const Vector3f& v) {
        return std::sqrt(v.x * v.x + v.y * v.y + v.z * v.z);
    }


    inline float degreesToRadians(float degrees) {
        return degrees * (3.14159265f / 180.0f);
    }

    Vector3f normalize(const Vector3f& v) {
        float mag = sqrt(dot(v, v));
        return (mag == 0) ? Vector3f(0, 0, 0) : v / mag;
    }



    // USING

    Vector2f projectToPerspective2D(const Vector3f& point, Vector3f position, Vector3f forward, Vector3f up, float focalLength = 500.0f) {
        using namespace MathUtils;

        Vector3f relative = point - position;

        cout << "REALATIVE  : "; display3DVector(relative);

        Vector3f right = normalize(cross(forward, up));
        Vector3f Up = normalize(up);

        cout << "UP:"; display3DVector(Up);
        cout << "right:"; display3DVector(right);
        cout << endl;


        float x = dot(relative, right);
        float y = dot(relative, Up);
        float z = dot(relative, forward);



        if (z <= 0.01f) z = 0.01f;

        return Vector2f(x * (focalLength / z), y * (focalLength / z));
    }


    Vector2f projectToScreen2D(const Vector3f& point, Vector3f centre, Vector3f up, Vector3f right)
    {
        using namespace MathUtils;

        Vector3f toPoint = point - centre;

        float x = dot(toPoint, right);
        float y = dot(toPoint, up);

        return Vector2f(x, y);
    }

    float distanceToPlane(const Vector3f& targetPoint,
        const Vector3f& planePoint,
        const Vector3f& planeNormal)
    {

        float normalLength = sqrt(planeNormal.x * planeNormal.x +
            planeNormal.y * planeNormal.y +
            planeNormal.z * planeNormal.z);

        if (normalLength == 0) {
            throw invalid_argument("Plane normal cannot be a zero vector");
        }

        sf::Vector3f unitNormal = planeNormal / normalLength;

        float distance = unitNormal.x * (targetPoint.x - planePoint.x) +
            unitNormal.y * (targetPoint.y - planePoint.y) +
            unitNormal.z * (targetPoint.z - planePoint.z);


        return std::abs(distance);
    }


    bool isInFrontOfDirectedPlane(Vector3f point, Vector3f position, Vector3f forward)
    {
        using namespace MathUtils;

        Vector3f toPolygon = point - position;

        float dotResult = dot(toPolygon, forward);

        return dotResult > 0;
    }


    Vector3f rotateAroundAxis(const Vector3f& P, const Vector3f& axis, const Vector3f& center, float angleRad)
    {
        Vector3f r = P - center;
        float cosA = cos(angleRad);
        float sinA = sin(angleRad);
        float dotAR = dot(axis, r);

        Vector3f rotated =
            r * cosA +
            cross(axis, r) * sinA +
            axis * dotAR * (1 - cosA);

        return center + rotated;
    }

    void rotateVectorAroundAxis(Vector3f& vecToRotate, const Vector3f& axis, float angleRad)
    {
        float cosA = cos(angleRad);
        float sinA = sin(angleRad);
        float dotAR = dot(axis, vecToRotate);

        vecToRotate =
            vecToRotate * cosA +
            cross(axis, vecToRotate) * sinA +
            axis * dotAR * (1 - cosA);
    }

    Vector3f getRotationAxis(const Vector3f& worldUp, const Vector3f& forward)
    {
        Vector3f axis = worldUp - forward * dot(worldUp, forward);
        if (length(axis) < 0.001f)
        {
            Vector3f altUp = Vector3f(1.0f, 0.0f, 0.0f);
            axis = altUp - forward * dot(altUp, forward);
        }
        return normalize(axis);
    }

    struct DepthOrder {
        float distance;
        float centerWeight; // Higher = more centered in view

        bool operator<(const DepthOrder& other) const {
            if (distance != other.distance) {
                return distance < other.distance;
            }
            return centerWeight > other.centerWeight;
        }

        bool operator>(const DepthOrder& other) const {
            if (distance != other.distance) {
                return distance > other.distance;
            }
            return centerWeight < other.centerWeight;
        }

        friend std::ostream& operator<<(std::ostream& os, const DepthOrder& d) {
            os << "[Dist: " << d.distance << ", CenterW: " << d.centerWeight << "]";
            return os;
        }

    };

    //CameraRelativePosition getCameraRelativePosition(
    //    const Vector3f& cameraPos,
    //    const Vector3f& cameraForward,
    //    const Vector3f& point)
    //{
    //    Vector3f toPoint = point - cameraPos;
    //    float distance = length(toPoint);
    //    float angle = acos(dot(normalize(toPoint), cameraForward));

    //    float angleWeight = angle * (1.0f - 1.0f / (1.0f + distance / 100.0f));

    //    return { distance, angleWeight };
    //}


    DepthOrder calculateDepthOrder(
        const Vector3f& cameraPos,
        const Vector3f& cameraForward,
        const Vector3f& point)
    {
        Vector3f toPoint = point - cameraPos;
        float distance = length(toPoint);

        // Calculate how centered the object is (1 = dead center, 0 = edge of view)
        float centerWeight = dot(normalize(toPoint), cameraForward);

        return { distance, centerWeight };
    }

}

using namespace MathUtils;



class Camera {
public:
    Vector3f position;
    Vector3f forward;
    Vector3f up;

    float scale;
    float focalLength;



    Camera(Vector3f pos = { 0, 0, -500 }, Vector3f lookAt = { 0, 0, 0 }, Vector3f worldUp = { 0, 1, 0 }, float scale = 1.f, float focalLength = 500.f) : scale(scale), focalLength(focalLength)
    {
        using namespace MathUtils;
        position = pos;
        forward = normalize(lookAt - pos);
        Vector3f right = normalize(cross(forward, worldUp));
        up = cross(right, forward);
    }

    Vector3f getCenter() const { return position; }

    Vector3f right() const
    {
        using namespace MathUtils;
        return normalize(cross(forward, up));
    }

    void move(const Vector3f& delta) {
        position += delta;
    }

    void rotateAround(const Vector3f& axis, float angleRad) {
        using namespace MathUtils;
        Vector3f origin = Vector3f(0.f, 0.f, 0.f);
        forward = normalize(rotateAroundAxis(forward, axis, origin, angleRad));
        up = normalize(rotateAroundAxis(up, axis, origin, angleRad));
    }

    void rotateAroundA(Vector3f worldUp, float deltaAngleDegrees, float deltaTime) {
        using namespace MathUtils;
        Vector3f axis = getRotationAxis(worldUp, forward);
        float angleRad = degreesToRadians(deltaAngleDegrees) * deltaTime;
        rotateAround(axis, angleRad);
    }

    void rotateAroundY(float deltaAngleDegrees, float deltaTime) {
        rotateAroundA(Vector3f(0.0f, 1.0f, 0.0f), deltaAngleDegrees, deltaTime);
    }

    void rotateAroundX(float deltaAngleDegrees, float deltaTime) {
        rotateAroundA(Vector3f(1.0f, 0.0f, 0.0f), deltaAngleDegrees, deltaTime);
    }

    void rotateAroundZ(float deltaAngleDegrees, float deltaTime) {
        rotateAroundA(Vector3f(0.0f, 0.0f, 1.0f), deltaAngleDegrees, deltaTime);
    }

    void rotateInPlane(float deltaAngleDegrees, float deltaTime) {
        using namespace MathUtils;
        float angleRad = degreesToRadians(deltaAngleDegrees) * deltaTime;
        up = normalize(rotateAroundAxis(up, forward, Vector3f(0.f, 0.f, 0.f), angleRad));
    }


    void rotateYawPitch(float deltaYawDeg, float deltaPitchDeg)
    {
        using namespace MathUtils;


        float yawRad = degreesToRadians(deltaYawDeg);
        float pitchRad = degreesToRadians(deltaPitchDeg);


        Vector3f right = normalize(cross(forward, up));


        forward = normalize(rotateAroundAxis(forward, { 0.f, 1.f, 0.f }, { 0.f, 0.f, 0.f }, yawRad));


        right = normalize(cross(forward, { 0.f, 1.f, 0.f }));

        forward = normalize(rotateAroundAxis(forward, right, { 0.f, 0.f, 0.f }, pitchRad));
        up = normalize(cross(right, forward));
    }
};






class Brick;
class Fragment;



// ==================== FRAGMENT CLASS ====================
class Fragment : public Drawable {
private:
    VertexArray m_vertices;
    bool m_filled;
    Color m_color;
    vector<Vector2f> m_points;
    Brick* m_subject;

public:

    Fragment() :
        m_filled(false),
        m_color(sf::Color::White),
        m_vertices(sf::LineStrip)
    {
    }


    Fragment(const Fragment& other) :
        m_filled(other.m_filled),
        m_color(other.m_color),
        m_points(other.m_points),
        m_vertices(other.m_vertices)
    {
        updateShape();
    }


    void setSubject(Brick* subj)
    {
        m_subject = subj;
    }

    Brick* getSubject()
    {
        return m_subject;
    }


    void fill() {
        m_filled = true;
        updateShape();
    }

    void outline() {
        m_filled = false;
        updateShape();
    }

    void setColor(const sf::Color& color) {
        m_color = color;
        updateShape();
    }

    void changeEdges(size_t count) {
        m_points.resize(count);
        updateShape();
    }

    void setPoint(size_t index, const sf::Vector2f& point) {
        if (index < m_points.size()) {
            m_points[index] = point;
            updateShape();
        }
    }

    void updateShape() {
        m_vertices.clear();
        m_vertices.setPrimitiveType(m_filled ? sf::TriangleFan : sf::LineStrip);

        for (const auto& point : m_points) {
            m_vertices.append(sf::Vertex(point, m_color));
        }

        if (!m_filled && !m_points.empty()) {
            m_vertices.append(sf::Vertex(m_points[0], m_color));
        }
    }


    void draw(RenderTarget& target, RenderStates states) const override {
        target.draw(m_vertices, states);
    }


    bool isFilled() const { return m_filled; }
    const sf::Color& getColor() const { return m_color; }
    size_t getPointCount() const { return m_points.size(); }
    const sf::Vector2f& getPoint(size_t index) const {
        return m_points.at(index);
    }
};



class Brick {
private:
    vector<Vector3f> m_points;
    bool m_filled = false;
    sf::Color m_color = sf::Color::White;
    Vector3f m_normal = { 0,0,1 };

public:
    Brick(size_t pointCount = 4,double radius = 250.f) : m_points(pointCount) {
        createRoundedShape(radius);
        updateNormal();
    }

    ~Brick() = default;

    Brick(const Brick&) = delete;
    Brick& operator=(const Brick&) = delete;

    void createRoundedShape(float radius)
    {
        float theta = 2 * PI / m_points.size();
        for (size_t i = 0; i < m_points.size(); ++i)
        {
            float angle = i * theta;
            m_points[i] = Vector3f(
                radius * cos(angle),
                radius * sin(angle),
                0.0f
            );
        }
        updateNormal();
    }

    void setPoint(size_t index, const Vector3f& point)
    {
        if (index < m_points.size())
        {
            m_points[index] = point;
            updateNormal();
        }
    }


    void setColor(const sf::Color& color) { m_color = color; }
    void fill() { m_filled = true; }
    void outline() { m_filled = false; }
    const sf::Color& getColor() const { return m_color; }
    bool isFilled() const { return m_filled; }


    Vector3f getCenter() const
    {
        Vector3f sum(0.f, 0.f, 0.f);
        for (const auto& p : m_points) {
            sum += p;
        }
        return sum / static_cast<float>(m_points.size());
    }

    const Vector3f& getPlaneNormal() const { return m_normal; }
    const vector<Vector3f>& getPoints() const { return m_points; }
    size_t getPointCount() const { return m_points.size(); }


    void move(const Vector3f& delta)
    {
        for (auto& p : m_points) {
            p += delta;
        }

    }

    void setPosition(const Vector3f& newPos)
    {
        move(newPos - getCenter());
    }

    void rotateAround(const Vector3f& axis, float angleRad)
    {
        Vector3f center = getCenter();
        for (auto& p : m_points) {
            p = rotateAroundAxis(p, axis, center, angleRad);
        }
        m_normal = rotateAroundAxis(m_normal, axis, Vector3f(0, 0, 0), angleRad);
    }

    void rotateAroundA(Vector3f worldUp, float deltaAngleDegrees, float deltaTime)
    {
        Vector3f normal = getPlaneNormal();
        Vector3f projectedUp = worldUp - normal * dot(worldUp, normal);

        if (length(projectedUp) < 0.001f) {
            projectedUp = Vector3f(1.0f, 0.0f, 0.0f) - normal * dot(Vector3f(1.0f, 0.0f, 0.0f), normal);
        }

        Vector3f axis = normalize(projectedUp);
        float angleRad = degreesToRadians(deltaAngleDegrees) * deltaTime;
        rotateAround(axis, angleRad);
    }


    void perspectiveProject(Fragment* frag, Vector3f position,
        Vector3f forward, Vector3f up,
        Vector2f winSize, float focalLength = 500.f) const
    {
        if (m_filled)
        {
            frag->fill();
        }
        else
        {
            frag->outline();
        }


        frag->setColor(m_color);

        frag->changeEdges(m_points.size());

        Vector2f screenCenter = winSize * 0.5f;
        for (size_t i = 0; i < m_points.size(); i++) {
            Vector2f local = projectToPerspective2D(m_points[i], position, forward, up, focalLength);
            frag->setPoint(i, screenCenter + Vector2f(local.x, -local.y));
        }
        frag->updateShape();
    }

    void orthogonalProject(Fragment* frag, Vector3f centre,
        Vector3f up, Vector3f right,
        Vector2f winSize, float scale = 1.f) const
    {
        if (m_filled)
        {
            frag->fill();
        }
        else
        {
            frag->outline();
        }


        frag->setColor(m_color);
        frag->changeEdges(m_points.size());

        Vector2f screenCenter = winSize * 0.5f;
        for (size_t i = 0; i < m_points.size(); i++) {
            Vector2f local = projectToScreen2D(m_points[i], centre, up, right);
            frag->setPoint(i, screenCenter + Vector2f(local.x, -local.y) * scale);
        }
        frag->updateShape();
    }

private:
    void updateNormal()
    {
        if (m_points.size() >= 3) {
            Vector3f U = m_points[1] - m_points[0];
            Vector3f V = m_points[2] - m_points[0];
            m_normal = normalize(cross(U, V));
        }
    }


};

class WObject
{
protected:
    Vector3f m_up = { 0, 0, 1 };
    Vector3f m_forward = { 0, 1, 0 };

public:
    vector<Brick*> m_segments;


    virtual ~WObject()
    {

        for (auto brick : m_segments) {
            delete brick;
        }
        m_segments.clear();
    }

    Vector3f right() const
    {
        return normalize(cross(m_up, m_forward));
    }

    const Vector3f& up() const { return m_up; }
    const Vector3f& forward() const { return m_forward; }

    virtual Vector3f getPosition() const = 0;
    virtual void setPosition(const Vector3f& newPos) = 0;


    virtual void perspectiveProject(Fragment* frag, Vector3f position,
        Vector3f forward, Vector3f up,
        Vector2f winSize, float focalLength = 500.f) = 0;
    virtual void orthogonalProject(Fragment* frag, Vector3f centre,
        Vector3f up, Vector3f right,
        Vector2f winSize, float scale = 1.f) = 0;


    virtual void move(const Vector3f& delta) = 0;
    virtual void rotateAround(const Vector3f& axis, float angleRad) = 0;
    virtual void rotateAroundA(Vector3f worldUp, float deltaAngleDegrees, float deltaTime) = 0;
    virtual void rotateAroundY(float deltaAngleDegrees, float deltaTime) = 0;
    virtual void rotateAroundX(float deltaAngleDegrees, float deltaTime) = 0;
    virtual void rotateAroundZ(float deltaAngleDegrees, float deltaTime) = 0;




    virtual void fill() = 0;
    virtual void outline() = 0;
    virtual void setColor(const sf::Color& col) = 0;


    void addBrick(Brick* brick)
    {
        m_segments.push_back(brick);
    }

    const Brick* getBrick(size_t index) const
    {
        return index < m_segments.size() ? m_segments[index] : nullptr;
    }

    size_t brickCount() const { return m_segments.size(); }

    void handleInput(float moveSpeed)
    {
        Vector3f dir = m_forward;
        Vector3f per = right();

        if (Keyboard::isKeyPressed(Keyboard::W))
        {
            setPosition(getPosition() + dir * moveSpeed);
        }
        if (Keyboard::isKeyPressed(Keyboard::S))
        {
            setPosition(getPosition() - dir * moveSpeed);
        }
        if (Keyboard::isKeyPressed(Keyboard::A))
        {
            setPosition(getPosition() - per * moveSpeed);
        }
        if (Keyboard::isKeyPressed(Keyboard::D))
        {
            setPosition(getPosition() + per * moveSpeed);
        }
    }

    void handleRotation(float deltaAngle, float deltaTime)
    {
        if (Keyboard::isKeyPressed(Keyboard::R))
        {
            rotateAroundX(deltaAngle, deltaTime);
        }
        if (Keyboard::isKeyPressed(Keyboard::T))
        {
            rotateAroundX(-deltaAngle, deltaTime);
        }
        if (Keyboard::isKeyPressed(Keyboard::Y))
        {
            rotateAroundY(deltaAngle, deltaTime);
        }
        if (Keyboard::isKeyPressed(Keyboard::U))
        {
            rotateAroundY(-deltaAngle, deltaTime);
        }
        if (Keyboard::isKeyPressed(Keyboard::I))
        {
            rotateAroundZ(deltaAngle, deltaTime);
        }
        if (Keyboard::isKeyPressed(Keyboard::O))
        {
            rotateAroundZ(-deltaAngle, deltaTime);
        }
    }

    void setUp(const Vector3f& newUp)
    {
        
        m_up = normalize(newUp);

        // Recalculate forward to ensure orthogonality
        Vector3f rightVec = normalize(cross(m_up, m_forward));
        m_forward = normalize(cross(rightVec, m_up));
    }

    void setForward(const Vector3f& newForward)
    {
        
        m_forward = normalize(newForward);

        // Recalculate up to ensure orthogonality
        Vector3f rightVec = normalize(cross(m_up, m_forward));
        m_up = normalize(cross(m_forward, rightVec));
    }

//protected:
    void updateOrientation(const Vector3f& newUp, const Vector3f& newForward)
    {
        m_up = normalize(newUp);
        m_forward = normalize(newForward);
    }
};


class PolyGon : public WObject
{
public:
    PolyGon(unsigned int n = 4,double radius=250.f)
    {
        Brick* brick = new Brick(n,radius);
        m_segments.push_back(brick);
        updateOrientation(brick->getPlaneNormal(), Vector3f(0, 1, 0));
    }

    ~PolyGon() override = default;

    Vector3f getPosition() const override
    {
        return !m_segments.empty() ? m_segments[0]->getCenter() : Vector3f(0, 0, 0);
    }

    void setPosition(const Vector3f& newPos) override
    {
        if (!m_segments.empty())
        {
            Vector3f delta = newPos - m_segments[0]->getCenter();
            move(delta);
        }
    }

    // Projection methods
    void perspectiveProject(Fragment* frag, Vector3f position,
        Vector3f forward, Vector3f up,
        Vector2f winSize, float focalLength = 500.f) override
    {


        if (!m_segments.empty())
        {
            m_segments[0]->perspectiveProject(frag, position, forward, up, winSize, focalLength);
        }
    }

    void orthogonalProject(Fragment* frag, Vector3f centre,
        Vector3f up, Vector3f right,
        Vector2f winSize, float scale = 1.f) override
    {
        if (!m_segments.empty())
        {
            m_segments[0]->orthogonalProject(frag, centre, up, right, winSize, scale);
        }
    }


    void move(const Vector3f& delta) override
    {
        for (auto brick : m_segments)
        {
            brick->move(delta);
        }

    }

    void rotateAround(const Vector3f& axis, float angleRad) override
    {
        using namespace MathUtils;

        Vector3f center = getPosition();
        for (auto brick : m_segments)
        {
            brick->rotateAround(axis, angleRad);
        }

        updateOrientation(
            MathUtils::rotateAroundAxis(m_up, axis, Vector3f(0, 0, 0), angleRad),
            rotateAroundAxis(m_forward, axis, Vector3f(0, 0, 0), angleRad)
        );
    }

    void rotateAroundA(Vector3f worldUp, float deltaAngleDegrees, float deltaTime) override
    {
        if (!m_segments.empty())
        {
            m_segments[0]->rotateAroundA(worldUp, deltaAngleDegrees, deltaTime);

            updateOrientation(m_segments[0]->getPlaneNormal(), m_forward);
        }
    }

    void rotateAroundY(float deltaAngleDegrees, float deltaTime) override
    {
        if (!m_segments.empty())
        {
            float angleRad = degreesToRadians(deltaAngleDegrees) * deltaTime;
            Vector3f yAxis(0, 1, 0);
            for (auto brick : m_segments)
            {
                brick->rotateAround(yAxis, angleRad);
            }

            updateOrientation(
                rotateAroundAxis(m_up, yAxis, Vector3f(0, 0, 0), angleRad),
                rotateAroundAxis(m_forward, yAxis, Vector3f(0, 0, 0), angleRad)
            );
        }
    }

    void rotateAroundX(float deltaAngleDegrees, float deltaTime) override
    {
        if (!m_segments.empty())
        {
            float angleRad = degreesToRadians(deltaAngleDegrees) * deltaTime;
            Vector3f xAxis(1, 0, 0);
            for (auto brick : m_segments)
            {
                brick->rotateAround(xAxis, angleRad);
            }
            updateOrientation(
                rotateAroundAxis(m_up, xAxis, Vector3f(0, 0, 0), angleRad),
                rotateAroundAxis(m_forward, xAxis, Vector3f(0, 0, 0), angleRad)
            );
        }
    }

    void rotateAroundZ(float deltaAngleDegrees, float deltaTime) override
    {
        if (!m_segments.empty())
        {
            float angleRad = degreesToRadians(deltaAngleDegrees) * deltaTime;
            Vector3f zAxis(0, 0, 1);
            for (auto brick : m_segments)
            {
                brick->rotateAround(zAxis, angleRad);
            }
            updateOrientation(
                rotateAroundAxis(m_up, zAxis, Vector3f(0, 0, 0), angleRad),
                rotateAroundAxis(m_forward, zAxis, Vector3f(0, 0, 0), angleRad)
            );
        }
    }

    // Rendering options
    void fill() override
    {
        for (auto brick : m_segments)
        {
            brick->fill();
        }
    }

    void outline() override
    {
        for (auto brick : m_segments)
        {
            brick->outline();
        }
    }

    void setColor(const sf::Color& col) override
    {
        for (auto brick : m_segments)
        {
            brick->setColor(col);
        }
    }
};


// NEW //

class WorldSpace;  // Added forward declaration for WorldSpace

class viewWindow {
private:
   
    WorldSpace* world = nullptr;
    vector<Fragment> fragments;

public:

    RenderWindow window;
    Camera cam;

    viewWindow(VideoMode mode, const string& title)
        : window(mode, title) {
    }

    ~viewWindow() {
        detachWorld();
    }


    // Method definitions that WorldSpace will call
    void onObjectAdded(WObject* obj);
    void onObjectRemoved(WObject* obj);

    void attachWorld(WorldSpace* ws);
    void detachWorld();
    void updateFragments();
    void render();
    void handleInput(float moveSpeed);
    void handleRotation(float deltaAngle, float deltaTime);
    void handlePanning();
};

class WorldSpace {
public:
    vector<viewWindow*> views;
    vector<WObject*> objects;

    ~WorldSpace() {

        for (auto view : views) {
            view->detachWorld();
        }
        for (auto obj : objects) {
            delete obj;
        }
    }

    void addObject(WObject* obj) {
        objects.push_back(obj);
        for (auto view : views) {
            view->onObjectAdded(obj);
        }
    }

    void removeObject(WObject* obj) {
        auto it = find(objects.begin(), objects.end(), obj);
        if (it != objects.end()) {
            objects.erase(it);
            for (auto view : views) {
                view->onObjectRemoved(obj);
            }
            delete obj;
        }
    }
};


inline void viewWindow::onObjectAdded(WObject* obj) {
    fragments.reserve(fragments.size() + obj->brickCount());
}

inline void viewWindow::onObjectRemoved(WObject* obj) {

}

void viewWindow::attachWorld(WorldSpace* ws) {
    if (world) detachWorld();
    world = ws;
    ws->views.push_back(this);
    updateFragments();
}

void viewWindow::detachWorld() {
    if (world) {
        auto& views = world->views;
        views.erase(remove(views.begin(), views.end(), this), views.end());
        world = nullptr;
    }
    fragments.clear();
}

void viewWindow::updateFragments() {
    fragments.clear();
    if (!world) return;
    for (auto obj : world->objects) {
        fragments.resize(fragments.size() + obj->brickCount());
    }
}




void viewWindow::render() {
    if (!world) return;

    // Define comparator
    auto compare = [](const DepthOrder& a, const DepthOrder& b) {
        if (a.distance != b.distance) return a.distance < b.distance;
        return a.centerWeight > b.centerWeight;
        };

    // Create AVL tree with explicit comparator type
    using AVLType = AVL<DepthOrder, Fragment*, decltype(compare)>;
    AVLType depthSorted(compare);

    size_t fragIndex = 0;

    for (auto obj : world->objects) {
        for (size_t i = 0; i < obj->brickCount(); ++i) {
            Fragment& frag = fragments[fragIndex++];
            const Brick* brick = obj->getBrick(i);
            Vector3f brickCenter = brick->getCenter();

           /* if (!MathUtils::isInFrontOfDirectedPlane(
                brickCenter, cam.position, cam.forward)) continue;*/

            brick->perspectiveProject(&frag, cam.position, cam.forward,
                cam.up, Vector2f(window.getSize()), cam.focalLength);

            auto order = MathUtils::calculateDepthOrder(
                cam.position,
                cam.forward,
                brickCenter);

            depthSorted.insert(order, &frag);
        }
    }

    window.clear();
    if (!depthSorted.isEmpty()) {

        Stack<typename AVLType::Node*> stack;
        auto* current = depthSorted.root;

        while (current || !stack.isEmpty()) {
            while (current) {
                stack.push(current);
                current = current->right;
            }
            current = stack.pop();
            window.draw(*current->data);
            current = current->left;
        }
    }
    window.display();
}



//
//void viewWindow::render() {
//    if (!world) return;
//
//    using namespace MathUtils;
//
//    
//    AvL<Fragment,float> depthSorted;
//
//    size_t fragIndex = 0;
//
//    for (auto obj : world->objects) 
//    {
//        for (size_t i = 0; i < obj->brickCount(); ++i) 
//        {
//            Fragment& frag = fragments[fragIndex++];
//            const Brick* brick = obj->getBrick(i);
//            Vector3f brickCenter = brick->getCenter();
//
//            if (!MathUtils::isInFrontOfDirectedPlane(
//                brickCenter, cam.position, cam.forward)) continue;
//
//            brick->perspectiveProject(&frag, cam.position, cam.forward,
//                cam.up, Vector2f(window.getSize()), cam.focalLength);
//
//            auto order = MathUtils::calculateDepthOrder(
//                cam.position,
//                cam.forward,
//                brickCenter);
//
//            depthSorted.insert(order, &frag);
//        }
//    }
//
//    window.clear();
//    if (!depthSorted.isEmpty()) {
//
//        Stack<typename AVLType::Node*> stack;
//        auto* current = depthSorted.root;
//
//        while (current || !stack.isEmpty()) {
//            while (current) {
//                stack.push(current);
//                current = current->right;
//            }
//            current = stack.pop();
//            window.draw(*current->data);
//            current = current->left;
//        }
//    }
//    window.display();
//}
//
//

void viewWindow::handleInput(float moveSpeed) 
{
    using namespace MathUtils;
    Vector3f dir = cam.forward;
    Vector3f upr = cam.up;
    Vector3f per = cam.right();

    if (Keyboard::isKeyPressed(Keyboard::W)) {
        cam.position += dir * moveSpeed;
    }
    if (Keyboard::isKeyPressed(Keyboard::S)) {
        cam.position -= dir * moveSpeed;
    }
    if (Keyboard::isKeyPressed(Keyboard::A)) {
        cam.position -= per * moveSpeed;
    }
    if (Keyboard::isKeyPressed(Keyboard::D)) {
        cam.position += per * moveSpeed;
    }
    if (Keyboard::isKeyPressed(Keyboard::Space))
    {
        cam.position += upr * moveSpeed;
    }
    if(Keyboard::isKeyPressed(Keyboard::LControl))
    {
        cam.position -= upr * moveSpeed;
    }
}

void viewWindow::handleRotation(float deltaAngle, float deltaTime) 
{
    if (Keyboard::isKeyPressed(Keyboard::R)) {
        cam.rotateAroundX(deltaAngle, deltaTime);
    }
    if (Keyboard::isKeyPressed(Keyboard::T)) {
        cam.rotateAroundX(-deltaAngle, deltaTime);
    }
    if (Keyboard::isKeyPressed(Keyboard::Y)) {
        cam.rotateAroundY(deltaAngle, deltaTime);
    }
    if (Keyboard::isKeyPressed(Keyboard::U)) {
        cam.rotateAroundY(-deltaAngle, deltaTime);
    }
    if (Keyboard::isKeyPressed(Keyboard::I)) {
        cam.rotateAroundZ(deltaAngle, deltaTime);
    }
    if (Keyboard::isKeyPressed(Keyboard::O)) {
        cam.rotateAroundZ(-deltaAngle, deltaTime);
    }
}

void viewWindow::handlePanning() 
{
    static bool panningMode = false;
    static Vector2i prevMousePos;
    static bool firstFrameInPanning = true;
    static bool tabPreviouslyPressed;

    if (Keyboard::isKeyPressed(Keyboard::Tab)) {
        if (!tabPreviouslyPressed) {
            panningMode = !panningMode;
            firstFrameInPanning = true;
        }
        tabPreviouslyPressed = true;
    }
    else {
        tabPreviouslyPressed = false;
    }

    if (panningMode) {
        Vector2i currentMousePos = Mouse::getPosition(window);
        if (firstFrameInPanning) {
            prevMousePos = currentMousePos;
            firstFrameInPanning = false;
        }
        else {
            Vector2i delta = currentMousePos - prevMousePos;
            prevMousePos = currentMousePos;
            float sensitivity = 1.0f;
            cam.rotateYawPitch(-delta.x * sensitivity, -delta.y * sensitivity);
        }
    }
}



int main() {
    // Create window
    viewWindow window(VideoMode(800, 600), "3D Rendering System");
    window.window.setFramerateLimit(60);

    WorldSpace world;

    // Create objects
    WObject* hexagon = new PolyGon(6);  
    hexagon->fill();
    hexagon->setColor(Color::Red);



    WObject* cubeSide1 = new PolyGon(4,40);   
    cubeSide1->setColor(Color::Green);
    cubeSide1->setPosition({ 40, 0, 40 });

    cubeSide1->fill();

    WObject* cubeSide2 = new PolyGon(4, 40);
    cubeSide2->setColor(Color::Red);
    cubeSide2->setPosition({ 0, 40, 40 });

    cubeSide2->fill();

    WObject* cubeSide3 = new PolyGon(4, 40);
    cubeSide3->setColor(Color::Blue);
    cubeSide3->setPosition({ -40, 0, 40 });
    cubeSide3->fill();
    


    WObject* cubeSide4 = new PolyGon(4, 40);
    cubeSide4->setColor(Color::White);
    cubeSide4->setPosition({ 0, -40, 40 });
    cubeSide4->fill();








    world.addObject(cubeSide1);
    world.addObject(cubeSide2);
    world.addObject(cubeSide3);
    world.addObject(cubeSide4);



    // Add objects to world
    /*world.addObject(hexagon);*/
    

    // Attach world to window
    window.attachWorld(&world);

    float moveSpeed = 19.0f;

    while (window.window.isOpen()) {
        Event event;
        while (window.window.pollEvent(event)) {
            if (event.type == Event::Closed) {
                window.window.close();
            }

            // Zoom with mouse wheel
            if (event.type == Event::MouseWheelScrolled) {
                float zoom = event.mouseWheelScroll.delta * 10.0f;
                window.cam.move({ 0, zoom, 0 });  
            }
        }


        //cout << "HEXAGON : " << calculateDepthOrder(window.cam.position, window.cam.forward, hexagon->getPosition());
        //cout << " ! ";
        //cout << "SQUARE : " << calculateDepthOrder(window.cam.position, window.cam.forward, square->getPosition());

        // Handle input


        window.handleInput(moveSpeed);
        /*window.handleRotation(4.0f, 1.0f); */
        cubeSide1->handleRotation(4.0f, 1.0f);
        cubeSide2->handleRotation(4.0f, 1.0f);
        cubeSide3->handleRotation(4.0f, 1.0f);
        cubeSide4->handleRotation(4.0f, 1.0f);

        window.handlePanning();

        // Render frame

        window.render();

        cout << endl;

    }

    return 0;
}