<?xml version='1.0'?>
<xsl:stylesheet xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
                xmlns:exsl="http://exslt.org/common"
                xmlns:ng="http://docbook.org/docbook-ng"
                xmlns:db="http://docbook.org/ns/docbook"
                exclude-result-prefixes="db ng exsl"
                version='1.0'>

<!-- ********************************************************************
     $Id: links.xsl 6524 2007-01-18 15:44:19Z xmldoc $
     ********************************************************************

     This file is part of the XSL DocBook Stylesheet distribution.
     See ../README or http://docbook.sf.net/release/xsl/current/ for
     copyright and other information.

     ******************************************************************** -->

<!-- ==================================================================== -->
<!-- * -->
<!-- * This stylesheet: -->
<!-- * -->
<!-- * 1. Identifies all "note sources" (link, annotation, alt, and -->
<!-- *    footnote instances) in a Refentry which do not have the same -->
<!-- *    "earmark" as any preceding notesource in that same Refentry -->
<!-- *    (and for notesources that are links, then only those links -->
<!-- *    whose string value and url attribute value are not identical). -->
<!-- * -->
<!-- * 2. Puts a number inline to mark the place where the notesource -->
<!-- *    occurs in the main text flow. -->
<!-- * -->
<!-- * 3. Generates a numbered endnotes list (titled NOTES in -->
<!-- *    English) at the end of the man page, with the contents of -->
<!-- *    each notesourcse. -->
<!-- * -->
<!-- * Note that table footnotes are not listed in the endnotes list, -->
<!-- * and are not handled by this stylesheet (they are instead -->
<!-- * handled by the table.xsl stylesheet). -->
<!-- * -->
<!-- * Also, we don't get notesources in *info sections or Refmeta or -->
<!-- * Refnamediv or Indexterm, because, in manpages output, contents -->
<!-- * of those are either suppressed or are displayed out of document -->
<!-- * order - for example, the Info/Author content gets moved to the -->
<!-- * end of the page. So, if we were to number notesources in the Author -->
<!-- * content, it would "throw off" the numbering at the beginning of -->
<!-- * the main text flow. -->
<!-- * -->
<!-- * Important: For any notesource whose earmark matches that of a -->
<!-- * preceding notesource in the same Refentry, we assign it the -->
<!-- * number of that previous notesource. -->
<!-- * -->
<!-- * For links, we check to see if the link is empty OR if its -->
<!-- * string value is identical to the value of its url attribute; if -->
<!-- * either of those is true, we just display the value of its url -->
<!-- * attribute (if the link itself is empty), and stop there. -->
<!-- * -->
<!-- * And for the record, one reason we don't use xsl:key to index -->
<!-- * the notesources is that we need to get and check the sets of -->
<!-- * notesources for uniqueness per-Refentry (not per-document). -->
<!-- * -->
<!-- * FIXME: mediaobject and inlinemediaobject should also be handled -->
<!-- * as notesources; should change most link* variable names to -->
<!-- * notesource*; as with "repeat" URLS, alt instances that have the -->
<!-- * same string value as preceding ones (likely to occur for repeat -->
<!-- * acroynyms and abbreviations) should be listed only once in -->
<!-- * the endnotes list, and numbered accordingly inline; split -->
<!-- * man.indent.width into man.indent.width.value (default 4) and -->
<!-- * man.indent.width.units (default n); also, if the first child of -->
<!-- * notesource is some block content other than a (non-formal) -->
<!-- * paragraph, the current code will probably end up generating a -->
<!-- * blank line after the corresponding number in the endnotes -->
<!-- * list... we should probably try to instead display the title of -->
<!-- * that block content there (if there is one: e.g., the list title, -->
<!-- * admonition title, etc.) -->
<!-- * -->
<!-- ==================================================================== -->
      
