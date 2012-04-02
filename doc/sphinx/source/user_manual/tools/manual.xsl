<?xml version="1.0" encoding="ISO-8859-1"?>

<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
<xsl:output method='text' indent='no'/>

<xsl:template match="/">
  <xsl:apply-templates/>
</xsl:template>

<xsl:template match="tool">
  <xsl:text>.. manpage-</xsl:text>
  <xsl:apply-templates select="name"/>
  <xsl:text>
  
Manual page for </xsl:text>
  <xsl:apply-templates select="name"/>
  <xsl:text>
--------------------------------------------------------------------------------

Usage
^^^^^

::

</xsl:text>
  <xsl:text>   </xsl:text>
  <xsl:apply-templates select="name"/>
  <xsl:text>   </xsl:text>
  <xsl:apply-templates select="usage"/>
  <xsl:text>

Description
^^^^^^^^^^^

</xsl:text>
  <xsl:apply-templates select="description"/>
<xsl:text>

Command line options
^^^^^^^^^^^^^^^^^^^^

</xsl:text>
  <xsl:apply-templates select="options"/>

  <xsl:if test="known_issues">
    <xsl:text>
  
Known issues
^^^^^^^^^^^^

</xsl:text>
    <xsl:apply-templates select="known_issues"/>
  </xsl:if>

  <xsl:text>

Author
^^^^^^

</xsl:text>
  <xsl:apply-templates select="author"/>
</xsl:template>

<xsl:template match="options">
  <xsl:for-each select="option">
    <xsl:if test="@standard='no'">
      <xsl:apply-templates select="."/>
    </xsl:if>
  </xsl:for-each>
  <xsl:if test="option/@standard='yes'">
    <xsl:text>
Standard options
""""""""""""""""

</xsl:text>
    <xsl:for-each select="option">
      <xsl:if test="@standard='yes'">
        <xsl:apply-templates select="."/>
      </xsl:if>
    </xsl:for-each>
  </xsl:if>
</xsl:template>

<xsl:template match="option">
  <xsl:if test="short">
    <xsl:text>``-</xsl:text><xsl:apply-templates select="short"/>
    <xsl:apply-templates select="option_argument">
      <xsl:with-param name="long" value="no"/>
    </xsl:apply-templates>
    <xsl:text>`` , </xsl:text>
  </xsl:if>
  <xsl:text>``--</xsl:text>
  <xsl:apply-templates select="long"/>
  <xsl:apply-templates select="option_argument">
    <xsl:with-param name="long" value="yes"/>
  </xsl:apply-templates>
  <xsl:text>``

  </xsl:text>
  <xsl:text>   </xsl:text>
  <xsl:apply-templates select="description"/>
  <xsl:text>
  
  </xsl:text>
  <xsl:apply-templates select="option_argument/values"/>
  <xsl:text>
      
</xsl:text>
</xsl:template>

<xsl:template match="values">
  <xsl:apply-templates select="value"/>
</xsl:template>

<xsl:template match="value">
  <xsl:text>   </xsl:text>
  <xsl:if test="short">
    <xsl:text>``</xsl:text><xsl:apply-templates select="short"/>
    <xsl:text>``, </xsl:text>
  </xsl:if>
  <xsl:text>``</xsl:text><xsl:apply-templates select="long"/>
  <xsl:text>``
  
  </xsl:text>
  <xsl:text>      </xsl:text>
  <xsl:apply-templates select="description"/>
  <xsl:text>
  
  </xsl:text>
</xsl:template>

<xsl:template match="description">
  <xsl:apply-templates/>
</xsl:template>

<xsl:template match="option_argument">
<xsl:param name="long"/>
  <xsl:if test="@optional='yes'">
    <xsl:text>[</xsl:text>
  </xsl:if>
  <xsl:if test="$long='yes'">
    <xsl:text>=</xsl:text>
  </xsl:if>
  <xsl:apply-templates select="name" />
  <xsl:if test="@optional='yes'">
    <xsl:text>]</xsl:text>
  </xsl:if>
</xsl:template>

</xsl:stylesheet>
