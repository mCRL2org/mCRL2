// Author(s): Carst Tankink, based on font renderers by VitaminB100 and 
//            A. J. (Hannes) Pretorius
//            
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file font_renderer.cpp
//
// \brief Implements the font renderer class.

#include "wx.hpp" // precompiled headers

#include "font_renderer.h"
#include "font/character_set.xpm"
#include <wx/image.h>
#include <algorithm>

#ifdef __APPLE__
  #include <OpenGL/glu.h>
#else
  #include <GL/glu.h>
#endif

namespace mcrl2 {
  namespace utilities {
    namespace wx {
      inline GLubyte (&font_renderer::character_textures())[CHARSETSIZE][CHARHEIGHT * CHARWIDTH] {
        static GLubyte textures[CHARSETSIZE][CHARHEIGHT * CHARWIDTH];

        return textures;
      }

      inline GLuint (&font_renderer::character_texture_id())[CHARSETSIZE] {
        static GLuint textures[CHARSETSIZE];
 
        return textures;
      }

      bool font_renderer::initialise() {
        // Create textures.
        //
        int red = 0;
        int green = 0;
        int blue = 0;
 
        // Allocate texture memory
        glGenTextures(CHARSETSIZE, character_texture_id());
 
        // Create the textures from the character set
        for(size_t i = 0; i < CHARSETSIZE; ++i)
        {
          // Bind texture
          glBindTexture(GL_TEXTURE_2D, character_texture_id()[i]);

          // Create image from xpm
          wxImage image(characters[i]);
          // Read in rgb-value of texture
          int count = 0;

          for (size_t h = 0; h < CHARHEIGHT; ++h)
          {
            for (size_t w = 0; w < CHARWIDTH; ++w)
            {
              red   = (GLubyte)image.GetRed(w, h);
              green = (GLubyte)image.GetGreen(w, h);
              blue  = (GLubyte)image.GetBlue(w, h);
 
              character_textures()[i][count] = (GLubyte)(255.0 - (red + green + blue) / 3.0);
 
              ++count;
            }
          }

          // Build a MIP map for the texture. This gives faster renders and better
          // anti-aliasing effects. See http://en.wikipedia.org/wiki/Mipmap for an
          // introduction to the technique.

          gluBuild2DMipmaps(
            GL_TEXTURE_2D,
            GL_ALPHA,
            CHARWIDTH,
            CHARHEIGHT,
            GL_ALPHA,
            GL_UNSIGNED_BYTE,
            character_textures()[i] );
        }

        return true;
      }

      font_renderer::font_renderer()
      {
        static bool initialised = initialise();

        static_cast< void >(initialised); // prevent unused variable warnings
      }
 
      font_renderer::~font_renderer()
      {
        // Free texture resources
      }

