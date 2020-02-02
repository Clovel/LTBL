/** 
 * Let There Be Light project
 * 
 * @file webpage.cpp
 */

/* Includes -------------------------------------------- */
#include "webpage.hpp"

#include "Relay.hpp"

#include "Logger.hpp"

/* Defines --------------------------------------------- */

/* Variable declarations ------------------------------- */
extern elec::Relay *gRelay;
extern Logger *gLogger;

/* Web page variables ---------------------------------- */
const char * const htmlResponseCode200 = R"=====(HTTP/1.1 200 OK
Content-type:text/html
Connection: close
)=====";

const char * const htmlResponseCode400 = R"=====(HTTP/1.0 400 BAD REQUEST
Content-Type: text/html
Connection: close

<!DOCTYPE html>
<html lang ="en">
    <head>
        <title>400 - BAD REQUEST</title>
    </head>
    <body>
        Your browser sent a bad request, 
        such as a POST without a Content-Length.
    </body>
</html>
)=====";

const char * const htmlResponseCode404 = R"=====(HTTP/1.0 404 NOT FOUND
Content-Type: text/html
Connection: close

<!DOCTYPE html>
<html lang ="en">
    <head>
        <title>404 - Not found</title>
    </head>
    <body>
        The server could not fulfill your request because the resource specified"
        is unavailable or nonexistent.
        <br>"
            Please check that you entered the correct URL.
        </br>
    </body>
</html>
)=====";

const char * const htmlResponseCode500 = R"=====(HTTP/1.0 500 Server internal error
Content-Type: text/html
Connection: close

<!DOCTYPE html>
<html lang ="en">
    <head>
        <title>500 - Server internal error</title>
    </head>
    <body>
        Server-side error prohibited execution.
    </body>
</html>
)=====";

const char * const htmlResponseCode501 = R"=====(HTTP/1.1 501 Method Not Implemented
Content-type:text/html
Connection: close

<!DOCTYPE html>
<html lang="en">
    <head>
        <title>
            501 - Method Not Implemented
        </title>
    </head>
    <body>
        HTTP request method not supported.
    </body>
</html>
)=====";

const char * const htmlDocType = R"=====(<!DOCTYPE html>
)=====";

const char * const htmlPageBegin = R"=====(<html lang="en">
)=====";

const char * const htmlPageEnd = R"=====(</html>

)=====";

// CSS to style the on/off buttons 
// Feel free to change the background-color and font-size attributes to fit your preferences
const char * const htmlHead = R"=====(
    <head>
        <meta name="viewport" content="width=device-width, initial-scale=1">
        <link rel="icon" href="data:,">
        <style>
        html {
            font-family: Helvetica;
            display: inline-block;
            margin: 0px auto;
            text-align: center;
        }
        .button {
            background-color: #195B6A;
            border: none;
            color: white;
            padding: 16px 40px;
            text-decoration: none;
            font-size: 30px;
            margin: 2px;
            cursor: pointer;
        }
        .button2 {
            background-color: #77878A;
        }
        </style>
    </head>
)=====";

const char * const htmlPageTitle = R"=====(

)=====";

const char * const htmlGeneralKenobi = R"=====(
    <html lang ="en">
        <head>
            <title>web-example</title>
        </head>
        <body>
            - Hello there !
            - General Kenobi !
        </body>
    </html>
)=====";

#if 0
const char * const index = R"=====(
<!DOCTYPE html>
<html>
    <head>
        <meta name="viewport" content="width=device-width, initial-scale=1">
        <link rel="icon" href="data:,">
        <style>
        html {
            font-family: Helvetica;
            display: inline-block;
            margin: 0px auto;
            text-align: center;
        };
        .button {
            background-color: #195B6A; border: none; color: white; padding: 16px 40px;
            text-decoration: none; font-size: 30px; margin: 2px; cursor: pointer;
        };
        .button2 {
            background-color: #77878A;
        }
        </style>
    </head>

    <body>
        <h1>
            ESP8266 Web Server - LTBL
        </h1>
        <p>
            <a href="/5/off">
                <button class="button button2">
                    OFF
                </button>
            </a>
        </p>
    </body>
</html>

)=====";

const char * const MAIN_page = R"=====(
<!DOCTYPE html>
<html>
<body>

<div id="demo">
<h1>The ESP8266 NodeMCU Update web page without refresh</h1>
	<button type="button" onclick="sendData(1)">LED ON</button>
	<button type="button" onclick="sendData(0)">LED OFF</button><BR>
</div>

<div>
	ADC Value is : <span id="ADCValue">0</span><br>
    LED State is : <span id="LEDState">NA</span>
</div>
<script>
function sendData(led) {
  var xhttp = new XMLHttpRequest();
  xhttp.onreadystatechange = function() {
    if (this.readyState == 4 && this.status == 200) {
      document.getElementById("LEDState").innerHTML =
      this.responseText;
    }
  };
  xhttp.open("GET", "setLED?LEDstate="+led, true);
  xhttp.send();
}

setInterval(function() {
  // Call a function repetatively with 2 Second interval
  getData();
}, 2000); //2000mSeconds update rate

function getData() {
  var xhttp = new XMLHttpRequest();
  xhttp.onreadystatechange = function() {
    if (this.readyState == 4 && this.status == 200) {
      document.getElementById("ADCValue").innerHTML =
      this.responseText;
    }
  };
  xhttp.open("GET", "readADC", true);
  xhttp.send();
}
</script>
<br><br><a href="https://circuits4you.com">Circuits4you.com</a>
</body>
</html>
)=====";
#endif /* 0 */
