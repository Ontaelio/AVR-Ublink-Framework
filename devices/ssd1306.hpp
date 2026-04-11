

#ifndef UB_SSD1306_H
#define UB_SSD1306_H


#include <i2c.hpp>


#define SEQUENTIAL_PINS   0x00
#define ALTERNATIVE_PINS  0x10
#define DISABLE_COM_REMAP 0x00
#define ENABLE_COM_REMAP  0x20

#define SCROLL_SLOWEST 0b011
#define SCROLL_SLOWER  0b010
#define SCROLL_SLOW    0b001 
#define SCROLL_MIDS    0b110
#define SCROLL_MIDF    0b000
#define SCROLL_FAST    0b101
#define SCROLL_FASTER  0b100
#define SCROLL_FASTEST 0b111

class ssd1306 {
private:
    Twi display;

public:

    ssd1306(Twi dev) : display(dev) {}
    ssd1306(uint8_t addr) : display(addr) {}

    void sendCommand(uint8_t cmd) {
        display.startWrite()
            .write(0x00)
            .write(cmd)
            .stop();
    }

    void sendCommand(uint8_t opCode, uint8_t param1) {
        display.startWrite()
            .write(0x00)
            .write(opCode)
            .write(param1)
            .stop();
    }

    void sendCommand(uint8_t opCode, uint8_t param1, int8_t param2) {
        display.startWrite()
            .write(0x00)
            .write(opCode)
            .write(param1)
            .write(param2)
            .stop();
    }

    void sendData(uint8_t* dat, uint16_t len){
        display.startWrite()
               .write(0x40)
               .writeStream(dat, len)
               .stop();
    }

    // display control
    ssd1306& on() {sendCommand(0xAF); return *this;}
    ssd1306& off() {sendCommand(0xAE); return *this;}
    ssd1306& ignoreRAM() {sendCommand(0xA5); return *this;}
    ssd1306& followRAM() {sendCommand(0xA4); return *this;}
    ssd1306& normalDisplay() {sendCommand(0xA6); return *this;}
    ssd1306& inverseDisplay() {sendCommand(0xA7); return *this;}
    ssd1306& contrast(uint8_t c = 0x7F) {sendCommand(0x81, c); return *this;}

    // scrolling
    ssd1306& scrollRight(uint8_t startPage, uint8_t endPage, uint8_t speed){
        display.startWrite()
               .write(0x00)
               .write(0x26)
               .write(0x00)
               .write(startPage)
               .write(speed)
               .write(endPage)
               .write(0x00)
               .write(0xFF)
               .stop();
        return *this;
    }

    ssd1306& scrollLeft(uint8_t startPage, uint8_t endPage, uint8_t speed){
        display.startWrite()
               .write(0x00)
               .write(0x27)
               .write(0x00)
               .write(startPage)
               .write(speed)
               .write(endPage)
               .write(0x00)
               .write(0xFF)
               .stop();
        return *this;
    }

    ssd1306& verticalScrollRight(uint8_t startPage, uint8_t endPage, uint8_t offset, uint8_t speed){
        display.startWrite()
               .write(0x00)
               .write(0x29)
               .write(0x00)
               .write(startPage)
               .write(speed)
               .write(endPage)
               .write(offset)
               .stop();
        return *this;
    }

    ssd1306& verticalScrollLeft(uint8_t startPage, uint8_t endPage, uint8_t offset, uint8_t speed){
        display.startWrite()
               .write(0x00)
               .write(0x2A)
               .write(0x00)
               .write(startPage)
               .write(speed)
               .write(endPage)
               .write(offset)
               .stop();
        return *this;
    }

    ssd1306& scrollArea(uint8_t top = 0, uint8_t rows = 64) {
        sendCommand(0xA3, top, rows);
        return *this;
    }
    
    ssd1306& scrollStart() {sendCommand(0x2F); return *this;}
    ssd1306& scrollStop() {sendCommand(0x2E); return *this;}
    