      void font_renderer::draw_text(
        const std::string s,
        const double x, 
        const double y,
        const double scale,
        const Alignment align_horizontal,
        const Alignment align_vertical)
      {
        // Render text at the specified location
        double xSLft;
        double ySBot;
 
        switch(align_horizontal)
        {
          case al_left: 
          {
            xSLft = x - (s.length() * CHARWIDTH * scale);
            break;
          }
          case al_center:
          {
            xSLft = x - (0.5 * s.length() * CHARWIDTH * scale);
            break;
          }
          case al_right: // Fall through to default case
          default: 
          {
            xSLft = x;
            break;
          }
        }

        switch(align_vertical)
        {
          case al_top:
          {
            ySBot = y;
            break;
          }
          case al_bottom:
          {
            ySBot = y - (CHARHEIGHT * scale);
            break;
          }
          case al_center: // Fall through to default case
          default:
          {
            ySBot = y - (.5 * CHARHEIGHT * scale);
            break;
          }
        }

        // Check if there actually is a label to render.
        if(s.size() > 0)
        {
          // Enable texture mapping
          glEnable(GL_TEXTURE_2D);

          for( size_t i = 0; i < s.length(); ++i)
          {
            double xLft = xSLft + i * scale * CHARWIDTH;
            double xRgt = xSLft + (i+1) * scale * CHARWIDTH;
            double yTop = ySBot + .5 * scale * CHARHEIGHT;
            double yBot = ySBot - .5 * scale * CHARHEIGHT;
            
            size_t index = index_from_char(s[i]);
            glBindTexture(GL_TEXTURE_2D, character_texture_id()[index]);

            // Setup texture parameters
            glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
            glTexParameterf(GL_TEXTURE_2D, 
                            GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
            glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
            glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
            glBegin(GL_QUADS);
              glTexCoord2f(0.0, 0.0); glVertex3f(xLft, yTop, 0.5);
              glTexCoord2f(0.0, 1.0); glVertex3f(xLft, yBot, 0.5);
              glTexCoord2f(1.0, 1.0); glVertex3f(xRgt, yBot, 0.5);
              glTexCoord2f(1.0, 0.0); glVertex3f(xRgt, yTop, 0.5);
            glEnd();
            glDisable(GL_BLEND);
          }
        }
      }

      void font_renderer::draw_bounding_box(
        const std::string s,
        const double x, 
        const double y,
        const double scale,
        const Alignment align_horizontal,
        const Alignment align_vertical,
        const bool draw_border)
      {
        // Render text's bb at the specified location
        double xSLft;
        double ySBot;

        switch(align_horizontal)
        {
          case al_left: 
          {
            xSLft = x - (s.length() * CHARWIDTH * scale);
            break;
          }
          case al_center:
          {
            xSLft = x - (0.5 * s.length() * CHARWIDTH * scale);
            break;
          }
          case al_right: // Fall through to default case
          default: 
          {
            xSLft = x;
            break;
          }
        }

        switch(align_vertical)
        {
          case al_top:
          {
            ySBot = y;
            break;
          }
          case al_bottom:
          {
            ySBot = y - (CHARHEIGHT * scale);
            break;
          }
          case al_center: // Fall through to default case
          default:
          {
            ySBot = y - (.5 * CHARHEIGHT * scale);
            break;
          }
        }

        // The bounding box has width CHARWIDTH * scale * s.length()...
        double xSRgt = xSLft + CHARWIDTH * scale * s.length();
        // ... and height CHARHEIGHT * scale
        ySBot = ySBot - .5  * scale * CHARHEIGHT;
        double ySTop = ySBot + CHARHEIGHT * scale;

        // Draw the bounding box
        glBegin(GL_QUADS); {
          glVertex2d(xSRgt, ySBot);
          glVertex2d(xSRgt, ySTop);
          glVertex2d(xSLft, ySTop);
          glVertex2d(xSLft, ySBot);
        }
        glEnd();

        if(draw_border) {
          glBegin(GL_LINE_STRIP); {
            glVertex2d(xSRgt, ySBot);
            glVertex2d(xSRgt, ySTop);
            glVertex2d(xSLft, ySTop);
            glVertex2d(xSLft, ySBot);     
          }
          glEnd();
        }
      }

      void font_renderer::draw_cropped_text(
        const std::string s,
        const double x, 
        const double y,
        const double xLft, 
        const double xRgt,
        const double yTop, 
        const double yBot,
        const double scale,
        const Alignment align_horizontal,
        const Alignment align_vertical)
      {
        // Crop text to fit into bounding box.
      }

      void font_renderer::draw_wrapped_text(
        const std::string s,
        const double xLft, 
        const double xRgt,
        const double yTop, 
        const double yBot,
        const double scale,
        const Alignment align_horizontal,
        const Alignment align_vertical)
      {
        // Wrapped text to fit into bounding box. 

        const size_t addpos = static_cast< size_t >((xRgt-xLft)/(CHARWIDTH*scale));
        double transx = 0;
        double transy = 0;
        std::string subs;
        std::string temps;

        double maxheight = 0;
        std::string::size_type startpos = 0;
        double y = 0;
        temps = s;
        // caluclate the maximum height of the text
        while (startpos < temps.length())
        {
          std::string::size_type findid = temps.find_first_of('\n');
        
          if ((findid < 0) || (findid > startpos + addpos))
          {
             // if there is not a new line character 
             // just take the next part of the string
             subs = temps.substr((std::min)(temps.length(), startpos), (std::min)(temps.length()-startpos, addpos));
             startpos = startpos + addpos;
          } 
          else
          {
            if (findid <= startpos + addpos) 
            {
              // if we found a new line character 
              // cut the string at the new line position
              subs = temps.substr((std::min)(temps.length(), startpos), (std::min)(temps.length()-startpos, findid-startpos));
              startpos = 0;
              temps = temps.substr(findid+1);
            }
          }               
          y = y - CHARHEIGHT*scale;

          //update maximum width and height
          maxheight = maxheight + CHARHEIGHT * scale;

          // break if we are outside the boundingbox
          if (!((y - CHARHEIGHT*scale >= yBot-yTop) && (y - CHARHEIGHT*scale <= 0))) break;
        }

        startpos = 0;
        temps = s;
        y = 0;
        // print text
        while (startpos < s.length())
        {
          std::string::size_type findid = temps.find_first_of('\n');

          if ((findid < 0) || (findid > startpos + addpos))
          {
             // if there is not a new line character 
             // just take the next part of the string
             subs = temps.substr((std::min)(temps.length(), startpos), (std::min)(temps.length()-startpos, addpos));
             startpos = startpos + addpos;
          } 
          else
          {
            if (findid <= startpos + addpos) 
            {
              // if we found a new line character 
              // cut the string at the new line position
              subs = temps.substr((std::min)(temps.length(), startpos), (std::min)(temps.length()-startpos, findid-startpos));
              startpos = 0;
              temps = temps.substr(findid+1);
            }
          }

          // print dots if there are more lines and we are at the bottem of our bounding box
          if (((!((y - 2*CHARHEIGHT*scale >= yBot-yTop) && (y - 2*CHARHEIGHT*scale <= 0))) && ((subs.length() >= 3) || (subs.find_first_of('\n') >= 0))))
          {
            subs = subs.substr(0, subs.length()-3);
            subs = subs.append("...");
          }

          // set correct horizontal alignment        
          switch(align_horizontal)
          {
            case al_left: 
            {
              transx = xLft;
              break;
            }
            case al_center:
            {
              transx = xLft + .5 * (xRgt - xLft - CHARWIDTH * scale * subs.length());
              break;
            }
            case al_right: // Fall through to default case
            default: 
            {
              transx = xRgt - CHARWIDTH * scale * subs.length();
              break;
            }
          }

          // set correct vertical alignment
          switch(align_vertical)
          {
            case al_top:
            {
              transy = yTop + y;
              break;
            }
            case al_center:
            {
              transy = yTop + y + .5*(yBot - yTop + maxheight);
              break;
            }
            case al_bottom: // Fall through to default case
            default:
            {
              transy = yBot + maxheight + y;
              break;
            }
          }

          draw_text(subs, transx, transy , scale, al_right, al_center);  

          // calculate new y position
          y = y - CHARHEIGHT*scale;

          // break if we are outside the boundingbox
          if (!((y - CHARHEIGHT*scale >= yBot-yTop) && (y - CHARHEIGHT*scale <= 0))) break;
        }      
      }

      size_t font_renderer::index_from_char(const char & c)
      {
        size_t result = 80; // Question mark
        size_t ascii = static_cast<int>(c);

        // lowercase
        if (97 <= ascii && ascii <= 122)
        {
          result = ascii - 97;
        }

        // uppercase
        else if (65  <= ascii && ascii <= 90)
        {
          result = ascii -65 + 26;
        }

        // Numers 0 ... 9
        else if ( 48 <= ascii && ascii <= 57)
        {
          result = ascii - 48 + 52;
        }

        // Special characters
        else
        {
          switch (c)
          {
            case ' ':  result = 62; break;
            case '_':  result = 63; break;
            case '-':  result = 64; break;
            case '|':  result = 65; break;
            case '/':  result = 66; break;
            case '\\': result = 67; break;
            case '"':  result = 68; break;
            case '\'': result = 69; break;
            case ':':  result = 70; break;
            case ';':  result = 71; break;
            case '.':  result = 72; break;
            case ',':  result = 73; break;
            case '(':  result = 74; break;
            case ')':  result = 75; break;
            case '[':  result = 76; break;
            case ']':  result = 77; break;
            case '{':  result = 78; break;
            case '}':  result = 79; break;
            case '?':  result = 80; break;
            case '<':  result = 81; break;
            case '>':  result = 82; break;
            case '+':  result = 83; break;
            case '=':  result = 84; break;
            case '*':  result = 85; break;
            case '&':  result = 86; break;
            case '^':  result = 87; break;
            case '%':  result = 88; break;
            case '$':  result = 89; break;
            case '#':  result = 90; break;
            case '@':  result = 91; break;
            case '!':  result = 92; break;
            case '`':  result = 93; break;
            case '~':  result = 94; break;
            default :  result = 80; break;
          }
        }

        return result;
      }
    } // namespace wx
  } // namespace utilities
} // namespace mcrl2
