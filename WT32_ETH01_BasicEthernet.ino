/**
 * A very simple demonstration of the ETH library for the WT32-ETH01 devkit.
 * This sketch will attempt to connect to a HTTP server, and if successful, will print the response from that server to the serial port.
 */

#include <ETH.h>

unsigned long lastTestTime       = 0;                    // The time of the last test.
const unsigned long testInterval = 10000;                // The time in milliseconds between tests.
const unsigned int serverPort    = 1880;                 // The network port of the server to test against.
const char *serverAddress        = "theocho.local";      // The network address of the server to test against.
const char *HOSTNAME             = "WT32-ETH01";         // The device hostname.
char macAddress[18]              = "AA:BB:CC:00:11:22";  // A character array to hold the MAC address and a null terminator.
char ipAddress[16]               = "127.0.0.1";          // A character array to hold the IP address and a null terminator.
char duplex[12]                  = "HALF_DUPLEX";        // A character array to hold the link duplex state and a null terminator.
bool eth_connected               = false;                // A flag to indicate the connection state.
uint8_t linkSpeed                = 42;                   // This default value helps identify when the real value has not been assigned yet.


/**
 * @brief This is a callback function to handle network events.
 * @param event a pre-defined event to process.
 * Even though the event type is WiFiEvent_t, the same events work with the ETH.h library.
 */
void NetworkEvent( WiFiEvent_t event )
{
   networkCallbackCount++;
   switch( event )
   {
      case ARDUINO_EVENT_ETH_START:
         Serial.println( "ETH Started" );
         ETH.setHostname( HOSTNAME );
         break;
      case ARDUINO_EVENT_ETH_CONNECTED:
         Serial.println( "ETH Connected" );
         break;
      case ARDUINO_EVENT_ETH_GOT_IP:
         snprintf( macAddress, 18, "%s", ETH.macAddress().c_str() );
         snprintf( ipAddress, 16, "%d.%d.%d.%d", ETH.localIP()[0], ETH.localIP()[1], ETH.localIP()[2], ETH.localIP()[3] );
         if( ETH.fullDuplex() )
            snprintf( duplex, 12, "%s", "FULL_DUPLEX" );
         else
            snprintf( duplex, 12, "%s", "HALF_DUPLEX" );
         linkSpeed = ETH.linkSpeed();
         Serial.printf( "ETH MAC: %s, IPv4: %s, %s, %u Mbps", macAddress, ipAddress, duplex, linkSpeed );
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
}  // End of the NetworkEvent() callback function.


/**
 * @brief The testClient() function will attempt to connect to a server and verify the connection succeeds.
 * @param host the server address.
 * @param port the server port.
 */
void testClient( const char *host, const unsigned int port )
{
   Serial.printf( "\nConnecting to %s\n", host );
   Serial.printf( "ESP32 IP address: %s\n", ipAddress );

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

   Serial.println( "Closing connection\n" );
   client.stop();
}  // End of the testClient() function.


void setup()
{
   delay( 1000 );
   Serial.begin( 115200 );
   if( !Serial )
      delay( 1000 );
   Serial.println( "\n" );
   Serial.println( "Function setup() is beginning." );
   WiFi.onEvent( NetworkEvent );
   ETH.begin();
   // This delay give the Ethernet hardware time to initialize.
   delay( 300 );
}  // End of the setup() function.


void loop()
{
   unsigned long currentTime = millis();
   if( eth_connected && ( lastTestTime == 0 || ( currentTime > testInterval && ( currentTime - testInterval ) > lastTestTime ) ) )
   {
      testClient( serverAddress, serverPort );
      lastTestTime = millis();
   }
}  // End of the loop() function.
