<?xml version="1.0" encoding="ISO-8859-1"?>

<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
<xsl:output method='text' indent='no'/>
<xsl:strip-space elements="*"/>

<xsl:param name="dir"/>

<xsl:template match="/">
  <xsl:param name="file"/>
  <xsl:apply-templates>
    <xsl:with-param name="file" select="$file"/> 
  </xsl:apply-templates>
</xsl:template>


<!-- a <compounddef> generates a pages in RST -->
<xsl:template name="compounddef" match="compounddef">
<xsl:param name="file"/>

<xsl:choose>
  <xsl:when test="@kind='file'">
    <xsl:text>
.. _</xsl:text>
    <xsl:value-of select="compoundname"/>
  <xsl:text>:
  
</xsl:text>
  </xsl:when>
</xsl:choose>

<xsl:choose>
  <xsl:when test="@kind!='namespace'">
<xsl:text>
===============================================================================
</xsl:text>
<xsl:value-of select="compoundname"/>
<xsl:text>
===============================================================================

</xsl:text>
  </xsl:when>
</xsl:choose>

  <xsl:choose>
    <xsl:when test="@kind='class'">
      <xsl:text>.. cpp:class:: </xsl:text>
      <xsl:value-of select="compoundname"/>
      <xsl:text>
      
   </xsl:text>
      <xsl:value-of select="briefdescription"/>
      <xsl:text>
      
   </xsl:text>
      <xsl:value-of select="detaileddescription"/>
      <xsl:text>
      
</xsl:text>
      
      <xsl:apply-templates select="sectiondef"/>
    </xsl:when>

    <xsl:when test="@kind='file'">
      <xsl:text>File </xsl:text>
      <xsl:value-of select="compoundname"/>
      <xsl:text>
      
   </xsl:text>
      <xsl:value-of select="briefdescription"/>
      <xsl:text>
      
   </xsl:text>
      <xsl:value-of select="detaileddescription"/>
      <xsl:text>

</xsl:text>

      <xsl:if test="innerclass">
      <xsl:text>
Classes
-------

</xsl:text>
      <xsl:apply-templates select="innerclass"/>
      </xsl:if>
      
      <xsl:apply-templates select="innernamespace">
        <xsl:with-param name="file" select="compoundname"/>
      </xsl:apply-templates>
      
    </xsl:when>
    
    <xsl:when test="@kind='namespace'">
      <xsl:apply-templates select="sectiondef">
        <xsl:with-param name="file" select="$file"/>
      </xsl:apply-templates>
    </xsl:when>
  </xsl:choose>

</xsl:template>

<!-- a <sectiondef> generates a section, with a list of elements -->
<xsl:template match="sectiondef">
  <xsl:param name="file"/>
  
  <xsl:if test="not(memberdef/location) or contains(memberdef/location/@file, $file)">
  <xsl:choose>
    <xsl:when test="@kind='enum'">
      <xsl:text>Enumerated types</xsl:text>
    </xsl:when>
    <xsl:when test="@kind='var'">
      <xsl:text>Variables</xsl:text>
    </xsl:when>
    <xsl:when test="@kind='func'">
      <xsl:text>Functions</xsl:text>
    </xsl:when>
    <xsl:when test="@kind='public-func'">
      <xsl:text>Public member functions</xsl:text>
    </xsl:when>  
    <xsl:when test="@kind='public-type'">
      <xsl:text>Public types</xsl:text>
    </xsl:when>
    <xsl:when test="@kind='protected-attrib'">
      <xsl:text>Protected attributes</xsl:text>
    </xsl:when>  
    <xsl:otherwise>
      <xsl:value-of select="@kind" />
    </xsl:otherwise>
  </xsl:choose>
  <xsl:text>
-------------------------------------------------------------------------------

</xsl:text>
  </xsl:if>
  <xsl:apply-templates select="memberdef">
    <xsl:with-param name="file" select="$file"/>
  </xsl:apply-templates>
</xsl:template>

