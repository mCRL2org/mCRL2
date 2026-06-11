Implementation of LTSGraph3D
============================

*Author: Ali Deniz Aladagli*

Introduction
------------

The ltsgraph tool allows the visualization of a given labeled transition system,
an LTS, as a graph and the manipulation of its layout. These graphs are state
spaces, where every transition is directed and has a label. The ltsgraph tool
visualizes this state space only in two dimensions. The LTSGraph3D tool discussed
in this report visualizes a graph of an LTS in three dimensions. The changes in
classes with respect to the implementation of ltsgraph are discussed in the
sections below.

Utils namespace
---------------

The Utils namespace provides the LTSGraph3D tool with some general utilities that
are required in the program, such as a combined 3D position variable, vector and
matrix operations. The rotation matrix generating function is also defined here.

Utils::Vect
^^^^^^^^^^^

``Utils::Vect`` is a structure that defines three double precision numbers that
are either used to represent the x, y and z coordinates of an object or a vector
in 3D space. It is used by the ``State`` class to represent the positions of the
states. It is also used in the matrix normalization in the ``GLCanvas`` class.

vecLength
^^^^^^^^^

The ``vecLength`` method returns the length of the vector by using the Pythagoras
theorem in 3D.

angDiff
^^^^^^^

Given two vectors the ``angDiff`` function returns the angle between these two
vectors in radians, by using the fact that the dot product of two vectors is
equal to the product of the two vectors' lengths multiplied by the cosine of the
angle between them. Given the length found by ``Utils::vecLength`` and the dot
product found by ``Utils::dotProd``, this angle can be found by using the arccos
function. To prevent errors produced by the lack of precision of double variables,
the value of the input to the arccos function is always clamped to the
:math:`[-1, 1]` interval where the function is defined.

dotProd
^^^^^^^

Given two vectors, the ``dotProd`` function returns their dot product.

MultGLMatrices
^^^^^^^^^^^^^^

The ``MultGLMatrices`` function takes in two arrays of size 16 that represent two
:math:`4 \times 4` matrices laid out according to OpenGL's column-major
convention. A matrix :math:`M_{4 \times 4}` is represented by an array :math:`A`
of size 16 as:

.. math::

   \begin{pmatrix}
   A_0 & A_4 & A_8    & A_{12} \\
   A_1 & A_5 & A_9    & A_{13} \\
   A_2 & A_6 & A_{10} & A_{14} \\
   A_3 & A_7 & A_{11} & A_{15}
   \end{pmatrix}

Using this notation, this method multiplies the first matrix by the second and
returns the result as the third parameter.

genRotArbAxs
^^^^^^^^^^^^

Given an angle and the normal of the axis to be rotated about, the
``genRotArbAxs`` function generates a rotation matrix.

To create a rotation matrix the unit vector of the rotation axis is needed.
According to right-handed rotations used in OpenGL the unit vector of the
rotation matrix can be found as:

.. math::

   \alpha = \arctan\!\left(\frac{y}{x}\right), \quad
   U_x = -\sin(\alpha), \quad
   U_y = \cos(\alpha)

Where :math:`x` and :math:`y` are the x and y components of the input normal
vector and :math:`U_x` and :math:`U_y` are the x and y components of the unit
vector :math:`\vec{U}` of the rotation axis. Since rotation around the z-axis is
not needed in the program, calculations involving the z component are ignored. A
rotation matrix around an arbitrary axis given by its unit vector can be generated
as:

.. math::

   \begin{pmatrix}
   (1-c) U_x^2 + c          & (1-c) U_x U_y + s U_z  & (1-c) U_x U_z - s U_y  & 0 \\
   (1-c) U_x U_y - s U_z    & (1-c) U_y^2 + c         & (1-c) U_y U_z + s U_x  & 0 \\
   (1-c) U_x U_y + s U_y    & (1-c) U_y U_z - s U_x  & (1-c) U_z^2 + c         & 0 \\
   0                         & 0                        & 0                       & 1
   \end{pmatrix}

