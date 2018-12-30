#include "data.h"

#include "gfx.h"

alignas(4) const uint8_t cell_background[] = {
  0x10,0x42,0x44,0x44,0x41,0xA7,0xEE,0xEE,0x52,0xA9,0xAA,0xAA,0x64,0xBB,0xBB,0xBB,0x74,0xBB,0xBB,0xBB,0x74,0xBB,0xBB,0xBB,0x74,0xBB,0xBB,0xCC,0x74,0xBB,0xBB,0xCC,
  0x44,0x44,0x24,0x01,0xEE,0xEE,0x7A,0x14,0xAA,0xAA,0xAA,0x25,0xBB,0xBB,0xBB,0x46,0xBB,0xBB,0xBB,0x47,0xBB,0xBB,0xBB,0x47,0xCC,0xBC,0xBB,0x47,0xCC,0xBC,0xBB,0x47,
  0x74,0xBB,0xBB,0xCC,0x74,0xBB,0xBB,0xCC,0x74,0xBB,0xBB,0xCC,0x74,0xBB,0xBB,0xBB,0x64,0xBB,0xBB,0xBB,0x52,0xAA,0xAA,0xAA,0x41,0xA7,0xEE,0xEE,0x10,0x42,0x44,0x44,
  0xCC,0xBC,0xBB,0x47,0xCC,0xBC,0xBB,0x47,0xCC,0xBC,0xBB,0x47,0xBB,0xBB,0xBB,0x47,0xBB,0xBB,0xBB,0x46,0xAA,0x88,0xA8,0x25,0xDE,0xDD,0x6A,0x13,0x44,0x44,0x24,0x01,
};

alignas(4) const uint32_t cell_highlight[] = {
  0x11111110,0x00000001,0x00000001,0x00000001,0x00000001,0x00000001,0x00000001,0x00000001,
  0x01111111,0x10000000,0x10000000,0x10000000,0x10000000,0x10000000,0x10000000,0x10000000,
  0x00000001,0x00000001,0x00000001,0x00000001,0x00000001,0x00000001,0x00000001,0x11111110,
  0x10000000,0x10000000,0x10000000,0x10000000,0x10000000,0x10000000,0x10000000,0x01111111,
};

alignas(4) const uint32_t mirror_north[] = {
  0x00000000,0x00000000,0x00020000,0x002B2000,0x22251000,0x88887100,0xFFFEDA10,0x66643100,
  0x00000000,0x00000000,0x00002000,0x0002B200,0x00015222,0x00178888,0x01ADEFFF,0x00134666,
  0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,
  0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,
};

alignas(4) const uint32_t mirror_north_east[] = {
  0x00000000,0x00000000,0x01110000,0x277A1000,0xB9D30000,0xBF400000,0xF6000000,0x60000000,
  0x00000000,0x00000000,0x00000002,0x0000002C,0x00000002,0x0000002B,0x000202BB,0x002C2BBF,
  0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,
  0x0002BBF6,0x00179F60,0x0017D400,0x001A3000,0x00010000,0x00000000,0x00000000,0x00000000,
};

alignas(4) const uint32_t mirror_east[] = {
  0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,
  0x00000000,0x00000010,0x000001A1,0x000217D3,0x002B58E4,0x000228F6,0x000028F6,0x000028F6,
  0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,
  0x000028F6,0x000028F6,0x000228F6,0x002B58E4,0x000217D3,0x000001A1,0x00000010,0x00000000,
};

alignas(4) const uint32_t mirror_south_east[] = {
  0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,
  0x00000000,0x00000000,0x00000000,0x00010000,0x00132000,0x00165400,0x00169870,0x000CBBA7,
  0x70000000,0xA7000000,0xBA400000,0xB9520000,0xCEE31000,0x01110000,0x00000000,0x00000000,
  0x00CDCBBA,0x000C0CBB,0x000000CB,0x0000000C,0x000000CD,0x0000000C,0x00000000,0x00000000,
};


alignas(4) const uint32_t prism_north[] = {
  0x00000000,0x00000000,0x00031210,0x003F1410,0x03FE1510,0x3FAF1510,0xF7DF1510,0x7FBF1510,
  0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000003,0x0000003F,
  0xFE9F1510,0xEE8F1510,0xE96F1510,0x866F1510,0xFFF61410,0x33331210,0x00000000,0x00000000,
  0x000003F7,0x00003F7F,0x0003F6FE,0x003FACB9,0x03FDFFFF,0x06333333,0x00000000,0x00000000,
};

