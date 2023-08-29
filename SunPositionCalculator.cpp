// SunPositionCalculator.cpp
#include "SunPositionCalculator.h"
#include <math.h>

SunPositionCalculator::SunPositionCalculator() {}

SunPosition SunPositionCalculator::calculateSunPosition(int year, int month, int day, int hour, int min, int sec, float lat, float lon) {
    int startIndex = month - 1;
    int days = 0;

    // Get day of the year
    int month_days[] = {0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30};
    for (int i = startIndex; i >= 0; i--) {
    days += month_days[i];
    }
    day += days;
    bool leapdays = year % 4 == 0 && (year % 400 == 0 || year % 100 != 0) && day >= 60 && !(month == 2 && day == 60);
    if (leapdays) {
        day += 1;
    }

    // Get Julian date - 2400000
    hour += min / 60 + sec / 3600; // hour plus fraction
    int delta = year - 1949;
    int leap = delta / 4; // former leapyears
    float jd = 32916.5 + delta * 365 + leap + day + hour / 24.0;

    // The input to the Astronomer's almanac is the difference between the Julian date and JD 2451545.0 (noon, 1 January 2000)
    float time = jd - 51545.0;

    // Ecliptic coordinates
    // Mean longitude
    float mnlong = 280.460 + 0.9856474 * time;
    mnlong = fmod(mnlong, 360.0);
    if (mnlong < 0) {
        mnlong += 360.0;
    }

    // Mean anomaly
    float mnanom = 357.528 + 0.9856003 * time;
    mnanom = fmod(mnanom, 360.0);
    if (mnanom < 0) {
        mnanom += 360.0;
    }
    mnanom *= deg2rad;

    // Ecliptic longitude and obliquity of the ecliptic
    float eclong = mnlong + 1.915 * sin(mnanom) + 0.020 * sin(2 * mnanom);
    eclong = fmod(eclong, 360.0);
    if (eclong < 0) {
        eclong += 360.0;
    }
    float oblqec = 23.439 - 0.0000004 * time;
    eclong *= deg2rad;
    oblqec *= deg2rad;

    // Celestial coordinates
    // Right ascension and declination
    float num = cos(oblqec) * sin(eclong);
    float den = cos(eclong);
    float ra = atan(num / den);
    if (den < 0) {
        ra += PI;
    }
    if (den >= 0 && num < 0) {
        ra += twopi;
    }
    float dec = asin(sin(oblqec) * sin(eclong));

    // Local coordinates
    // Greenwich mean sidereal time
    float gmst = 6.697375 + 0.0657098242 * time + hour;
    gmst = fmod(gmst, 24.0);
    if (gmst < 0) {
        gmst += 24.0;
    }

    // Local mean sidereal time
    float lmst = gmst + lon / 15.0;
    lmst = fmod(lmst, 24.0);
    if (lmst < 0) {
        lmst += 24.0;
    }
    lmst *= 15.0 * deg2rad;

    // Hour angle
    float ha = lmst - ra;
    if (ha < -PI) {
        ha += twopi;
    }
    if (ha > PI) {
        ha -= twopi;
    }

    // Latitude to radians
    lat *= deg2rad;

    // Azimuth and elevation
    float el = asin(sin(dec) * sin(lat) + cos(dec) * cos(lat) * cos(ha));
    float az = asin(-cos(dec) * sin(ha) / cos(el));

    // For logic and names, see Spencer, J.W. 1989. Solar Energy. 42(4):353
    bool cosAzPos = 0 <= sin(dec) - sin(el) * sin(lat);
    bool sinAzNeg = sin(az) < 0;
    if (cosAzPos && sinAzNeg) {
        az += twopi;
    } else if (!cosAzPos) {
        az = PI - az;
    }

    el /= deg2rad;
    az /= deg2rad;
    lat /= deg2rad;

    SunPosition result = {el, az, day};
    return result;
}