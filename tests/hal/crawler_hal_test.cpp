#include "../../hal/crawler_hal.h"
#include <stdio.h>
#include <termios.h>
#include <unistd.h>
#include <string>
#include "../../utils/filesystem.h"

class DriveSetting
{
public:
    int headingAngle;
    int movingPower;

    DriveSetting()
    {
        headingAngle = 45;
        movingPower = 0;
    }
};

char menu(DriveSetting &settings, bool lastAck)
{
    std::string hs;
    std::string ms;
    std::string ack;

    if (settings.headingAngle >= 45)
        hs = "[+]";
    else
        hs = "[-]";

    if (settings.headingAngle >= 0)
        ms = "[+]";
    else
        ms = "[-]";

    if (lastAck)
        ack = "ACK";
    else
        ack = "-";

    // system("clear");
    printf("Hardware testing menu\n\n");
    printf("                  (w)       forward pwr++\n");
    printf("left pwr++   (a)       (d)      right pwr++\n");
    printf("                  (s)       backward pwr++\n");
    printf("\n\n");
    printf(">>>> moving wheeldrive: H: %s, power: %d\n", ms.c_str(), settings.movingPower);
    printf("<--> heading wheeldrive: H: %s, angle: %d\n", hs.c_str(), settings.headingAngle);
    printf("\n\n");
    printf("last command status: %s\n", ack.c_str());
    printf("\n\n");
    printf("(q) stop\n");
    printf("(Esc) to quit\n\n");
    return getchar();
}

unsigned int setupTerminal()
{
    static struct termios term_flags;
    tcgetattr(STDIN_FILENO, &term_flags);
    unsigned int oldFlags = term_flags.c_lflag;
    // newt = oldt;
    term_flags.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &term_flags);
    return oldFlags;
}

void restoreTerminal(int oldFlags)
{
    static struct termios term_flags;
    tcgetattr(STDIN_FILENO, &term_flags);
    term_flags.c_lflag = oldFlags;
    tcsetattr(STDIN_FILENO, TCSANOW, &term_flags);
}

static struct termios oldt;

int main(int argc, char **argv)
{
    if (!fileExists("/dev/ttyUSB0")) {
        printf("/dev/ttyUSB0 not found" );
        return 1;
    }


    auto flags = setupTerminal();
    DriveSetting settings;
    CrawlerHAL hal;
    bool run = true;
    bool stop = false;
    bool lastAck = false;

    while (run)
    {
        char ch = menu(settings, lastAck);
        stop = false;

        switch (ch)
        {
        case 'w':
            settings.movingPower+=25;
            if (settings.movingPower > 255)
                settings.movingPower = 255;
            break;
        case 's':
            settings.movingPower-=25;
            if (settings.movingPower < -255)
                settings.movingPower = -255;
            break;
        case 'a':
            settings.headingAngle-=5;
            if (settings.headingAngle < 0)
                settings.headingAngle = 0;
            break;
        case 'd':
            settings.headingAngle+=5;
            if (settings.headingAngle > 90)
                settings.headingAngle = 90;
            break;
        case 'q':
            settings.headingAngle = 45;
            settings.movingPower = 0;
            break;
        case 27:
            run = false;
            break;
        default:
            break;
        }

        if (!run)
            break;

        if (settings.movingPower >= 0)
            lastAck = hal.setEngineForward(settings.movingPower);
        else 
            lastAck = hal.setEngineBackward(-settings.movingPower);

        lastAck = hal.setWheelFrontAngle(settings.headingAngle);

        lastAck = hal.setWheelBackAngle(settings.headingAngle);

    }

    restoreTerminal(flags);
    return 0;
}