#include "Polygon.h"
#include<iostream>
#include<numbers>
#include<cmath>
#include<vector>


using namespace sf;
using namespace std;

const double PI = 3.14159265358979323846f;




Vector2f Polygon::projectPoint(const Vector3f& pt)
{
    // Simple orthographic projection: ignore Z
    return Vector2f(pt.x, pt.y);

    // Or perspective:
    // float depth = pt.z + 200.f;
    // float scale = 300.f / depth;
    // return Vector2f(pt.x * scale, pt.y * scale);
}

Polygon::Polygon(int edge, float radius, Vector3f center, unsigned int intensity)
    : edges(edge), radius(radius), center(center), intensity(intensity)
{
    shape = new VertexArray(LineStrip, edges + 1);
    defVertex();
}

void Polygon::defVertex()
{
    vertices3D.clear();
    float theta = 2 * PI / edges;

    for (int i = 0; i < edges; ++i)
    {
        float angle = i * theta;
        float x = center.x + radius * cos(angle);
        float y = center.y + radius * sin(angle);
        float z = center.z;

        vertices3D.emplace_back(x, y, z);
    }

    // Close loop
    vertices3D.push_back(vertices3D[0]);

    projectTo2D();
}

void Polygon::projectTo2D()
{
    shape->clear();
    for (const auto& v : vertices3D)
        shape->append(Vertex(projectPoint(v), Color::White));
}

void Polygon::move(const Vector3f& d_m)
{
    for (auto& v : vertices3D)
        v += Vector3f(d_m);

    center += Vector3f(d_m);
    projectTo2D();
}

void Polygon::illuminate(const RenderWindow& win, unsigned int intensity)
{
    this->intensity = intensity;
    // Not implemented yet
}

void Polygon::fill()
{
    if (shape) delete shape;
    shape = new VertexArray(TriangleFan, edges);
    defVertex(); // Will auto project
}

void Polygon::setEdge(size_t e)
{
    edges = e;
    if (shape) delete shape;
    shape = new VertexArray(LineStrip, edges + 1);
    defVertex();
}

void Polygon::clear()
{
    if (shape)
    {
        delete shape;
        shape = nullptr;
    }
}

Polygon::~Polygon()
{
    clear();
}

void Polygon::transform(Camera* scr)
{
    for (int i = 0; i < edges; ++i)
    { 


    }
}


void sf::Polygon::draw(sf::RenderTarget& target, sf::RenderStates states) const {
    if (shape)
        target.draw(*shape, states); // assuming shape is a VertexArray*
}

// Custom draw function
void sf::draw(RenderTarget& target, Polygon& pol)
{
    target.draw(*pol.shape);
}