#include <SFML/Graphics.hpp>
#include <iostream>
#include <fstream>
#include <set>
#include <sstream>
#include <cmath>


float perpendicularDistanceToLine(const sf::Vector2f& point, const sf::Vector2f& linePoint, const sf::Vector2f& lineNormal) {
    sf::Vector2f unitNormal = lineNormal;
    float magnitude = std::sqrt(lineNormal.x * lineNormal.x + lineNormal.y * lineNormal.y);
    if (magnitude != 0.f)
        unitNormal /= magnitude;
    sf::Vector2f diff = point - linePoint;
    return std::abs(diff.x * unitNormal.x + diff.y * unitNormal.y);
}

bool isBehindLine(const sf::Vector2f& point, const sf::Vector2f& linePoint, const sf::Vector2f& normal) {
    sf::Vector2f diff = point - linePoint;
    float dot = diff.x * normal.x + diff.y * normal.y;
    return dot < 0.f;
}

class cell {
public:
    sf::Vector2f position; // in cartesian coordinates
    float cellSize;
    float scale;
    sf::RectangleShape square;
    float rank;

    cell(sf::Vector2f pos, float cellSize, float scale,float rank=0)
        : position(pos), cellSize(cellSize), scale(scale) {
        square.setSize(sf::Vector2f(cellSize * scale, cellSize * scale));
        square.setOrigin(square.getSize() / 2.0f); // center origin

        // Make it a hollow square
        square.setFillColor(sf::Color::Transparent);           // No fill
        square.setOutlineThickness(1.f);                       // Outline width
        square.setOutlineColor(sf::Color::White);              // Outline color
    }

    sf::Vector2f getCentre() const {
        return position;
    }

    void setRank(float rank)
    {
        this->rank = rank;
    }

    void draw(sf::RenderWindow& window, const sf::Vector2f& windowCenter) const {
        sf::Vector2f pixelPos = windowCenter + sf::Vector2f(position.x * scale, -position.y * scale);
        sf::RectangleShape drawable = square;
        drawable.setPosition(pixelPos);
        window.draw(drawable);
    }
};

class Grid {
public:
    int size;
    float cellSize;
    sf::Vector2f position; // origin in cartesian
    float scale;

    std::vector<cell*> cells;

    Grid(int size, sf::Vector2f position, float cellSize, float scale = 50.0f)
        : size(size), position(position), cellSize(cellSize), scale(scale) {
        generateGrid();
    }

    void generateGrid() {
        cells.clear();
        int half = size / 2;

        for (int y = half; y >= -half; --y) { // top to bottom
            for (int x = -half; x <= half; ++x) { // left to right
                cells.push_back(new cell(sf::Vector2f(x, y), cellSize, scale));
            }
        }
    }

    float getScale() const {
        return scale;
    }

    sf::Vector2f getCellCenter(std::size_t index) const {
        if (index >= cells.size()) return sf::Vector2f(0, 0);
        return cells[index]->getCentre();
    }

    std::size_t getCellCount() const 
    {
        return cells.size();
    }

    void draw(sf::RenderWindow* target) {
        sf::Vector2f windowCenter(target->getSize().x / 2.0f, target->getSize().y / 2.0f);
        for (auto& c : cells) {
            c->draw(*target, windowCenter);
        }
    }
};
void display2DVector(sf::Vector2f v)
{
    std::cout << "(" << v.x << ", " << v.y << ")   ";
}

class Ray
{
public:
    sf::Vector2f position;    
    sf::Vector2f normal;   
    float lengthUnits;       
    float scale;            
    float rotationSpeed;
    float moveStep;

    float angleDegrees = 0.f;  // Angle around the circle
    float orbitRadius = 1.f;   // In Cartesian units

    void updatePositionOnOrbit(float direction) {
        angleDegrees += direction;
        if (angleDegrees >= 360.f) angleDegrees -= 360.f;
        if (angleDegrees < 0.f) angleDegrees += 360.f;

        float radians = angleDegrees * 3.14159265f / 180.f;
        position.x = orbitRadius * std::cos(radians);
        position.y = orbitRadius * std::sin(radians);

        // Normal should point toward origin  (0,0) - position
        sf::Vector2f toCenter = -position;
        float mag = std::sqrt(toCenter.x * toCenter.x + toCenter.y * toCenter.y);
        if (mag != 0.f) normal = toCenter / mag;
    }

