// Author(s): Carst Tankink, based on font renderers by VitaminB100 and 
//            A. J. (Hannes) Pretorius
// 
// Distributed unaer the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file font_renderer.h
//
// \brief Defines a font renderer class for use with OpenGL.

#ifndef FONT_RENDERER_H
#define FONT_RENDERER_H

#include <string>
#include <wx/wx.h>
#include <wx/glcanvas.h>

namespace mcrl2
{
  namespace utilities
  {
    namespace wx
    {
      /**
       * \brief Type of text alignment. This contains both vertical and horizontal
       *        alignments.
       **/
      enum Alignment
      {          al_top,
        al_left, al_center, al_right,
                 al_bottom
      };

      /**
       * \brief Number of characters within the character set.
       **/
      const size_t CHARSETSIZE = 96;

      /**
       * \brief Pixel width per character.
       **/
      const size_t CHARWIDTH = 16;

      /**
       * \brief Pixel height per character.
       **/
      const size_t CHARHEIGHT = 32;

      /**
       * \brief A font renderer for use with OpenGL
       *
       * The font renderer maintains the resources used for drawing text on an 
       * OpenGL canvas. This is intended for wxGL implementations, but will 
       * probably work on generic OpenGL versions.
       * The renderer provides three functions: 
       *   - draw a string,
       *   - draw a string cropped to a specified bounding box,
       *   - draw the bounding box of a string
       **/
      class font_renderer
      {
        public:
          /// \brief Constructor
          font_renderer();

          /// \brief Destructor
          ~font_renderer();

          /// \brief Renders the string s.
          /// \param[in] s The string to be drawn.
          /// \param[in] x The x coordinate of the string.
          /// \param[in] y The y coordinate of the string.
          /// \param[in] scale The scale factor for the text.
          /// \param[in] align_horizontal The horizontal alignment of the text.
          ///            If the alignment is not one of {left, center, right}, the
          ///            text defaults to right-aligned.
          /// \param[in] align_vertical The vertical alignment of the text.
          ///            If the alignment is not one of {top, center, bottom}, the
          ///            text defaults to center-aligned.
          /// \pre There is an initialized canvas, text color is initialized and
          ///      any necessary rotation has been done.
          ///
          /// \post The text is rendered at the specified location, with the 
          ///       specified alignments and at the specified scale.
          void draw_text(
            const std::string s,
            const double x, 
            const double y,
            const double scale,
            const Alignment align_horizontal,
            const Alignment align_vertical
          );

          /// \brief Renders a bounding box for string s.
          /// \param[in] s The string to be drawn.
          /// \param[in] x The x coordinate of the string.
          /// \param[in] y The y coordinate of the string.
          /// \param[in] scale The scale factor for the text.
          /// \param[in] align_horizontal The horizontal alignment of the text.
          /// \param[in] align_vertical The vertical alignment of the text.
          /// \param[in] draw_border Whether or not to draw a (black) border.
          ///
          /// \pre There is an initialized canvas, box color is initialized and
          ///      any necessary rotation has been done.
          ///
          /// \post A bounding box for the given string is drawn.
          void draw_bounding_box(
            const std::string s,
            const double x, 
            const double y,
            const double scale,
            const Alignment align_horizontal,
            const Alignment align_vertical,
            const bool draw_border
          );

          /// \brief Renders the string s.
          /// \param[in] s The string to be drawn.
          /// \param[in] x The x coordinate of the string.
          /// \param[in] y The y coordinate of the string.
          /// \param[in] xLft The left side of the bounding box.
          /// \param[in] xRgt The right side of the bounding box.
          /// \param[in] yBot The bottom of the bounding box.
          /// \param[in] yTop The top of the bounding box.
          /// \param[in] scale The scale factor for the text.
          /// \param[in] align_horizontal The horizontal alignment of the text.
          /// \param[in] align_vertical The vertical alignment of the text.
          ///
          /// \pre There is an initialized canvas, text color is initialized and
          ///      any necessary rotation has been done.
          ///
          /// \post The text is rendered at the specified location, with the 
          ///       specified alignments and at the specified scale. It is cropped
          ///       To fit within the bounding box defined by xLft, xRgt, yBot and
          ///       yTop.
          void draw_cropped_text(
            const std::string s,
            const double x,
            const double y,
            const double xLft,
            const double xRgt,
            const double yTop,
            const double yBot,
            const double scale,
            const Alignment align_horizontal,
            const Alignment align_vertical
          );

          /// \brief Renders the string s wrapped into a bounding box.
          /// \param[in] s The string to be drawn.
          /// \param[in] xLft The left side of the bounding box.
          /// \param[in] xRgt The right side of the bounding box.
          /// \param[in] yBot The bottom of the bounding box.
          /// \param[in] yTop The top of the bounding box.
          /// \param[in] scale The scale factor for the text.
          /// \param[in] align_horizontal The horizontal alignment of the text.
          /// \param[in] align_vertical The vertical alignment of the text.
          ///
          /// \pre There is an initialized canvas, text color is initialized and
          ///      any necessary rotation has been done.
          ///
          /// \post The text is rendered at the specified location, with the 
          ///       specified alignments and at the specified scale. It is cropped
          ///       To fit within the bounding box defined by xLft, xRgt, yBot and
          ///       yTop.
          void draw_wrapped_text(
            const std::string s,
            const double xLft, 
            const double xRgt,
            const double yTop, 
            const double yBot,
            const double scale,
            const Alignment align_horizontal,
            const Alignment align_vertical
          );

          private:
            static GLuint  (&character_texture_id())[CHARSETSIZE];
            static GLubyte (&character_textures())[CHARSETSIZE][CHARHEIGHT * CHARWIDTH];

            size_t index_from_char(const char & c);

            bool initialise();
      }; // class font_renderer
    } // namespace wx
  } // namespace utilities
} // namespace mcrl2

#endif // FONT_RENDERER_H
