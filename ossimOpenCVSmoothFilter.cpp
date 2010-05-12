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

#include "ossimOpenCVSmoothFilter.h"

#include <ossim/base/ossimRefPtr.h>

#include <ossim/base/ossimConstants.h>
#include <ossim/base/ossimCommon.h>
#include <ossim/base/ossimKeywordlist.h>
#include <ossim/base/ossimKeywordNames.h>
#include <ossim/imaging/ossimImageSourceFactoryBase.h>
#include <ossim/imaging/ossimImageSourceFactoryRegistry.h>
#include <ossim/imaging/ossimU8ImageData.h>
#include <ossim/base/ossimRefPtr.h>

RTTI_DEF1(ossimOpenCVSmoothFilter, "ossimOpenCVSmoothFilter", ossimImageSourceFilter)


ossimOpenCVSmoothFilter::ossimOpenCVSmoothFilter(ossimObject* owner)
   :ossimImageSourceFilter(owner),
   theTile(NULL),
   theParam1(3),
   theParam2(3),
   theParam3(0),
   theParam4(0),
   theSmoothType(2)
{
}

ossimOpenCVSmoothFilter::~ossimOpenCVSmoothFilter()
{
}

ossimRefPtr<ossimImageData> ossimOpenCVSmoothFilter::getTile(const ossimIrect& tileRect, ossim_uint32 resLevel) {

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

void ossimOpenCVSmoothFilter::initialize()
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

ossimScalarType ossimOpenCVSmoothFilter::getOutputScalarType() const
{
   if(!isSourceEnabled())
   {
      return ossimImageSourceFilter::getOutputScalarType();
   }   
   return OSSIM_UCHAR;
}

ossim_uint32 ossimOpenCVSmoothFilter::getNumberOfOutputBands() const
{
   if(!isSourceEnabled())
   {
      return ossimImageSourceFilter::getNumberOfOutputBands();
   }
   return theInputConnection->getNumberOfOutputBands();

}

bool ossimOpenCVSmoothFilter::saveState(ossimKeywordlist& kwl, const char* prefix)const {
   
   ossimImageSourceFilter::saveState(kwl, prefix);

   kwl.add(prefix,"param1",theParam1,true);
   kwl.add(prefix,"param2",theParam2,true);
   kwl.add(prefix,"param3",theParam3,true);
   kwl.add(prefix,"param4",theParam4,true);
   switch(theSmoothType) {
	case 0:
	   kwl.add(prefix,"smooth_type","CV_BLUR_NO_SCALE",true);
	break;
	case 1:
	   kwl.add(prefix,"smooth_type","CV_BLUR",true);
	break;
	case 2:
	   kwl.add(prefix,"smooth_type","CV_GAUSSIAN",true);
	break;
	case 3:
	   kwl.add(prefix,"smooth_type","CV_MEDIAN",true);
	break;
	case 4:
	   kwl.add(prefix,"smooth_type","CV_BILATERAL",true);
	break;
   }
   return true;
}

bool ossimOpenCVSmoothFilter::loadState(const ossimKeywordlist& kwl, const char* prefix) { 

   ossimImageSourceFilter::loadState(kwl, prefix);

   const char* lookup = kwl.find(prefix, "param1");
   if(lookup)
   {
      theParam1 = ossimString(lookup).toInt();
      printf("Read from spec file. param1: %d\n",theParam1);
   }
   lookup = kwl.find(prefix, "param2");
   if(lookup)
   {
      theParam2 = ossimString(lookup).toInt();
      printf("Read from spec file. param2: %d\n",theParam2);
   }
   lookup = kwl.find(prefix, "param3");
   if(lookup)
   {
      theParam3 = ossimString(lookup).toDouble();
      printf("Read from spec file. param3: %f\n",theParam3);
   }
   lookup = kwl.find(prefix, "param4");
   if(lookup)
   {
      theParam4 = ossimString(lookup).toDouble();
      printf("Read from spec file. param4: %f\n",theParam4);
   }
   lookup = kwl.find(prefix, "smooth_type");
   if(lookup)
   {
	if(strcmp(lookup,"CV_BLUR_NO_SCALE")==0){
		theSmoothType=0; 		
	        printf("Read from spec file. smooth_type: %s\n",lookup);
	}
	else if(strcmp(lookup,"CV_BLUR")==0){
		theSmoothType=1; 		
	        printf("Read from spec file. smooth_type: %s\n",lookup);
	}
	else if(strcmp(lookup,"CV_GAUSSIAN")==0){
		theSmoothType=2; 		
	        printf("Read from spec file. smooth_type: %s\n",lookup);
	}
	else if(strcmp(lookup,"CV_MEDIAN")==0){
		theSmoothType=3; 		
	        printf("Read from spec file. smooth_type: %s\n",lookup);
	}
	else if(strcmp(lookup,"CV_BILATERAL")==0){
		theSmoothType=4; 		
        	printf("Read from spec file. smooth_type: %s\n",lookup);
	}
	else {
		printf("%s not supported as smooth_type parameter for OpenCVSmoothFilter!\nDefault smooth_type: CV_GAUSSIAN\n",lookup);
	}
   }
   return true;
}

void ossimOpenCVSmoothFilter::runUcharTransformation(ossimImageData* tile)
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
		cvSmooth(input,output,theSmoothType,theParam1,theParam2,theParam3,theParam4);
		cvReleaseImageHeader(&input);
		cvReleaseImageHeader(&output);
	}

	theTile->validate(); 
 
   }
