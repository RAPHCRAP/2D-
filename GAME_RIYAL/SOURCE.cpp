//
//#include <vector>
//#include <memory>
//#include <cassert>
//#include <cmath>
//#include <SFML/Graphics.hpp>
//
//using namespace sf;
//using namespace std;
//
//
//
//const double PI = 3.14159265358979323846f;
//
//using Vector3f = sf::Vector3f;
//using Vector2f = sf::Vector2f;
//using Vector2u = sf::Vector2u;
//using RenderWindow = sf::RenderWindow;
//using VertexArray = sf::VertexArray;
//
//// ==================== UTILITY FUNCTIONS ====================
//
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
//        float mag = std::sqrt(dot(v, v));
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
//// ==================== SCREEN CLASS ====================
//
//class Screen {
//public:
//    Vector3f vertices[4];
//
//    Screen(Vector2u winSize) {
//
//        Vector2i size = (Vector2i)winSize;
//
//        vertices[0] = Vector3f(-size.x/2, size.y / 2, 0);
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
//    void rotate(const Vector3f& delta) 
//    {
//        if (delta.x == 0 && delta.y == 0) return;
//
//        Vector2f dir = { delta.x, delta.y };
//        float mag = std::sqrt(MathUtils::dot(dir, dir));
//        dir /= mag;
//
//        float angle = std::atan2(dir.y, dir.x);
//        Vector3f center = getCenter();
//        float cosA = std::cos(angle);
//        float sinA = std::sin(angle);
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
//
//
//// ==================== VOBJECT HIERARCHY ====================
//
//class VObject {
//public:
//
//    Fragment* Observer;
//    virtual ~VObject() = default;
//    virtual void project(Fragment* frag ,Screen* scr, RenderWindow* win) = 0;
//};
//
//class Polygon : public VObject {
//    std::vector<Vector3f> points;
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
//    void rounded()
//    {
//
//        unsigned int edges = points.size();
//        float theta = 2 * PI / edges;
//
//        Vector3f center(0.0, 0.0, 0.0);
//
//        float radius = 250.0;
//
//        for (int i = 0; i < edges; ++i)
//        {
//            float angle = i * theta;
//            float x = center.x + radius * cos(angle);
//            float y = center.y + radius * sin(angle);
//            float z = center.z;
//
//            setPoint(i, Vector3f(x, y, z));
//        }
//    }
//
//public:
//
//  
//
//    explicit Polygon(size_t pointCount) : points(pointCount) 
//    {
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
//    }
//
//    void fill()
//    {
//        filled = true;
//    }
//
//    void outline()
//    {
//        filled = false;
//    }
//
//    void project(Fragment* fragment,Screen* scr, RenderWindow* win) override {
//        
//
//        for (size_t i = 0; i < points.size(); i++) {
//            Vector2f v = projectToRectCoords(points[i], scr->vertices);
//            v = MathUtils::toScreen(v, win->getSize());
//            fragment->setPoint(i, v);
//        }
//
//        if (filled) fragment->fill();
//        
//    }
//};
//
//// ==================== FRAGMENT CLASS ====================
//
//class Fragment : public sf::Drawable
//{
//    std::vector<Vector2f> points;
//    VertexArray shape;
//
//
//
//    bool filled = false;
//
//    void updateShape() {
//        shape.clear();
//        for (size_t i = 0; i < points.size(); ++i) {
//            shape.append({ points[i], sf::Color::White });
//        }
//        if (!filled && !points.empty()) {
//            shape.append({ points[0], sf::Color::White });
//        }
//    }
//
//public:
//
//    VObject* Subject;
//
//    explicit Fragment(size_t pointCount) :
//        points(pointCount),
//        shape(filled ? sf::TriangleFan : sf::LineStrip, pointCount + (filled ? 0 : 1))
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
//        shape.setPrimitiveType(sf::TriangleFan);
//        updateShape();
//    }
//
//    void outline() {
//        filled = false;
//        shape.setPrimitiveType(sf::LineStrip);
//        updateShape();
//    }
//
//    void reFresh(Screen* scr, RenderWindow* win)
//    {
//        Subject->project(this, scr, win);
//    }
//
//    void draw(sf::RenderTarget& target, sf::RenderStates states) const override {
//        target.draw(shape, states);
//    }
//};
//
//// ==================== PLANE (RENDERER) ====================
//
//class Plane {
//    
//    void makeScreen()
//    {
//        screen = new Screen(window->getSize());
//    }
//
//public:
//    RenderWindow* window;
//    Screen* screen;
//    std::vector<std::unique_ptr<Fragment>> fragments;
//
//
//    
//
//    Plane(RenderWindow* win) : window(win) 
//    {
//        makeScreen();
//    }
//
//    void add(VObject* obj) 
//    {
//        Fragment* frag = new Fragment;
//
//        obj->project(frag,screen, window);
//
//        frag->Subject = obj;
//
//        obj->Observer = frag;
//
//        fragments.push_back(frag);
//
//        
//        
//    }
//
//    void render() {
//        for (auto& fragment : fragments) 
//        {
//            window->draw(*fragment);
//        }
//    }
//
//
//    void reFreshAllFragments()
//    {
//        for (auto& frag : fragments)
//        {
//            frag->reFresh(screen,window);
//        }
//    }
//
//    void move(Vector3f delta)
//    {
//        this->screen->move(delta);
//
//        reFreshAllFragments();
//    }
//
//    void rotate(Vector3f delta)
//    {
//        this->screen->rotate(delta);
//
//        reFreshAllFragments();
//    }
//
//    void clear() 
//    {
//        fragments.clear();
//    }
//};
//
//
////////////////////////////// MAIN ///////////////////////////////////
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
// //   while (window.isOpen())
// //   {
//
// //       
// //       Event event;
// //       while (window.pollEvent(event))
// //       {
// //           if (event.type == sf::Event::Closed)
// //               window.close();
//
//
// //           if (event.type == sf::Event::KeyPressed)
// //           {
// //               
//
// //               if (event.key.code == sf::Keyboard::Space)
// //               {
// //                   
//
// //                   if (isFilled)
// //                       po.outline();
// //                   else
// //                       po.fill();
//
// //                   isFilled = !isFilled;
// //               }
// //           }
// //       }
//
// //       d_m = Mouse::getPosition(window) - mouse;
// //       mouse = Mouse::getPosition(window);
//
// //       
//
// //       Vector3i d_p(d_m.x, d_m.y, 0); // CHANGED: Use Vector3i for 3D move
//
//
//
// //      
//
// //       // CHANGED: Remove getBounds and use proximity check manually
// //       if (Mouse::isButtonPressed(Mouse::Left))
// //       {
// //           po.move(Vector3f(d_p));
// //       }
//
// //       // Keyboard input
// //       if (Keyboard::isKeyPressed(Keyboard::W))
// //       {
// //           scr.move(Vector3f(0, -0.2, 0));
// //       }
// //       if (Keyboard::isKeyPressed(Keyboard::S))
// //       {
// //           scr.move(Vector3f(0, 0.2, 0)); // CHANGED
// //       }
// //       if (Keyboard::isKeyPressed(Keyboard::A))
// //       {
// //           scr.move(Vector3f(-0.2, 0, 0)); // CHANGED
// //       }
// //       if (Keyboard::isKeyPressed(Keyboard::D))
// //       {
// //           scr.move(Vector3f(0.2, 0, 0)); // CHANGED
// //       }
//
// //       if (event.type == sf::Event::MouseWheelScrolled)
// //       {
// //           float dz = event.mouseWheelScroll.delta * 1.0f;  // Can adjust speed
// //           scr.move(Vector3f(0.f, 0.f, dz));
// //       }
//
//
// //       if (clock.getElapsedTime() >= duration)
// //       {
// //           e++;
// //           //pol.setEdge(e);
// //           clock.restart();
// //       }
//
// //       for (int i = 0; i < 4; i++)
// //       {
// //           cout << scr.ScrVert[i].x << "-" << scr.ScrVert[i].y << "-" << scr.ScrVert[i].z << endl;
// //       }
//
// //       cout << ">> \n\n\n";
//
// //       /*pol.tranform(&scr);*/
//
//
// //       window.clear();
// ///*       window.draw(pol);*/ // draws 3D-projected polygon
// //       window.draw(po);
//
// //       //window.draw(ClosedShape);
// //       //window.draw(polygon);
// //       //window.draw(triangleFan);
//
// //       window.display();
// //   }
//
//    return 0;
//}
