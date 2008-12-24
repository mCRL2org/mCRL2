// Author(s): Diana Koenraadt, Remco Blewanus, Bram Schoenmakers, Thorstin Crijns, Hans Poppelaars, Bas Luksenburg, Jonathan Nelisse
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file font_renderer.cpp
//
// Implements the text rendering function.

#include "boost/shared_array.hpp"
#include "font_renderer.h"
#include "font/fonts.h"
#include "workarounds.h"

using namespace grape::grapeapp;

texture_pool grape::grapeapp::g_pool;

void grape::grapeapp::init_textures(void)
{
  // load xpms
  wxBitmap bmps[256];
  bmps[0] = wxBitmap(x0);
  bmps[1] = wxBitmap(x1);
  bmps[2] = wxBitmap(x2);
  bmps[3] = wxBitmap(x3);
  bmps[4] = wxBitmap(x4);
  bmps[5] = wxBitmap(x5);
  bmps[6] = wxBitmap(x6);
  bmps[7] = wxBitmap(x7);
  bmps[8] = wxBitmap(x8);
  bmps[9] = wxBitmap(x9);
  bmps[10] = wxBitmap(x10);
  bmps[11] = wxBitmap(x11);
  bmps[12] = wxBitmap(x12);
  bmps[13] = wxBitmap(x13);
  bmps[14] = wxBitmap(x14);
  bmps[15] = wxBitmap(x15);
  bmps[16] = wxBitmap(x16);
  bmps[17] = wxBitmap(x17);
  bmps[18] = wxBitmap(x18);
  bmps[19] = wxBitmap(x19);
  bmps[20] = wxBitmap(x20);
  bmps[21] = wxBitmap(x21);
  bmps[22] = wxBitmap(x22);
  bmps[23] = wxBitmap(x23);
  bmps[24] = wxBitmap(x24);
  bmps[25] = wxBitmap(x25);
  bmps[26] = wxBitmap(x26);
  bmps[27] = wxBitmap(x27);
  bmps[28] = wxBitmap(x28);
  bmps[29] = wxBitmap(x29);
  bmps[30] = wxBitmap(x30);
  bmps[31] = wxBitmap(x31);
  bmps[32] = wxBitmap(x32);
  bmps[33] = wxBitmap(x33);
  bmps[34] = wxBitmap(x34);
  bmps[35] = wxBitmap(x35);
  bmps[36] = wxBitmap(x36);
  bmps[37] = wxBitmap(x37);
  bmps[38] = wxBitmap(x38);
  bmps[39] = wxBitmap(x39);
  bmps[40] = wxBitmap(x40);
  bmps[41] = wxBitmap(x41);
  bmps[42] = wxBitmap(x42);
  bmps[43] = wxBitmap(x43);
  bmps[44] = wxBitmap(x44);
  bmps[45] = wxBitmap(x45);
  bmps[46] = wxBitmap(x46);
  bmps[47] = wxBitmap(x47);
  bmps[48] = wxBitmap(x48);
  bmps[49] = wxBitmap(x49);
  bmps[50] = wxBitmap(x50);
  bmps[51] = wxBitmap(x51);
  bmps[52] = wxBitmap(x52);
  bmps[53] = wxBitmap(x53);
  bmps[54] = wxBitmap(x54);
  bmps[55] = wxBitmap(x55);
  bmps[56] = wxBitmap(x56);
  bmps[57] = wxBitmap(x57);
  bmps[58] = wxBitmap(x58);
  bmps[59] = wxBitmap(x59);
  bmps[60] = wxBitmap(x60);
  bmps[61] = wxBitmap(x61);
  bmps[62] = wxBitmap(x62);
  bmps[63] = wxBitmap(x63);
  bmps[64] = wxBitmap(x64);
  bmps[65] = wxBitmap(x65);
  bmps[66] = wxBitmap(x66);
  bmps[67] = wxBitmap(x67);
  bmps[68] = wxBitmap(x68);
  bmps[69] = wxBitmap(x69);
  bmps[70] = wxBitmap(x70);
  bmps[71] = wxBitmap(x71);
  bmps[72] = wxBitmap(x72);
  bmps[73] = wxBitmap(x73);
  bmps[74] = wxBitmap(x74);
  bmps[75] = wxBitmap(x75);
  bmps[76] = wxBitmap(x76);
  bmps[77] = wxBitmap(x77);
  bmps[78] = wxBitmap(x78);
  bmps[79] = wxBitmap(x79);
  bmps[80] = wxBitmap(x80);
  bmps[81] = wxBitmap(x81);
  bmps[82] = wxBitmap(x82);
  bmps[83] = wxBitmap(x83);
  bmps[84] = wxBitmap(x84);
  bmps[85] = wxBitmap(x85);
  bmps[86] = wxBitmap(x86);
  bmps[87] = wxBitmap(x87);
  bmps[88] = wxBitmap(x88);
  bmps[89] = wxBitmap(x89);
  bmps[90] = wxBitmap(x90);
  bmps[91] = wxBitmap(x91);
  bmps[92] = wxBitmap(x92);
  bmps[93] = wxBitmap(x93);
  bmps[94] = wxBitmap(x94);
  bmps[95] = wxBitmap(x95);
  bmps[96] = wxBitmap(x96);
  bmps[97] = wxBitmap(x97);
  bmps[98] = wxBitmap(x98);
  bmps[99] = wxBitmap(x99);
  bmps[100] = wxBitmap(x100);
  bmps[101] = wxBitmap(x101);
  bmps[102] = wxBitmap(x102);
  bmps[103] = wxBitmap(x103);
  bmps[104] = wxBitmap(x104);
  bmps[105] = wxBitmap(x105);
  bmps[106] = wxBitmap(x106);
  bmps[107] = wxBitmap(x107);
  bmps[108] = wxBitmap(x108);
  bmps[109] = wxBitmap(x109);
  bmps[110] = wxBitmap(x110);
  bmps[111] = wxBitmap(x111);
  bmps[112] = wxBitmap(x112);
  bmps[113] = wxBitmap(x113);
  bmps[114] = wxBitmap(x114);
  bmps[115] = wxBitmap(x115);
  bmps[116] = wxBitmap(x116);
  bmps[117] = wxBitmap(x117);
  bmps[118] = wxBitmap(x118);
  bmps[119] = wxBitmap(x119);
  bmps[120] = wxBitmap(x120);
  bmps[121] = wxBitmap(x121);
  bmps[122] = wxBitmap(x122);
  bmps[123] = wxBitmap(x123);
  bmps[124] = wxBitmap(x124);
  bmps[125] = wxBitmap(x125);
  bmps[126] = wxBitmap(x126);
  bmps[127] = wxBitmap(x127);
  bmps[128] = wxBitmap(x128);
  bmps[129] = wxBitmap(x129);
  bmps[130] = wxBitmap(x130);
  bmps[131] = wxBitmap(x131);
  bmps[132] = wxBitmap(x132);
  bmps[133] = wxBitmap(x133);
  bmps[134] = wxBitmap(x134);
  bmps[135] = wxBitmap(x135);
  bmps[136] = wxBitmap(x136);
  bmps[137] = wxBitmap(x137);
  bmps[138] = wxBitmap(x138);
  bmps[139] = wxBitmap(x139);
  bmps[140] = wxBitmap(x140);
  bmps[141] = wxBitmap(x141);
  bmps[142] = wxBitmap(x142);
  bmps[143] = wxBitmap(x143);
  bmps[144] = wxBitmap(x144);
  bmps[145] = wxBitmap(x145);
  bmps[146] = wxBitmap(x146);
  bmps[147] = wxBitmap(x147);
  bmps[148] = wxBitmap(x148);
  bmps[149] = wxBitmap(x149);
  bmps[150] = wxBitmap(x150);
  bmps[151] = wxBitmap(x151);
  bmps[152] = wxBitmap(x152);
  bmps[153] = wxBitmap(x153);
  bmps[154] = wxBitmap(x154);
  bmps[155] = wxBitmap(x155);
  bmps[156] = wxBitmap(x156);
  bmps[157] = wxBitmap(x157);
  bmps[158] = wxBitmap(x158);
  bmps[159] = wxBitmap(x159);
  bmps[160] = wxBitmap(x160);
  bmps[161] = wxBitmap(x161);
  bmps[162] = wxBitmap(x162);
  bmps[163] = wxBitmap(x163);
  bmps[164] = wxBitmap(x164);
  bmps[165] = wxBitmap(x165);
  bmps[166] = wxBitmap(x166);
  bmps[167] = wxBitmap(x167);
  bmps[168] = wxBitmap(x168);
  bmps[169] = wxBitmap(x169);
  bmps[170] = wxBitmap(x170);
  bmps[171] = wxBitmap(x171);
  bmps[172] = wxBitmap(x172);
  bmps[173] = wxBitmap(x173);
  bmps[174] = wxBitmap(x174);
  bmps[175] = wxBitmap(x175);
  bmps[176] = wxBitmap(x176);
  bmps[177] = wxBitmap(x177);
  bmps[178] = wxBitmap(x178);
  bmps[179] = wxBitmap(x179);
  bmps[180] = wxBitmap(x180);
  bmps[181] = wxBitmap(x181);
  bmps[182] = wxBitmap(x182);
  bmps[183] = wxBitmap(x183);
  bmps[184] = wxBitmap(x184);
  bmps[185] = wxBitmap(x185);
  bmps[186] = wxBitmap(x186);
  bmps[187] = wxBitmap(x187);
  bmps[188] = wxBitmap(x188);
  bmps[189] = wxBitmap(x189);
  bmps[190] = wxBitmap(x190);
  bmps[191] = wxBitmap(x191);
  bmps[192] = wxBitmap(x192);
  bmps[193] = wxBitmap(x193);
  bmps[194] = wxBitmap(x194);
  bmps[195] = wxBitmap(x195);
  bmps[196] = wxBitmap(x196);
  bmps[197] = wxBitmap(x197);
  bmps[198] = wxBitmap(x198);
  bmps[199] = wxBitmap(x199);
  bmps[200] = wxBitmap(x200);
  bmps[201] = wxBitmap(x201);
  bmps[202] = wxBitmap(x202);
  bmps[203] = wxBitmap(x203);
  bmps[204] = wxBitmap(x204);
  bmps[205] = wxBitmap(x205);
  bmps[206] = wxBitmap(x206);
  bmps[207] = wxBitmap(x207);
  bmps[208] = wxBitmap(x208);
  bmps[209] = wxBitmap(x209);
  bmps[210] = wxBitmap(x210);
  bmps[211] = wxBitmap(x211);
  bmps[212] = wxBitmap(x212);
  bmps[213] = wxBitmap(x213);
  bmps[214] = wxBitmap(x214);
  bmps[215] = wxBitmap(x215);
  bmps[216] = wxBitmap(x216);
  bmps[217] = wxBitmap(x217);
  bmps[218] = wxBitmap(x218);
  bmps[219] = wxBitmap(x219);
  bmps[220] = wxBitmap(x220);
  bmps[221] = wxBitmap(x221);
  bmps[222] = wxBitmap(x222);
  bmps[223] = wxBitmap(x223);
  bmps[224] = wxBitmap(x224);
  bmps[225] = wxBitmap(x225);
  bmps[226] = wxBitmap(x226);
  bmps[227] = wxBitmap(x227);
  bmps[228] = wxBitmap(x228);
  bmps[229] = wxBitmap(x229);
  bmps[230] = wxBitmap(x230);
  bmps[231] = wxBitmap(x231);
  bmps[232] = wxBitmap(x232);
  bmps[233] = wxBitmap(x233);
  bmps[234] = wxBitmap(x234);
  bmps[235] = wxBitmap(x235);
  bmps[236] = wxBitmap(x236);
  bmps[237] = wxBitmap(x237);
  bmps[238] = wxBitmap(x238);
  bmps[239] = wxBitmap(x239);
  bmps[240] = wxBitmap(x240);
  bmps[241] = wxBitmap(x241);
  bmps[242] = wxBitmap(x242);
  bmps[243] = wxBitmap(x243);
  bmps[244] = wxBitmap(x244);
  bmps[245] = wxBitmap(x245);
  bmps[246] = wxBitmap(x246);
  bmps[247] = wxBitmap(x247);
  bmps[248] = wxBitmap(x248);
  bmps[249] = wxBitmap(x249);
  bmps[250] = wxBitmap(x250);
  bmps[251] = wxBitmap(x251);
  bmps[252] = wxBitmap(x252);
  bmps[253] = wxBitmap(x253);
  bmps[254] = wxBitmap(x254);
  bmps[255] = wxBitmap(x255);

  // convert to images
  wxImage images[256];
  for(int i=0; i<256; ++i)
  {
    images[i] = bmps[i].ConvertToImage();
  }

  // create textures
  g_pool.m_textures = new GLuint[256];
  glGenTextures(256, &g_pool.m_textures[0]);
  unsigned char *src = 0;
  int img_width, img_height;
  for(int i=0; i<256; ++i)
  {
    img_width = images[i].GetWidth();
    img_height = images[i].GetHeight();

    // expand image to alpha
    boost::shared_array<unsigned char> target(new unsigned char[img_width * img_height * 4]);
    src = images[i].GetData();
    for(int j=0; j<img_width*img_height; ++j)
    {
      target[j*4] = src[j*3];
      target[j*4+1] = src[j*3+1];
      target[j*4+2] = src[j*3+2];
      target[j*4+3] = (src[j*3] ^ 0xff);
    }
    glBindTexture(GL_TEXTURE_2D, g_pool.m_textures[i]);
    glTexImage2D(GL_TEXTURE_2D, 0, 4, img_width, img_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, &target);
//    gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGBA, img_width, img_height, GL_RGBA, GL_UNSIGNED_BYTE, &target);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
//    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  }
}

void grape::grapeapp::render_text(const wxString &p_str, float p_x, float p_y, float max_width, float max_height, bool p_center)
{
  float curr_x = 0;
  float curr_y = 0;

  glPushMatrix();

  if (p_center)
  {
    //align text center
    if (float(p_str.Len()*(g_size - g_space)) > max_width) 
    {
      glTranslatef(-max_width/2, -g_text_space, 0.0f);
    } 
    else
    {
      glTranslatef(-float(p_str.Len()*(g_size - g_space)*0.5), -g_text_space, 0.0f);
    }
     
  } 
  else
  {
    //align text normal
    glTranslatef(0, -g_text_space, 0.0f);
  }

  // change GL state
  glColor3f(1.0f, 1.0f, 1.0f);
  glEnable(GL_TEXTURE_2D);
  glEnable(GL_BLEND);

  // set initial position
  curr_x = p_x;
  curr_y = p_y;

  // loop over the string
  for( unsigned int i=0; i<p_str.Len(); ++i )
  {
    unsigned char chr = p_str[i];

    // check for newline
    if ((chr == '\n')||(max_width < curr_x-p_x+g_text_space*1.5))
    {
      // update horizontal and vertical position
      curr_y -= g_size;
      curr_x = p_x;
      if ((chr == '\n')&&(-max_height+g_size*3 <= curr_y-p_y))
      {
        continue;
      }
    }

    // check if we want to print dots
    if ((((max_width-3*(g_size - g_space) < curr_x-p_x+g_text_space*1.5)||(chr == '\n'))&&(-max_height+g_size*3 > curr_y-p_y)))
    {
      // loop over the string
      for( unsigned int j=0; j<3; ++j )
      {
        // check for newline
        if (max_width < curr_x-p_x+g_text_space*1.5)
        {
          // update horizontal and vertical position
          curr_y -= g_size;
          curr_x = p_x;
          if (chr == '\n')
          {
            continue;
          }
        }

        // only print if we are not outside the object
        if (max_width >= curr_x-p_x+g_text_space*1.5)
        {
          // render dots
          glBindTexture(GL_TEXTURE_2D, g_pool.m_textures['.']);
          glBegin(GL_QUADS);
            glTexCoord2f(0.0f, 1.0f);
            glVertex3f(curr_x, curr_y, 0.0f);
            glTexCoord2f(1.0f, 1.0f);
            glVertex3f(curr_x+g_size, curr_y, 0.0f);
            glTexCoord2f(1.0f, 0.0f);
            glVertex3f(curr_x+g_size, curr_y+g_size, 0.0f);
            glTexCoord2f(0.0f, 0.0f);
            glVertex3f(curr_x, curr_y+g_size, 0.0f);
          glEnd();
        }
        // update horizontal position
        curr_x += g_size - g_space;
      }    
      break;
    } 
    else 
    {
      // only print if we are not outside the object
      if ((max_width >= curr_x-p_x+g_text_space*1.5))
      {
        // render character  
        glBindTexture(GL_TEXTURE_2D, g_pool.m_textures[chr]);
        glBegin(GL_QUADS);
          glTexCoord2f(0.0f, 1.0f);
          glVertex3f(curr_x, curr_y, 0.0f);
          glTexCoord2f(1.0f, 1.0f);
          glVertex3f(curr_x+g_size, curr_y, 0.0f);
          glTexCoord2f(1.0f, 0.0f);
          glVertex3f(curr_x+g_size, curr_y+g_size, 0.0f);
          glTexCoord2f(0.0f, 0.0f);
          glVertex3f(curr_x, curr_y+g_size, 0.0f);
        glEnd();
      }
    }

    // update horizontal position
    curr_x += g_size - g_space;
  }

  glDisable(GL_TEXTURE_2D);
  glDisable(GL_BLEND);

  glPopMatrix();

}
