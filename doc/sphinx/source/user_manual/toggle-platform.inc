.. tip::

   In the instructions below, some parts are specific to an operating system.
   Clicking on the operating system logo will toggle visibility of
   instructions for other operating systems.   

.. raw:: html

   <script type="text/javascript"><!--
   var only = undefined;
   function toggleonly(platform) {
     if (only !== undefined)
     {
       $('.platform-specific').removeClass('invisible');
       only = undefined;
     }
     else
     {
       $('.platform-specific').addClass('invisible');
       $('.' + platform + '-only').removeClass('invisible');
       only = platform;
     }
   }
   //--></script>

.. |enable_toggle| raw:: html

      <script type="text/javascript"><!-- 
        $('div.win-only').bind('click', function() { toggleonly('win'); });
        $('div.mac-only').bind('click', function() { toggleonly('mac'); });
        $('div.linux-only').bind('click', function() { toggleonly('linux'); });
      //--></script>
