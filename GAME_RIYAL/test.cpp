//#include <vector>
//#include <memory>
//#include <cassert>
//#include <cmath>
//#include <SFML/Graphics.hpp>
//
//using namespace sf;
//using namespace std;
//
//const float PI = 3.14159265358979323846f;
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
//}
//
////// ?????
//
//
//
//// ==================== SCREEN CLASS ====================
//
//
//class Screen {
//public:
//    Vector3f vertices[4];
//
//    Screen(Vector2u winSize) {
//        Vector2i size = static_cast<Vector2i>(winSize);
//        vertices[0] = Vector3f(-size.x / 2, size.y / 2, 0);
//        vertices[1] = Vector3f(size.x / 2, size.y / 2, 0);
//        vertices[2] = Vector3f(-size.x / 2, -size.y / 2, 0);
//        vertices[3] = Vector3f(size.x / 2, -size.y / 2, 0);
//    }
//
//    Vector3f getCenter() const {
//        return (vertices[0] + vertices[1] + vertices[2] + vertices[3]) * 0.25f;
//    }
//
//    void move(const Vector3f& delta) {
//        for (auto& v : vertices) {
//            v += delta;
//        }
//    }
//
//    void rotate(const Vector3f& delta) {
//        if (delta.x == 0 && delta.y == 0) return;
//
//        Vector2f dir = { delta.x, delta.y };
//        float mag = sqrt(MathUtils::dot(dir, dir));
//        dir /= mag;
//
//        float angle = atan2(dir.y, dir.x);
//        Vector3f center = getCenter();
//        float cosA = cos(angle);
//        float sinA = sin(angle);
//
//        for (auto& v : vertices) {
//            float x = v.x - center.x;
//            float y = v.y - center.y;
//            v.x = center.x + x * cosA - y * sinA;
//            v.y = center.y + x * sinA + y * cosA;
//        }
//    }
//};
//
//class Fragment;
//class VObject;
//
//// ==================== FRAGMENT CLASS ====================
//class Fragment : public Drawable {
//
//
//
//
//
//
//public:
//    VertexArray shape;
//    bool filled = false;
//    VObject* Subject = nullptr;
//    vector<Vector2f> points;
//
//
//
//    void updateShape() {
//        shape.clear();
//        for (size_t i = 0; i < points.size(); ++i) {
//            shape.append({ points[i], Color::White });
//        }
//        if (!filled && !points.empty()) {
//            shape.append({ points[0], Color::White });
//        }
//    }
//
//    explicit Fragment(size_t pointCount = 0) :
//        points(pointCount),
//        shape(filled ? TriangleFan : LineStrip, pointCount + (filled ? 0 : 1))
//    {
//    }
//
//    void setPoint(size_t index, const Vector2f& point) {
//        assert(index < points.size());
//        points[index] = point;
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
//    void reFresh(Screen* scr, RenderWindow* win);
//
//    void draw(RenderTarget& target, RenderStates states) const override {
//        target.draw(shape, states);
//    }
//};
//
//// ==================== VOBJECT HIERARCHY ====================
//class VObject {
//public:
//    Fragment* Observer = nullptr;
//    virtual ~VObject() = default;
//    virtual void project(Fragment* frag, Screen* scr, RenderWindow* win) = 0;
//};
//
//class Polygon : public VObject {
//    vector<Vector3f> points;
//    bool filled = false;
//
//    Vector2f projectToRectCoords(const Vector3f& P, const Vector3f R[4]) {
//        using namespace MathUtils;
//
//        Vector3f center = projectPointToPlane({ 0,0,0 }, R[0], R[1], R[2]);
//        Vector3f U = normalize(R[1] - R[0]);
//        Vector3f V = normalize(R[2] - R[0]);
//        V = normalize(V - dot(V, U) * U);
//        Vector3f N = normalize(cross(U, V));
//
//        Vector3f P_proj = P - dot(P - center, N) * N;
//        Vector3f offset = P_proj - center;
//        return { dot(offset, U), dot(offset, V) };
//    }
//
//    void rounded() {
//        unsigned edges = points.size();
//        float theta = 2 * PI / edges;
//        float radius = 250.0f;
//
//        for (unsigned i = 0; i < edges; ++i) {
//            float angle = i * theta;
//            setPoint(i, Vector3f(
//                radius * cos(angle),
//                radius * sin(angle),
//                0.0f
//            ));
//        }
//    }
//
//public:
//    explicit Polygon(size_t pointCount) : points(pointCount) {
//        rounded();
//    }
//
//    void setPoint(size_t index, const Vector3f& point) {
//        assert(index < points.size());
//        points[index] = point;
//    }
//
//    void move(const Vector3f& delta) {
//        for (auto& v : points) {
//            v += delta;
//        }
//        if (Observer) Observer->reFresh(nullptr, nullptr);
//    }
//
//    void fill() {
//        filled = true;
//        if (Observer) Observer->fill();
//    }
//
//    void outline() {
//        filled = false;
//        if (Observer) Observer->outline();
//    }
//
//    void project(Fragment* frag, Screen* scr, RenderWindow* win) override {
//        if (!frag || !scr || !win) return;
//
//        frag->Subject = this;
//        Observer = frag;
//
//        frag->points.resize(points.size()); // Make sure it's safe to access [i]
//
//        for (size_t i = 0; i < points.size(); i++) {
//            Vector2f v = projectToRectCoords(points[i], scr->vertices);
//            v = MathUtils::toScreen(v, win->getSize());
//            frag->setPoint(i, v);
//        }
//
//        if (filled) frag->fill();
//        else frag->outline();
//    }
//};
//
//void Fragment::reFresh(Screen* scr, RenderWindow* win)
//{
//    if (Subject) {
//        Subject->project(this, scr, win);
//    }
//}
//
//
//// ==================== PLANE (RENDERER) ====================
//class Plane {
//    RenderWindow* window;
//    Screen* screen;
//    vector<Fragment*> fragments;
//
//    void makeScreen() {
//        screen = new Screen(window->getSize());
//    }
//
//public:
//    Plane(RenderWindow* win) : window(win) {
//        makeScreen();
//    }
//
//    ~Plane() {
//        for (auto frag : fragments) delete frag;
//        delete screen;
//    }
//
//    void add(VObject* obj) {
//        Fragment* frag = new Fragment(obj->Observer ? obj->Observer->points.size() : 0);
//        obj->project(frag, screen, window);
//        fragments.push_back(frag);
//    }
//
//    void render() {
//        for (auto fragment : fragments) {
//            window->draw(*fragment);
//        }
//    }
//
//    void reFreshAllFragments() {
//        for (auto frag : fragments) {
//            frag->reFresh(screen, window);
//        }
//    }
//
//    void move(Vector3f delta) {
//        screen->move(delta);
//        reFreshAllFragments();
//    }
//
//    void rotate(Vector3f delta) {
//        screen->rotate(delta);
//        reFreshAllFragments();
//    }
//
//    void clear() {
//        for (auto frag : fragments) delete frag;
//        fragments.clear();
//    }
//};
//
//
//int main()
//{
//
//    // Create window
//    RenderWindow window(VideoMode(800, 600), "3D Projection Test");
//    window.setFramerateLimit(60);
//
//
//    Plane camera(&window);
//
//    // Create test pentagon
//    ::Polygon pentagon(5);
//    pentagon.setPoint(0, { 500.0f, 300.0f, 0.0f });
//    pentagon.setPoint(1, { 430.9f, 395.1f, 0.0f });
//    pentagon.setPoint(2, { 319.1f, 358.8f, 0.0f });
//    pentagon.setPoint(3, { 319.1f, 241.2f, 0.0f });
//    pentagon.setPoint(4, { 430.9f, 204.9f, 0.0f });
//
//    // Create test hexagon (using rounded function)
//    ::Polygon hexagon(6);
//    hexagon.move({ -200, 0, 0 }); // Position left of pentagon
//
//    // Add shapes to camera
//    camera.add(&pentagon);
//    camera.add(&hexagon);
//
//    // Control variables
//    bool isFilled = false;
//    Vector2i mousePrevPos = Mouse::getPosition(window);
//    Clock clock;
//    float moveSpeed = 0.5f;
//
//    // Main loop
//    while (window.isOpen()) {
//        Event event;
//        while (window.pollEvent(event)) {
//            if (event.type == Event::Closed)
//                window.close();
//
//            // Toggle fill mode with Space
//            if (event.type == Event::KeyPressed && event.key.code == Keyboard::Space)
//            {
//                isFilled = !isFilled;
//                if (isFilled) {
//                    pentagon.fill();
//                    hexagon.fill();
//                }
//                else {
//                    pentagon.outline();
//                    hexagon.outline();
//                }
//            }
//
//            // Zoom with mouse wheel
//            if (event.type == Event::MouseWheelScrolled) {
//                float zoom = event.mouseWheelScroll.delta * 10.0f;
//                camera.move({ 0, 0, zoom });
//            }
//        }
//
//        // Handle mouse movement for object dragging
//        Vector2i mouseCurrentPos = Mouse::getPosition(window);
//        Vector2i mouseDelta = mouseCurrentPos - mousePrevPos;
//        mousePrevPos = mouseCurrentPos;
//
//        if (Mouse::isButtonPressed(Mouse::Left)) {
//            pentagon.move({ static_cast<float>(mouseDelta.x),
//                          static_cast<float>(mouseDelta.y),
//                          0.0f });
//        }
//
//        // Handle camera movement with WASD
//        if (Keyboard::isKeyPressed(Keyboard::W)) camera.move({ 0, -moveSpeed, 0 });
//        if (Keyboard::isKeyPressed(Keyboard::S)) camera.move({ 0, moveSpeed, 0 });
//        if (Keyboard::isKeyPressed(Keyboard::A)) camera.move({ -moveSpeed, 0, 0 });
//        if (Keyboard::isKeyPressed(Keyboard::D)) camera.move({ moveSpeed, 0, 0 });
//
//        // Handle screen rotation with Q/E
//        if (Keyboard::isKeyPressed(Keyboard::Q)) camera.rotate({ -1, 0, 0 });
//        if (Keyboard::isKeyPressed(Keyboard::E)) camera.rotate({ 1, 0, 0 });
//
//        // Clear and render
//        window.clear(Color::Black);
//        camera.render();
//        window.display();
//    }
//
//
//
//
//    return 0;
//}
