LedSync communication protocol
==============================

.. image:: img/Demokit_gr2.jpg
   :align: right
   :width: 250px

mCRL2 was used during the development of the LedSync communication protocol. The
LedSync protocol is used for communication between devices in a light network.
This network consists of input devices that control the lighting (setting
dimming level or changing the colour) and output devices (LEDs). These devices
are part of a physical network such as DMX, Radio Frequency or Powerline
network. For example, when a network is running with LedSync, a user can make
use of a remote control (input device) to dim the light. The input device that
receives the instructions from the user will talk to the corresponding LEDs
(output device) and reduce the light intensity. 

With LedSync it is possible to have multiple in- and output devices. This means
that it must be able to process simultaneously incoming signals and always
produce the correct output. mCRL2 is used to prove the correctness of the
communication protocol and assisted the designers by verifying ideas and
prototypes during the development of the system in order to prevent flaws. 

Besides processing user input LedSync is also responsible for: the discovery of
connected devices, perform periodic light control, control light shows, the
handling of on- and offline devices and recovery from exceptional situations
(e.g. loss of power). 

Technical details
-----------------
To reduce the complexity and the corresponding mCRL2 models the system is
designed using a layered architecture. Similar actions and functionality are
grouped together into several layers, each having its own specific tasks:

  * Physical layer, this layer reads and writes bits from and to the network.
    It processes the digital information into analogue data and vice versa.
  * Service layer, this layer is responsible for reliable message exchange and collision prevention.
  * Application layer, this layer is used for the functions that the users can
    observe or interact with such as processing user input and discovering connected devices.

For each layer a mCRL2 model was created and verified. To combine all layers in
one model was not possible due to the increasing complexity of the system and
the resulting state-space that becomes too big for the computers to calculate.
The system integration containing all layers is verified using an abstract model
containing only the actions that interact between the layers.

Type of verification
^^^^^^^^^^^^^^^^^^^^
Deadlock, reachability and safety properties were validated by explicit
state-space generation (breadth-first search) and the application of modal
formulae.
 
Equipment (computers, CPU, RAM)
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
All verifications were performed on a 64-bit Linux machine with 128GB RAM. 

Models
^^^^^^
The models are confidential. 

Organizational context
----------------------
.. list-table:: 

  * - **Contact person**
    - Jan Friso Groote, Technische Universiteit Eindhoven, The Netherlands.
  * - **Other people involved**
    -   * Koen Schuurman, Technische Universiteit Eindhoven, The Netherlands.
        * Marc Saes (Supervisor, eldoLED, Eindhoven)
  * - **Institution**
    - Technische Universiteit Eindhoven, The Netherlands.
  * - **Industrial partner**
    - eldoLED, Eindhoven, the Netherlands
  * - **Time period**
    - The project was started in February 2008 and was successfully finished in
      August 2008. Since August 2008, some changes have been made and extensions
      have been added to the model. Furthermore, the model was verified with
      bigger instances.