alignas(4) const uint32_t prism_north_east[] = {
  0x00000000,0x00000000,0x00000000,0x10000000,0x51000000,0x15100000,0xF1510000,0x6F141000,
  0x00000000,0x00000110,0x00001451,0x00003115,0x00003EF1,0x00003FAF,0x00003FD7,0x00003FBF,
  0xCAF10000,0x6F300000,0xF3000000,0x30000000,0x00000000,0x00000000,0x00000000,0x00000000,
  0x00003F9E,0x00003FBF,0x00003FD7,0x00003FAF,0x00003EF3,0x00003F30,0x00006300,0x00000000,
};

alignas(4) const uint32_t prism_east[] = {
  0x00000000,0x11111100,0x55554200,0x11111100,0xFFFF6300,0x9866F300,0xEE96F300,0xFEE8F300,
  0x00000000,0x00111111,0x00245555,0x00111111,0x003FEFFF,0x0003FADB,0x00003F7F,0x000003F7,
  0x7FE9F300,0xF7FBF300,0x3F6CF300,0x03FAF300,0x003FD300,0x0003F300,0x00003600,0x00000000,
  0x0000003F,0x00000003,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,
};

alignas(4) const uint32_t prism_south_east[] = {
  0x00000000,0x00000000,0x00000000,0x00000000,0x10000000,0xF2000000,0x8F200000,0xA5F20000,
  0x00000000,0x00000000,0x00000000,0x00000001,0x00000013,0x00000141,0x0000141F,0x000141F5,
  0xCE6F2000,0x79B8F200,0xFFFFDF20,0x22222250,0x00000000,0x00000000,0x00000000,0x00000000,
  0x00141F6E,0x0141F8B9,0x0131DFFF,0x00122222,0x00000000,0x00000000,0x00000000,0x00000000,
};

alignas(4) const uint32_t source_north[] = {
  0x11300000,0xC7133000,0xEEEEA300,0x555AE300,0xAAA5D300,0xB5B5E300,0xB5B9E300,0xB5BAE300,
  0x00000311,0x0003317C,0x003AEEEE,0x003EA555,0x003D5AAA,0x003D5B5B,0x003E9B5B,0x003EAB5B,
  0xB5BAE300,0xB5BAE300,0xBBB5E300,0xDDDDB300,0x33333000,0x00000000,0x00000000,0x00000000,
  0x003EAB5B,0x003EAB5B,0x003E5BBB,0x003BDDDD,0x00033333,0x00000000,0x00000000,0x00000000,
};

alignas(4) const uint32_t source_north_east[] = {
  0x00000000,0x33330000,0xDDDA3000,0x9A5D3000,0xBBBD3000,0xBBBE3000,0xBBBE3000,0x5BBE3000,
  0x00030000,0x006C3333,0x06F2DDDD,0x3C2D4455,0x03D5BBBB,0x03D5BB5B,0x03E5B5B5,0x03E5BB5B,
  0xB8BE3000,0x8BBE3000,0xBB5E3000,0xEEEB3000,0x33330000,0x00000000,0x00000000,0x00000000,
  0x03E9BBB8,0x03EABBBB,0x03E5BBBB,0x03BEEEEE,0x00333333,0x00000000,0x00000000,0x00000000,
};

alignas(4) const uint32_t source_east[] = {
  0x00000000,0x00000000,0x33330000,0xEEEB3000,0xAA5D3000,0xBBBD3000,0x55BD3000,0xBBBD3000,
  0x00000000,0x00000000,0x00333333,0x03AEDEEE,0x03EA559A,0x31E5ABBB,0x17E5A555,0x1CE5ABBB,
  0xBBBD3000,0x55BD3000,0xBBBD3000,0xAA5D3000,0xEEEB3000,0x33330000,0x00000000,0x00000000,
  0x1CE5ABBB,0x17E5A555,0x31E5ABBB,0x03EA559A,0x03AEDDEE,0x00333333,0x00000000,0x00000000,
};

alignas(4) const uint32_t source_south_east[] = {
  0x00000000,0x00000000,0x00000000,0x33330000,0xEEEB3000,0xBB5E3000,0x8BBE3000,0xB8BE3000,
  0x00000000,0x00000000,0x00000000,0x00333333,0x03BEEEEE,0x03E5BBBB,0x03EABBBB,0x03E9BBB8,
  0x5BBE3000,0xBBBE3000,0xBBBE3000,0xBBBD3000,0x9A5D3000,0xDDDA3000,0x33330000,0x00000000,
  0x03E5BB5B,0x03E5B5B5,0x03D5BB5B,0x03D5BBBB,0x3C2D4455,0x06F2DDDD,0x006C3333,0x00030000,
};

