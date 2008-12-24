// Author(s): Diana Koenraadt, Remco Blewanus, Bram Schoenmakers, Thorstin Crijns, Hans Poppelaars, Bas Luksenburg, Jonathan Nelisse
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file font_renderer.h
//
// Declares the text rendering function.

#ifndef GRAPEAPP_FONT_RENDERER_H
#define GRAPEAPP_FONT_RENDERER_H

#include <wx/wx.h>
#include <wx/string.h>
#include <wx/glcanvas.h>

const float g_size = 0.05f;
const float g_space = 0.0225f;
const float g_text_space = g_size-g_space;

namespace grape
{
  namespace grapeapp
  {

    /**
     * \short A texture pool. Houses all used textures (and display lists).
     */
    struct texture_pool
    {
      GLuint  *m_textures;
    };

    /**
     * The global texture pool.
     */
    extern texture_pool g_pool;

    /**
     * Initializes the global texture pool.
     * @pre There exists an active rendering context.
     * @post The global texture pool contains textures usable for rendering.
     */
    void init_textures(void);

    /**
     * Renders a string of text.
     * @param p_str The string to render.
     * @param p_x The horizontal position to render the string at.
     * @param p_y The vertical position to render the string at.
     * @param p_sz The size of each rendered character.
     * @param p_space The horizontal space between characters.
     * @param max_width maximum width where text will be printed.
     * @param max_height maximum height where text will be printed.
     * @pre True.
     * @post The string is rendered at the specified position, with the specified size and horizontal space.
     */
    void render_text(const wxString &p_str, float p_x, float p_y, float max_width, float max_height, bool p_center = false);

  } // namespace grapeapp
} // namespace grape

#endif // GRAPEAPP_FONT_RENDERER_H
