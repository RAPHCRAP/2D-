//#include <SFML/Graphics.hpp>
//#include <vector>
//#include <cmath>
//#include <algorithm>
//#include <iostream>
//
//float perpendicularDistanceToLine(const sf::Vector2f& point, const sf::Vector2f& linePoint, const sf::Vector2f& lineNormal) {
//    sf::Vector2f unitNormal = lineNormal;
//    float magnitude = std::sqrt(lineNormal.x * lineNormal.x + lineNormal.y * lineNormal.y);
//    if (magnitude != 0.f)
//        unitNormal /= magnitude;
//    sf::Vector2f diff = point - linePoint;
//    return std::abs(diff.x * unitNormal.x + diff.y * unitNormal.y);
//}
//
//bool isBehindLine(const sf::Vector2f& point, const sf::Vector2f& linePoint, const sf::Vector2f& normal) {
//    sf::Vector2f diff = point - linePoint;
//    float dot = diff.x * normal.x + diff.y * normal.y;
//    return dot < 0.f;
//}
//
//class Grid {
//private:
//    int size;
//    float cellSize;
//    float scale;
//    std::vector<sf::RectangleShape> cells;
//
//public:
//    Grid(int size, float scale = 50.0f)
//        : size(size), scale(scale), cellSize(scale) {
//        generateGrid();
//    }
//
//    void generateGrid() {
//        cells.clear();
//        int half = size / 2;
//        for (int y = -half; y <= half; ++y) {
//            for (int x = -half; x <= half; ++x) {
//                sf::RectangleShape cell(sf::Vector2f(cellSize, cellSize));
//                cell.setFillColor(sf::Color::Transparent);
//                cell.setOutlineColor(sf::Color::White);
//                cell.setOutlineThickness(1.f);
//                cell.setOrigin(cellSize / 2.f, cellSize / 2.f);
//                sf::Vector2f position(x * scale, -y * scale);
//                cell.setPosition(position);
//                cells.push_back(cell);
//            }
//        }
//    }
//
//    void draw(sf::RenderWindow& window) {
//        sf::Vector2u windowSize = window.getSize();
//        sf::Vector2f center(windowSize.x / 2.f, windowSize.y / 2.f);
//        for (auto& cell : cells) {
//            sf::RectangleShape shifted = cell;
//            shifted.move(center);
//            window.draw(shifted);
//        }
//    }
//
//    float getScale() const {
//        return scale;
//    }
//
//    sf::Vector2f getCellCenterCartesian(std::size_t index) const {
//        if (index >= cells.size())
//            return sf::Vector2f(0.f, 0.f);
//
//        sf::Vector2f pixelPos = cells[index].getPosition();
//        float x = pixelPos.x / scale;
//        float y = -pixelPos.y / scale;
//        return sf::Vector2f(x, y);
//    }
//
//    std::size_t getCellCount() const {
//        return cells.size();
//    }
//
//    const sf::RectangleShape& getCell(std::size_t index) const {
//        return cells[index];
//    }
//};
//
//class Ray {
//private:
//    sf::Vector2f position;
//    sf::Vector2f normal;
//    float lengthUnits;
//    float scale;
//    float rotationSpeed;
//    float moveStep;
//
//public:
//    Ray(const sf::Vector2f& position, const sf::Vector2f& normal, float lengthUnits = 1.f, float scale = 50.f)
//        : position(position), lengthUnits(lengthUnits), scale(scale), rotationSpeed(5.f), moveStep(0.1f) {
//        float magnitude = std::sqrt(normal.x * normal.x + normal.y * normal.y);
//        this->normal = (magnitude != 0.f) ? normal / magnitude : sf::Vector2f(1.f, 0.f);
//    }
//
//    void setScale(float newScale) {
//        scale = newScale;
//    }
//
//    void move(float dx, float dy) {
//        position.x += dx;
//        position.y += dy;
//    }
//
//    void rotate(float angleDegrees) {
//        float radians = angleDegrees * 3.14159265f / 180.f;
//        float cosA = std::cos(radians);
//        float sinA = std::sin(radians);
//        sf::Vector2f rotated;
//        rotated.x = normal.x * cosA - normal.y * sinA;
//        rotated.y = normal.x * sinA + normal.y * cosA;
//        float mag = std::sqrt(rotated.x * rotated.x + rotated.y * rotated.y);
//        if (mag != 0.f) normal = rotated / mag;
//    }
//
//    void handleInput() {
//        if (sf::Keyboard::isKeyPressed(sf::Keyboard::W)) move(0.f, moveStep);
//        if (sf::Keyboard::isKeyPressed(sf::Keyboard::S)) move(0.f, -moveStep);
//        if (sf::Keyboard::isKeyPressed(sf::Keyboard::A)) move(-moveStep, 0.f);
//        if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)) move(moveStep, 0.f);
//        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Q)) rotate(-rotationSpeed);
//        if (sf::Keyboard::isKeyPressed(sf::Keyboard::E)) rotate(rotationSpeed);
//    }
//
//    void draw(sf::RenderWindow& window) const {
//        sf::Vector2u windowSize = window.getSize();
//        sf::Vector2f center(windowSize.x / 2.f, windowSize.y / 2.f);
//        sf::Vector2f pixelPos(position.x * scale, -position.y * scale);
//        sf::Vector2f screenPos = pixelPos + center;
//
//        sf::CircleShape originDot(4.f);
//        originDot.setOrigin(4.f, 4.f);
//        originDot.setFillColor(sf::Color::Red);
//        originDot.setPosition(screenPos);
//        window.draw(originDot);
//
//        sf::Vector2f endOffset = normal * (lengthUnits * scale);
//        sf::Vector2f endPoint = screenPos + endOffset;
//
//        sf::Vertex line[] = {
//            sf::Vertex(screenPos, sf::Color::Yellow),
//            sf::Vertex(endPoint, sf::Color::Yellow)
//        };
//        window.draw(line, 2, sf::Lines);
//    }
//
//    void colorByDistanceToLine(sf::RenderWindow& window, const Grid& grid) {
//        std::vector<sf::Color> spectrum = {
//            sf::Color::Red,
//            sf::Color(255, 165, 0),
//            sf::Color::Yellow,
//            sf::Color::Green,
//            sf::Color::Blue,
//            sf::Color(75, 0, 130),
//            sf::Color(143, 0, 255)
//        };
//
//        float bandWidth = 0.5f;
//        sf::Vector2u winSize = window.getSize();
//        sf::Vector2f center(winSize.x / 2.f, winSize.y / 2.f);
//
//        for (std::size_t i = 0; i < grid.getCellCount(); ++i) 
//        {
//
//            
//
//            sf::Vector2f cellCenter = grid.getCellCenterCartesian(i);
//
//            if (isBehindLine(cellCenter, position, normal))
//            {
//                continue;
//            }
//
//            float distance = perpendicularDistanceToLine(cellCenter, position, normal);
//
//            int band = static_cast<int>(std::floor(distance / bandWidth));
//
//            if (band > spectrum.size())
//            {
//                continue;
//            }
//
//            sf::Color color = spectrum[band % spectrum.size()];
//
//            sf::Vector2f pixelPos(cellCenter.x * scale, -cellCenter.y * scale);
//            sf::Vector2f screenPos = pixelPos + center;
//
//            sf::RectangleShape box(sf::Vector2f(scale, scale));
//            box.setOrigin(scale / 2.f, scale / 2.f);
//            box.setPosition(screenPos);
//            box.setFillColor(color);
//            box.setOutlineColor(sf::Color::White);
//            box.setOutlineThickness(1.f);
//            window.draw(box);
//        }
//    }
//};
//
//int main() {
//    sf::RenderWindow window(sf::VideoMode(600, 600), "Ray Distance Coloring");
//    window.setFramerateLimit(60);
//
//    int gridSize = 3;
//    Grid grid(gridSize, 50.f);
//    Ray ray({ 0, 0 }, { 1, 0 }, 2.f, grid.getScale());
//
//    while (window.isOpen()) {
//        sf::Event event;
//        while (window.pollEvent(event)) {
//            if (event.type == sf::Event::Closed)
//                window.close();
//        }
//
//        ray.handleInput();
//
//        window.clear(sf::Color::Black);
//        ray.colorByDistanceToLine(window, grid);
//        grid.draw(window);
//        ray.draw(window);
//        window.display();
//    }
//
//    return 0;
//}