Where :math:`c` is the cosine of the angle to be rotated, :math:`s` is the sine
of the angle to be rotated and :math:`U_z` is the z coordinate of the unit vector
of the axis. In the implementation used in this tool, :math:`U_z` is always taken
as zero.

GLUnTransform
^^^^^^^^^^^^^

Given a modeling and viewing transformation matrix, the ``GLUnTransform`` function
reverses these transformations for a given vector. To do this, the following facts
are used:

- The inverse of a rotation matrix is equal to its transpose, since the
  determinant of a proper rotation matrix is equal to 1.
- To reverse the effects of a modeling and viewing transformation matrix, the
  inverse of that matrix is required.

Since a rotation matrix is itself a modeling and viewing transformation matrix, a
prerequisite that the input matrix is a proper rotation matrix allows us to use
these two facts. Even if the input matrix contains translations in the rightmost
column, it can be ignored: when transposed, this column only affects the fourth
coordinate of the input vector (which defines how it will be affected by further
translations, but is not used in this program again). The transpose of the input
:math:`4 \times 4` matrix and the :math:`4 \times 1` coordinates to be reversed
are multiplied using matrix multiplication. The first three elements of the result
are the x, y and z coordinates respectively.

State
-----

The ``State`` class is used to represent the states. The only change from ltsgraph
is that this class now uses the ``Utils::Vect`` structure, now defined in 3D, to
represent the state's location in the three dimensional coordinate system. The
constructor is changed accordingly.

Transition
----------

The ``Transition`` class is used to represent transitions. Each transition has a
label and a handle (drawn as a small square). The line drawn to represent the
transition always goes through this handle and the user can click on this handle
and move it to curve the transition. The main changes in the ``Transition`` class,
apart from adding the z coordinate to the label positions, concern the coordinates
of the handles of the transitions. In both ltsgraph and LTSGraph3D, when a handle
is moved and given a new position, this position is kept by how it differs from
its original position. The original position of a handle is the mid-point of the
origin and the target states for non-self loop transitions, which creates a
straight line. While in 2D keeping one angle is enough to clarify this difference,
in 3D three angles are needed.

If there is a transition between an origin state :math:`O`, a target state
:math:`T` and the new coordinates of the handle are defined by the vector
:math:`\vec{H}`, the parameters needed to recalculate :math:`\vec{H}` are:

.. math::

   \begin{gathered}
   \vec{TRA} = \vec{T} - \vec{O} \\[4pt]
   \vec{HAN} = \vec{H} - \vec{O} \\[4pt]
   control\alpha = \arctan\!\left(\frac{\vec{HAN}_x}{\sqrt{\vec{HAN}_y^2 + \vec{HAN}_z^2}}\right)
                 - \arctan\!\left(\frac{\vec{TRA}_x}{\sqrt{\vec{TRA}_y^2 + \vec{TRA}_z^2}}\right) \\[4pt]
   control\beta  = \arctan\!\left(\frac{\vec{HAN}_y}{\sqrt{\vec{HAN}_x^2 + \vec{HAN}_z^2}}\right)
                 - \arctan\!\left(\frac{\vec{TRA}_y}{\sqrt{\vec{TRA}_x^2 + \vec{TRA}_z^2}}\right) \\[4pt]
   control\gamma = \arctan\!\left(\frac{\vec{HAN}_z}{\sqrt{\vec{HAN}_y^2 + \vec{HAN}_x^2}}\right)
                 - \arctan\!\left(\frac{\vec{TRA}_z}{\sqrt{\vec{TRA}_y^2 + \vec{TRA}_x^2}}\right) \\[4pt]
   controlDistance = \frac{|\vec{HAN}|}{|\vec{TRA}|}
   \end{gathered}

Where :math:`\vec{TRA}` is the vector that represents the straight line between
the origin and the target states. :math:`\vec{HAN}` is the vector that represents
the straight line between the origin state and the new handle position.
:math:`control\alpha`, :math:`control\beta` and :math:`control\gamma` are the
angles that need to be kept and :math:`controlDistance` is the proportion needed
to calculate where the handle stands in the direction of :math:`\vec{HAN}`. These
angles are the decompositions of the angle between :math:`\vec{TRA}` and
:math:`\vec{HAN}` into three angles with three planes (y-z, x-z and x-y
respectively).

