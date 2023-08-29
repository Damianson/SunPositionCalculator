// SunPositionCalculator.h
#ifndef SunPositionCalculator_h
#define SunPositionCalculator_h

#include <Arduino.h>

struct SunPosition {
    float elevation;
    float azimuth;
    int day;
};

class SunPositionCalculator {
public:
    SunPositionCalculator();
    SunPosition calculateSunPosition(int year, int month, int day, int hour, int min, int sec, float lat, float lon);

private:
    const float twopi = 2 * PI;
    const float deg2rad = PI / 180.0;
};

#endif
