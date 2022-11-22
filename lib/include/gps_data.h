#ifndef _GPS_DATA_H
#define _GPS_DATA_H

class GPSData
{
private:
    /* data */
public:
    float lat;
    float lon;

    GPSData *clone()
    {
        GPSData *p = new GPSData();
        p->lat = lat;
        p->lon = lon;
        return p;
    }

    const char *toJson()
    {
        std::stringstream ss;
        ss << "{\n";
        ss << "'lat' : " << lat << "\n";
        ss << "'lon' : " << lon << "\n";
        ss << "}\n";
        return ss.str().c_str();
    }
};

#endif