/**
 * A very simple demonstration of the ETH library for the WT32-ETH01 devkit.
 * This sketch will attempt to connect to a HTTP server, and if successful, will print the response from that server to the serial port.
 */

#include <ETH.h>

static bool eth_connected        = false;
unsigned long lastTestTime       = 0;
const unsigned long testInterval = 10000;
const unsigned int serverPort    = 1880;
const char *serverAddress        = "theocho.local";


/**
 * @brief This is a callback function to handle network events.
 * @param event a pre-defined event to process.
 * Even though the event type is WiFiEvent_t, the same events work with the ETH.h library.
 */
void NetworkEvent( WiFiEvent_t event )
{
	switch( event )
	{
		case ARDUINO_EVENT_ETH_START:
			Serial.println( "ETH Started" );
			//set eth hostname here
			ETH.setHostname( "esp32-ethernet" );
			break;
		case ARDUINO_EVENT_ETH_CONNECTED:
			Serial.println( "ETH Connected" );
			break;
		case ARDUINO_EVENT_ETH_GOT_IP:
			Serial.print( "ETH MAC: " );
			Serial.print( ETH.macAddress() );
			Serial.print( ", IPv4: " );
			Serial.print( ETH.localIP() );
			if( ETH.fullDuplex() )
			{
				Serial.print( ", FULL_DUPLEX" );
			}
			Serial.print( ", " );
			Serial.print( ETH.linkSpeed() );
			Serial.println( "Mbps" );
			eth_connected = true;
			break;
		case ARDUINO_EVENT_ETH_DISCONNECTED:
			Serial.println( "ETH Disconnected" );
			eth_connected = false;
			break;
		case ARDUINO_EVENT_ETH_STOP:
			Serial.println( "ETH Stopped" );
			eth_connected = false;
			break;
		default:
			break;
	}
}


/**
 * @brief The testClient() function will attempt to connect to a server and verify the connection succeeds.
 * @param host the server address.
 * @param port the server port.
 */
void testClient( const char *host, const unsigned int port )
{
	Serial.print( "\nConnecting to " );
	Serial.println( host );

	WiFiClient client;
	if( !client.connect( host, port ) )
	{
		Serial.println( "connection failed" );
		return;
	}
	client.printf( "GET / HTTP/1.1\r\nHost: %s\r\n\r\n", host );
	while( client.connected() && !client.available() )
		;
	while( client.available() )
	{
		Serial.write( client.read() );
	}

	Serial.println( "closing connection\n" );
	client.stop();
}


void setup()
{
	Serial.begin( 115200 );
	WiFi.onEvent( NetworkEvent );
	ETH.begin();
}


void loop()
{
	unsigned long currentTime = millis();
	if( eth_connected && ( lastTestTime == 0 || ( currentTime > testInterval && ( currentTime - testInterval ) > lastTestTime ) ) )
	{
		testClient( serverAddress, serverPort );
		lastTestTime = millis();
	}
}
