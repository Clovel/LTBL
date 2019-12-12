/** 
 * Let There Be Light project
 * 
 * @brief Switch class definition
 * 
 * @file Switch.hpp
 */

/* Includes -------------------------------------------- */
#include "elec.hpp"

/* Defines --------------------------------------------- */

/* Class definition ------------------------------------ */
namespace elec {
    class Switch {
        public:
            Switch(const gpioPin_t &pPin);
            virtual ~Switch();

            /* Getters */
            bool isActive(void) const;
        
        protected:
        private:
            gpioPin_t mPin;
    };
}
