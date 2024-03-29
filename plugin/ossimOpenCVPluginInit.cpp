// Copyright (C) 2010 Argongra 
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
//*************************************************************************

/** 
 \mainpage OpenCV Plugin for OSSIM 

 \section intro_sec Introduction
 
 This project was created to provide developers a framekwork to use OpenCV functions in OSSIM image processing chains.  

 "OpenCV (Open Source Computer Vision) is a library of programming functions for real time computer vision."

 To learn more about OpenCV, visit
 
 - http://opencv.willowgarage.com/wiki/Welcome
 - http://sourceforge.net/projects/opencvlibrary/ 

 "Open Source Software Image Map (OSSIM) is a high performance engine for remote sensing, image processing, geographical information systems and photogrammetry."
 
 Information about OpenCV can be found at:
 - http://www.ossim.org

 \section install_sec Installation
 
 Step 1: Download the latest project source code from svn

 \code 
 >> svn co http://opencv-ossim-plugin.googlecode.com/svn/trunk/ opencv-ossim-plugin-read-only
 \endcode

 Step 2: Build OpenCV plugin 

 \code
 >> cd /path/to/source/ 
 >> cmake -G "Unix Makefiles"
 >> make 
 \endcode
 
 Step 3: Add the plugin path in "ossim_preferences" file 

 To add the OpenCV plugin to the OSSIM plugin bridge support, add: 
 \code 
 ...
 plugin.file1: /path/to/source/libossimopencv_plugin.so
 ...
 \endcode

 Don't forget to set the OSSIM_PREFS_FILE variable pointing to the "ossim_preferences" file:  
 \code 
 >> export OSSIM_PREFS_FILE=path/to/file/ossim_preferences
 \endcode
 
 Step 4: Check the plugin is successfully loaded

 \code 
  >> ossim-info --plugins
 \endcode

 \section doc_sec Documentation

 Under the current project source code folder, type: 
 \code 
 >> doxygen doxyfile
 \endcode
 It will generate an on-line documentation browser (in HTML) from a the documented source files.

 **/

#include "ossimOpenCVPluginFactory.h"
#include "ossimPluginConstants.h"

#include <ossim/imaging/ossimImageSourceFactoryRegistry.h>
#include <ossim/plugin/ossimSharedObjectBridge.h>
#include <ossim/base/ossimString.h>

extern "C"
{
  ossimSharedObjectInfo myInfo;
  ossimString theDescription;
  std::vector<ossimString> theObjList;

  const char* getDescription()
  {
    return theDescription.c_str();
  }

  int getNumberOfClassNames()
  {
      return (int)theObjList.size();
  }

  const char* getClassName(int idx)
  {
      if(idx < (int)theObjList.size())
	{
	  return theObjList[idx].c_str();
	}
      return 0;
  }

  OSSIM_PLUGINS_DLL  void ossimSharedLibraryInitialize(ossimSharedObjectInfo** info)
  {
    myInfo.getDescription        = getDescription;
    myInfo.getNumberOfClassNames = getNumberOfClassNames;
    myInfo.getClassName          = getClassName;
    *info = &myInfo;
    
    theDescription = ossimString("OpenCV Plugin");
    
    theObjList.push_back("ossimOpenCVCannyFilter");
    theObjList.push_back("ossimOpenCVDilateFilter");
    theObjList.push_back("ossimOpenCVErodeFilter");
    theObjList.push_back("ossimOpenCVLaplaceFilter");
    theObjList.push_back("ossimOpenCVSmoothFilter");
    theObjList.push_back("ossimOpenCVSobelFilter");
    theObjList.push_back("ossimOpenCVThresholdFilter");
    theObjList.push_back("ossimOpenCVGoodFeaturesToTrack");
    theObjList.push_back("ossimOpenCVPyrSegmentation");

    ossimImageSourceFactoryRegistry::instance()->registerFactory(ossimOpenCVPluginFactory::instance());

  }

  OSSIM_PLUGINS_DLL void ossimSharedLibraryFinalize()
  {
    ossimImageSourceFactoryRegistry::instance()->unregisterFactory(ossimOpenCVPluginFactory::instance());
  }

}