For a self-loop transition the difference of the handle point with respect to the
state is kept in terms of these angles:

.. math::

   \begin{gathered}
   control\alpha = \arctan\!\left(\frac{\vec{HAN}_x}{\sqrt{\vec{HAN}_y^2 + \vec{HAN}_z^2}}\right) \\[4pt]
   control\beta  = \arctan\!\left(\frac{\vec{HAN}_y}{\sqrt{\vec{HAN}_x^2 + \vec{HAN}_z^2}}\right) \\[4pt]
   control\gamma = \arctan\!\left(\frac{\vec{HAN}_z}{\sqrt{\vec{HAN}_y^2 + \vec{HAN}_x^2}}\right) \\[4pt]
   controlDistance = \frac{|\vec{HAN}|}{200}
   \end{gathered}

Where :math:`\vec{HAN}` is defined as it is for non-self looping transitions.

The reason the handle position is kept in this way is to guarantee that the
handle moves consistently when the origin or the target states move. In this way
the angle of the curve is preserved.

LTSGraph3D
----------

The ``LTSGraph3D`` class contains the entry point to the program. Nearly all of
the implementation is the same as ltsgraph. Having access to nearly all the other
classes, the following methods are implemented here.

LTSGraph3D::moveObject
^^^^^^^^^^^^^^^^^^^^^^

In LTSGraph3D some of the drawn objects can be moved by selecting them with the
left mouse button. These objects can be a state, a transition handle or a
transition label. The ``LTSGraph3D::moveObject`` method moves the selected object
according to the current modeling and viewing transformations, so that the object
always moves in the same direction as the mouse movement and by the same
magnitude. This enables moving an object in three dimensions. To do this we
assume that the movement on the screen is the transformed version of the movement
that should have been done internally. Reversing the transformation on the
movement input with ``Utils::GLUnTransform`` gives us the internal movement.
This allows us to apply the proper direction to the movement. We still have to
convert the screen coordinates to the way they are stored in the ``Graph`` class
and reverse the projection so that objects at any depth stay under the mouse
while being moved. The screen-to-storage conversion is done by reversing the
calculations performed while drawing in ``Visualizer::visualize``:

.. math::

   rad = radius \times pixelSize

:math:`rad` is equal to the :math:`radius` used for the states returned by
``Visualizer::getRadius`` multiplied by the :math:`pixelSize` returned by
``GLCanvas::getPixelSize``. Pixels are the units used for screen coordinates.

.. math::

   pixelToWorld = 3 \times 550 \;/\; pheight

:math:`pixelToWorld` is a coefficient for screen-to-world coordinates conversion
found by trial and error for an object moving along the world y-axis at a
constant depth. :math:`550` is the window height in pixels used for the trials,
:math:`pheight` is the height in pixels of the current window (used to make
movement consistent with any window size) and :math:`3` is the constant found
through these trials.

.. math::

   \begin{gathered}
   x' = x \times 2000 \;/\; (width  - rad \times 2) \times pixelToWorld \\
   y' = y \times 2000 \;/\; (height - rad \times 2) \times pixelToWorld \\
   z' = z \times 2000 \;/\; (depth  - rad \times 2) \times pixelToWorld
   \end{gathered}

:math:`x`, :math:`y` and :math:`z` are the untransformed pixel coordinates;
:math:`width`, :math:`height` and :math:`depth` are the world coordinates of the
dimensions of the window returned by ``GLCanvas::getSize``; and :math:`2000` is
the constant used for the conversion between world coordinates and storing
coordinates in ``Visualizer::visualize``. :math:`x'`, :math:`y'` and :math:`z'`
are the new world coordinates, yet to be modified to the correct magnitude.

