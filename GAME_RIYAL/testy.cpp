
#include<iostream>
#include <vector>
#include <memory>
#include <cassert>
#include <cmath>
#include <SFML/Graphics.hpp>

using namespace sf;
using namespace std;

const float PI = 3.14159265358979323846f;



// ==================== UTILITY FUNCTIONS ====================
namespace MathUtils {
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

    Vector3f projectPointToPlane(const Vector3f& P, const Vector3f& A,
        const Vector3f& B, const Vector3f& D) {
        Vector3f U = B - A;
        Vector3f V = D - A;
        Vector3f N = normalize(cross(U, V));
        return P - dot(P - A, N) * N;
    }

    Vector2f toScreen(const Vector2f& logical, const Vector2u& windowSize) {
        return {
            logical.x + windowSize.x / 2.f,
            -logical.y + windowSize.y / 2.f
        };
    }

    
    Vector3f projectOntoPlane(
        const Vector3f& point,
        const Vector3f& A,
        const Vector3f& B,
        const Vector3f& C
    ) {
        Vector3f v1 = B - A;
        Vector3f v2 = C - A;
        Vector3f normal = cross(v1, v2);
        Vector3f AP = point - A;
        float distance = dot(AP, normal) / dot(normal, normal);
        return point - normal * distance;
    }

    Vector2f toPlane2DCoordinates(
        const Vector3f& pointOnPlane,
        const Vector3f& origin,
        const Vector3f& dir1,
        const Vector3f& dir2
    ) {
        Vector3f w = pointOnPlane - origin;

        float d11 = dot(dir1, dir1);
        float d12 = dot(dir1, dir2);
        float d22 = dot(dir2, dir2);
        float dw1 = dot(w, dir1);
        float dw2 = dot(w, dir2);

        float denom = d11 * d22 - d12 * d12;
        if (std::abs(denom) < 1e-6f) {
            return { 0.f, 0.f };
        }

        float a = (dw1 * d22 - dw2 * d12) / denom;
        float b = (dw2 * d11 - dw1 * d12) / denom;
        return { a, b };
    }

    Vector2f projectPointToPlane2D(
        const Vector3f& point,
        const Vector3f& A,
        const Vector3f& B,
        const Vector3f& C,
        const Vector3f& origin
    ) {
        Vector3f projected = projectOntoPlane(point, A, B, C);
        Vector3f dir1 = A - origin;
        Vector3f dir2 = B - origin;
        return toPlane2DCoordinates(projected, origin, dir1, dir2);
    }

}




class Screen {
public:
    Vector3f position;   // Camera center
    Vector3f forward;    // View direction (normalized)
    Vector3f up;         // What is 'up' (normalized)

    Screen(Vector3f pos = { 0, 0, -500 }, Vector3f lookAt = { 0, 0, 0 }, Vector3f worldUp = { 0, 1, 0 }) 
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

    Vector3f rotateAroundAxis(const Vector3f& P, const Vector3f& axis, const Vector3f& center, float angleRad)
    {
        using namespace MathUtils;

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

    //void rotateAround(const Vector3f& axis, float angleRad)
    //{
    //    using namespace MathUtils;

    //    Vector3f center = getCenter();
    //    for (auto& P : points) {
    //        P = rotateAroundAxis(P, axis, center, angleRad);
    //    }

    //    up = rotateAroundAxis(up, axis, Vector3f(0, 0, 0), angleRad); // up is a direction
    //}

    void rotateAroundUp(float deltaAngleDegrees, float deltaTime) {
        using namespace MathUtils;
        float angle = degreesToRadians(deltaAngleDegrees) * deltaTime;
        forward = rotateAroundAxis(forward, up, Vector3f(0, 0, 0), angle);
    }

    void rotateInPlane(float deltaAngleDegrees, float deltaTime) {
        using namespace MathUtils;
        float angle = degreesToRadians(deltaAngleDegrees) * deltaTime;
        Vector3f N = forward;

        Vector3f newRight = rotateAroundAxis(right(), N, Vector3f(0, 0, 0), angle);
        up = rotateAroundAxis(up, N, Vector3f(0, 0, 0), angle);

        // Recalculate forward to ensure orthonormality (optional)
        forward = normalize(forward);
    }


    void pitch(float deltaAngleDegrees, float deltaTime) {
        using namespace MathUtils;
        float angle = degreesToRadians(deltaAngleDegrees) * deltaTime;
        Vector3f rightVec = right();
        forward = rotateAroundAxis(forward, rightVec, Vector3f(0, 0, 0), angle);
        up = normalize(cross(rightVec, forward));
    }
};



Screen* sc;

// ==================== FRAGMENT CLASS ====================
class Fragment : public Drawable {
public:
    // Member variables
    VertexArray shape;
    bool filled = false;
    vector<Vector2f> points;