<xsl:template name="get.all.links.with.unique.urls">
  <xsl:if test="$man.links.are.numbered != 0">
    <xsl:for-each select="//refentry">
      <refentry.link.set>
        <xsl:attribute name="idref">
          <xsl:value-of select="generate-id()"/>
        </xsl:attribute>
        <xsl:for-each
            select=".//*[self::ulink
                    or self::footnote[not(ancestor::table)]
                    or self::annotation
                    or self::alt]
                    [node()
                    and not(ancestor::refentryinfo)
                    and not(ancestor::info)
                    and not(ancestor::docinfo)
                    and not(ancestor::refmeta)
                    and not(ancestor::refnamediv)
                    and not(ancestor::indexterm)
                    and not(. = @url)
                    and not(@url =
                    preceding::ulink[node()
                    and not(ancestor::refentryinfo)
                    and not(ancestor::info)
                    and not(ancestor::docinfo)
                    and not(ancestor::refmeta)
                    and not(ancestor::refnamediv)
                    and not(ancestor::indexterm)
                    and (generate-id(ancestor::refentry)
                    = generate-id(current()))]/@url)]">
          <notesource>
            <xsl:attribute name="earmark">
              <xsl:choose>
                <xsl:when test="@url">
                  <xsl:value-of select="@url"/>
                </xsl:when>
                <xsl:otherwise>
                  <xsl:value-of select="generate-id()"/>
                </xsl:otherwise>
              </xsl:choose>
            </xsl:attribute>
            <xsl:copy>
              <xsl:copy-of select="node()"/>
            </xsl:copy>
          </notesource>
        </xsl:for-each>
      </refentry.link.set>
    </xsl:for-each>
  </xsl:if>
</xsl:template>

<!-- ==================================================================== -->

<xsl:template match="ulink|footnote[not(ancestor::table)]|annotation|alt">
  <xsl:variable name="get.all.links.with.unique.urls">
    <xsl:call-template  name="get.all.links.with.unique.urls"/>
  </xsl:variable>
  <xsl:variable name="all.links.with.unique.urls"
                select="exsl:node-set($get.all.links.with.unique.urls)"/>
  <xsl:variable name="get.links.with.unique.urls">
    <!-- * get the set of all unique notesources in the ancestor Refentry of -->
    <!-- * this notesource -->
    <xsl:copy-of
        select="$all.links.with.unique.urls/refentry.link.set
                [@idref = generate-id(current()/ancestor::refentry)]/notesource"/>
  </xsl:variable>
  <xsl:variable name="links.with.unique.urls"
                select="exsl:node-set($get.links.with.unique.urls)"/>
  <!-- * Identify the "earmark" for this notesource -->
  <xsl:variable name="earmark">
    <xsl:choose>
      <xsl:when test="@url">
        <xsl:value-of select="@url"/>
      </xsl:when>
      <xsl:otherwise>
        <xsl:value-of select="generate-id()"/>
      </xsl:otherwise>
    </xsl:choose>
  </xsl:variable>
  <!-- * for links, $notesource is either the link contents (if the -->
  <!-- * link is non-empty) or the "earmark" URL (if the link is empty) -->
  <xsl:variable name="notesource">
    <xsl:choose>
      <!-- * check to see if the element is empty or not; if it's -->
      <!-- * non-empty, get the content -->
      <xsl:when test="node()">
        <xsl:apply-templates/>
      </xsl:when>
      <xsl:otherwise>
        <!-- * Otherwise this is an empty link, so we just get the -->
        <!-- * value of its URL; note that we don't number empty links -->
        <!-- * -->
        <!-- * Add hyphenation suppression in URL output only if -->
        <!-- * break.after.slash is also non-zero -->
        <xsl:if test="$man.hyphenate.urls = 0 and
                      $man.break.after.slash = 0">
          <xsl:call-template name="suppress.hyphenation"/>
          <xsl:text>&#x2593;%</xsl:text>
        </xsl:if>
        <xsl:value-of select="$earmark"/>
      </xsl:otherwise>
    </xsl:choose>
  </xsl:variable>
  <xsl:if test="self::ulink">
  <xsl:choose>
    <!-- * if user wants links underlined, underline (ital) it -->
    <xsl:when test="$man.links.are.underlined != 0">
      <xsl:variable name="link.wrapper">
        <italic><xsl:value-of select="$notesource"/></italic>
      </xsl:variable>
      <xsl:apply-templates mode="italic" select="exsl:node-set($link.wrapper)"/>
    </xsl:when>
    <xsl:otherwise>
      <!-- * user doesn't want links underlined, so just display content -->
      <xsl:value-of select="$notesource"/>
    </xsl:otherwise>
  </xsl:choose>
  </xsl:if>
  <!-- * If link is non-empty AND its string value is not equal to the -->
  <!-- * value of its url attribute AND user wants notesources numbered, -->
  <!-- * then output a number for it -->
  <xsl:if test="node() and not(. = @url) and $man.links.are.numbered != 0">
    <!-- * We number notesources by checking the $links.with.unique.urls -->
    <!-- * set and finding the notesource whose earmark matches the -->
    <!-- * earmark for this notesource. -->
    <!-- * -->
    <!-- * If this is the only instance in this Refentry of a notesource -->
    <!-- * with this earmark, then it gets a unique number. -->
    <!-- * -->
    <!-- * But if this is a notesource for which there are multiple -->
    <!-- * instances of notesources in this Refentry that have the same -->
    <!-- * earmark as this notesource, then the number assigned is the -->
    <!-- * number of the _first_ instance of a notesource in this -->
    <!-- * Refentry with the earmark for this notesource (which be the -->
    <!-- * number of this notesource itself, if it happens to be the -->
    <!-- * first instance). -->
    <xsl:variable name="notesource.number">
      <xsl:apply-templates
          select="$links.with.unique.urls/notesource[@earmark = $earmark][1]"
          mode="notesource.number"/>
    </xsl:variable>
    <!-- * Format the number by placing it in square brackets. FIXME: -->
    <!-- * This formatting should probably be made user-configurable, -->
    <!-- * to allow something other than just square brackets; e.g., -->
    <!-- * Angle brackets<10> or Braces{10}  -->
    <xsl:text>&#x2593;&amp;[</xsl:text>
    <xsl:value-of select="$notesource.number"/>
    <xsl:text>]</xsl:text>
    <!-- * Note that the reason for the \& before the opening bracket -->
    <!-- * is to prevent any possible linebreak from being introduced -->
    <!-- * between the opening bracket and the following text. -->
  </xsl:if>
  </xsl:template>

