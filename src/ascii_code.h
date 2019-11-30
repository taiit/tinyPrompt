#ifndef __ASCII_CODE_H__
#define __ASCII_CODE_H__

// ASCII control characters (character code 0-31).
#define ASCII_CODE_NUL      0x0     // Null char.
#define ASCII_CODE_SOH      0x1     // Start of Heading.
#define ASCII_CODE_STX      0x2     // Start of Text.
#define ASCII_CODE_ETX      0x3     // End of Text.
#define ASCII_CODE_EOT      0x4     // End of Transmission.
#define ASCII_CODE_ENQ      0x5     // Enquiry.
#define ASCII_CODE_ACK      0x6     // Acknowledgment.
#define ASCII_CODE_BEL      0x7     // Bell.
#define ASCII_CODE_BS       0x8     // Back Space.
#define ASCII_CODE_HT       0x9     // Horizontal Tab.
#define ASCII_CODE_LF       0x0A    // Line Feed.
#define ASCII_CODE_VT       0x0B    // Vertical Tab.
#define ASCII_CODE_FF       0x0C    // Form Feed.
#define ASCII_CODE_CR       0x0D    // Carriage Return.
#define ASCII_CODE_SO       0x0E    // Shift Out / X-On.
#define ASCII_CODE_SI       0x0F    // Shift In / X-Off.
#define ASCII_CODE_DLE      0x10    // Data Line Escape.
#define ASCII_CODE_DC1      0x11    // Device Control 1 (oft. XON).
#define ASCII_CODE_DC2      0x12    // Device Control 2.
#define ASCII_CODE_DC3      0x13    // Device Control 3 (oft. XOFF).
#define ASCII_CODE_DC4      0x14    // Device Control 4.
#define ASCII_CODE_NAK      0x15    // Negative Acknowledgment.
#define ASCII_CODE_SYN      0x16    // Synchronous Idle.
#define ASCII_CODE_ETB      0x17    // End of Transmit Block.
#define ASCII_CODE_CAN      0x18    // Cancel.
#define ASCII_CODE_EM       0x19    // End of Medium.
#define ASCII_CODE_SUB      0x1A    // Substitute.
#define ASCII_CODE_ESC      0x1B    // Escape.
#define ASCII_CODE_FS       0x1C    // File Separator.
#define ASCII_CODE_GS       0x1D    // Group Separator.
#define ASCII_CODE_RS       0x1E    // Record Separator.
#define ASCII_CODE_US       0x1F    // Unit Separator.

// ASCII printable characters (character code 32-127).


#define ASCII_CODE_DEL      0x7F    // 127, Delete.

#endif //__ASCII_CODE_H__