To apply the correct magnitude, we need the depth of the object in world
coordinates (where it is drawn). Depth is defined with respect to the local
coordinate system of the graph and differs from the z coordinate stored in the
object when any rotations have been done. To obtain it we translate to the
coordinates of the object, just as if we were drawing it. At this point the
:math:`14^{th}` element of the modeling and viewing matrix (OpenGL notation),
after the translation, is the real depth where the object was drawn. For any
visible object this value is always negative (in OpenGL, the screen interior is
the negative z axis). Since projection is directly proportional to depth, we can
multiply the real depth directly with the storing coordinates computed above:

.. math::

   x'' = x' \times depth, \quad
   y'' = y' \times depth, \quad
   z'' = z' \times depth

By using the object's setters we can directly add these values (:math:`x''`,
:math:`y''` and :math:`z''`) to its current coordinates.

LTSGraph3D::getCanvasMdlvwMtrx
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

This method returns the current modeling and viewing transformation matrix for
classes that can access ``LTSGraph3D`` but not ``GLCanvas``, by forwarding to
``GLCanvas::getMdlvwMtrx``.

LTSGraph3D::getCanvasCamPos
^^^^^^^^^^^^^^^^^^^^^^^^^^^^

This method returns the current camera position for classes that can access
``LTSGraph3D`` but not ``GLCanvas``, by forwarding to ``GLCanvas::getCamPos``.

GLCanvas
--------

As in the tool ltsgraph, canvas operations such as projection and object picking
are all implemented in the ``GLCanvas`` class, which is an extension of the
``wxGLCanvas`` class defined in wxWidgets. Depth testing is enabled at
initialization, meaning that objects behind others will not be drawn.

GLCanvas::display
^^^^^^^^^^^^^^^^^

The perspective projections needed to apply the 3D effect are performed by calls
to ``gluPerspective`` from the glu library. The far clipping plane distance is
selected so that after any rotations the whole graph remains visible. To allow the
user to visualize the graph in any way, features such as rotation, panning and
zooming are implemented using modeling and viewing transformations. For ease of
other calculations these transformations are implemented so that the camera is
always at the center of the world coordinates and the model is rotated or moved
instead of the camera.

Rotations are implemented as arc-ball rotations to avoid problems such as gimbal
lock (which may cause unexpected rotations). An arc-ball rotation always occurs in
planes produced by the world axes (which never change) rather than in planes
produced by the local axes of the model (which change with every new rotation).
Since the OpenGL library performs all rotations in local planes, functions to
create and multiply rotation matrices are implemented in the Utils namespace. To
achieve an arc-ball rotation, every new rotation is applied before the previous
modeling and viewing transformations; these transformations are represented by a
:math:`4 \times 4` matrix implemented as an array of size 16 for compatibility
with the OpenGL library. While a rotation tool is being used, every mouse movement
defines a vector whose magnitude is the angle to be rotated and whose direction is
the normal to the rotation axis. The z component of this vector is always zero
since mouse movement cannot be mapped to the third dimension. Giving these values
as input to ``Utils::genRotArbAxs`` generates the required rotation matrix. After
this :math:`4 \times 4` matrix is generated, it is multiplied with the previous
modeling and viewing matrix using ``Utils::MultGLMatrices``:

.. math::

   MVT_{4 \times 4}' = RM_{4 \times 4} \times MVT_{4 \times 4}

Where :math:`RM_{4 \times 4}` is the rotation matrix generated by the formula
above and :math:`MVT_{4 \times 4}` is the current modeling and viewing
transformation matrix. Even though double-precision variables are used, some
accuracy is always lost during these operations, resulting in shearing and
stretching in the drawn graph. Therefore after a rotation, :math:`MVT_{4 \times
4}` is normalized by ``GLCanvas::normalizeMatrix`` to minimize these errors. This
function uses OpenGL's rotation functions on an initially identity matrix to
reconstruct a rotation matrix without shearing or stretching that is closest to
the matrix to be normalized; call it :math:`N_{4 \times 4}`. The normalizing
function works on the observation that :math:`MVT_{4 \times 4}` stores, in its
columns, the unit vectors for the three axes of the local coordinate system of the
graph (translations are ignored):