    // hardware configuration
    ssd1306& multiplexRatio(uint8_t mr = 0x3F) {sendCommand(0xA8, mr); return *this;}
    ssd1306& displayOffset(uint8_t dof = 0x00) {sendCommand(0xD3, dof); return *this;}
    ssd1306& startLine(uint8_t dsl = 0x00) {sendCommand(0x40 + dsl); return *this;}
    ssd1306& segmentRemap() {sendCommand(0xA1); return *this;} // invert X
    ssd1306& noSegmentRemap() {sendCommand(0xA0); return *this;} // normal X
    ssd1306& commonRemap() {sendCommand(0xC8); return *this;} // invert Y
    ssd1306& noCommonRemap() {sendCommand(0xC0); return *this;} // normal Y
    ssd1306& invertX() {sendCommand(0xA1); return *this;} // same as above
    ssd1306& normalX() {sendCommand(0xA0); return *this;} // same as above
    ssd1306& invertY() {sendCommand(0xC8); return *this;} // same as above
    ssd1306& normalY() {sendCommand(0xC0); return *this;} // same as above
    ssd1306& configureCOMpins(uint8_t pincfg = (ALTERNATIVE_PINS | DISABLE_COM_REMAP)) {
        sendCommand(0xDA, (pincfg | 0x02)); return *this;
    }

    ssd1306& clockDivideAndFrequency(uint8_t div = 0, uint8_t freq = 0x08) {
        sendCommand(0xD5, ((freq << 4) | (div & 0x0F)));
        return *this;
    }

    ssd1306& prechargePeriod(uint8_t phase1 = 2, uint8_t phase2 = 2) {
        sendCommand(0xD9, ((phase2 << 4) | (phase1 & 0x0F)));
        return *this;
    }
    
    ssd1306& deselectLevel(uint8_t level = 0b010) {
        sendCommand(0xDB, (level << 4));
        return *this;
    }

    ssd1306& chargePump(uint8_t cpe = 1) {
        if (cpe == 0) sendCommand(0xAE);
        sendCommand(0x8D, (0x10 | (cpe << 2))); return *this;
    }
    
    ssd1306& nop() {sendCommand(0xE3); return *this;}

    // addressing
    ssd1306& horizontalMode() {sendCommand(0x20, 0x00); return *this;}
    ssd1306& verticalMode() {sendCommand(0x20, 0x01); return *this;}
    ssd1306& pageMode() {sendCommand(0x20, 0x02); return *this;}
    ssd1306& columnRange(uint8_t cstart = 0, uint8_t cend = 127) {sendCommand(0x21, cstart, cend); return *this;}
    ssd1306& pageRange(uint8_t pstart = 0, uint8_t pend = 7) {sendCommand(0x22, pstart, pend); return *this;}
    
    // page mode
    ssd1306& page(uint8_t pa) {sendCommand(0xB0 | pa); return *this;}
    ssd1306& column(uint8_t col) {
        sendCommand(0x00 | (col & 0x0F));
        sendCommand(0x10 | (col >> 4)); 
        return *this;
    }
    ssd1306& clearPage(uint8_t pag){
        page(pag).column(0);
        display.startWrite().write(0x40);
        for (uint8_t k = 0; k < 128; k++){
            display.write(0);
        }
        display.stop();
        return *this;
    }
    ssd1306& clearPages(){
        for (uint8_t p = 0; p < 8; p++) clearPage(p);
        return *this;
    }

    // horizontal mode
    ssd1306& cls() {
        columnRange(0, 127);
        pageRange(0, 7);
        display.startWrite().write(0x40);
        for (uint16_t k = 0; k<1024; k++){
            display.write(0x00);
        }
        display.stop();
        return *this;               
    }

    ssd1306& displayInit(){
         off()
        .chargePump(1);        // enable   
        return *this;
    }

    ssd1306& reset(){
         off()
        .scrollStop()          // stop scrolling
        .multiplexRatio(0x3F)  // default
        .displayOffset(0)      // default
        .startLine(0)          // default
        .noSegmentRemap()      // default, don't invert X
        .noCommonRemap()       // default, don't invert Y
        .configureCOMpins(ALTERNATIVE_PINS | DISABLE_COM_REMAP) // default
        .contrast(0x7F)        // default
        .followRAM()           // default
        .normalDisplay()       // default - not inverted 
        .contrast(0x7F)        // default
        .pageMode()            // default
        .columnRange()         // restore defaults
        .pageRange()           // restore defaults
        .clearPages()          // clear screen in Page mode
        .clockDivideAndFrequency(0x00, 0x08) // default
        .chargePump(0);         // disable   
        return *this;
    }


};



#endif // UB_SSD1306_H