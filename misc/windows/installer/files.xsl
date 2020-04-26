<?xml version="1.0" encoding="UTF-8"?>
<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
                              xmlns:wi="http://schemas.microsoft.com/wix/2006/wi">
  <xsl:output method="xml" version="1.0" encoding="UTF-8" indent="yes"/>
  <xsl:template match="//wi:Wix/wi:Fragment/wi:ComponentGroup/wi:Component/wi:File[Source='$(sys.CURRENTDIR)\Talkie.exe']">
    <xsl:attribute name="Id">TalkieExeFile</xsl:attribute>
  </xsl:template>
</xsl:stylesheet>