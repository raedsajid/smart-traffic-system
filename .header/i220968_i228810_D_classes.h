#pragma once
#include <SFML/Graphics.hpp>

class RenderVehicle {
public:
    sf::Sprite sprite;
    std::string direction;
    float speed; 
    float maxSpeed; 
    sf::Clock speedUpdateClock;

    RenderVehicle() : speed(0.f), maxSpeed(0.f) {}
};


class Vehicle {
public:
    std::string numberPlate;
    std::string type;
    std::string filePath;
    std::string challanStatus;

    Vehicle(const std::string& plate, const std::string& vehicleType, const std::string& path)
        : numberPlate(plate), type(vehicleType), filePath(path), challanStatus("Inactive") {}
};