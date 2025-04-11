#include <SFML/Graphics.hpp>
#include <vector>
#include <thread>
#include <mutex>
#include <queue>
#include <string>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include "win2.h"
#include "VehiclePath.h"
#include<random>
#include<array>
#include"classes.h"
#include"renderVehicle.h"
#include <Windows.h>  // For pipes

int counter = 0;

using namespace std;
using namespace sf;


const int GRID_WIDTH = 776;
const int GRID_HEIGHT = 746;
const int LANE_CAPACITY = 10;

const float NORTH_LEFT_LANE_X = 320.f;
const float NORTH_RIGHT_LANE_X = 270.f;
const float NORTH_LEFT_LANE_Y = -25.f;
const float NORTH_RIGHT_LANE_Y = -25.f;


const float SOUTH_LEFT_LANE_X = 410.f;
const float SOUTH_RIGHT_LANE_X = 460.f;
const float SOUTH_LEFT_LANE_Y = 750.f;
const float SOUTH_RIGHT_LANE_Y = 750.f;


const float WEST_LEFT_LANE_X = -25.f;
const float WEST_RIGHT_LANE_X = -25.f;
const float WEST_LEFT_LANE_Y = 385.f;
const float WEST_RIGHT_LANE_Y = 445.f;


const float EAST_LEFT_LANE_X = 750.f;
const float EAST_RIGHT_LANE_X = 750.f;
const float EAST_LEFT_LANE_Y = 310.f;
const float EAST_RIGHT_LANE_Y = 255.f;

// Traffic light state values
const int GREEN = 0;
const int YELLOW = 1;
const int RED = 2;
const float GREEN_DURATION = 10.f;
const float YELLOW_DURATION = 2.f;

std::mutex renderMutex;

std::vector<RenderVehicle> NorthRenderQueue;
std::vector<RenderVehicle> SouthRenderQueue;
std::vector<RenderVehicle> EastRenderQueue;
std::vector<RenderVehicle> WestRenderQueue;


bool isColliding(const sf::Sprite& sprite1, const sf::Sprite& sprite2) {
    return sprite1.getGlobalBounds().intersects(sprite2.getGlobalBounds());
}

bool positionOccupied(const sf::Sprite& sprite, const std::vector<RenderVehicle>& renderQueue) {
    for (const auto& existingVehicle : renderQueue) {
        if (isColliding(sprite, existingVehicle.sprite)) {
            return true; 
        }
    }
    return false;
}

