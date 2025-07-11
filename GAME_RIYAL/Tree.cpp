//
//#include<iostream>
//#include <vector>
//#include <memory>
//#include <cassert>
//#include <cmath>
//#include <SFML/Graphics.hpp>
//#include "CDS.h"
//
//using namespace sf;
//using namespace std;
//
//const float PI = 3.14159265358979323846f;
//
//
//
//// ==================== UTILITY FUNCTIONS ====================
//namespace MathUtils {
//    Vector3f cross(const Vector3f& a, const Vector3f& b) {
//        return {
//            a.y * b.z - a.z * b.y,
//            a.z * b.x - a.x * b.z,
//            a.x * b.y - a.y * b.x
//        };
//    }
//
//    float dot(const Vector2f& a, const Vector2f& b) {
//        return a.x * b.x + a.y * b.y;
//    }
//
//    float dot(const Vector3f& a, const Vector3f& b) {
//        return a.x * b.x + a.y * b.y + a.z * b.z;
//    }
//
//
//    float length(const Vector3f& v) {
//        return std::sqrt(v.x * v.x + v.y * v.y + v.z * v.z);
//    }
//
//
//    inline float degreesToRadians(float degrees) {
//        return degrees * (3.14159265f / 180.0f);
//    }
//
//    Vector3f normalize(const Vector3f& v) {
//        float mag = sqrt(dot(v, v));
//        return (mag == 0) ? Vector3f(0, 0, 0) : v / mag;
//    }
//
//    Vector3f projectPointToPlane(const Vector3f& P, const Vector3f& A,
//        const Vector3f& B, const Vector3f& D) {
//        Vector3f U = B - A;
//        Vector3f V = D - A;
//        Vector3f N = normalize(cross(U, V));
//        return P - dot(P - A, N) * N;
//    }
//
//    Vector2f toScreen(const Vector2f& logical, const Vector2u& windowSize) {
//        return {
//            logical.x + windowSize.x / 2.f,
//            -logical.y + windowSize.y / 2.f
//        };
//    }
//
//
//    Vector3f projectOntoPlane(
//        const Vector3f& point,
//        const Vector3f& A,
//        const Vector3f& B,
//        const Vector3f& C
//    ) {
//        Vector3f v1 = B - A;
//        Vector3f v2 = C - A;
//        Vector3f normal = cross(v1, v2);
//        Vector3f AP = point - A;
//        float distance = dot(AP, normal) / dot(normal, normal);
//        return point - normal * distance;
//    }
//
//    Vector2f toPlane2DCoordinates(
//        const Vector3f& pointOnPlane,
//        const Vector3f& origin,
//        const Vector3f& dir1,
//        const Vector3f& dir2
//    ) {
//        Vector3f w = pointOnPlane - origin;
//
//        float d11 = dot(dir1, dir1);
//        float d12 = dot(dir1, dir2);
//        float d22 = dot(dir2, dir2);
//        float dw1 = dot(w, dir1);
//        float dw2 = dot(w, dir2);
//
//        float denom = d11 * d22 - d12 * d12;
//        if (std::abs(denom) < 1e-6f) {
//            return { 0.f, 0.f };
//        }
//
//        float a = (dw1 * d22 - dw2 * d12) / denom;
//        float b = (dw2 * d11 - dw1 * d12) / denom;
//        return { a, b };
//    }
//
//    Vector2f projectPointToPlane2D(
//        const Vector3f& point,
//        const Vector3f& A,
//        const Vector3f& B,
//        const Vector3f& C,
//        const Vector3f& origin
//    ) {
//        Vector3f projected = projectOntoPlane(point, A, B, C);
//        Vector3f dir1 = A - origin;
//        Vector3f dir2 = B - origin;
//        return toPlane2DCoordinates(projected, origin, dir1, dir2);
//    }
//
//
//
//
//
//
//
//
//}
//
//
//
//
//class Camera {
//public:
//    Vector3f position;
//    Vector3f forward;
//    Vector3f up;
//
//    float scale;
//    float focalLength;
//
//
//
//    Camera(Vector3f pos = { 0, 0, -500 }, Vector3f lookAt = { 0, 0, 0 }, Vector3f worldUp = { 0, 1, 0 }, float scale = 1.f, float focalLength = 500.f) : scale(scale), focalLength(focalLength)
//    {
//        using namespace MathUtils;
//        position = pos;
//        forward = normalize(lookAt - pos);
//        Vector3f right = normalize(cross(forward, worldUp));
//        up = cross(right, forward);
//    }
//
//    Vector3f getCenter() const { return position; }
//
//    Vector3f right() const
//    {
//        using namespace MathUtils;
//        return normalize(cross(forward, up));
//    }
//
//    void move(const Vector3f& delta) {
//        position += delta;
//    }
//
//    Vector3f rotateAroundAxis(const Vector3f& P, const Vector3f& axis, const Vector3f& center, float angleRad) const {
//        using namespace MathUtils;
//
//        Vector3f r = P - center;
//        float cosA = cos(angleRad);
//        float sinA = sin(angleRad);
//        float dotAR = dot(axis, r);
//
//        Vector3f rotated =
//            r * cosA +
//            cross(axis, r) * sinA +
//            axis * dotAR * (1 - cosA);
//
//        return center + rotated;
//    }
//
//    void rotateAround(const Vector3f& axis, float angleRad) {
//        using namespace MathUtils;
//
//        Vector3f origin = Vector3f(0.f, 0.f, 0.f);
//
//        forward = normalize(rotateAroundAxis(forward, axis, origin, angleRad));
//        up = normalize(rotateAroundAxis(up, axis, origin, angleRad));
//    }
//
//    void rotateAroundA(Vector3f worldUp, float deltaAngleDegrees, float deltaTime) {
//        using namespace MathUtils;
//
//        Vector3f axis = worldUp - forward * dot(worldUp, forward);
//        if (length(axis) < 0.001f) {
//            worldUp = Vector3f(1.0f, 0.0f, 0.0f);
//            axis = worldUp - forward * dot(worldUp, forward);
//        }
//
//        axis = normalize(axis);
//        float angleRad = degreesToRadians(deltaAngleDegrees) * deltaTime;
//        rotateAround(axis, angleRad);
//    }
//
//    void rotateAroundY(float deltaAngleDegrees, float deltaTime) {
//        Vector3f Up = Vector3f(0.0f, 1.0f, 0.0f);
//        rotateAroundA(Up, deltaAngleDegrees, deltaTime);
//    }
//
//    void rotateAroundX(float deltaAngleDegrees, float deltaTime) {
//        Vector3f Up = Vector3f(1.0f, 0.0f, 0.0f);
//        rotateAroundA(Up, deltaAngleDegrees, deltaTime);
//    }
//
//    void rotateAroundZ(float deltaAngleDegrees, float deltaTime) {
//        Vector3f Up = Vector3f(0.0f, 0.0f, 1.0f);
//        rotateAroundA(Up, deltaAngleDegrees, deltaTime);
//    }
//
//    void rotateInPlane(float deltaAngleDegrees, float deltaTime) {
//        using namespace MathUtils;
//
//        float angleRad = degreesToRadians(deltaAngleDegrees) * deltaTime;
//        Vector3f axis = forward;
//
//        up = normalize(rotateAroundAxis(up, axis, Vector3f(0.f, 0.f, 0.f), angleRad));
//    }
//
//
//    void rotateYawPitch(float deltaYawDeg, float deltaPitchDeg)
//    {
//        using namespace MathUtils;
//
//
//        float yawRad = degreesToRadians(deltaYawDeg);
//        float pitchRad = degreesToRadians(deltaPitchDeg);
//
//
//        Vector3f right = normalize(cross(forward, up));
//
//
//        forward = normalize(rotateAroundAxis(forward, { 0.f, 1.f, 0.f }, { 0.f, 0.f, 0.f }, yawRad));
//
//
//        right = normalize(cross(forward, { 0.f, 1.f, 0.f }));
//
//        forward = normalize(rotateAroundAxis(forward, right, { 0.f, 0.f, 0.f }, pitchRad));
//        up = normalize(cross(right, forward));
//    }
//};
//
//
//
//namespace MathUtils
//{
//    // USING
//
//    Vector2f projectToPerspective2D(const Vector3f& point, Vector3f position, Vector3f forward, Vector3f up, float focalLength = 500.0f) {
//        using namespace MathUtils;
//
//        Vector3f relative = point - position;
//
//        Vector3f right = normalize(cross(forward, up));
//        Vector3f Up = normalize(up);
//
//        float x = dot(relative, right);
//        float y = dot(relative, up);
//        float z = dot(relative, forward);
//
//        if (z <= 0.01f) z = 0.01f;
//
//        return Vector2f(x * (focalLength / z), y * (focalLength / z));
//    }
//
//
//    Vector2f projectToScreen2D(const Vector3f& point, Vector3f centre, Vector3f up, Vector3f right)
//    {
//        using namespace MathUtils;
//
//        Vector3f toPoint = point - centre;
//
//        float x = dot(toPoint, right);
//        float y = dot(toPoint, up);
//
//        return Vector2f(x, y);
//    }
//
//    float distanceToPlane(const Vector3f& targetPoint,
//        const Vector3f& planePoint,
//        const Vector3f& planeNormal)
//    {
//
//        float normalLength = sqrt(planeNormal.x * planeNormal.x +
//            planeNormal.y * planeNormal.y +
//            planeNormal.z * planeNormal.z);
//
//        if (normalLength == 0) {
//            throw invalid_argument("Plane normal cannot be a zero vector");
//        }
//
//        sf::Vector3f unitNormal = planeNormal / normalLength;
//
//        float distance = unitNormal.x * (targetPoint.x - planePoint.x) +
//            unitNormal.y * (targetPoint.y - planePoint.y) +
//            unitNormal.z * (targetPoint.z - planePoint.z);
//
//
//        return std::abs(distance);
//    }
//
//
//    bool isInFrontOfDirectedPlane(Vector3f point, Vector3f position, Vector3f forward)
//    {
//        using namespace MathUtils;
//
//        Vector3f toPolygon = point - position;
//
//        float dotResult = dot(toPolygon, forward);
//
//        return dotResult > 0;
//    }
//}
//
//
//Camera* sc;
//
//class VObject;
//
//// ==================== FRAGMENT CLASS ====================
//class Fragment : public Drawable {
//public:
//    // Member variables
//    VertexArray shape;
//    bool filled = false;
//    sf::Color clr;
//
//    vector<Vector2f> points;
//
//    VObject* subject;
//
//    // Constructors
//    explicit Fragment(size_t pointCount = 0) :
//        points(pointCount),
//        shape(filled ? TriangleFan : LineStrip, pointCount + (filled ? 0 : 1))
//    {
//
//        clr = Color(255, 255, 255);
//    }
//
//
//    Fragment(const Fragment& other) :
//        filled(other.filled),
//        points(other.points),
//        shape(other.shape)
//    {
//
//        updateShape();
//    }
//
//    // Public interface
//    Vector2f getCenter() const {
//        if (points.empty()) return Vector2f(0.f, 0.f);
//
//        Vector2f sum(0.f, 0.f);
//        for (const auto& p : points) {
//            sum += p;
//        }
//        return sum / static_cast<float>(points.size());
//    }
//
//    void setPoint(size_t index, const Vector2f& point) {
//        assert(index < points.size());
//        points[index] = point;
//        updateShape();
//    }
//
//    void changeEdges(int n) {
//        if (n < 3) return;
//
//        points.resize(n);
//        rounded();
//        updateShape();
//    }
//
//    void rounded() {
//        const float PI = 3.14159265f;
//        const unsigned edges = points.size();
//        if (edges < 3) return;
//
//        const float theta = 2 * PI / edges;
//        const float radius = 250.0f;
//        const Vector2f center = getCenter();
//
//        for (unsigned i = 0; i < edges; ++i) {
//            const float angle = i * theta;
//            points[i] = center + Vector2f(
//                radius * cos(angle),
//                radius * sin(angle)
//            );
//        }
//
//        updateShape();
//    }
//
//    void fill() {
//        filled = true;
//        shape.setPrimitiveType(TriangleFan);
//        updateShape();
//    }
//
//    void outline() {
//        filled = false;
//        shape.setPrimitiveType(LineStrip);
//        updateShape();
//    }
//
//    void move(const Vector2f& delta) {
//        for (auto& v : points) {
//            v += delta;
//        }
//        updateShape();
//    }
//
//    void rotate(float deltaAngleDegrees, float deltaTime = 1.0f) {
//        if (points.empty()) return;
//
//        const float angleRad = deltaAngleDegrees * (3.14159265f / 180.0f) * deltaTime;
//        const float cosA = cos(angleRad);
//        const float sinA = sin(angleRad);
//        const Vector2f center = getCenter();
//
//        for (auto& v : points) {
//            const float x = v.x - center.x;
//            const float y = v.y - center.y;
//            v.x = center.x + x * cosA - y * sinA;
//            v.y = center.y + x * sinA + y * cosA;
//        }
//
//        updateShape();
//    }
//
//    void draw(RenderTarget& target, RenderStates states) const override {
//        target.draw(shape, states);
//    }
//
//    // Helper methods
//    void updateShape() {
//        shape.clear();
//        for (const auto& point : points) {
//            shape.append({ point, clr });
//        }
//
//        // Close the loop if not filled
//        if (!filled && !points.empty()) {
//            shape.append({ points[0], clr });
//        }
//    }
//};
//
//
//// ==================== VOBJECT HIERARCHY ====================
//class VObject {
//public:
//    Vector3f up;
//    virtual ~VObject() = default;
//
//    virtual Vector3f getPosition() = 0;
//
//    virtual void prespectiveProject(Fragment* frag, Vector3f position, Vector3f forward, Vector3f up, Vector2f winSize, float focalLength = 500.f) = 0;
//    virtual void orthogonalProject(Fragment* frag, Vector3f centre, Vector3f up, Vector3f right, Vector2f winSize, float scale = 1.f) = 0;
//
//
//    virtual void move(const Vector3f& delta) = 0;
//    virtual void rotateAround(const Vector3f& axis, float angleRad) = 0;
//    virtual void rotateAroundA(Vector3f worldUp, float deltaAngleDegrees, float deltaTime) = 0;
//    virtual void rotateAroundY(float deltaAngleDegrees, float deltaTime) = 0;
//    virtual void rotateAroundX(float deltaAngleDegrees, float deltaTime) = 0;
//    virtual void rotateAroundZ(float deltaAngleDegrees, float deltaTime) = 0;
//    virtual void rotateInPlane(float deltaAngleDegrees, float deltaTime) = 0;
//    virtual void fill() = 0;
//    virtual void outline() = 0;
//    virtual void setColor(const sf::Color& col) = 0;
//    virtual void setPosition(const Vector3f& newPos) = 0;
//
//    void handleInput(float moveSpeed)
//    {
//
//        if (Keyboard::isKeyPressed(Keyboard::W))
//        {
//            this->move({ 0, -moveSpeed, 0 });
//
//
//
//        }
//        if (Keyboard::isKeyPressed(Keyboard::S))
//        {
//            this->move({ 0, moveSpeed, 0 });
//
//
//        }
//
//        if (Keyboard::isKeyPressed(Keyboard::A))
//        {
//            this->move({ -moveSpeed, 0, 0 });
//
//
//        }
//        if (Keyboard::isKeyPressed(Keyboard::D))
//        {
//            this->move({ moveSpeed, 0, 0 });
//
//
//        }
//
//
//
//
//
//    }
//
//    void handleRotation(float deltaAngle, float deltaTime)
//    {
//        if (Keyboard::isKeyPressed(Keyboard::R))
//        {
//            this->rotateAroundX(deltaAngle, deltaTime);
//
//
//
//        }
//
//
//        if (Keyboard::isKeyPressed(Keyboard::T))
//        {
//            this->rotateAroundX(-deltaAngle, deltaTime);
//
//
//        }
//
//
//
//        if (Keyboard::isKeyPressed(Keyboard::Y))
//        {
//            this->rotateAroundY(deltaAngle, deltaTime);
//
//
//
//        }
//
//
//        if (Keyboard::isKeyPressed(Keyboard::U))
//        {
//            this->rotateAroundY(-deltaAngle, deltaTime);
//
//
//        }
//
//
//
//        if (Keyboard::isKeyPressed(Keyboard::I))
//        {
//            this->rotateAroundZ(deltaAngle, deltaTime);
//
//
//
//        }
//
//
//        if (Keyboard::isKeyPressed(Keyboard::O))
//        {
//            this->rotateAroundZ(-deltaAngle, deltaTime);
//
//
//        }
//
//    }
//
//
//};
//
//
//class wsNode 
//{
//public:
//
//    float radius;
//    float outlineThickness;
//    Color fillColor;
//    bool filled;
//    string label;
//    Vector3f position;
//
//
//    
//
//    wsNode(Vector3f pos = {0,0,0},float r = 10.0f, float outlineT = 2.0f, const Color & color = Color::Green, bool fill = 1)
//        : radius(r), outlineThickness(outlineT), fillColor(color) ,filled(fill)
//    {
//        label = "";
//    }
//
//    void setLabel(const string& text) 
//    {
//        label = text;
//    }
//
//        
//
//    
//};
//
//class wsGraph
//{
//public:
//
//    vector<vector<wsNode*>*> adjList;
//    
//
//    void createNode(Vector3f pos = { 0,0,0 }, float r = 10.0f, float outlineT = 2.0f, const Color& color = Color::Green, bool fill = 1)
//    {
//        
//
//
//        
//    }
//
//};
//
//
//
//
//void display3DVector(Vector3f v)
//{
//    cout << "(" << v.x << ", " << v.y << ", " << v.z << ")   ";
//}
//
//void display2DVector(Vector2f v)
//{
//    cout << "(" << v.x << ", " << v.y << ")   ";
//}
//
//
////
////void createOrthogonalFragment(Fragment& fg, Camera* sc, VObject* po, RenderWindow* window) 
////{
////    using namespace MathUtils;
////    float scale = 1.0f;
////
////    Polygon3D* pol = static_cast<Polygon3D*>(po);
////    fg.changeEdges(pol->points.size());
////
////    Vector2f screenCenter = Vector2f(window->getSize()) * 0.5f;
////
////    for (int i = 0; i < fg.points.size(); i++) {
////        Vector2f local = projectToScreen2D(pol->points[i], sc->getCenter(),sc->up,sc->right());
////        Vector2f px = screenCenter + Vector2f(local.x, -local.y) * scale;
////        fg.setPoint(i, px);
////        fg.updateShape();
////    }
////}
//
//
////void createPerspectiveFragment(Fragment& fg, Camera* sc, VObject* po, RenderWindow* window) 
////{
////    using namespace MathUtils;
////
////    Polygon3D* pol = static_cast<Polygon3D*>(po);
////    fg.changeEdges(pol->points.size());
////
////    Vector2f screenCenter = Vector2f(window->getSize()) * 0.5f;
////    float focalLength = 500.0f;
////
////    for (int i = 0; i < fg.points.size(); i++) {
////        Vector2f local = projectToPerspective2D(pol->points[i], *sc, focalLength);
////        Vector2f px = screenCenter + Vector2f(local.x, -local.y);
////        fg.setPoint(i, px);
////    }
////
////    fg.updateShape();
////}
//
//
//
//
//class viewWindow;
//
//class WorldSpace
//{
//public:
//
//    vector<viewWindow*> observerWindows;
//    vector<VObject*> objects;
//
//
//    void addObject(VObject* obj)
//    {
//        objects.push_back(obj);
//    }
//
//
//
//
//
//
//
//
//
//
//
//
//};
//
//class viewWindow
//{
//public:
//
//    RenderWindow* window;
//    Camera* cam;
//
//    WorldSpace* worldSpace;
//
//    vector<Fragment*> fragments;
//
//public:
//
//
//    viewWindow(VideoMode vm, string name)
//    {
//        window = new RenderWindow(vm, name);
//
//        cam = new Camera;
//
//        worldSpace = nullptr;
//
//    }
//
//    void attachWorldSpace(WorldSpace* ws)
//    {
//        worldSpace = ws;
//
//        ws->observerWindows.push_back(this);
//
//        for (auto& obj : ws->objects)
//        {
//            Fragment* frag = new Fragment;
//            frag->subject = obj;
//
//
//
//            fragments.push_back(frag);
//        }
//    }
//
//    void render()
//    {
//        using namespace MathUtils;
//
//        if (!worldSpace) return;
//
//        AVL<float, Fragment*> tree;
//
//        int i = 1;
//
//        for (auto& frag : fragments) {
//            frag->subject->prespectiveProject(frag, cam->position, cam->forward,
//                cam->up, Vector2f(window->getSize()),
//                cam->focalLength);
//
//
//
//            if (isInFrontOfDirectedPlane(frag->subject->getPosition(),
//                cam->position, cam->forward)) {
//
//                float distance = distanceToPlane(frag->subject->getPosition(),
//                    cam->position, cam->forward);
//
//
//
//
//                tree.insert(distance, frag);
//            }
//
//
//        }
//
//
//
//        if (!tree.isEmpty()) {
//
//            Stack<typename AVL<float, Fragment*>::Node*> nodeStack;
//
//            typename AVL<float, Fragment*>::Node* current = tree.root;
//
//            while (current || !nodeStack.isEmpty()) {
//                while (current) {
//                    nodeStack.push(current);
//                    current = current->right;
//                }
//
//
//
//                current = nodeStack.pop();
//
//
//
//                window->draw(*(current->data));
//                current = current->left;
//            }
//        }
//
//    }
//
//
//
//
//
//
//
//
//
//    void handleInput(float moveSpeed)
//    {
//
//        using namespace MathUtils;
//
//        Vector3f dir = cam->up;
//        Vector3f per = cam->right();
//
//        if (Keyboard::isKeyPressed(Keyboard::W))
//        {
//
//
//            cam->position += dir * moveSpeed;
//
//
//
//
//
//        }
//        if (Keyboard::isKeyPressed(Keyboard::S))
//        {
//            cam->position -= dir * moveSpeed;
//
//
//        }
//
//        if (Keyboard::isKeyPressed(Keyboard::A))
//        {
//            cam->position -= per * moveSpeed;
//
//
//        }
//        if (Keyboard::isKeyPressed(Keyboard::D))
//        {
//            cam->position += per * moveSpeed;
//
//
//        }
//
//
//
//
//
//    }
//
//    private:
//
//    void handleRotation(float deltaAngle, float deltaTime)
//    {
//        if (Keyboard::isKeyPressed(Keyboard::R))
//        {
//            this->cam->rotateAroundX(deltaAngle, deltaTime);
//
//
//
//        }
//
//
//        if (Keyboard::isKeyPressed(Keyboard::T))
//        {
//            this->cam->rotateAroundX(-deltaAngle, deltaTime);
//
//
//        }
//
//
//
//        if (Keyboard::isKeyPressed(Keyboard::Y))
//        {
//            this->cam->rotateAroundY(deltaAngle, deltaTime);
//
//
//
//        }
//
//
//        if (Keyboard::isKeyPressed(Keyboard::U))
//        {
//            this->cam->rotateAroundY(-deltaAngle, deltaTime);
//
//
//        }
//
//
//
//        if (Keyboard::isKeyPressed(Keyboard::I))
//        {
//            this->cam->rotateAroundZ(deltaAngle, deltaTime);
//
//
//
//        }
//
//
//        if (Keyboard::isKeyPressed(Keyboard::O))
//        {
//            this->cam->rotateAroundZ(-deltaAngle, deltaTime);
//
//
//        }
//
//    }
//
//
//    void clear() {
//        for (auto frag : fragments) delete frag;
//        fragments.clear();
//    }
//};
//
//
//
//int main()
//{
//
//    // Create window
//    viewWindow window(VideoMode(800, 600), "HELLO");
//
//    window.window->setFramerateLimit(60);
//
//    WorldSpace ws;
//
//
//    window.attachWorldSpace(&ws);
//
//
//
//
//    float moveSpeed = 19.f;
//
//
//    while (window.window->isOpen())
//    {
//
//        Event event;
//
//        while (window.window->pollEvent(event)) {
//            if (event.type == Event::Closed)
//            {
//                window.window->close();
//                /*windov.window->close();*/
//            }
//
//
//
//            // Zoom with mouse wheel
//            if (event.type == Event::MouseWheelScrolled) {
//                float zoom = event.mouseWheelScroll.delta * 10.0f;
//                window.cam->move({ 0, zoom,  0 });
//                /* pov->move({ 0, 0, zoom });*/
//            }
//
//
//
//
//
//        }
//
//
//
//        static bool firstClick = true;
//        static Vector2i prevMousePos;
//
//
//
//        // === Mouse Look Code ===
//        if (Mouse::isButtonPressed(Mouse::Right))
//        {
//
//            Vector2i currentMousePos = Mouse::getPosition(*(window.window));
//
//
//
//            if (firstClick) {
//                prevMousePos = currentMousePos;
//                firstClick = false;
//            }
//            else {
//                Vector2i delta = currentMousePos - prevMousePos;
//                prevMousePos = currentMousePos;
//
//                float sensitivity = 1.0f;
//                float yaw = delta.x * sensitivity;
//                float pitch = delta.y * sensitivity;
//
//                window.cam->rotateYawPitch(yaw, pitch);
//            }
//        }
//        else {
//            firstClick = true;
//        }
//
//
//
//        window.handleInput(moveSpeed);
//
//
//
//
//
//
//
//
//
//        window.window->clear();
//
//        window.render();
//
//
//        window.window->display();
//
//
//        cout << endl;
//
//    }
//
//
//
//    return 0;
//}
