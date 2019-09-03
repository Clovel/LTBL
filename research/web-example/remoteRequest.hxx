/** 
 * Copyright Clovis Durand
 * Let There Be Light project
 * 
 * @file acceptRequest.hxx
 */

/* Includes -------------------------------------------- */

/* Defines --------------------------------------------- */

/* Type definitions ------------------------------------ */
typedef enum _restMethods {
    REST_UNKNOWN = 0,
    REST_INDEX,
    REST_GET,
    REST_POST,
    REST_PUT
} restMethod_t;

/* Variable declarations ------------------------------- */

/* Functions ------------------------------------------- */
int acceptRequest(const int pSocket, int * const pResult);