alignas(4) const uint32_t red_laser[] = {
  0x00DEED00,0x00DEED00,0x00DEED00,0x00DEED00,0x00DEED00,0x00DEED00,0x00DEED00,0x00DEED00,
  0xEED00000,0xEEED0000,0xDEEED000,0x0DEEED00,0x00DEEED0,0x000DEEED,0x0000DEEE,0x00000DEE,
  0x00000000,0x00000000,0xDDDDDDDD,0xEEEEEEEE,0xEEEEEEEE,0xDDDDDDDD,0x00000000,0x00000000,
};

alignas(4) const uint32_t green_laser[] = {
  0x00566500,0x00566500,0x00566500,0x00566500,0x00566500,0x00566500,0x00566500,0x00566500,
  0x66500000,0x66650000,0x56665000,0x05666500,0x00566650,0x00056665,0x00005666,0x00000566,
  0x00000000,0x00000000,0x55555555,0x66666666,0x66666666,0x55555555,0x00000000,0x00000000,
};

alignas(4) const uint32_t yellow_laser[] = {
  0x00344300,0x00344300,0x00344300,0x00344300,0x00344300,0x00344300,0x00344300,0x00344300,
  0x44300000,0x44430000,0x34443000,0x03444300,0x00344430,0x00034443,0x00003444,0x00000344,
  0x00000000,0x00000000,0x33333333,0x44444444,0x44444444,0x33333333,0x00000000,0x00000000,
};

alignas(4) const uint32_t blue_laser[] = {
  0x009AA900,0x009AA900,0x009AA900,0x009AA900,0x009AA900,0x009AA900,0x009AA900,0x009AA900,
  0xAA900000,0xAAA90000,0x9AAA9000,0x09AAA900,0x009AAA90,0x0009AAA9,0x00009AAA,0x000009AA,
  0x00000000,0x00000000,0x99999999,0xAAAAAAAA,0xAAAAAAAA,0x99999999,0x00000000,0x00000000,
};

alignas(4) const uint32_t purple_laser[] = {
  0x00BCCB00,0x00BCCB00,0x00BCCB00,0x00BCCB00,0x00BCCB00,0x00BCCB00,0x00BCCB00,0x00BCCB00,
  0xCCB00000,0xCCCB0000,0xBCCCB000,0x0BCCCB00,0x00BCCCB0,0x000BCCCB,0x0000BCCC,0x00000BCC,
  0x00000000,0x00000000,0xBBBBBBBB,0xCCCCCCCC,0xCCCCCCCC,0xBBBBBBBB,0x00000000,0x00000000,
};

alignas(4) const uint32_t cyan_laser[] = {
  0x00877800,0x00877800,0x00877800,0x00877800,0x00877800,0x00877800,0x00877800,0x00877800,
  0x77800000,0x77780000,0x87778000,0x08777800,0x00877780,0x00087778,0x00008777,0x00000877,
  0x00000000,0x00000000,0x88888888,0x77777777,0x77777777,0x88888888,0x00000000,0x00000000,
};

alignas(4) const uint32_t white_laser[] = {
  0x00122100,0x00122100,0x00122100,0x00122100,0x00122100,0x00122100,0x00122100,0x00122100,
  0x22100000,0x22210000,0x12221000,0x01222100,0x00122210,0x00012221,0x00001222,0x00000122,
  0x00000000,0x00000000,0x11111111,0x22222222,0x22222222,0x11111111,0x00000000,0x00000000,
};


alignas(4) sprite_info_t sprites[] =
{
  { cell_background, 2, 2 },
  { cell_highlight, 2, 2},
  { mirror_north, 2, 2 },
  { mirror_north_east, 2, 2 },
  { mirror_east, 2, 2 },
  { mirror_south_east, 2, 2},
  { prism_north, 2, 2},
  { prism_north_east, 2, 2},
  { prism_east, 2, 2},
  { prism_south_east, 2, 2},
  { source_north, 2, 2},
  { source_north_east, 2, 2},
  { source_east, 2, 2},
  { source_south_east, 2, 2},
  
  { red_laser, 3, 1 },
  { green_laser, 3, 1 },
  { yellow_laser, 3, 1 },
  { blue_laser, 3, 1 },
  { purple_laser, 3, 1 },
  { cyan_laser, 3, 1 },
  { white_laser, 3, 1}
};

const sprite_info_t* Data::getSpriteData(u32 index) { return &sprites[index]; }