.. math::

   \vec{X} = \{MVT_{0,0}, MVT_{1,0}, MVT_{2,0}\}, \quad
   \vec{Y} = \{MVT_{0,1}, MVT_{1,1}, MVT_{2,1}\}, \quad
   \vec{Z} = \{MVT_{0,2}, MVT_{1,2}, MVT_{2,2}\}

Where :math:`\vec{X}`, :math:`\vec{Y}` and :math:`\vec{Z}` are the unit vectors
in world coordinates. The unit vector for the z-axis (:math:`\vec{Z}`) is assumed
to be in the correct direction. Two rotations are needed to align
:math:`N_{4 \times 4}`'s z-axis with :math:`\vec{Z}`. The first rotation is
around the local y-axis of :math:`N_{4 \times 4}` by the angle between the world
z-axis and the projection of :math:`\vec{Z}` on the x-z plane. The second
rotation is around the local x-axis of :math:`N_{4 \times 4}` by the angle
between :math:`\vec{Z}` and that projection:

.. math::

   \beta  = \arctan\!\left(\frac{MVT_{0,2}}{MVT_{2,2}}\right), \quad
   \gamma = \arctan\!\left(\frac{MVT_{1,2}}{\sqrt{MVT_{0,2}^2 + MVT_{2,2}^2}}\right)

Where :math:`\beta` and :math:`\gamma` are respectively the first and the second
angles. With two calls to ``glRotate``, the new z-axis points in the correct
direction with minimal shearing or stretching. At this step a second assumption is
made: the projection of the deformed y-axis unit vector :math:`\vec{Y}` onto the
local x-y plane gives the correct direction for the new y-axis. This projection
provides an unambiguous angle to rotate around the local z-axis of
:math:`N_{4 \times 4}`. The projection normal is the z-axis of
:math:`N_{4 \times 4}`:

.. math::

   \vec{Y}_{projection} = \vec{Y} - \bigl((\vec{Y} \cdot \vec{normal}) \times \vec{normal}\bigr)

Where :math:`\cdot` is the dot product, implemented as ``Utils::dotProd``.
``Utils::angDiff`` then computes the angle between :math:`\vec{Y}_{projection}`
and the local y-axis of :math:`N_{4 \times 4}`. An ambiguity arises because the
cosine function is modulo :math:`\pi`; it is resolved by checking the sign of the
y coordinate of :math:`\vec{X}` and calling ``glRotate`` with the appropriate
sign. After these rotations the matrix in the current OpenGL context is the
corrected rotation matrix. :math:`MVT_{4 \times 4}` is then replaced with this
matrix.

After doing these calculations, panning and zooming are straightforward:

.. math::

   MVT_{0,3} = -lookX, \quad MVT_{1,3} = -lookY, \quad MVT_{2,3} = -lookZ

Where :math:`lookX` is the panning in the x-axis, :math:`lookY` is the panning
in the y-axis and :math:`lookZ` is the total zooming. For a more realistic
display, a light that always comes from the direction of the camera is added (but
not yet enabled). This is set up before loading the modeling and viewing
transformation matrix to OpenGL so that the light's direction is unaffected by
the transformations. A material behavior is also added to enable state coloring.
After the matrix is loaded, control is given to ``Visualizer::visualize`` to draw
the LTS to the screen. When this function returns, upon the user's request, a
representation of the local coordinate system of the model can be shown in the
bottom-left corner of the window in a separate viewport. To make it more visible,
this coordinate system is not affected by the panning and zooming effects.

Mouse Functions
^^^^^^^^^^^^^^^

All mouse functions are initiated by the wxWidgets event handler. Mouse movement
is processed by ``GLCanvas::onMouseMove``, which only acts when a mouse button is
pressed. The three buttons behave as follows:

- **Left Mouse Button** — Defined by ``GLCanvas::onMouseLftDown``, the left mouse
  button can have up to two functions, one active when an object is clicked and
  the other active otherwise. When not clicking on an object, the left mouse can
  be used for panning, zooming and rotating, or this tool can be completely
  disabled to prevent unwanted viewpoint changes. When clicked on an object,
  either the state's color is changed to the active color or the state is moved
  while the button is held; this object-interaction tool cannot be disabled. These
  tools are identified by numbers defined in the ``IDS`` namespace, extended after
  the last identification number used by wxWidgets. To check whether an object is
  clicked, ``GLCanvas::pickObjects`` returns a boolean instead of void. To move an
  object, new mouse coordinates are passed to ``LTSGraph3D::moveObject`` since the
  depth of the selected object must be known and the ``GLCanvas`` class does not
  have access to the selected objects.

- **Right Mouse Button** — The right mouse button has two functions. If a state
  is clicked, that state's position is stabilized so that only the user can move
  it; the layout algorithm cannot. If empty space is clicked, the button always
  performs a rotation.

- **Middle Mouse Button** — Zooming can be done by turning the wheel
  (``GLCanvas::onMouseWhl``) and rotation by clicking the middle button anywhere
  in the window and moving the mouse (``GLCanvas::onMouseMidDown``).

When one of these functions is active the mouse cursor is changed by
``GLCanvas::setMouseCursor`` to a cursor appropriate for the active tool,
identified by its number.

GLCanvas::pickObjects
^^^^^^^^^^^^^^^^^^^^^

To pick objects on the screen, the rendering mode is switched to selection mode.
A picking region is created around the click location. In this mode the names
assigned to objects when they were drawn are recorded in a buffer when the objects
fall inside the picking region (called a hit). Using these names the selected
object can be identified. If the number of hits is not greater than zero, no
object is selected. Only one object can be selected at a time; clicking on empty
space or a new object clears the previous selection. Selected objects are pointed
to by the ``LTSGraph3D`` class and their boolean selected flag is set to true by
the ``Graph`` class.

GLCanvas::getCamPos
^^^^^^^^^^^^^^^^^^^

The ``GLCanvas::getCamPos`` method returns the camera position as it would be if
the camera were moved instead of the graph.

GLCanvas::getSize
^^^^^^^^^^^^^^^^^

The ``GLCanvas::getSize`` method defines the height and width in world coordinates
as in the same-named method in ltsgraph. The depth is defined as their arithmetic
mean to provide balanced sizes in the environment.

GLCanvas::getMdlvwMtrx
^^^^^^^^^^^^^^^^^^^^^^

The ``GLCanvas::getMdlvwMtrx`` method supplies other classes with the current
modeling and viewing transformation matrix.

Visualizer
----------

All drawing is done in this class, as it is in the ltsgraph tool.

Visualizer::drawStates
^^^^^^^^^^^^^^^^^^^^^^

The ``Visualizer::drawStates`` method initiates the drawing of the objects. For
every state, its outgoing transitions and self-loops are drawn. Text rendering is
deferred to a separate pass because texts cannot be rendered without disabling the
depth mask (which prevents writes to the depth buffer used for depth testing),
so they must be rendered after everything else. After every state and transition
are drawn by calls to ``Visualizer::drawState`` and ``Visualizer::drawTransition``
(``Visualizer::drawSelfLoop`` for self-loops) respectively, texts are rendered
using ``Visualizer::drawTransLabel`` and ``Visualizer::drawStateText``. All four
methods convert storing coordinates to world coordinates using:

.. math::

   \begin{gathered}
   rad = radius \times pixelSize \\
   x' = (x \;/\; 2000) \times (width  - rad \times 2) \\
   y' = (y \;/\; 2000) \times (height - rad \times 2) \\
   z' = (z \;/\; 2000) \times (depth  - rad \times 2)
   \end{gathered}

:math:`rad` is equal to the :math:`radius` kept in the ``Visualizer`` class
multiplied by the :math:`pixelSize` returned by ``GLCanvas::getPixelSize``.
:math:`width`, :math:`height` and :math:`depth` are the world coordinates of the
window dimensions returned by ``GLCanvas::getSize``, and :math:`2000` is the
constant for the conversion between world and storing coordinates. These
calculations ensure consistent rendering across different window sizes and aspect
ratios.

