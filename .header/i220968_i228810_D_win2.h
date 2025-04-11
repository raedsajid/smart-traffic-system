#pragma once

int totalSeconds = 0, hours = 0, minutes = 0, second = 0;
int totalSimulatedSeconds = 0, Thours = 6, Tminutes = 0, Tseconds = 0;
const int timeScaleFactor = 180;
std::string period = "AM";

class Information {
private:
    sf::Text info1;
    sf::Text info2;
    sf::Font font;

public:
    Information();
    void updateSimulationTime(int hours, int minutes, int seconds);
    void updateClockTime(int hours, int minutes, int seconds, const std::string& period);
    void draw(sf::RenderWindow& window);
};

Information::Information() {
    if (!font.loadFromFile("arial.ttf")) {
        std::cerr << "Error loading font\n";
    }
    info1.setFont(font);
    info1.setFillColor(sf::Color::White);
    info1.setCharacterSize(20);
    info1.setPosition(50, 50);

    info2.setFont(font);
    info2.setFillColor(sf::Color::White);
    info2.setCharacterSize(20);
    info2.setPosition(50, 70);
}

void Information::updateSimulationTime(int hours, int minutes, int seconds) {
    info1.setString("Simulation Time: " + std::to_string(hours) + ":" +
        (minutes < 10 ? "0" : "") + std::to_string(minutes) + ":" +
        (seconds < 10 ? "0" : "") + std::to_string(seconds));
}

void Information::updateClockTime(int hours, int minutes, int seconds, const std::string& period) {
    info2.setString("Time: " + std::to_string(hours) + ":" +
        (minutes < 10 ? "0" : "") + std::to_string(minutes) + ":" +
        (seconds < 10 ? "0" : "") + std::to_string(seconds) + " " + period);
}

void Information::draw(sf::RenderWindow& window) {
    window.draw(info1);
    window.draw(info2);
}


void initWindow2(sf::RenderWindow& window2) {
    window2.create(sf::VideoMode(300, 100), "Time");
    window2.setPosition(sf::Vector2i(1350, 167));
}


void updateWindow2Content(Information& info, int& totalSimulatedSeconds, int timeScaleFactor, sf::Clock& clock, int& Thours, int& Tminutes, int& Tseconds, std::string& period) {
    // Calculate scaled time
    totalSimulatedSeconds = static_cast<int>(clock.getElapsedTime().asSeconds() * timeScaleFactor);
    Tseconds = totalSimulatedSeconds % 60;
    Tminutes = (totalSimulatedSeconds / 60) % 60;
    int rawHours = 6 + (totalSimulatedSeconds / 3600);

    //12-hour format
    if (rawHours < 12) {
        period = "AM";
        Thours = rawHours == 0 ? 12 : rawHours;
    }
    else {
        period = "PM";
        Thours = (rawHours > 12) ? rawHours - 12 : rawHours;
    }

    info.updateClockTime(Thours, Tminutes, Tseconds, period);
}

void renderWindow2(sf::RenderWindow& window2, Information& info, int hours, int minutes, int seconds) {
    window2.clear();
    info.updateSimulationTime(hours, minutes, seconds);
    info.draw(window2);
    window2.display();
}

void calculatetime(sf::Clock clock)
{
    totalSeconds = clock.getElapsedTime().asSeconds();
    second = totalSeconds % 60;
    minutes = (totalSeconds / 60) % 60;
    hours = (totalSeconds / 3600);
}

bool isPeakHour(int hours, int minutes, const std::string& period) {
    if (period == "AM" && (hours == 7 || (hours == 8 && minutes < 30))) {
        return true; // 7:00 AM to 9:30 AM
    }
    if (period == "PM" && ((hours >= 4 && hours <= 7) || (hours == 8 && minutes < 30))) {
        return true; // 4:30 PM to 8:30 PM
    }
    return false;
}

