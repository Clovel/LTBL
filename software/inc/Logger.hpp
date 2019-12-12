/** 
 * Let There Be Light project
 * 
 * @file Logger.hpp
 */

/* Includes -------------------------------------------- */

/* Defines --------------------------------------------- */

/* Forward declarations -------------------------------- */

/* Class declaration ----------------------------------- */
#ifndef TESTS
class Logger {
    public:
        //Logger();
        //virtual ~Logger();
    protected:
    private:
};
#else /* TESTS */
using Logger = std::cout;
#endif /* TESTS */