void vehicleArrival(const std::string& direction, float regularRate, float emergencyInterval,
    int emergencyProbability, const vector<std::string>& regularPaths, const vector<std::string>& heavyPaths,
    const vector<std::string>& emergencyPaths, float leftLaneX, float rightLaneX, float leftLaneY, float rightLaneY) {
    int vehicleID = 1;
    sf::Clock regularClock, heavyClock, emergencyClock;

    while (true) {

        std::vector<RenderVehicle>* targetQueue;
        if (direction == "North") targetQueue = &NorthRenderQueue;
        else if (direction == "South") targetQueue = &SouthRenderQueue;
        else if (direction == "East") targetQueue = &EastRenderQueue;
        else targetQueue = &WestRenderQueue;

        // Handle Emergency Vehicles
        if (emergencyClock.getElapsedTime().asSeconds() >= emergencyInterval) {
            if ((std::rand() % 100) <= emergencyProbability) {
                int randomIndex = std::rand() % emergencyPaths.size();
                Vehicle vehicle(direction + std::to_string(vehicleID++), "Emergency", emergencyPaths[randomIndex]);
                RenderVehicle renderVehicle = createRenderVehicle(vehicle, direction, leftLaneX, rightLaneX, leftLaneY, rightLaneY);
                std::lock_guard<std::mutex> lock(renderMutex);
                if (!positionOccupied(renderVehicle.sprite, *targetQueue)) {
                    targetQueue->push_back(renderVehicle);
                }
            }
            emergencyClock.restart();
        }

        // Handle Heavy Vehicles
        if (heavyClock.getElapsedTime().asSeconds() >= 15.0f) {
            if (!isPeakHour(Thours, Tminutes, period)) { // not allowed during restricted hours

                int randomIndex = std::rand() % heavyPaths.size();
                Vehicle vehicle(direction + std::to_string(vehicleID++), "Heavy", heavyPaths[randomIndex]);
                RenderVehicle renderVehicle = createRenderVehicle(vehicle, direction, leftLaneX, rightLaneX, leftLaneY, rightLaneY);
                std::lock_guard<std::mutex> lock(renderMutex);
                if (!positionOccupied(renderVehicle.sprite, *targetQueue)) {
                    targetQueue->push_back(renderVehicle);
                }
                heavyClock.restart();
                regularClock.restart(); //  1-second delay regular vehicles
            }
            else {
                heavyClock.restart();
            }
        }

        // Handle Regular Vehicles
        if (regularClock.getElapsedTime().asSeconds() >= regularRate) {
            int randomIndex = std::rand() % regularPaths.size();
            Vehicle vehicle(direction + std::to_string(vehicleID++), "Light", regularPaths[randomIndex]);
            RenderVehicle renderVehicle = createRenderVehicle(vehicle, direction, leftLaneX, rightLaneX, leftLaneY, rightLaneY);
            std::lock_guard<std::mutex> lock(renderMutex);
            if (!positionOccupied(renderVehicle.sprite, *targetQueue)) {
                targetQueue->push_back(renderVehicle);
            }
            regularClock.restart();
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
}


int main() {

    srand(static_cast<unsigned int>(time(nullptr)));
    std::random_device rd; 
    unsigned int randomSeed = rd(); 

    // Shuffle each vector
    shufflePaths(northRegular, randomSeed + 1);
    shufflePaths(northHeavy, randomSeed + 2);
    shufflePaths(northEV, randomSeed + 3);

    shufflePaths(southRegular, randomSeed + 4);
    shufflePaths(southHeavy, randomSeed + 5);
    shufflePaths(southEV, randomSeed + 6);

    shufflePaths(westRegular, randomSeed + 7);
    shufflePaths(westHeavy, randomSeed + 8);
    shufflePaths(westEV, randomSeed + 9);

    shufflePaths(eastRegular, randomSeed + 10);
    shufflePaths(eastHeavy, randomSeed + 11);
    shufflePaths(eastEV, randomSeed + 12);

    sf::RenderWindow window(sf::VideoMode(GRID_WIDTH, GRID_HEIGHT), "Smart Traffic System");
    window.setFramerateLimit(60);

    sf::RenderWindow window2;
    initWindow2(window2);

    Information info;
    sf::Clock clock;


    sf::Texture backgroundTexture;
    if (!backgroundTexture.loadFromFile("Intersection(new).png")) {
        std::cerr << "Failed to load background texture.\n";
        return -1;
    }
    sf::Sprite backgroundSprite;
    backgroundSprite.setTexture(backgroundTexture);


    sf::Texture greenNS, yellowNS, redNS;
    sf::Texture greenWest, yellowWest, redWest;
    sf::Texture greenEast, yellowEast, redEast;

    if (!greenNS.loadFromFile("TrafficLight/NS/green.png") ||
        !yellowNS.loadFromFile("TrafficLight/NS/yellow.png") ||
        !redNS.loadFromFile("TrafficLight/NS/red.png") ||
        !greenWest.loadFromFile("TrafficLight/West/green.png") ||
        !yellowWest.loadFromFile("TrafficLight/West/yellow.png") ||
        !redWest.loadFromFile("TrafficLight/West/red.png") ||
        !greenEast.loadFromFile("TrafficLight/East/green.png") ||
        !yellowEast.loadFromFile("TrafficLight/East/yellow.png") ||
        !redEast.loadFromFile("TrafficLight/East/red.png")) {
        std::cerr << "Failed to load traffic light textures.\n";
        return -1;
    }

    std::array<sf::Sprite, 4> trafficLights;
    std::array<sf::Texture*, 4> greenTextures = { &greenNS, &greenEast, &greenNS, &greenWest };
    std::array<sf::Texture*, 4> yellowTextures = { &yellowNS, &yellowEast ,&yellowNS, &yellowWest };
    std::array<sf::Texture*, 4> redTextures = { &redNS, &redEast, &redNS, &redWest };
    std::array<sf::Texture*, 4> currentTextures = { greenTextures[0], redTextures[1], redTextures[2], redTextures[3] };
    std::array<sf::Vector2f, 4> positions = {
        sf::Vector2f(520.f, 504.f),  // South
        sf::Vector2f(517.f, 209.f),  // East
        sf::Vector2f(220.f, 172.f), // North
        sf::Vector2f(187.f, 504.f), // West
    };

    for (size_t i = 0; i < trafficLights.size(); ++i) {
        trafficLights[i].setTexture(*currentTextures[i]);
        trafficLights[i].setPosition(positions[i]);
    }

    // State management
    int currentGreen = 0; // Start with south
    sf::Clock lightTimer;

    std::thread northThread(vehicleArrival, "North", 1.0f, 15.0f, 20,
        northRegular, northHeavy, northEV,
        NORTH_LEFT_LANE_X, NORTH_RIGHT_LANE_X, NORTH_LEFT_LANE_Y, NORTH_RIGHT_LANE_Y);

    std::thread southThread(vehicleArrival, "South", 2.0f, 2.0f, 5,
        southRegular, southHeavy, southEV,
        SOUTH_LEFT_LANE_X, SOUTH_RIGHT_LANE_X, SOUTH_LEFT_LANE_Y, SOUTH_RIGHT_LANE_Y);

    std::thread eastThread(vehicleArrival, "East", 1.5f, 20.0f, 10,
        eastRegular, eastHeavy, eastEV,
        EAST_LEFT_LANE_X, EAST_RIGHT_LANE_X, EAST_LEFT_LANE_Y, EAST_RIGHT_LANE_Y);

    std::thread westThread(vehicleArrival, "West", 2.0f, 2.0f, 30,
        westRegular, westHeavy, westEV,
        WEST_LEFT_LANE_X, WEST_RIGHT_LANE_X, WEST_LEFT_LANE_Y, WEST_RIGHT_LANE_Y);

    northThread.detach();
    southThread.detach();
    westThread.detach();
    eastThread.detach();

    sf::Clock deltaClock;
    sf::Clock simulationClock;

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }
        }

        if (simulationClock.getElapsedTime().asSeconds() >= 300.f) {
            std::cout << "Simulation ended.\n";
            window.close(); 
            break;         
        }
        
        calculatetime(clock);

        // Update and render window2
        updateWindow2Content(info, totalSimulatedSeconds, timeScaleFactor, clock, Thours, Tminutes, Tseconds, period);
        renderWindow2(window2, info, hours, minutes, second);

        while (window2.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window2.close();
        }

        float deltaTime = deltaClock.restart().asSeconds();

        float elapsedTime = lightTimer.getElapsedTime().asSeconds();
        if (elapsedTime >= GREEN_DURATION) {
            // Switch the current green light to yellow
            trafficLights[currentGreen].setTexture(*yellowTextures[currentGreen]);
            currentTextures[currentGreen] = yellowTextures[currentGreen];

            if (elapsedTime >= GREEN_DURATION + YELLOW_DURATION) {
                // Move to the next light
                trafficLights[currentGreen].setTexture(*redTextures[currentGreen]);
                currentTextures[currentGreen] = redTextures[currentGreen];

                currentGreen = (currentGreen + 1) % 4; // Cycle to the next light
                trafficLights[currentGreen].setTexture(*greenTextures[currentGreen]);
                currentTextures[currentGreen] = greenTextures[currentGreen];

                lightTimer.restart();
            }
        }

        // Update vehicle positions and speeds
        {
            std::lock_guard<std::mutex> lock(renderMutex);

            auto updateQueue = [&](std::vector<RenderVehicle>& renderQueue) {
                for (auto& renderVehicle : renderQueue) {
                    sf::Vector2f currentPosition = renderVehicle.sprite.getPosition();
                    sf::Vector2f newPosition = currentPosition;



                    // Update position based on current speed
                    if (renderVehicle.direction == "North") {
                        newPosition.y += renderVehicle.speed * deltaTime; // Use speed for movement
                    }
                    if (renderVehicle.direction == "South") {
                        newPosition.y -= renderVehicle.speed * deltaTime;
                    }
                    if (renderVehicle.direction == "West") {
                        newPosition.x += renderVehicle.speed * deltaTime;
                    }
                    if (renderVehicle.direction == "East") {
                        newPosition.x -= renderVehicle.speed * deltaTime;
                    }

                    // Skip collision checks outside grid boundaries
                    if (newPosition.x < 0 || newPosition.x > GRID_WIDTH || newPosition.y < 0 || newPosition.y > GRID_HEIGHT) {
                        renderVehicle.sprite.setPosition(newPosition); // Just update position and continue
                        continue;
                    }

                    bool collisionDetected = false;
                    for (const auto& otherVehicle : renderQueue) {
                        if (&renderVehicle != &otherVehicle) {
                            sf::Sprite testSprite = renderVehicle.sprite;
                            testSprite.setPosition(newPosition);
                            // Check collision only if the other vehicle is within grid boundaries
                            sf::Vector2f otherPosition = otherVehicle.sprite.getPosition();
                            if (otherPosition.x >= 0 && otherPosition.x <= GRID_WIDTH &&
                                otherPosition.y >= 0 && otherPosition.y <= GRID_HEIGHT &&
                                isColliding(testSprite, otherVehicle.sprite)) {
                                collisionDetected = true;
                                break;
                            }
                        }
                    }

                    if (!collisionDetected) {
                        if (currentGreen == 0) { //0 is the index for South
                            renderVehicle.sprite.setPosition(newPosition);
                        }
                        else if (currentPosition.y >= 540.f && currentPosition.y < 545.f && currentPosition.x > 390.f) {
                                //renderVehicle.sprite.setPosition(newPosition);
                            renderVehicle.sprite.setPosition(currentPosition);
                            continue;
                        }

                        if (currentGreen == 1) { 
                            renderVehicle.sprite.setPosition(newPosition);
                        }
                        else if (currentPosition.x < 555.f && currentPosition.x >= 550.f && currentPosition.y < 360.f) {
                            //renderVehicle.sprite.setPosition(newPosition);
                            renderVehicle.sprite.setPosition(currentPosition);
                            continue;
                        }

                        if (currentGreen == 2) { 
                            renderVehicle.sprite.setPosition(newPosition);
                        }
                        else if (currentPosition.y < 165.f && currentPosition.y >= 160.f && currentPosition.x < 375.f) {
                            //renderVehicle.sprite.setPosition(newPosition);
                            renderVehicle.sprite.setPosition(currentPosition);
                            continue;
                        }

                        if (currentGreen == 3) { // Assuming 0 is the index for South
                            renderVehicle.sprite.setPosition(newPosition);
                        }
                        else if (currentPosition.x < 190.f && currentPosition.x >= 185.f && currentPosition.y > 370.f) {
                            //renderVehicle.sprite.setPosition(newPosition);
                            renderVehicle.sprite.setPosition(currentPosition);
                            continue;
                        }
                        
                    }

                    // Increase speed every 5 seconds without any limit
                    if (renderVehicle.speedUpdateClock.getElapsedTime().asSeconds() >= 5.f) {
                        renderVehicle.speed += 5.f; // Increase speed by 5 km/h
                        renderVehicle.speedUpdateClock.restart(); // Reset the clock
                    }
                }

                renderQueue.erase(std::remove_if(renderQueue.begin(), renderQueue.end(),
                    [](RenderVehicle& rv) {
                        sf::Vector2f pos = rv.sprite.getPosition();
                        bool outOfBounds = pos.x < -100 || pos.x > GRID_WIDTH + 100 || pos.y < -100 || pos.y > GRID_HEIGHT + 100;

                        if (outOfBounds) {
                            // Check if speed exceeds maxSpeed
                            if (rv.speed > rv.maxSpeed) {
                                std::cout << "Vehicle going out of bounds with speed "
                                    << rv.speed << " km/h exceeds max limit of "
                                    << rv.maxSpeed << " km/h. Challan status: ACTIVE.\n";
                            }
                            else {
                                std::cout << "Vehicle going out of bounds with speed "
                                    << rv.speed << " km/h is within speed limit. Challan status: INACTIVE.\n";
                            }

                            delete rv.sprite.getTexture();
                        }

                        return outOfBounds;
                    }),
                    renderQueue.end());

            };

            updateQueue(NorthRenderQueue);
            updateQueue(SouthRenderQueue);
            updateQueue(EastRenderQueue);
            updateQueue(WestRenderQueue);
        }

        window.clear();
        window.draw(backgroundSprite);
       
        {
            std::lock_guard<std::mutex> lock(renderMutex);

            for (const auto& renderVehicle : NorthRenderQueue) window.draw(renderVehicle.sprite);
            for (const auto& renderVehicle : SouthRenderQueue) window.draw(renderVehicle.sprite);
            for (const auto& renderVehicle : EastRenderQueue) window.draw(renderVehicle.sprite);
            for (const auto& renderVehicle : WestRenderQueue) window.draw(renderVehicle.sprite);
        }


        for (const auto& light : trafficLights) {
            window.draw(light);
        }

        window.display();
    }

    return 0;
}