<!-- ==================================================================== -->

<xsl:template match="*" mode="notesource.number">
  <xsl:value-of select="count(preceding::*[(self::ulink
                        or self::footnote[not(ancestor::table)]
                        or self::annotation
                        or self::alt)
                        and node()
                        and not(ancestor::refentryinfo)
                        and not(ancestor::info)
                        and not(ancestor::docinfo)
                        and not(ancestor::refmeta)
                        and not(ancestor::refnamediv)
                        and not(ancestor::indexterm)
                        and not(. = @url)
                        and not(@url =
                        preceding::ulink[node()
                        and not(ancestor::refentryinfo)
                        and not(ancestor::info)
                        and not(ancestor::docinfo)
                        and not(ancestor::refmeta)
                        and not(ancestor::refnamediv)
                        and not(ancestor::indexterm)
                        and (generate-id(ancestor::refentry)
                        = generate-id(current()/ancestor::refentry))]/@url)]
                        [generate-id(ancestor::refentry)
                        = generate-id(current()/ancestor::refentry)]) + 1"/>
</xsl:template>

<!-- ==================================================================== -->

<xsl:template name="endnotes.list">
  <xsl:variable name="notesources"
                select=".//*[(self::ulink
                  or self::footnote[not(ancestor::table)]
                  or self::annotation
                  or self::alt)
                  and node()
                  and not(ancestor::refentryinfo)
                  and not(ancestor::info)
                  and not(ancestor::docinfo)
                  and not(ancestor::refmeta)
                  and not(ancestor::refnamediv)
                  and not(ancestor::indexterm)
                  and not(. = @url)
                  and not(@url =
                  preceding::ulink[node()
                  and not(ancestor::refentryinfo)
                  and not(ancestor::info)
                  and not(ancestor::docinfo)
                  and not(ancestor::refmeta)
                  and not(ancestor::refnamediv)
                  and not(ancestor::indexterm)
                  and (generate-id(ancestor::refentry)
                  = generate-id(current()))]/@url)]"/>
  <!-- * check to see if we have actually found any notesources; if we -->
  <!-- * have, we generate the endnotes list, if not, we do nothing -->
  <xsl:if test="$notesources/node()">
    <xsl:call-template name="format.endnotes.list">
      <xsl:with-param name="notesources" select="$notesources"/>
    </xsl:call-template>
  </xsl:if>