Visualizer::drawState
^^^^^^^^^^^^^^^^^^^^^

To draw the states and their selection borders, ``gluSphere`` and
``gluPartialDisk`` from the glu library are used respectively. To make the borders
visible from any angle, the modeling and viewing transformation matrix is applied
only to the translation part of the ring (so the disk always faces the user). Once
the border is drawn, lighting is enabled to draw the state. Spheres are the only
3D objects that require lighting, so it is enabled only here. After two calls to
``glPushName`` make the state selectable, the sphere is drawn at its location with
radius :math:`rad` calculated above. Lighting is then disabled so that other
drawings such as transitions are unaffected.

Visualizer::drawTransition
^^^^^^^^^^^^^^^^^^^^^^^^^^

Second order Bézier curves using one control point are used to draw the
transitions, as in ltsgraph. The transition handle (the virtual control point)
lies at the mid-point of this control point and the mid-point of the two states
that the transition is tied to. After the transition is drawn using Bézier
equations, if displaying transition handles is enabled, a white cube with colored
sides (depending on selection) is drawn after a ``glPushName`` call to make it
selectable. To indicate the direction of the transition, small cones are drawn
using ``Visualizer::drawArrowHead``. First we translate to the point where the
target sphere was drawn. Since ``Visualizer::drawArrowHead`` draws cones pointing
outwards from the current local z-axis, we must align that direction with the
positive z-axis. The desired cone direction runs from the real control point
described above to the center of the target sphere. This is realized with two
``glRotate`` calls:

.. math::

   \beta  = \arctan\!\left(\frac{x_{To} - x_{Control}}{z_{To} - z_{Control}}\right), \quad
   \gamma = \arctan\!\left(\frac{y_{To} - y_{Control}}{\sqrt{(x_{To} - x_{Control})^2 + (z_{To} - z_{Control})^2}}\right)

The first rotation is around the local y-axis by angle :math:`\beta`. The second
rotation is around the local x-axis by angle :math:`-\gamma`. After these two
rotations, translating :math:`2 \times rad` along the local negative z-axis draws
the cone pointing toward the sphere rather than inside it.

Visualizer::drawSelfLoop
^^^^^^^^^^^^^^^^^^^^^^^^

Third order Bézier curves with two control points are used to curve the transition
back to its looping state, as in ltsgraph. Since these transitions have only one
handle, the plane of the self-loop cannot be rotated around the axis through the
state and the handle without also moving the handle. Therefore, for simplicity,
the two control points always share the same depth in the local coordinate system
of the graph. Instead of using the angle between the transition axis and the x-axis
as in ltsgraph, here the angle between the transition axis and the x-y plane is
used to compute the x and y coordinates of the two control points. Notice that the
sine of this angle gives the y coordinate of the handle. To preserve the distances
of the control points from the looping state when adding the z coordinate, the z
coordinate is computed in a similar manner to the x and y coordinates of the
control points:

.. math::

   \begin{gathered}
   zFactor  = \frac{4 \times (zVirtual - zState)}{3 \times \sin(\gamma)} \\[4pt]
   zControl = zState + zFactor \times \sin(\gamma)
   \end{gathered}

Where :math:`\gamma` is the angle between the transition axis and the x-y plane,
returned by ``Transition::getControlGamma``. :math:`zState` is the z coordinate
of the looping state and :math:`zVirtual` is the z coordinate of the handle.
:math:`zControl` is the depth of both control points. This calculation is only
performed when :math:`|\sin(\gamma)| > 0.01`; otherwise the formula is
undefined. When :math:`|\sin(\gamma)| \leq 0.01` the angle difference is
negligible and the z coordinate of the control points is set equal to that of
the looping state.

Visualizer::drawArrowHead
^^^^^^^^^^^^^^^^^^^^^^^^^

