#ifndef _SERVERCONFIGURATION_H
#define _SERVERCONFIGURATION_H

class ServerConfiguration
{
private:

protected:
    virtual void setup();
    virtual void loop();

public:
    ServerConfiguration();
};

#endif