    Ray(const sf::Vector2f& position, const sf::Vector2f& normal, float lengthUnits = 1.f, float scale = 50.f)
        : position(position), lengthUnits(lengthUnits), scale(scale), rotationSpeed(5.f), moveStep(0.1f) 
    {

        if (abs(normal.x) > 1 && abs(normal.y) > 1)
        {
            this->normal = sf::Vector2f(0.f,0.f);
        }
        this->normal = normal;

        angleDegrees = 0.f; // starting at angle 0
        orbitRadius = lengthUnits; // or custom value


        
    }

    void setScale(float newScale) { scale = newScale; }

    void move(float dx, float dy) { position.x += dx; position.y += dy; }

    void rotate(float angleDegrees) 
    {
        float radians = angleDegrees * 3.14159265f / 180.f;
        float cosA = std::cos(radians);
        float sinA = std::sin(radians);
        sf::Vector2f rotated;
        rotated.x = normal.x * cosA - normal.y * sinA;
        rotated.y = normal.x * sinA + normal.y * cosA;
        float mag = std::sqrt(rotated.x * rotated.x + rotated.y * rotated.y);
        if (mag != 0.f) normal = rotated / mag;

        
    }

    void handleInput() {
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::W)) move(0.f, moveStep);
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::S)) move(0.f, -moveStep);
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::A)) move(-moveStep, 0.f);
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)) move(moveStep, 0.f);
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Q)) rotate(-rotationSpeed);
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::E)) rotate(rotationSpeed);

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::P)) updatePositionOnOrbit(1.f);   // Clockwise
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::O)) updatePositionOnOrbit(-1.f);  // Counterclockwise

    }

    void draw(sf::RenderWindow& window) const {
        sf::Vector2f center(window.getSize().x / 2.f, window.getSize().y / 2.f);

        sf::Vector2f screenStart = center + sf::Vector2f(position.x * scale, -position.y * scale);
        sf::Vector2f normalPixels = sf::Vector2f(normal.x * scale, -normal.y * scale); // invert Y

        sf::Vector2f endPoint = screenStart + normalPixels * lengthUnits;


        sf::CircleShape originDot(4.f);
        originDot.setOrigin(4.f, 4.f);
        originDot.setFillColor(sf::Color::Red);
        originDot.setPosition(screenStart);
        window.draw(originDot);

        sf::Vertex line[] = {
            sf::Vertex(screenStart, sf::Color::Yellow),
            sf::Vertex(endPoint, sf::Color::Yellow)
        };
        window.draw(line, 2, sf::Lines);
    }

 

    void colorByDistanceToLine(sf::RenderWindow& window, const Grid& grid) {
        std::vector<sf::Color> spectrum = {
            sf::Color::Red,
            sf::Color(255, 165, 0),
            sf::Color::Yellow,
            sf::Color::Green,
            sf::Color::Blue,
            sf::Color(75, 0, 130),
            sf::Color(143, 0, 255)
        };

        float bandWidth = 0.5f;
        sf::Vector2f center(window.getSize().x / 2.f, window.getSize().y / 2.f);

        for (std::size_t i = 0; i < grid.getCellCount(); ++i) {
            sf::Vector2f cellCenter = grid.getCellCenter(i);  

          
            if (isBehindLine(cellCenter, position, normal)) continue;

            float distance = perpendicularDistanceToLine(cellCenter, position, normal);
            int band = static_cast<int>(std::floor(distance / bandWidth));
            /*if (band >= static_cast<int>(spectrum.size())) continue;*/

            sf::Color color = spectrum[band % spectrum.size()];

           
            sf::Vector2f screenPos = center + sf::Vector2f(cellCenter.x * scale, -cellCenter.y * scale);

            sf::RectangleShape box(sf::Vector2f(scale, scale));
            box.setOrigin(scale / 2.f, scale / 2.f);
            box.setPosition(screenPos);
            box.setFillColor(color);
            box.setOutlineColor(sf::Color::White);
            box.setOutlineThickness(1.f);
            window.draw(box);
        }
    }


    
    void labelCellsByDistanceToLine(sf::RenderWindow& window, const Grid& grid, const sf::Font& font) {
        float bandWidth = 0.5f;
        sf::Vector2f center(window.getSize().x / 2.f, window.getSize().y / 2.f);

        std::map<int, std::vector<sf::Vector2f>> bandToCells;

        for (std::size_t i = 0; i < grid.getCellCount(); ++i) {
            sf::Vector2f cellCenter = grid.getCellCenter(i);

            if (isBehindLine(cellCenter, position, normal)) continue;

            float distance = perpendicularDistanceToLine(cellCenter, position, normal);
            int band = static_cast<int>(std::floor(distance / bandWidth)) + 1;

            grid.cells[i]->setRank(band);

            bandToCells[band].push_back(cellCenter);
        }

        for (auto it = bandToCells.begin(); it != bandToCells.end(); ++it) {
            int band = it->first;

            const std::vector<sf::Vector2f>& cells = it->second;

            for (const auto& cellCenter : cells) {
                sf::Vector2f screenPos = center + sf::Vector2f(cellCenter.x * scale, -cellCenter.y * scale);

                sf::Text label;
                label.setFont(font);
                label.setString(std::to_string(band));
                label.setCharacterSize(static_cast<unsigned int>(scale * 0.4f));
                label.setFillColor(sf::Color::White);
                label.setOrigin(label.getLocalBounds().width / 2.f, label.getLocalBounds().height);
                label.setPosition(screenPos.x, screenPos.y + scale * 0.15f);

                window.draw(label);
            }
        }
    }


};
//
//int main() {
//    sf::RenderWindow window(sf::VideoMode(800, 600), "Ray Orbiting Grid");
//    window.setFramerateLimit(60);
//
//    sf::Font font;
//    if (!font.loadFromFile("C:/Windows/Fonts/arial.ttf")) {
//        std::cerr << "Failed to load font!\n";
//        return -1;
//    }
//
//    Grid grid(3, { 0, 0 }, 1, 50.f);
//    Ray ray({ 0, std::sqrt(3.f) }, { 0, -1 }, std::sqrt(3.f), grid.getScale());
//
//    std::set<std::string> loggedRankPatterns;
//    std::ofstream logFile("rank_log.txt", std::ios::app);
//
//    while (window.isOpen()) {
//        sf::Event event;
//        while (window.pollEvent(event)) {
//            if (event.type == sf::Event::Closed)
//                window.close();
//        }
//
//        std::ostringstream oss;
//        oss << "RAYS NORMAL : (" << ray.normal.x << ", " << ray.normal.y << ")  ORDER : { ";
//        for (int i = 0; i < grid.cells.size(); i++) {
//            oss << grid.cells[i]->rank << " , ";
//        }
//        oss << "}";
//
//        std::string entryString = oss.str();
//        std::cout << entryString;
//
//        if (loggedRankPatterns.find(entryString) == loggedRankPatterns.end()) {
//            logFile << entryString << "\n";
//            loggedRankPatterns.insert(entryString);
//        }
//
//        ray.handleInput();
//
//        window.clear(sf::Color::Black);
//        ray.colorByDistanceToLine(window, grid);
//        ray.labelCellsByDistanceToLine(window, grid, font);
//        grid.draw(&window);
//        ray.draw(window);
//        window.display();
//
//        std::cout << std::endl;
//    }
//
//    return 0;
//}

