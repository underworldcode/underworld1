/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
** Copyright (c) 2010, Monash University
** All rights reserved.
** Redistribution and use in source and binary forms, with or without modification,
** are permitted provided that the following conditions are met:
**
**       * Redistributions of source code must retain the above copyright notice,
**          this list of conditions and the following disclaimer.
**       * Redistributions in binary form must reproduce the above copyright
**         notice, this list of conditions and the following disclaimer in the
**         documentation and/or other materials provided with the distribution.
**       * Neither the name of the Monash University nor the names of its contributors
**         may be used to endorse or promote products derived from this software
**         without specific prior written permission.
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
** THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
** PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS
** BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
** CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
** SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
** HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
** LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
** OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
**
**
** Contact:
*%  Owen Kaluza - Owen.Kaluza(at)monash.edu
*%
*% Development Team :
*%  http://www.underworldproject.org/aboutus.html
**
**~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

#include "GraphicsUtil.h"
#include "ColourMap.h"
#include "Types.h"

#ifndef DrawingObject__
#define DrawingObject__

//Holds parameters for a drawing object
class DrawingObject
{
  public:
   std::string name;
   unsigned int id;
   bool persistent;  //Remains between timestep/window reloads
   bool skip;
   bool visible;

   Colour colour;
   std::vector<ColourMap*> colourMaps; // Uses these Colour Maps (by data type)
   float opacity;

   //Specific data for some object types...
   PropertyParser props;
   bool lit;
   bool cullface;
   bool wireframe;
   float lineWidth;
   float pointSize;
   int pointType;
   bool pointSmooth;
   float arrowHead;
   float scaling; //Vectors/tracers
   bool flat;  //Tracers only for now
   //Tracers...
   int steps;
   float time;
   //Colour Bars
   bool colourbar;
   //Texture
   TextureData* texture;

   DrawingObject(unsigned int id, bool persistent=false, std::string name="", int colour=0, ColourMap* map=NULL, float opacity=1.0, const char* properties="");
   ~DrawingObject() {if (texture) delete texture;}

   void addColourMap(ColourMap* map, lucGeometryDataType data_type);
   int useTexture();

   static unsigned int lastid;
};

#endif //DrawingObject__