</xsl:template>

<!-- ==================================================================== -->

<xsl:template name="format.endnotes.list">
  <xsl:param name="notesources"/>
  <xsl:call-template name="mark.subheading"/>
  <!-- * make the endnotes-list section heading -->
  <xsl:text>&#x2302;SH "</xsl:text>
  <xsl:call-template name="string.upper">
    <xsl:with-param name="string">
      <xsl:choose>
        <!-- * if user has specified a heading, use that -->
        <xsl:when test="$man.links.list.heading != ''">
          <xsl:value-of select="$man.links.list.heading"/>
        </xsl:when>
        <xsl:otherwise>
          <!-- * otherwise, get localized heading from gentext -->
          <!-- * (in English, NOTES) -->
          <xsl:call-template name="gentext">
            <xsl:with-param name="key" select="'References'"/>
          </xsl:call-template>
        </xsl:otherwise>
      </xsl:choose>
    </xsl:with-param>
  </xsl:call-template>
  <xsl:text>"&#10;</xsl:text>
  <xsl:for-each select="$notesources">
    <!-- * make paragraph with hanging indent, and starting with a -->
    <!-- * number in the form " 1." (padded to $man.indent.width - 1) -->
    <xsl:text>&#x2302;IP</xsl:text>
    <xsl:text> "</xsl:text>
    <xsl:variable name="endnote.number">
      <xsl:apply-templates select="." mode="notesource.number"/>
      <xsl:text>.</xsl:text>
    </xsl:variable>
    <xsl:call-template name="prepend-pad">
      <xsl:with-param name="padVar" select="$endnote.number"/>
      <!-- FIXME: the following assumes that $man.indent.width is in -->
      <!-- en's; also, this should probably use $list.indent instead -->
      <xsl:with-param name="length" select="$man.indent.width - 1"/>
    </xsl:call-template>
    <xsl:text>"</xsl:text>
    <xsl:if test="not($list-indent = '')">
      <xsl:text> </xsl:text>
      <xsl:value-of select="$list-indent"/>
    </xsl:if>
    <xsl:text>&#10;</xsl:text>
    <!-- * Print the endnote contents -->
    <!-- * -->
    <!-- * IMPORTANT: If there are multiple notesources in this Refentry -->
    <!-- * with the same earmark, this gets the contents of the first -->
    <!-- * instance of the notesource in this Refentry with that earmark -->
    <xsl:choose>
      <xsl:when test="self::ulink">
        <xsl:variable name="contents">
          <xsl:apply-templates/>
        </xsl:variable>
        <xsl:value-of select="normalize-space($contents)"/>
      </xsl:when>
      <xsl:otherwise>
        <xsl:apply-templates/>
      </xsl:otherwise>
    </xsl:choose>
    <xsl:text>&#10;</xsl:text> 
    <xsl:if test="@url">
      <xsl:text>&#x2302;RS</xsl:text>
      <xsl:if test="not($list-indent = '')">
        <xsl:text> </xsl:text>
        <xsl:value-of select="$list-indent"/>
      </xsl:if>
      <xsl:text>&#10;</xsl:text>
      <!-- * This is a link, so print its URL. -->
      <!-- * Add hyphenation suppression in URL output only if -->
      <!-- * $break.after.slash is also non-zero -->
      <xsl:if test="$man.hyphenate.urls = 0
                    and $man.break.after.slash = 0">
        <xsl:call-template name="suppress.hyphenation"/>
        <xsl:text>&#x2593;%</xsl:text>
      </xsl:if>
      <xsl:value-of select="@url"/>
      <xsl:text>&#10;</xsl:text>
      <xsl:text>&#x2302;RE</xsl:text>
      <xsl:text>&#10;</xsl:text>
    </xsl:if>
  </xsl:for-each>
</xsl:template>

</xsl:stylesheet>
