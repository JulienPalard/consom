#include "EmonLib.h"
#include <SPI.h>
#include <Ethernet.h>


EnergyMonitor emon1;
byte mac[] = { 0x98, 0xFC, 0x3B, 0x56, 0xC6, 0x71 };
EthernetServer server(80);
EthernetClient client;

float last_measured_watt = 0.0;
float last_measured_amp = 0.0;

void setup() {
    Serial.begin(9600);
    while (Ethernet.begin(mac) == 0) {
        Serial.println("Failed to configure Ethernet using DHCP");
        delay(10000);
    }
    server.begin();
    emon1.current(1, 111.1);
}

void loop() {
    last_measured_amp = emon1.calcIrms(1480);  // Calculate Irms only
    last_measured_watt = last_measured_amp * 240.0;
    listenForEthernetClients();
}

void listenForEthernetClients() {
    // listen for incoming clients
    EthernetClient client = server.available();
    if (client) {
        Serial.println("Got a client");
        // an http request ends with a blank line
        boolean currentLineIsBlank = true;
        while (client.connected()) {
            if (client.available()) {
                char c = client.read();
                // if you've gotten to the end of the line (received a newline
                // character) and the line is blank, the http request has ended,
                // so you can send a reply
                if (c == '\n' && currentLineIsBlank) {
                    client.println("HTTP/1.1 200 OK");
                    client.println("Content-Type: application/json");
                    client.println();
                    client.print("{\"amp\": ");
                    client.print(last_measured_amp);
                    client.print(", \"watt\": ");
                    client.print(last_measured_watt);
                    client.println("}");
                    break;
                }
                if (c == '\n') {
                    // you're starting a new line
                    currentLineIsBlank = true;
                } else if (c != '\r') {
                    // you've gotten a character on the current line
                    currentLineIsBlank = false;
                }
            }
        }
        delay(1);
        client.stop();
    }
}
