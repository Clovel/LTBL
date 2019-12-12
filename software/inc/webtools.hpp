/** 
 * Let There Be Light project
 * 
 * @file webtools.hpp
 */

/* Includes -------------------------------------------- */

/* Defines --------------------------------------------- */

/* Type definitions ------------------------------------ */
typedef enum _restMethods {
    REST_UNKNOWN = 0,
    REST_INDEX, /* Shows the index page */
    REST_GET, /* GET is used to request data from a specified resource. */
    REST_POST, /* POST is used to send data to a server to create/update a resource. */
    REST_PUT, /* PUT is used to send data to a server to create/update a resource. */
    REST_HEAD, /* HEAD is almost identical to GET, but without the response body. */
    REST_DELETE, /* The DELETE method deletes the specified resource. */
    REST_PATCH,
    REST_OPTIONS /* The OPTIONS method describes the communication options for the target resource. */
} restMethod_t;

/* Variable declarations ------------------------------- */

/* Functions ------------------------------------------- */
int acceptRequest(const int pSocket, int * const pResult);