    // Constructors
    explicit Fragment(size_t pointCount = 0) :
        points(pointCount),
        shape(filled ? TriangleFan : LineStrip, pointCount + (filled ? 0 : 1))
    {
    }


    Fragment(const Fragment& other) :
        filled(other.filled),
        points(other.points),
        shape(other.shape)
    {

        updateShape();
    }

    // Public interface
    Vector2f getCenter() const {
        if (points.empty()) return Vector2f(0.f, 0.f);

        Vector2f sum(0.f, 0.f);
        for (const auto& p : points) {
            sum += p;
        }
        return sum / static_cast<float>(points.size());
    }

    void setPoint(size_t index, const Vector2f& point) {
        assert(index < points.size());
        points[index] = point;
        updateShape();
    }

    void changeEdges(int n) {
        if (n < 3) return; 

        points.resize(n);
        rounded();
        updateShape();
    }

    void rounded() {
        const float PI = 3.14159265f;
        const unsigned edges = points.size();
        if (edges < 3) return;

        const float theta = 2 * PI / edges;
        const float radius = 250.0f;
        const Vector2f center = getCenter();

        for (unsigned i = 0; i < edges; ++i) {
            const float angle = i * theta;
            points[i] = center + Vector2f(
                radius * cos(angle),
                radius * sin(angle)
            );
        }

        updateShape();
    }

    void fill() {
        filled = true;
        shape.setPrimitiveType(TriangleFan);
        updateShape();
    }

    void outline() {
        filled = false;
        shape.setPrimitiveType(LineStrip);
        updateShape();
    }

    void move(const Vector2f& delta) {
        for (auto& v : points) {
            v += delta;
        }
        updateShape();
    }

    void rotate(float deltaAngleDegrees, float deltaTime = 1.0f) {
        if (points.empty()) return;

        const float angleRad = deltaAngleDegrees * (3.14159265f / 180.0f) * deltaTime;
        const float cosA = cos(angleRad);
        const float sinA = sin(angleRad);
        const Vector2f center = getCenter();

        for (auto& v : points) {
            const float x = v.x - center.x;
            const float y = v.y - center.y;
            v.x = center.x + x * cosA - y * sinA;
            v.y = center.y + x * sinA + y * cosA;
        }

        updateShape();
    }

    void draw(RenderTarget& target, RenderStates states) const override {
        target.draw(shape, states);
    }

    // Helper methods
    void updateShape() {
        shape.clear();
        for (const auto& point : points) {
            shape.append({ point, Color::White });
        }

        // Close the loop if not filled
        if (!filled && !points.empty()) {
            shape.append({ points[0], Color::White });
        }
    }
};





// ==================== VOBJECT HIERARCHY ====================
class VObject {
public:

    Vector3f up;
    virtual ~VObject() = default;
};


class Polygon3D : public VObject {
public:
    vector<Vector3f> points;
    bool filled = false;

    explicit Polygon3D(size_t pointCount) : points(pointCount) {
        rounded();
        up = getPlaneNormal(); 


    }

    void setPoint(size_t index, const Vector3f& point) {
        assert(index < points.size());
        points[index] = point;
    }

    void move(const Vector3f& delta) {
        for (auto& v : points) {
            v += delta;
        }
    }

    void fill() { filled = true; }
    void outline() { filled = false; }

    Vector3f getCenter() const {
        using namespace MathUtils;
        if (points.empty()) return Vector3f(0.f, 0.f, 0.f);

        Vector3f sum(0.f, 0.f, 0.f);
        for (const auto& p : points) {
            sum += p;
        }
        return sum / static_cast<float>(points.size());
    }

    Vector3f getPlaneNormal() const {
        using namespace MathUtils;
        if (points.size() < 3) return Vector3f(0.f, 0.f, 0.f);

        Vector3f U = points[1] - points[0];
        Vector3f V = points[2] - points[0];
        return normalize(cross(U, V));
    }

