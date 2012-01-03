<?xml version="1.0" encoding="ISO-8859-1"?>

<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
<xsl:output method='text'/>

<!-- Root of the document -->
<xsl:template match="/">
  <xsl:apply-templates/>
</xsl:template>

<!-- <doxygenindex> generates a class and file listing for the current
     library -->
<xsl:template match="doxygenindex">
  
  <xsl:text>
=========
Reference
=========

-------
Classes
-------

</xsl:text>

  <xsl:for-each select="compound">
    <xsl:if test="@kind='class'">
      <xsl:if test="not(starts-with(name, 'std::'))">
        <xsl:text>  * :cpp:class:`</xsl:text>
        <xsl:value-of select="name"/>
        <xsl:text>`
</xsl:text>
      </xsl:if>
    </xsl:if>
  </xsl:for-each>

<xsl:text>
-----
Files
-----

</xsl:text>

  <xsl:for-each select="compound">
    <xsl:if test="@kind='file'">
      <xsl:if test="contains(name, '.')">
        <xsl:text>  * :ref:`</xsl:text>
        <xsl:value-of select="name"/>
        <xsl:text>`
</xsl:text>
      </xsl:if>
    </xsl:if>
  </xsl:for-each>
  
<xsl:text>
.. include listing.rst
</xsl:text>
  
</xsl:template>

</xsl:stylesheet>