<!-- a <memberdef> generates the RST for a class member/typedef/attribute, etc -->
<xsl:template name="memberdef" match="memberdef">
  <xsl:param name="file"/>
  
  <xsl:choose>
    <xsl:when test="not(location) or contains(location/@file, $file)">
 
  <xsl:choose>
    <xsl:when test="@kind='typedef'">
      <xsl:text>   .. cpp:type:: </xsl:text>
      <xsl:value-of select="name"/>
    </xsl:when>
    <xsl:when test="@kind='enum'">
      <xsl:text>   .. cpp:type:: </xsl:text>
      <xsl:value-of select="name"/>
      <xsl:text>
      
      **Values:**
</xsl:text>
      <xsl:apply-templates select="enumvalue"/>
    </xsl:when>
    
    <xsl:when test="@kind='function'">
      <xsl:text>   .. cpp:function:: </xsl:text>
      <xsl:value-of select="type"/>
      <xsl:text> </xsl:text>
      <xsl:value-of select="name"/>
      <xsl:value-of select="argsstring"/>
    </xsl:when>
    
    <xsl:when test="@kind='variable'">
      <xsl:text>   .. cpp:member:: </xsl:text>
      <xsl:value-of select="type"/>
      <xsl:text> </xsl:text>
      <xsl:value-of select="name"/>
    </xsl:when>
    
    <xsl:otherwise>
      <xsl:value-of select="kind"/>
      <xsl:value-of select="name"/>
    </xsl:otherwise>
  </xsl:choose>
  <xsl:text>
      
      </xsl:text>
  <xsl:value-of select="briefdescription"/>
  <xsl:text>
  
      </xsl:text>
  <xsl:apply-templates select="detaileddescription"/>
    <xsl:text>
    
</xsl:text>
</xsl:when>
</xsl:choose>
</xsl:template>

<!-- a <detaileddescription> is printed -->
<xsl:template match="detaileddescription">
  <xsl:apply-templates />
</xsl:template>

<!-- a <innerclass> directive. This contains information about the defined
     class in a file -->
<xsl:template match="innerclass">
  <xsl:text>   * :cpp:class:`</xsl:text>
  <xsl:value-of select="."/>
  <xsl:text>`
  
</xsl:text>
</xsl:template>

<!-- a <innernamespace> directive. This contains information about the used
     namespaces in a file -->
<xsl:template match="innernamespace">
  <xsl:param name="file"/>
  <xsl:apply-templates select="document(concat('/', $dir, '/',@refid,'.xml'))">
    <xsl:with-param name="file" select="$file"/>
  </xsl:apply-templates>
</xsl:template>

<!-- an <enumvalue> -->
<xsl:template match="enumvalue">
  <xsl:text>        * **</xsl:text>
  <xsl:value-of select="name"/>
  <xsl:text>** </xsl:text>
  <xsl:value-of select="briefdescription"/>
  <xsl:value-of select="detaileddescription"/>
  <xsl:text>
  
</xsl:text>
</xsl:template>

<!-- a <para> is printed, and special tags are treatted specially -->
<xsl:template match="para">
  <xsl:apply-templates />
</xsl:template>

<!-- a <parameterlist> -->
<xsl:template match="parameterlist">
  <xsl:text>
  
      **Parameters:**
</xsl:text>
  <xsl:apply-templates select="parameteritem"/>
</xsl:template>

<!-- a <simplesect>, for example \returns -->
<xsl:template match="simplesect">
  <xsl:choose>
    <xsl:when test="@kind='return'">
      <xsl:text>
      
      **Returns:** </xsl:text>
      <xsl:value-of select="."/>
    </xsl:when>
    <xsl:when test="@kind='warning'">
      <xsl:text>
      .. warning:: </xsl:text>
      <xsl:value-of select="."/>
    </xsl:when>
    <xsl:when test="@kind='note'">
      <xsl:text>
      .. note:: </xsl:text>
      <xsl:value-of select="."/>
    </xsl:when>
    <xsl:otherwise>
      <xsl:value-of select="."/>
    </xsl:otherwise>
  </xsl:choose>
</xsl:template>

<!-- a <parameteritem>, an element of a parameterlist -->
<xsl:template match="parameteritem">
  <xsl:text>        * **</xsl:text>
  <xsl:value-of select="parameternamelist"/>
  <xsl:text>** </xsl:text>
  <xsl:value-of select="parameterdescription"/>
  <xsl:text>
  
</xsl:text>
</xsl:template>

</xsl:stylesheet> 
