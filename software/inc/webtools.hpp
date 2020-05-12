/** 
 * Let There Be Light project
 * 
 * @file webtools.hpp
 */

/* Includes -------------------------------------------- */
#include <string>

/* Defines --------------------------------------------- */

/* Forward declarations -------------------------------- */
class WiFiClient;

/* Type definitions ------------------------------------ */

/* Variable declarations ------------------------------- */

/* Functions ------------------------------------------- */
int acceptRequest(WiFiClient * const pClient, 
    std::string * const pCurrentLine, 
    std::string * const pHeader);