    void rounded() {
        using namespace MathUtils;

        unsigned edges = points.size();
        if (edges < 3) return;

        float theta = 2 * PI / edges;
        float radius = 250.0f;

        for (unsigned i = 0; i < edges; ++i) {
            float angle = i * theta;
            setPoint(i, Vector3f(
                radius * cos(angle),
                radius * sin(angle),
                0.0f
            ));
        }

        up = getPlaneNormal();  //  Set up properly after shape generation
    }

    Vector3f rotateAroundAxis(const Vector3f& P, const Vector3f& axis, const Vector3f& center, float angleRad) const {
        using namespace MathUtils;

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

    void rotateAround(const Vector3f& axis, float angleRad) {
        Vector3f center = getCenter();
        for (auto& P : points) {
            P = rotateAroundAxis(P, axis, center, angleRad);
        }
        up = rotateAroundAxis(up, axis, Vector3f(0.f, 0.f, 0.f), angleRad);
    }

    void rotateAroundA(Vector3f worldUp,float deltaAngleDegrees, float deltaTime)
    {
        using namespace MathUtils;

        Vector3f normal = getPlaneNormal();

        Vector3f projectedUp = worldUp - normal * dot(worldUp, normal);

        if (length(projectedUp) < 0.001f) {

            worldUp = Vector3f(1.0f, 0.0f, 0.0f);
            projectedUp = worldUp - normal * dot(worldUp, normal);
        }

        Vector3f axis = normalize(projectedUp);

        float angleRad = degreesToRadians(deltaAngleDegrees) * deltaTime;
        rotateAround(axis, angleRad);
    }

    void rotateAroundY(float deltaAngleDegrees, float deltaTime) {
        
        
        Vector3f Up = Vector3f(0.0f, 1.0f, 0.0f);


        rotateAroundA(Up,deltaAngleDegrees,deltaTime);
       
    }

    void rotateAroundX(float deltaAngleDegrees, float deltaTime) {


        Vector3f Up = Vector3f(0.1f, 0.0f, 0.0f);

        rotateAroundA(Up, deltaAngleDegrees, deltaTime);
    }

    void rotateAroundZ(float deltaAngleDegrees, float deltaTime) {


        Vector3f Up = Vector3f(0.0f, 0.0f, 1.0f);

        rotateAroundA(Up, deltaAngleDegrees, deltaTime);
    }

    void rotateInPlane(float deltaAngleDegrees, float deltaTime) {
        using namespace MathUtils;

        Vector3f center = getCenter();
        Vector3f normal = getPlaneNormal();
        float angleRad = degreesToRadians(deltaAngleDegrees) * deltaTime;

        for (auto& P : points) {
            P = rotateAroundAxis(P, normal, center, angleRad);
        }

        up = rotateAroundAxis(up, normal, Vector3f(0.f, 0.f, 0.f), angleRad); 
    }
};


void display3DVector(Vector3f v)
{
    cout << "(" << v.x << ", " << v.y << ", " << v.z << ")   ";
}

void display2DVector(Vector2f v)
{
    cout << "(" << v.x << ", " << v.y << ")   ";
}

Vector2f projectToScreen2D(const Vector3f& point, const Screen& screen) 
{
    using namespace MathUtils;

    Vector3f toPoint = point - screen.getCenter();

    float x = dot(toPoint, screen.right());
    float y = dot(toPoint, screen.up);

    return Vector2f(x, y);
}



void createFragment(Fragment& fg, Screen* sc, VObject* po, RenderWindow* window) {
    using namespace MathUtils;
    float scale = 1.0f;

    Polygon3D* pol = static_cast<Polygon3D*>(po);
    fg.changeEdges(pol->points.size());

    Vector2f screenCenter = Vector2f(window->getSize()) * 0.5f;

    for (int i = 0; i < fg.points.size(); i++) {
        Vector2f local = projectToScreen2D(pol->points[i], *sc);
        Vector2f px = screenCenter + Vector2f(local.x, -local.y) * scale;
        fg.setPoint(i, px);
        fg.updateShape();
    }
}


bool isInFrontOfCamera(const Polygon3D& poly, const Screen& screen)
{
    using namespace MathUtils;

    Vector3f toPolygon = poly.getCenter() - screen.position;
    Vector3f forward = screen.forward; 

    float dotResult = dot(toPolygon, forward);

    return dotResult > 0;
}


int main()
{

    // Create window
    RenderWindow window(VideoMode(800, 600), "3D Projection Test");
    window.setFramerateLimit(60);


    
    sc = new Screen;
    

        float moveSpeed = 4.0f;

        Fragment p(4);



            Polygon3D po(4);


    while (window.isOpen())
    {

        Event event;

                while (window.pollEvent(event)) {
                    if (event.type == Event::Closed)
                        window.close();
        

                    // Zoom with mouse wheel
                    if (event.type == Event::MouseWheelScrolled) {
                        float zoom = event.mouseWheelScroll.delta * 10.0f;
                        sc->move({ 0, 0, zoom });
                    /*    po.move({ 0, 0, zoom });*/
                    }
                }

                cout << "screen centre:"; display3DVector(sc->getCenter()); 
                cout << "object up:"; display3DVector(po.points[0]);

             /*   
                if (Keyboard::isKeyPressed(Keyboard::W))
                {
                    po.move({ 0, -moveSpeed, 0 });



                }
                if (Keyboard::isKeyPressed(Keyboard::S))
                {
                    po.move({ 0, moveSpeed, 0 });


                }

                if (Keyboard::isKeyPressed(Keyboard::A))
                {
                    po.move({ -moveSpeed, 0, 0 });


                }
                if (Keyboard::isKeyPressed(Keyboard::D))
                {
                    po.move({ moveSpeed, 0, 0 });


                }


                */

                if (Keyboard::isKeyPressed(Keyboard::Q))
                {
                    po.rotateInPlane(+4.0f, 1.f);


                }
                

                if (Keyboard::isKeyPressed(Keyboard::E))
                {
                    po.rotateInPlane(-4.0f, 1.f);


                }


                

                if (Keyboard::isKeyPressed(Keyboard::R))
                {
                    po.rotateAroundX(+4.0f, 1.f);
                    


                }


                if (Keyboard::isKeyPressed(Keyboard::T))
                {
                    po.rotateAroundX(-4.0f, 1.f);


                }



                if (Keyboard::isKeyPressed(Keyboard::Y))
                {
                    po.rotateAroundY(+4.0f, 1.f);



                }


                if (Keyboard::isKeyPressed(Keyboard::U))
                {
                    po.rotateAroundY(-4.0f, 1.f);


                }



                if (Keyboard::isKeyPressed(Keyboard::I))
                {
                    po.rotateAroundZ(+4.0f, 1.f);



                }


                if (Keyboard::isKeyPressed(Keyboard::O))
                {
                    po.rotateAroundZ(-4.0f, 1.f);


                }
        
                if (Keyboard::isKeyPressed(Keyboard::W))
                {
   
                           sc->move({ 0, -moveSpeed, 0 });


                }
                if (Keyboard::isKeyPressed(Keyboard::S))
                {
  
                             sc->move({ 0, moveSpeed, 0 });

                }

                if (Keyboard::isKeyPressed(Keyboard::A))
                {
  
                         sc->move({ -moveSpeed, 0, 0 });

                }
                if (Keyboard::isKeyPressed(Keyboard::D))
                {
 
                          sc->move({ moveSpeed, 0, 0 });

                }




                //if (Keyboard::isKeyPressed(Keyboard::Q))
                //{
                //              sc->rotateInPlane(+4.0f, 1.f);

                //}


                //if (Keyboard::isKeyPressed(Keyboard::E))
                //{
                //                sc->rotateInPlane(-4.0f, 1.f);

                //}


               /* if (Keyboard::isKeyPressed(Keyboard::R))
                {

                            sc->rotateAroundUp(+4.0f, 1.f);

                }


                if (Keyboard::isKeyPressed(Keyboard::T))
                {
                                     sc->rotateAroundUp(-4.0f, 1.f);

                }*/

                createFragment(p, sc, &po,&window);

                cout << "fragment : "; display2DVector(p.getCenter());


                cout << endl;


        window.clear();

        if(isInFrontOfCamera(po,*sc))
        {
            window.draw(p);
        }
        
        window.display();
      
        
    }



    return 0;
}
