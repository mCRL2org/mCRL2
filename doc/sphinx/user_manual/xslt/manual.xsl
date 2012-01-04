<?xml version="1.0" encoding="ISO-8859-1"?>

<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
<xsl:output method='text' indent='no'/>

<xsl:template match="/">
  <xsl:apply-templates/>
</xsl:template>

<xsl:template match="tool">
  <xsl:apply-templates select="name"/>
  <xsl:apply-templates select="arguments"/>
</xsl:template>

<xsl:template match="name">
  <xsl:text>Manual page for </xsl:text>
  <xsl:value-of select="."/>
  <xsl:text>
===============================================================================

</xsl:text>
</xsl:template>

<xsl:template match="arguments">
  <xsl:text>Command line arguments
==================================

</xsl:text>
  <xsl:apply-templates/>
</xsl:template>

<xsl:template match="argument">
  <xsl:apply-templates select="identifier"/>
  <xsl:apply-templates select="description"/>
  <xsl:apply-templates select="values"/>
</xsl:template>

</xsl:stylesheet>
