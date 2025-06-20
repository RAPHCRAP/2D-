#pragma once


#include <SFML/Graphics.hpp>
#include "winObj.h"
#include "Super.h"


class Camera;

namespace sf
{
    class Polygon :public Drawable ,public rayTrace, public Super
    {
    public:
        Vector3f center;
        float radius;
        int edges;
        unsigned int intensity;

        std::vector<Vector3f> vertices3D;
        VertexArray* shape;

        Polygon(int edge = 4, float radius = 5.0f, Vector3f center = Vector3f(0, 0, 0), unsigned int intensity = 0);

        void move(const Vector3f& d_m);
        void defVertex();
        void projectTo2D();
        void illuminate(const RenderWindow& win, unsigned int intensity = 20);
        void fill();
        void setEdge(size_t e = 4);
        void transform(Camera* scr);
        void clear();

        ~Polygon();


        virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const override;
    private:
        Vector2f projectPoint(const Vector3f& pt);



    };

    void draw(RenderTarget& target, Polygon& pol);
}