#include <ESP8266WebServer.h>

class WWWServer
{   
    int ServerPort;

public:

    ESP8266WebServer server; 

    WWWServer(int port)
    {
        ServerPort = port;
        server = ESP8266WebServer(port); //instantiate local web sevrer on port
    }    

    void webHandleRoot() 
    {
        String message = "Plastic Mickey - v1.0\n\n";
        message += "Created by Paul O'Brien";

        message += "------------------------------------------------------";
        message += "";
        message += "/goleft";
        message += "/goleft?duration=2000";
        message += "";

        server.send(200, "text/plain", message);
    }

    void webHandleNotFound() 
    {
        String message = "File Not Found\n\n";
        message += "URI: ";
        message += server.uri();
        message += "\nMethod: ";
        message += (server.method() == HTTP_GET) ? "GET" : "POST";
        message += "\nArguments: ";
        message += server.args();
        message += "\n";
        for (uint8_t i = 0; i < server.args(); i++) 
        {
            message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
        }
        server.send(404, "text/plain", message);
    }


    void Initialize()
    {
        server.on("/", std::bind(&WWWServer::webHandleRoot, this));
        server.onNotFound(std::bind(&WWWServer::webHandleNotFound, this));
        server.begin();
    }
};