Assuming the proper translations and rotations have been applied,
``Visualizer::drawArrowHead`` draws a cone (a cylinder with top radius zero) with
the given height and a base radius equal to one fifth of that height, using
``gluCylinder`` from the glu library. The drawn cone points toward the local
positive z-axis.

Springlayout
------------

Like in ltsgraph, when enabled the ``Springlayout`` class tries to find an
optimised, stable layout using force-directed placement, seeking an equilibrium
state for every vertex where the sum of all forces acting on it is zero. Vertices
function as equally charged particles that repel each other more strongly as they
get closer. Edges function as springs that exert force on the vertices connected
to them depending on the edge's current length relative to its zero-energy length.
If the length of an edge is smaller than its zero-energy length it exerts a
repulsive force; otherwise an attractive force.

Springlayout::layoutGraph
^^^^^^^^^^^^^^^^^^^^^^^^^

The algorithm works exactly the same way in three dimensions as in two dimensions.
The walls that keep the graph confined to the window in ltsgraph are removed here,
since the user can move, zoom or rotate anywhere in the viewing space. The third
coordinate is added and the force formulas are extended to 3D. The electric and
spring force formulas are:

.. math::

   \vec{F}^{electric}(u,v) = \frac{e}{\vec{d}(u,v)^2}

.. math::

   \vec{F}^{spring}(u,v) = s \times \log\!\left(\frac{\vec{d}(v,u)}{l}\right)

Where :math:`\vec{F}^{electric}(u,v)` is the repulsion force exerted on vertex
:math:`v` by vertex :math:`u`; :math:`\vec{F}^{spring}(u,v)` is the spring force
exerted on vertex :math:`v` by a spring connected to vertex :math:`u`;
:math:`\vec{d}(u,v)` is the distance between vertices :math:`u` and :math:`v`;
:math:`e` is the equal charge coefficient of all vertices; :math:`s` is the equal
stiffness coefficient; and :math:`l` is the equal zero-energy length of all edges.

The gravitational force components are scaled according to the imaginary screen
depth defined in the implementation:

.. math::

   F_x^{gravitation}(v) = -2 \, x_v \;/\; 2000, \quad
   F_y^{gravitation}(v) = -2 \, y_v \;/\; 2000, \quad
   F_z^{gravitation}(v) = -2 \, z_v \;/\; 2000

All three components of the total force on vertex :math:`v` are:

.. math::

   F_x^{total}(v) =
     \sum_{u \in V}\!\left(\|\vec{F}^{electric}(u,v)\| \cdot \frac{x_v - x_u}{\|\vec{d}(u,v)\|}\right)
   + \sum_{(u,v) \in E}\!\left(\|\vec{F}^{spring}(u,v)\| \cdot \frac{x_v - x_u}{\|\vec{d}(u,v)\|}\right)
   + F_x^{gravitation}(v)

.. math::

   F_y^{total}(v) =
     \sum_{u \in V}\!\left(\|\vec{F}^{electric}(u,v)\| \cdot \frac{y_v - y_u}{\|\vec{d}(u,v)\|}\right)
   + \sum_{(u,v) \in E}\!\left(\|\vec{F}^{spring}(u,v)\| \cdot \frac{y_v - y_u}{\|\vec{d}(u,v)\|}\right)
   + F_y^{gravitation}(v)

.. math::

   F_z^{total}(v) =
     \sum_{u \in V}\!\left(\|\vec{F}^{electric}(u,v)\| \cdot \frac{z_v - z_u}{\|\vec{d}(u,v)\|}\right)
   + \sum_{(u,v) \in E}\!\left(\|\vec{F}^{spring}(u,v)\| \cdot \frac{z_v - z_u}{\|\vec{d}(u,v)\|}\right)
   + F_z^{gravitation}(v)

The mass of each vertex is taken as one, so its acceleration equals the total
force; the velocity is not conserved and is reset to zero at the beginning of
each iteration. The new coordinates of vertex :math:`v` are therefore:

.. math::

   (x_v', y_v', z_v') =
   \bigl(x_v + F_x^{total}(v),\; y_v + F_y^{total}(v),\; z_v + F_z^{total}(v)\bigr)
