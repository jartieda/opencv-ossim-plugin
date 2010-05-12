// Copyright (C) 2000 ImageLinks Inc. 
//
// OSSIM is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License 
// as published by the Free Software Foundation.
//
// This software is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. 
//
// You should have received a copy of the GNU General Public License
// along with this software. If not, write to the Free Software 
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-
// 1307, USA.
//
// See the GPL in the COPYING.GPL file for more details.
//
// Author: Garrett Potts (gpotts@imagelinks)
// Description: A brief description of the contents of the file.
//
//*************************************************************************
// $Id: ossimSharedRgbToGreyFilter.cpp,v 1.10 2005/05/23 13:06:55 gpotts Exp $

#include <ossim/base/ossimRefPtr.h>
#include "ossimOpenCVDilateFilter.h"
#include <ossim/imaging/ossimU8ImageData.h>
#include <ossim/base/ossimConstants.h>
#include <ossim/base/ossimCommon.h>
#include <ossim/base/ossimKeywordlist.h>
#include <ossim/base/ossimKeywordNames.h>
#include <ossim/imaging/ossimImageSourceFactoryBase.h>
#include <ossim/imaging/ossimImageSourceFactoryRegistry.h>
#include <ossim/base/ossimRefPtr.h>
#include <ossim/base/ossimNumericProperty.h>

RTTI_DEF1(ossimOpenCVDilateFilter, "ossimOpenCVDilateFilter", ossimImageSourceFilter)

ossimOpenCVDilateFilter::ossimOpenCVDilateFilter(ossimObject* owner)
	:ossimImageSourceFilter(owner),
	theTile(NULL),
	theIterations(1)
{
}

ossimOpenCVDilateFilter::~ossimOpenCVDilateFilter()
{
}

ossimRefPtr<ossimImageData> ossimOpenCVDilateFilter::getTile(const ossimIrect& tileRect, ossim_uint32 resLevel)
{
	if(!isSourceEnabled())
   	{
	      return ossimImageSourceFilter::getTile(tileRect, resLevel);
	}
	long w     = tileRect.width();
	long h     = tileRect.height();
   
   	if(!theTile.valid()) initialize();
	if(!theTile.valid()) return 0;
  
	ossimRefPtr<ossimImageData> data = 0;
	if(theInputConnection)
	{
		data  = theInputConnection->getTile(tileRect, resLevel);
   	} else {
	      return 0;
   	}

	if(!data.valid()) return 0;
	if(data->getDataObjectStatus() == OSSIM_NULL ||  data->getDataObjectStatus() == OSSIM_EMPTY)
   	{
	     return 0;
   	}

	theTile->setImageRectangle(tileRect);
	theTile->makeBlank();
   
	theTile->setOrigin(tileRect.ul());
	runUcharTransformation(data.get());
   
	printf("Tile (%d,%d) finished!\n",tileRect.ul().x,tileRect.ul().y); 	
   	return theTile;


}

void ossimOpenCVDilateFilter::initialize()
{
	 if(theInputConnection)
	  {
	      ossimImageSourceFilter::initialize();

	      theTile = new ossimU8ImageData(this,
					     theInputConnection->getNumberOfOutputBands(),   
		                             theInputConnection->getTileWidth(),
		                             theInputConnection->getTileHeight());  
	      theTile->initialize();
	   }

}

ossimScalarType ossimOpenCVDilateFilter::getOutputScalarType() const
{
	if(!isSourceEnabled())
	{
		return ossimImageSourceFilter::getOutputScalarType();
	}

	return OSSIM_UCHAR;
}

ossim_uint32 ossimOpenCVDilateFilter::getNumberOfOutputBands() const
{
	if(!isSourceEnabled())
	{
		return ossimImageSourceFilter::getNumberOfOutputBands();
	}
        return theInputConnection->getNumberOfOutputBands();
}

bool ossimOpenCVDilateFilter::saveState(ossimKeywordlist& kwl,
										const char* prefix)const
{
ossimImageSourceFilter::saveState(kwl, prefix);

   kwl.add(prefix,"iterations",theIterations,true);
   
   return true;
}

bool ossimOpenCVDilateFilter::loadState(const ossimKeywordlist& kwl,
										const char* prefix)
{
	
   ossimImageSourceFilter::loadState(kwl, prefix);

   const char* lookup = kwl.find(prefix, "iterations");
   if(lookup)
   {
      theIterations = ossimString(lookup).toInt();
      printf("Read from spec file. iterations: %d\n",theIterations);
   }
   
   return true;
}

void ossimOpenCVDilateFilter::runUcharTransformation(ossimImageData* tile)
{   
  IplImage *input;
  IplImage *output;

  char* bSrc;
  char* bDst;

  int nChannels = tile->getNumberOfBands();

  for(int k=0; k<nChannels; k++) {
	printf("Channel %d\n",k);
	input=cvCreateImageHeader(cvSize(tile->getWidth(),tile->getHeight()),8,1);
	output=cvCreateImageHeader(cvSize(tile->getWidth(),tile->getHeight()),8,1);
	bSrc = static_cast<char*>(tile->getBuf(k));
	input->imageData=bSrc;
	bDst = static_cast<char*>(theTile->getBuf(k));
	output->imageData=bDst;
	cvDilate(input,output,NULL,theIterations); // a 3x3 rectangular structuring element is used
	cvReleaseImageHeader(&input);
	cvReleaseImageHeader(&output);
	}
  theTile->validate(); 
}

