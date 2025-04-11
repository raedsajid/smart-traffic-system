#pragma once
#include "classes.h" 
#include<random>
#include <iostream>

void shufflePaths(std::vector<std::string>& paths, unsigned int seed) {
    std::mt19937 gen(seed); 
    std::shuffle(paths.begin(), paths.end(), gen); // Shuffle the vector
}

RenderVehicle createRenderVehicle(const Vehicle& vehicle, const std::string& direction,
    float leftLaneX, float rightLaneX, float leftLaneY, float rightLaneY) {
    RenderVehicle renderVehicle;
    sf::Texture* texture = new sf::Texture();

    if (!texture->loadFromFile(vehicle.filePath)) {
        std::cerr << "Error loading file: " << vehicle.filePath << "\n";
        delete texture;
        return renderVehicle;
    }

    renderVehicle.sprite.setTexture(*texture);
    renderVehicle.direction = direction;

    // Assign lanes
    if (vehicle.type == "Heavy") {
        // Heavy vehicles left lane only
        renderVehicle.sprite.setPosition(leftLaneX, leftLaneY);
    }
    else {
        // Light and Emergency vehicles randomly use left or right lane
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> laneDist(0, 1);

        if (laneDist(gen) == 0) {
            renderVehicle.sprite.setPosition(leftLaneX, leftLaneY);
        }
        else {
            renderVehicle.sprite.setPosition(rightLaneX, rightLaneY);
        }
    }


    if (vehicle.type == "Light") {
        renderVehicle.speed = 40.f + (std::rand() % 21);
        renderVehicle.maxSpeed = 60.f;
    }
    else if (vehicle.type == "Heavy") {
        renderVehicle.speed = 30.f + (std::rand() % 11);
        renderVehicle.maxSpeed = 40.f;
    }
    else if (vehicle.type == "Emergency") {
        renderVehicle.speed = 60.f + (std::rand() % 21);
        renderVehicle.maxSpeed = 80.f;
    }

    return renderVehicle;
}
