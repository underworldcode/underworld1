<?xml version="1.0" encoding="ISO-8859-1"?>

<xsl:stylesheet version="1.0" 
xmlns:xsl="http://www.w3.org/1999/XSL/Transform">

<xsl:template match="UWA-Result">
  <html>
  <body>
  <h2>Underworld System Test Result</h2>
  <table border="1">
    <tr bgcolor="#9acd32">
      <th>Test Type</th>
      <th>Input files used</th>
      <th>Output path base</th>
      <th>Number of procs used</th>
      <th>Result</th>
      <th>status msg</th>
    </tr>
    <xsl:apply-templates/>
  </table>
  </body>
  </html>
</xsl:template>

<xsl:template match="StgSysTest">
    <tr> 
      <td><xsl:value-of select="@type"/></td>
      <td>
      <xsl:for-each select="testSpecification/inputFiles/inputFile">
      <xsl:value-of select="."/>, 
      </xsl:for-each>
      </td>
      <td><xsl:value-of select="testSpecification/outputPathBase"/></td>
      <td><xsl:value-of select="testSpecification/nproc"/></td>
    <xsl:choose>
      <xsl:when test="@status = 'Pass'">
        <td><xsl:value-of select="testResult/@status"/></td>
      </xsl:when>
      <xsl:otherwise>
        <td bgcolor="#ff00ff" ><xsl:value-of select="testResult/@status"/></td>
      </xsl:otherwise>
    </xsl:choose>
      <td><xsl:value-of select="testResult/statusMsg"/></td>
    </tr>
</xsl:template>


</xsl:stylesheet>