int main() {
    sf::RenderWindow window(sf::VideoMode(800, 600), "Ray Orbiting Grid");
    window.setFramerateLimit(60);

    sf::Font font;
    if (!font.loadFromFile("C:/Windows/Fonts/arial.ttf")) {
        std::cerr << "Failed to load font!\n";
        return -1;
    }

    Grid grid(3, { 0, 0 }, 1, 50.f);
    Ray ray({ 0, std::sqrt(3.f) }, { 0, -1 }, std::sqrt(3.f), grid.getScale());

    std::set<std::string> loggedOrders;
    std::ofstream logFile("permutation_log.txt", std::ios::app);

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        std::ostringstream orderOnly;
        orderOnly << "ORDER : { ";
        for (int i = 0; i < grid.cells.size(); i++) {
            orderOnly << grid.cells[i]->rank << " , ";
        }
        orderOnly << "}";

        std::string orderStr = orderOnly.str();
        std::cout << orderStr;

        if (loggedOrders.find(orderStr) == loggedOrders.end()) {
            logFile << orderStr << "\n";
            loggedOrders.insert(orderStr);
        }

        ray.handleInput();

        window.clear(sf::Color::Black);
        ray.colorByDistanceToLine(window, grid);
        ray.labelCellsByDistanceToLine(window, grid, font);
        grid.draw(&window);
        ray.draw(window);
        window.display();

        std::cout << std::endl;
    }

    return 0;
}
