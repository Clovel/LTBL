# Serial port read w/ terminal
`sudo cu -l /dev/cu.usbserial-1420 -s 115200`

# Prohibited functions/classes
`std::string::erase` - `Fatal exception 9(LoadStoreAlignmentCause)`
`std::stringstream` - `Fatal exception 9(LoadStoreAlignmentCause)`
