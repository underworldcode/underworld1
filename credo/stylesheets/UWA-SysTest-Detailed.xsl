<?xml version="1.0" encoding="ISO-8859-1"?>

<xsl:stylesheet version="1.0" 
xmlns:xsl="http://www.w3.org/1999/XSL/Transform">

<xsl:template match="/">
  <html>
  <body>
    <xsl:apply-templates />
  </body>
  </html>
</xsl:template>

<xsl:template match="StgSysTest[@type='Analytic']">
  <h2>Underworld System Test Result - Analytic</h2>
  <h3>Specification</h3>
  <table border="1">
    <xsl:call-template name="DefaultSpecification"/>
    <xsl:call-template name="AnalyticSpecification"/>
  </table>
  <h3>Result</h3>
  <table border="1">
    <xsl:call-template name="DefaultResult"/>
  </table>
  <h3>Results of Test Components applied</h3>
  <xsl:apply-templates select="testComponents/testComponent"/>
</xsl:template>

<xsl:template match="StgSysTest">
  <h2>Underworld System Test Result</h2>
  <table border="1">
    <xsl:call-template name="DefaultSpecification"/>
    <xsl:call-template name="DefaultResult"/>
  </table>
  <h3>Results of Test Components applied</h3>
  <xsl:apply-templates select="testComponents/testComponent"/>
</xsl:template>


<xsl:template name="DefaultSpecification">
    <tr>
      <td>Test Type</td>
      <td><xsl:value-of select="@type"/></td>
    </tr>  
    <tr>
      <td>Input files used</td>
      <td>
      <xsl:for-each select="testSpecification/inputFiles/inputFile">
      <xsl:value-of select="."/>, 
      </xsl:for-each>
      </td>
    </tr>  
    <tr>
      <td>Output path base</td>
      <td>
      <a><xsl:attribute name="href">
      <xsl:value-of select="testSpecification/outputPathBase"/>
       </xsl:attribute> 
      <xsl:value-of select="testSpecification/outputPathBase"/>
      </a>
      </td>
    </tr>  
    <tr>
      <td>Number of procs used</td>
      <td><xsl:value-of select="testSpecification/nproc"/></td>
    </tr>
</xsl:template>

<xsl:template name="DefaultResult">
    <tr>
      <td>Result</td>
        <xsl:choose>
          <xsl:when test="@status = 'Pass'">
            <td><xsl:value-of select="testResult/@status"/></td>
          </xsl:when>
          <xsl:otherwise>
            <td bgcolor="#ff00ff" ><xsl:value-of select="testResult/@status"/></td>
          </xsl:otherwise>
        </xsl:choose>
    </tr>  
    <tr>
      <td>status msg</td>
      <td><xsl:value-of select="testResult/statusMsg"/></td>
    </tr>
</xsl:template>

<!-- Specific sections -->

<xsl:template name="AnalyticSpecification">
    <tr>
      <td>Default Field Tolerance</td>
      <td><xsl:value-of select="testSpecification/defaultFieldTol"/></td>
    </tr>  
</xsl:template>

<xsl:template match="testComponent[@type='fieldWithinTol']">
  <table border="1">
    <tr>
      <!--<td>Test Component</td>-->
      <td colspan="2"><b><xsl:value-of select="@name"/> (Field Within Tolerance)</b></td>
    </tr>
    <tr>
      <td>Result</td>
        <xsl:choose>
          <xsl:when test="@status = 'Pass'">
            <td><xsl:value-of select="@status"/></td>
          </xsl:when>
          <xsl:otherwise>
            <td bgcolor="#ff00ff" ><xsl:value-of select="@status"/></td>
          </xsl:otherwise>
        </xsl:choose>
    </tr>
    <tr>
      <td>Status</td>
      <td><xsl:value-of select="result/statusMsg"/></td>
    </tr>
  </table>
  <h4>Field Results:</h4>
  <table border="1">
    <tr>
      <td>Field</td>
      <td>Req'd tolerance</td>
      <td>Actual errors</td>
    </tr>
    <xsl:for-each select="result/fieldResultDetails/field">
    <tr>
      <xsl:variable name="FieldName" select="@name"/>
      <td><xsl:value-of select="$FieldName"/></td>
      <td><xsl:value-of select="../../../specification/fields/field[@name=$FieldName]/@tol"/></td>
      <td>
      (<xsl:for-each select="run/dofErrors/dofError">
      <xsl:value-of select="@error"/>
      <xsl:text>, </xsl:text>
      </xsl:for-each>)
      </td>
    </tr>
    </xsl:for-each>
  </table>
</xsl:template>

</xsl:stylesheet>
